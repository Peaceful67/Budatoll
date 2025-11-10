<?php

if (!defined('ABSPATH')) {
    die();
}

function budatoll_is_page_allowed() {

    $permalink = strtok(rtrim($_SERVER['REQUEST_URI'], '/'), '?');
    if (substr($permalink, -1, 1) == '/') {
        $permalink = substr($permalink, 0, -1);
    }
    if (bt_user_has_role(BUDATOLL_ROLE_ADMIN)) { // Ennek bármit engedünk
        return;
    }
    $page_acccess = [// Ezeket az oldalakat csak a megadott szerepkörrel érhetjük el
        BUDATOLL_ROLE_VISITOR => ['', '/nincs-jogosultsag', '/belepes', '/gyik', '/rolunk'],
        BUDATOLL_ROLE_COACH => ['/alkalom-naptar', '/alkalom-lista', '/edzesek-naptar', '/edzesek-lista', '/egyenlegek', '/egyenleg', '/edzeseim-naptar', '/szabalyok'
            , '/beallitasok', '/naplok', '/edzes-tipusok', '/emailek', '/jatekosok', '/sablon-naptar', '/jelszo',],
        BUDATOLL_ROLE_PLAYER => ['/edzeseim-lista', '/edzeseim-naptar', '/egyenlegem', '/szabalyok', '/jelszo',],
    ];

    if (is_user_logged_in() && bt_is_user_active(get_current_user_id())) {
        $user = wp_get_current_user();
        $roles = $user->roles;
    } else {
        $roles = [];
    }
    $roles[] = BUDATOLL_ROLE_VISITOR;
    foreach ($roles as $role) {  // Vesszük az összes szerepet 
        if (in_array($permalink, $page_acccess[$role])) { // Ha benne van az aktuális, örülünk
            return;
        }
    }
//    error_log('Nincs jogosultság');
    wp_redirect(site_url('/nincs-jogosultsag'));
    exit();
}

function bt_is_deletable($item_id, $related_arr) {
    global $wpdb;
    foreach ($related_arr as $table => $field) {
        $sql = 'SELECT * FROM `' . $table . '` WHERE `' . $field . '`=' . $item_id . ' LIMIT 1;';
        $wpdb->get_results($sql);
        if ($wpdb->num_rows > 0) {
            return false;
        }
    }
    return true;
}

function bt_is_day_before($day) {
    $eventTime = new DateTime($day, new DateTimeZone(wp_timezone_string()));
    $eventTime->modify('-1 day')->setTime(0, 0);
    $currentTime = new DateTime('now', new DateTimeZone(wp_timezone_string()));
    return ($currentTime >= $eventTime);
}

function getPostVariable($var_name, $default = false) {
//    $ret = filter_input(INPUT_POST, $var_name, FILTER_NULL_ON_FAILURE);
//    error_log('getPostVariable '.$var_name.': '.$_POST[$var_name]);
    $ret = $_POST[$var_name] ?? $default;
    return $ret;
}

function putHiddenValue($var_name, $value) {
    return '<input type="hidden" name="' . $var_name . '" id="' . $var_name . '" value="' . $value . '">';
}

function bt_put_script($script) {
    return '<script type="text/javascript">' . $script . '</script>';
}

function bt_welcome_message() {
    $user = wp_get_current_user();
    if ($user->ID == 0) {
        $ret = 'Lépj be vagy regisztrálj játékosként!';
    } else {
        $balance = bt_get_balance_of_user($user->ID);
        $ret = 'Belépve: ' . $user->display_name;
        if (bt_user_has_role(BUDATOLL_ROLE_PLAYER)) {
            $ret .= '<p ';
            $ret .= $balance < 0 ? ' style="color:red">' : '>';
            $ret .= 'Egyenleged: ' . bt_currency($balance) . '</p>';
        }
    }
    return $ret;
}

function bt_get_current_userid() {
    $user = wp_get_current_user();
    return ($user->ID == 0) ? false : $user->ID;
}

function bt_send_email($user_id, $subject, $message) {
    $user = get_userdata($user_id);
    if (!$user) {
        return false; // User not found
    }
    $user_email = sanitize_email($user->user_email);
    if (!is_email($user_email)) {
        return false;
    }
    $headers = [
        'Content-Type: text/html; charset=UTF-8',
        'From: ' . get_option('blogname') . ' <from@budatoll.hu>',
        'Reply-to: no-reply@budatoll.hu',
    ];
    $message .= '<br><br>' . get_option(BT_CUSTOM_EMAIL_SIGN, 'Üdvözlettel:<br>Budatoll csapata');
    bt_logger(BUDATOLL_LOGGER_TYPE_EMAILS, $user_id, $subject . ', ' . substr($message, 0, 128));
    $ret = wp_mail($user_email, $subject, $message, $headers);
    return $ret;
}

function budatoll_menu_based_on_role($args) {
    if (is_user_logged_in() AND ('primary-menu' === $args['theme_location'])) {
        $user = wp_get_current_user();
        if (in_array(BUDATOLL_ROLE_ADMIN, (array) $user->roles) AND wp_get_nav_menu_object('Admin')) {
            $args['menu'] = 'Admin';
        } elseif (in_array(BUDATOLL_ROLE_COACH, (array) $user->roles) AND wp_get_nav_menu_object('Coach')) {
            $args['menu'] = 'Coach';
        } elseif (in_array(BUDATOLL_ROLE_PLAYER, (array) $user->roles) AND wp_get_nav_menu_object('Player')) {
            $args['menu'] = 'Player';
        }
    } elseif (wp_get_nav_menu_object('Visitor')) {
        $args['menu'] = 'Visitor';
    } else {
        $args['menu'] = '';
    }

    return $args;
}

function budatoll_redirect_after_login() {
    wp_redirect(home_url());
    exit();
}

function budatoll_deactivated() {
    $role_player = get_role(BUDATOLL_ROLE_PLAYER);
    if ($role_player) {
        $role_player->remove_cap(BUDATOLL_ACCESS_AJAX);
        $role_player->remove_cap(BUDATOLL_ACCESS_MANAGE_PLAYER);
    }
    $role_coach = get_role(BUDATOLL_ROLE_COACH);
    if ($role_coach) {
        $role_coach->remove_cap(BUDATOLL_ACCESS_AJAX);
        $role_coach->remove_cap(BUDATOLL_ACCESS_MANAGE_COACH);
    }
    remove_role(BUDATOLL_ROLE_COACH);
    remove_role(BUDATOLL_ROLE_PLAYER);
    wp_clear_scheduled_hook('budatoll_cron_hook');
}

function budatoll_activated() {
    global $wpdb;
    add_role(BUDATOLL_ROLE_COACH, 'Edző', ['read' => false, BUDATOLL_ACCESS_MANAGE_OPTIONS => false, BUDATOLL_ACCESS_MANAGE_COACH => true]);
    add_role(BUDATOLL_ROLE_PLAYER, 'Játékos', ['read' => false, BUDATOLL_ACCESS_MANAGE_OPTIONS => false, BUDATOLL_ACCESS_MANAGE_PLAYER => true]);
    $role_player = get_role(BUDATOLL_ROLE_PLAYER);
    if ($role_player) {
        $role_player->add_cap(BUDATOLL_ACCESS_AJAX);
        update_option('default_role', BUDATOLL_ROLE_PLAYER);
    }
    $role_coach = get_role(BUDATOLL_ROLE_COACH);
    if ($role_coach) {
        $role_coach->add_cap(BUDATOLL_ACCESS_AJAX);
    }
    if (!wp_next_scheduled('budatoll_cron_hook')) {
        wp_schedule_event(time(), 'every_five_minutes', 'budatoll_cron_hook');
    }
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_EVENT_TYPE_TABLE . '` (`'
            . BUDATOLL_EVENT_TYPE_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_EVENT_TYPE_SHORT . '` varchar(50) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_LONG . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_EVENT_TYPE_PRICE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_MAX_PLAYERS . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_PUBLIC . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_START . '` time(1) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_END . '` time(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_EVENT_TYPE_ID . '` (`' . BUDATOLL_EVENT_TYPE_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_LOGGER_TABLE . '` (`'
            . BUDATOLL_LOGGER_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_LOGGER_TYPE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_LOGGER_USER . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_LOGGER_RELATED . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_LOGGER_COMMENT . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_LOGGER_TIME . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_LOGGER_ID . '` (`' . BUDATOLL_LOGGER_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_EVENTS_TABLE . '` (`'
            . BUDATOLL_EVENTS_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_EVENTS_SHORT . '` varchar(50) NOT NULL,`'
            . BUDATOLL_EVENTS_LONG . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_EVENTS_DAY . '` DATE NOT NULL ,`'
            . BUDATOLL_EVENTS_PRICE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENTS_EVENT_TYPE_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_EVENTS_MAX_PLAYERS . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_START . '` time(1) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_END . '` time(1) NOT NULL, `'
            . BUDATOLL_EVENTS_DONE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENTS_COMMENT . '` varchar(255) DEFAULT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_EVENTS_ID . '` (`' . BUDATOLL_EVENTS_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_ACCOUNT_TABLE . '` (`'
            . BUDATOLL_ACCOUNT_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_ACCOUNT_AMOUNT . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_ACCOUNT_EVENT_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_ACCOUNT_PLAYER_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_ACCOUNT_COMMENT . '` varchar(255) DEFAULT NULL, `'
            . BUDATOLL_ACCOUNT_TIME . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_ACCOUNT_ID . '` (`' . BUDATOLL_ACCOUNT_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_TRAINING_TABLE . '` (`'
            . BUDATOLL_TRAINING_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_TRAINING_PLAYER_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_TRAINING_EVENT_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_TRAINING_DONE . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_CONFIRMED . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_DELETABLE . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_PAID . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_COMMENT . '` varchar(255) DEFAULT NULL, `'
            . BUDATOLL_TRAINING_CREATED . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_TRAINING_ID . '` (`' . BUDATOLL_TRAINING_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
}

function bt_user_has_role($role) {
    if (is_user_logged_in()) {
        $user = wp_get_current_user();
        return in_array($role, $user->roles);
    } else {
        return false;
    }
}

function bt_get_option($category, $name) {
    $opt_arr = unserialize(get_option($category));
    return $opt_arr[$name] ?? false;
}

function bt_set_option($category, $name, $value) {
    $opt_arr = unserialize(get_option($category));
    $opt_arr[$name] = $value;
    update_option($category, serialize($opt_arr));
}

function bt_set_default_role($user_id) {
    $user = new WP_User($user_id);
    foreach ($user->roles as $role) {
        $user->remove_role($role);
    }
    $user->add_role(BUDATOLL_ROLE_PLAYER);
}

function bt_get_results($sql, $args = false) {
    global $wpdb;
//       error_log('SQL: ' . $sql . ', args: ' . print_r($args, true));
    if ($args) {
        $prep = $wpdb->prepare($sql, $args);
    } else {
        $prep = $sql;
    }
    $res = $wpdb->get_results($prep);
    if (is_null($res) || !empty($wpdb->last_error)) {
        return false;
    } else {
        return $res;
    }
}

function bt_currency($num) {
    return number_format($num, 0, '.', ' ') . ' Ft';
}

function bt_time($sql_format) {
    $timestamp = strtotime($sql_format);
    return $timestamp ? date('H:i', $timestamp) : 'Invalid time';
}

function bt_is_user_active($user_id) {
    if (user_has_role($user_id, BUDATOLL_ROLE_COACH)) {
        return true;
    }
    return get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, true) == 1;
}

function bt_get_all_posts() {
    $ret = '';
    foreach ($_POST as $name => $value) {
        if (is_array($value)) {
            $ret .= print_r($value, true) . '<br>';
        } else {
            $ret .= $name . ' => ' . $value . '<br>';
        }
    }
    return $ret;
}

function bt_init() {
    if (!headers_sent()) {
        ob_start();
    }
}

function user_has_role($user_id, $role_name) {
    $user_meta = get_userdata($user_id);
    $user_roles = $user_meta->roles;
    return in_array($role_name, $user_roles);
}

function bt_rule_options() {
    $ret = '';
    $ret .= '<strong>Edzés lemondható: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_TRAINING_CANCELABLE) ? 'Az edzés megkezdéséig.' : 'Csak 24 órával az edzés megkezdése előtt.';
    $ret .= '<br><strong>Negatív egyenleg esetén: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT) ? 'Csak előző napon lehet edzést foglalni.' : 'Szabadon foglalhatóak az edzések.';
    $ret .= '<br><strong>Email lemondáskor: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_WAITINGLIST_EMAIL) ? 'Értesítést kapnak a várólistán szereplők.' : 'Nincs értesítés.';
    $ret .= '<br><strong>Játékos beállításainak módosításáról: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY) ? 'Értesítést kap a játékos.' : 'Nincs értesítés.';
    $ret .= '<br><strong>A befizetés lekönyveléséről: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT) ? 'Értesítést kap a játékos.' : 'Nincs értesítés.';
    $ret .= '<br><strong>Az egyenleg elfogyásáról: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE) ? 'Értesítést kap a játékos.' : 'Nincs értesítés.';
    $ret .= '<br><strong>Az előző napi jelentkezés: </strong>';
    $ret .= btIsOptionEnabled(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE) ? 'Lemondható a játék kezdetéig.' : 'Általános szabály szerint mondható le.';
    return $ret;
}

function add_remember_me_to_divi_login() {
    ?>
    <script>
        document.addEventListener("DOMContentLoaded", function () {
            var loginForm = document.querySelector(".et_pb_login form");
            if (loginForm) {
                // Create the Remember Me checkbox
                var rememberMeDiv = document.createElement("div");
                rememberMeDiv.style.marginTop = "10px"; // Add some spacing
                rememberMeDiv.innerHTML = `
                    <label style="display: flex; align-items: center;">
                        <input type="checkbox" checked="checked" name="rememberme" value="forever" style="margin-right: 7px;">
                        Remember Me
                    </label>`;

                // Append the checkbox to the form
                loginForm.appendChild(rememberMeDiv);
            }
        });
    </script>
    <?php

}

function custom_remember_me_duration($expirein, $user_id, $remember) {
    if ($remember) {
        return 90 * DAY_IN_SECONDS; // Keep the user logged in for 90 days
    }
    return $expirein;
}

function budatoll_logout() {
    wp_redirect(home_url());
    exit;
}

function bt_get_coaches() {
    $ret = [];
    $selector = [
        'role' => BUDATOLL_ROLE_COACH,
        'number' => -1,
        'fields' => 'all',
        'orderby' => 'display_name',
        'count_total' => true,
    ];
    $coaches = new WP_User_Query($selector);
    foreach ($coaches->get_results() as $coach) {
        $ret[] = $coach->data;
    }
    return $ret;
}

function bt_warn_players() {  // Azoknak az aktív játékosoknak küld levelete, akik érintettek mától az utolsó feltöltött alkalomig
    $players = bt_get_active_players();
    $num = false;
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENTS_DAY . '`>= CURDATE();';
    $events = bt_get_results($sql);
    if (!$events) {  // Valami hiba van
        return false;
    }
    if (empty($events)) { // Nincs ilyen alakalom.
        return 0;
    }
    $event_types = [];
    $public_events = get_public_event_types();
    $is_public = false;
    foreach ($events as $event) {
        $type_id = $event->{BUDATOLL_EVENTS_EVENT_TYPE_ID};
        $event_types[$type_id] = true;
        if (in_array($type_id, $public_events)) { // hav van publikus
            $is_public = true;
        }
    }
    foreach ($players as $player) {
        $user_id = $player->ID ?? -1;
        $user_allowed = false;
        $events_arr = get_user_meta($user_id, BUDATOLL_USER_META_EVENTS, false);
        if ($is_public) { // Ha van az időszakban publikus, akkor nem keresünk tovább
            $user_allowed = true;
        } elseif (!empty($events_arr)) {
            foreach ($events_arr[0] as $event_type_id) {
                if (isset($event_types[$event_type_id])) {
                    $user_allowed = true;
                }
            }
        }
        if ($user_allowed) {
            if (bt_send_custom_email($user_id, BT_CUSTOM_EMAIL_WARN_PLAYERS)) {
                if ($num) {
                    $num++;
                } else {
                    $num = 1;
                }
            }
        }
    }
    return $num;
}
