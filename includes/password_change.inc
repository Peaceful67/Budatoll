<?php
/**
 * Frontend jelszóváltoztatás (shortcode: [budatoll_password_change])
 * Requires: WordPress 5.2+
 */

if ( ! defined( 'ABSPATH' ) ) { exit; }

class Budatoll_Password_Change {

    const NONCE_ACTION = 'budatoll_change_password_action';
    const NONCE_NAME   = 'budatoll_change_password_nonce';
    const SC          = 'budatoll_password_change';

    public static function init() {
        add_shortcode( self::SC, [ __CLASS__, 'shortcode' ] );
        add_action( 'wp_enqueue_scripts', [ __CLASS__, 'assets' ] );
        add_action( 'init', [ __CLASS__, 'handle_post' ] );
    }

    /**
     * Enqueue password strength meter only when shortcode is on the page.
     */
    public static function assets() {
        if ( is_admin() ) return;

        // Egyszerű detektálás: ha a jelenlegi tartalomban van shortcode.
        if ( is_singular() ) {
            global $post;
            if ( $post && has_shortcode( (string) $post->post_content, self::SC ) ) {
                wp_enqueue_script( 'password-strength-meter' ); // beépített WP script
                // pici inline JS az erősség visszajelzéshez
                wp_add_inline_script( 'password-strength-meter', self::inline_js(), 'after' );
                wp_add_inline_style( 'wp-block-library', self::inline_css() );
            }
        }
    }

    /**
     * Shortcode kirajzolása
     */
    public static function shortcode() {
        if ( ! is_user_logged_in() ) {
            return '<div class="bt-pass-box bt-pass-error">' . esc_html__( 'You must be logged in to change your password.', 'budatoll' ) . '</div>';
        }

        $messages = self::consume_flash_messages();

        ob_start(); ?>
        <div class="bt-pass-box">
            <?php foreach ( $messages as $m ) : ?>
                <div class="bt-pass-notice <?php echo esc_attr( $m['type'] ); ?>"><?php echo esc_html( $m['text'] ); ?></div>
            <?php endforeach; ?>

            <form method="post" class="bt-pass-form" autocomplete="off">
                <?php wp_nonce_field( self::NONCE_ACTION, self::NONCE_NAME ); ?>
                <input type="hidden" name="bt_pass_form" value="1" />

                <div class="bt-pass-field">
                    <label for="current_password"><?php esc_html_e( 'Jelenlegi jelszó', 'budatoll' ); ?></label>
                    <input type="password" id="current_password" name="current_password" required>
                </div>

                <div class="bt-pass-field">
                    <label for="new_password_1"><?php esc_html_e( 'Új jelszó', 'budatoll' ); ?></label>
                    <input type="password" id="new_password_1" name="new_password_1" required minlength="8" aria-describedby="pw-strength-msg">
                </div>

                <div class="bt-pass-field">
                    <label for="new_password_2"><?php esc_html_e( 'Még egyszer', 'budatoll' ); ?></label>
                    <input type="password" id="new_password_2" name="new_password_2" required minlength="8">
                </div>

                <div id="pw-strength-msg" class="bt-pass-help" aria-live="polite"></div>

                <div class="bt-pass-actions">
                    <button type="submit" class="budatoll-button"><?php esc_html_e( 'Mentés', 'budatoll' ); ?></button>
                </div>
            </form>
        </div>
        <?php
        return ob_get_clean();
    }

    /**
     * POST feldolgozás – ugyanabban a kérésben fusson (init)
     */
    public static function handle_post() {
        if ( empty( $_POST['bt_pass_form'] ) ) {
            return;
        }

        if ( ! is_user_logged_in() ) {
            self::flash( 'error', __( 'You must be logged in to change your password.', 'budatoll' ) );
            return;
        }

        if ( ! isset( $_POST[ self::NONCE_NAME ] ) || ! wp_verify_nonce( $_POST[ self::NONCE_NAME ], self::NONCE_ACTION ) ) {
            self::flash( 'error', __( 'Security check failed. Please try again.', 'budatoll' ) );
            return;
        }

        $current  = (string) ( $_POST['current_password']  ?? '' );
        $new1     = (string) ( $_POST['new_password_1']    ?? '' );
        $new2     = (string) ( $_POST['new_password_2']    ?? '' );

        $user = wp_get_current_user();

        // 1) jelenlegi jelszó helyes?
        if ( empty( $current ) || ! wp_check_password( $current, $user->user_pass, $user->ID ) ) {
            self::flash( 'error', __( 'Your current password is incorrect.', 'budatoll' ) );
            return;
        }

        // 2) új jelszó validáció
        if ( strlen( $new1 ) < 8 ) {
            self::flash( 'error', __( 'Your new password must be at least 8 characters.', 'budatoll' ) );
            return;
        }
        if ( $new1 !== $new2 ) {
            self::flash( 'error', __( 'The two new passwords do not match.', 'budatoll' ) );
            return;
        }
        if ( $new1 === $current ) {
            self::flash( 'error', __( 'Your new password must be different from the current password.', 'budatoll' ) );
            return;
        }

        // 3) jelszó beállítása (ez érvényteleníti a sessziókat)
        wp_set_password( $new1, $user->ID );
        bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user->ID, 'Sikeres jelszó módosítás');
        // 4) kényelmi re-login (ne dobja ki a felhasználót)
        $creds = [
            'user_login'    => $user->user_login,
            'user_password' => $new1,
            'remember'      => true,
        ];
        $signed = wp_signon( $creds, is_ssl() );
        if ( is_wp_error( $signed ) ) {
            // ha valamiért nem sikerül, akkor is sikeres volt a jelszócsere – irány a login
            self::flash( 'success', __( 'Password changed. Please sign in again.', 'budatoll' ) );
        } else {
            // sikeres újrabejelentkezés
            wp_set_current_user( $signed->ID, $signed->user_login );
            wp_set_auth_cookie( $signed->ID, true );
            self::flash( 'success', __( 'Your password has been changed successfully.', 'budatoll' ) );
        }

        // 5) Redirect POST-Redirect-GET mintára
        wp_safe_redirect( remove_query_arg( [ 'pwd' ], add_query_arg( 'pwd', 'changed', wp_get_referer() ?: home_url( '/' ) ) ) );
        exit;
    }

    /* -------- Helpers: flash üzenetek -------- */

    protected static function flash( $type, $text ) {
        $key = 'bt_pass_flash';
        $msgs = isset( $_SESSION[ $key ] ) && is_array( $_SESSION[ $key ] ) ? $_SESSION[ $key ] : [];
        $msgs[] = [ 'type' => $type, 'text' => wp_strip_all_tags( $text ) ];
        // biztos ami biztos: induljon session
        if ( ! session_id() ) { @session_start(); }
        $_SESSION[ $key ] = $msgs;
    }

    protected static function consume_flash_messages() {
        $key = 'bt_pass_flash';
        if ( ! session_id() ) { @session_start(); }
        $msgs = isset( $_SESSION[ $key ] ) ? (array) $_SESSION[ $key ] : [];
        unset( $_SESSION[ $key ] );
        return $msgs;
    }

    /* -------- UI: minimál CSS + JS -------- */

    protected static function inline_css() {
        return <<<CSS
.bt-pass-box{max-width:480px;margin:1rem auto;padding:1rem;border:1px solid #e5e7eb;border-radius:12px;background:#fff;box-shadow:0 4px 10px rgba(0,0,0,.04)}
.bt-pass-field{margin-bottom:.8rem}
.bt-pass-field label{display:block;font-weight:600;margin-bottom:.25rem}
.bt-pass-field input{width:100%;padding:.6rem;border:1px solid #d1d5db;border-radius:8px}
.bt-pass-actions{margin-top:1rem}
.bt-pass-btn{display:inline-block;padding:.6rem 1rem;border-radius:8px;border:0;background:#111;color:#fff;cursor:pointer}
.bt-pass-notice{padding:.6rem .8rem;border-radius:8px;margin:.4rem 0}
.bt-pass-notice.success{background:#ecfdf5;color:#065f46;border:1px solid #a7f3d0}
.bt-pass-notice.error{background:#fef2f2;color:#991b1b;border:1px solid #fecaca}
#pw-strength-msg{font-size:.9rem;opacity:.9}
CSS;
    }

    protected static function inline_js() {
        // használja a WP passwordStrength metódust (zxcvbn alapú)
        return <<<JS
(function(){
    function byId(id){ return document.getElementById(id); }

    var p1 = byId('new_password_1');
    var p2 = byId('new_password_2');
    var msg = byId('pw-strength-msg');

    if(!p1 || !p2 || !msg || typeof window.passwordStrength !== 'function') return;

    function renderStrength(){
        var pass1 = p1.value, user = '';
        var strength = window.passwordStrength(pass1, [], pass1);
        var texts = {
            0: 'Very weak',
            1: 'Weak',
            2: 'Medium',
            3: 'Strong',
            4: 'Very strong'
        };
        var t = texts[strength] || '';
        var matchTxt = (p2.value && p1.value !== p2.value) ? ' – passwords do not match' : '';
        msg.textContent = t + matchTxt;
    }

    p1.addEventListener('input', renderStrength);
    p2.addEventListener('input', renderStrength);
})();
JS;
    }
}
function bt_change_password() {
    
    error_log('Password_change');
    //return 'Jelszó módositás';
    Budatoll_Password_Change::shortcode();

}
Budatoll_Password_Change::init();