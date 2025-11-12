<?php

define("BUDATOLL_EMAIL_TITLE", 'title');
define("BUDATOLL_EMAIL_DESCRIPTION", 'desc');
define("BUDATOLL_EMAIL_SUBJECT", 'subject');
define("BUDATOLL_EMAIL_BODY", 'body');

define("BT_CUSTOM_EMAIL_PAYMENT_CREATE", 'payment_create');
define("BT_CUSTOM_EMAIL_PAYMENT_MODIFY", 'payment_modify');
define("BT_CUSTOM_EMAIL_CHARGE_MODIFY", 'charge_modify');
define("BT_CUSTOM_EMAIL_WARN_NEGATIV", 'warn_negativ');
define("BT_CUSTOM_EMAIL_WAITING", 'waiting');
define("BT_CUSTOM_EMAIL_CONFIRM_WAITING", 'confirm_waiting');
define("BT_CUSTOM_EMAIL_TRAINING_DELETED", 'training-deleted');
define("BT_CUSTOM_EMAIL_TRAINING_SAVED", 'training-saved');
define("BT_CUSTOM_EMAIL_PLAYER_REGISTERED", 'player-registered');
define("BT_CUSTOM_EMAIL_PLAYER_MODIFIED", 'player-modified');
define("BT_CUSTOM_EMAIL_SELF_REGISTERED_PLAYER", 'self-registered-player');
define("BT_CUSTOM_EMAIL_SELF_REGISTERED_COACH", 'self-registered-coach');
define("BT_CUSTOM_EMAIL_WARN_PLAYERS", 'warn-players');

define("BT_CUSTOM_EMAIL_SIGN", 'email-sign');
define("BT_CUSTOM_EMAIL_DEFAULT_SIGN", 'Üdvözlettel:<br>Budatoll csapata');
define("BT_REPLACE_USER_NAME", "#%#");

$bt_default_emails = [
    BT_CUSTOM_EMAIL_PAYMENT_CREATE => [
        BUDATOLL_EMAIL_TITLE => 'Befizetés lekönyveléséről',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor a befizetése a rendszerbe bekerül.<br> '
        . '#1# paraméter a befizetett összeget helyettesíti,<br>#2# helyére az új egyenleg fog kerülni.',
        BUDATOLL_EMAIL_SUBJECT => 'Befizetés lekönyvelése',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>#1# összegű befizetésed az egyenlegeden jóváírásra került.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_PAYMENT_MODIFY => [
        BUDATOLL_EMAIL_TITLE => 'Befizetés módosításáról',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy korábbi befizetése az egyenlegének tételei szerkesztése során módosult.<br>'
        . '#1# paraméter az új összeget helyettesíti,<br>#2# helyére az új egyenleg fog kerülni.',
        BUDATOLL_EMAIL_SUBJECT => 'Befizetés módosítása',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy korábbi befizetésed #1# összegre módosult.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_CHARGE_MODIFY => [
        BUDATOLL_EMAIL_TITLE => 'Terhelés módosításáról',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy korábbi terhelése az egyenlegének tételei szerkesztése során módosult.<br>'
        . '#1# paraméter az új összeget helyettesíti,<br>#2# helyére az új egyenleg fog kerülni.',
        BUDATOLL_EMAIL_SUBJECT => 'Terhelés módosítása',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy korábbi terhelésed #1# összegre módosult.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_WARN_NEGATIV => [
        BUDATOLL_EMAIL_TITLE => 'Negatív egyenlegről',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egyenlege negatívvá válik. <br>Erről csak egy emailt kap.<br>'
        . '#1# helyére az új egyenleg fog kerülni.',
        BUDATOLL_EMAIL_SUBJECT => 'Negatív egyenleg',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Az egyenleged elfogyott. <br>'
        . 'A jelenlegi egyenleged: #1#<br>Kérünk egy befizetéssel töltsd fel!',
    ],
    BT_CUSTOM_EMAIL_WAITING => [
        BUDATOLL_EMAIL_TITLE => 'Várólista felszabadult',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy olyan edzésen lett szabad hely, ahova várólistára jelentkezett.<br>'
        . 'Ha többen is vannak a várólistán, mindenki kap erről értesítést.<br>#1# helyére az edzés hosszú neve fog kerülni.<br>'
        . '#2# helyére az edzés dátuma fog kerülni.<br>#3# helyére az edzés kezdési ideje fog kerülni.<br>#4# helyére az edzés vége fog kerülni.<br>',
        BUDATOLL_EMAIL_SUBJECT => 'Várólista felszabadult',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésről egy játékos törölte a jelentkezését, így lehetőséged van a várólistás jelentkezésedet megerősíteni. '
        . '<br>Siess, mert más várólistások is hasonló értesítést kaphattak!.<br>',
    ],
     BT_CUSTOM_EMAIL_CONFIRM_WAITING => [
        BUDATOLL_EMAIL_TITLE => 'Várólistáról bekerült az edzésre',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor bekererül a várólistáról az edzésre .<br>'
        . '#1# helyére az edzés hosszú neve fog kerülni.<br>'
        . '#2# helyére az edzés dátuma fog kerülni.<br>#3# helyére az edzés kezdési ideje fog kerülni.<br>#4# helyére az edzés vége fog kerülni.<br>',
        BUDATOLL_EMAIL_SUBJECT => 'Bekerülés várólistáról',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésen felszabadult egy hely, amiről így bekerültél a várólistáról. Várunk az edzésen.<br>',
    ],
    BT_CUSTOM_EMAIL_TRAINING_DELETED => [
        BUDATOLL_EMAIL_TITLE => 'Edzés törölve',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy edzésre jelentkezését egy edző törölte.<br>'
        . '#1# az edzés hosszú neve fog kerülni.<br>'
        . '#2# az edzés dátuma fog kerülni.<br>'
        . '#3# az edzés kezdési ideje fog kerülni.<br>'
        . '#4# az edzés vége fog kerülni.<br>'
        . '#5# az edző neve, aki törölte a jelentkezést.',
        BUDATOLL_EMAIL_SUBJECT => 'Edzés törölve',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésre jelentkezesédet letöröltük.<br>Amennyiben ezzel kapcsolatban kérdésed van, keresd: #5#!',
    ],
    BT_CUSTOM_EMAIL_TRAINING_SAVED => [
        BUDATOLL_EMAIL_TITLE => 'Edzés rögzítve',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy edzésre jelentkezését egy edző felvette.<br>'
        . '#1# az edzés hosszú neve fog kerülni.<br>'
        . '#2# az edzés dátuma fog kerülni.<br>'
        . '#3# az edzés kezdési ideje fog kerülni.<br>'
        . '#4# az edzés vége fog kerülni.<br>'
        . '#5# az edző neve, aki rögzítette a jelentkezést.<br>',
        BUDATOLL_EMAIL_SUBJECT => 'Edzés rögzítve',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésre jelentkezesédet rögzítettük.<br>Amennyiben ezzel kapcsolatban kérdésed van, keresd: #5#!',
    ],
    BT_CUSTOM_EMAIL_PLAYER_REGISTERED => [
        BUDATOLL_EMAIL_TITLE => 'Játékos regisztráció',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy edző a játékost regisztrálta.<br>'
        . '#1# a játékos felhasználó nevének felel meg.<br>'
        . '#2# a játékos megadott email címét helyettesíti.',
        BUDATOLL_EMAIL_SUBJECT => 'Játékos regisztráció',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br>Üdvözöllek a tollaslabda klubbunk oldalán.<br>'
        . 'A belépéshez tartozó felhasználó neved: #1#<br>'
        . 'Jelszót a megadott #2# email címre tudsz kérni.',
    ],
    BT_CUSTOM_EMAIL_PLAYER_MODIFIED => [
        BUDATOLL_EMAIL_TITLE => 'Adatok módosultak',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor az adatit egy edző módosította.<br>#1# helyetesíti a modosítások listáját.',
        BUDATOLL_EMAIL_SUBJECT => 'Adatok módosultak',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Módosultak az adataid a "' . BUDATOLL_COMPANY_NAME . '" honlapján.<br>'
        . '#1#<br>',
    ],
    BT_CUSTOM_EMAIL_SELF_REGISTERED_PLAYER => [
        BUDATOLL_EMAIL_TITLE => 'Kapcsolat felvétel',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor az oldalon regisztráció funckcióval saját magát regisztrálja játékosként.',
        BUDATOLL_EMAIL_SUBJECT => 'Kapcsolat felvétel',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Üdvözöllek tollaslabda klubunkban!<br>'
        . 'Kérlek vedd fel velem telefonon a kapcsolatot az alábbi számon: +36203752539<br>'
        . 'Egy rövid beszélgetés közben felteheted a kérdéseidet és együtt megtaláljuk melyik óratípus passzol hozzád a legjobban.',
    ],
    BT_CUSTOM_EMAIL_SELF_REGISTERED_COACH => [
        BUDATOLL_EMAIL_TITLE => 'Ismeretlen játékos regisztrált',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt az összes edzőkapja, amikor egy játékos magát regisztrálja az oldalon.<br>'
        . '#1# helyén a játékos felhasználó neve fog megjelenni.',
        BUDATOLL_EMAIL_SUBJECT => 'Új játékos regisztrált',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy új játékos regisztrált a tollasklubban. A felhasználó neve:#1#<br>'
        . 'A vezérlőpulton álltsd be számára a megfelelő játékokat, edzéseket!',
    ],
    BT_CUSTOM_EMAIL_WARN_PLAYERS => [
        BUDATOLL_EMAIL_TITLE => 'Felhívás jelentkezésre',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt az összes olyan aktív játékos kapja, aki a mai naptól kezdve bármely beállított alkalomra jelentkezhet.<br>'
        . '#1# helyén a játékos felhasználó neve fog megjelenni.',
        BUDATOLL_EMAIL_SUBJECT => 'Edzésalkalmak feltöltve',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A tollaslabda klubunk honlapján új edzés alkalmak lettek beállítva.<br>'
        . 'Lépj be az oldalra és jelentkezz, amelyikre szeretnél eljönni!',
    ],
];

function budatoll_manage_emails() {
    $ret = '';
    global $bt_default_emails;
    $ret .= '<h2>Kimenő emailek szerkesztése</h2>';
    $ret .= '<form id="BudatollForm" method="POST" action="/emailek">';
    $selected_custom_email = getPostVariable('budatoll_custom_email', 'choose');
    $email_arr = bt_get_custom_email($selected_custom_email);
    if (getPostVariable('email_save')) {
        $sign = getPostVariable('bt_ce_sign', BT_CUSTOM_EMAIL_DEFAULT_SIGN);
        if (strcmp($sign, get_option(BT_CUSTOM_EMAIL_SIGN)) != 0) {
            update_option(BT_CUSTOM_EMAIL_SIGN, $sign);
            bt_logger(BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY, -1, 'Az új aláírás: ' . $sign);
        }
        if (isset($bt_default_emails[$selected_custom_email])) {
            $subject = (getPostVariable('bt_ce_subject', $bt_default_emails[$selected_custom_email][BUDATOLL_EMAIL_SUBJECT]));
            $body = (getPostVariable('bt_ce_body', $bt_default_emails[$selected_custom_email][BUDATOLL_EMAIL_BODY]));
            if (strcmp($subject, $email_arr[BUDATOLL_EMAIL_SUBJECT]) != 0 OR strcmp($body, $email_arr[BUDATOLL_EMAIL_BODY]) != 0) {
                update_option(bt_get_ce_option_name($selected_custom_email), [
                    BUDATOLL_EMAIL_SUBJECT => $subject,
                    BUDATOLL_EMAIL_BODY => $body,
                ]);
                bt_logger(BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY, -1, $selected_custom_email . ' azonosítójú módosítva. Tárgy: ' . $subject . ', Törzs: ' . $body);
            }
        }
    }
    if (getPostVariable('email_default')) {
        if (strcmp(BT_CUSTOM_EMAIL_DEFAULT_SIGN, get_option(BT_CUSTOM_EMAIL_SIGN))) {
            update_option(BT_CUSTOM_EMAIL_SIGN, BT_CUSTOM_EMAIL_DEFAULT_SIGN);
            bt_logger(BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY, -1, 'Az aláírás vissza lett állítva: ' . BT_CUSTOM_EMAIL_DEFAULT_SIGN);
        }
        if (isset($bt_default_emails[$selected_custom_email])) {
            $subject = $bt_default_emails[$selected_custom_email][BUDATOLL_EMAIL_SUBJECT];
            $body = $bt_default_emails[$selected_custom_email][BUDATOLL_EMAIL_BODY];
            if (strcmp($subject, $email_arr[BUDATOLL_EMAIL_SUBJECT]) != 0 OR strcmp($body, $email_arr[BUDATOLL_EMAIL_BODY]) != 0) {
                update_option(bt_get_ce_option_name($selected_custom_email), [
                    BUDATOLL_EMAIL_SUBJECT => $subject,
                    BUDATOLL_EMAIL_BODY => $body,
                ]);
                bt_logger(BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY, -1, $selected_custom_email . ' azonosítójú visszaállítva. Tárgy: ' . $subject . ', Törzs: ' . $body);
            }
        }
    }
    $ret .= '';
    $ret .= '<div class="budatoll-form-group"><label for="budatoll_custom_email">Email esemény: </label>';
    $ret .= '<select name="budatoll_custom_email" id="budatoll_custom_email" onChange="document.getElementById(\'BudatollForm\').submit();">';
    $ret .= '<option value="choose" ' . selected($selected_custom_email, 'choose', false) . '>Válassz!</option>';
    foreach ($bt_default_emails as $email_id => $email) {
        $email_arr = bt_get_custom_email($email_id);
        $ret .= '<option value="' . esc_attr($email_id) . '" ' . selected($selected_custom_email, $email_id, false) . '>'
                . esc_html($email_arr[BUDATOLL_EMAIL_TITLE]) . '</option>';
    }
    $ret .= '</select></div>';
    if (isset($bt_default_emails[$selected_custom_email])) {
        $ret .= '<div class="budatoll-keret">';
        $email_arr = bt_get_custom_email($selected_custom_email);
        $ret .= '<div class="budatoll-form-group"><label>Email leírása: </label><p>'
                . $email_arr[BUDATOLL_EMAIL_DESCRIPTION] . '<br>A ' . BT_REPLACE_USER_NAME . ' minden esetben a játékos teljes nevére cserélődik le.</p></div>';
        $ret .= '<div class="budatoll-form-group"><label for="bt_ce_subject">Email tárgya: </label>'
                . '<input type="text" value="' . esc_attr($email_arr[BUDATOLL_EMAIL_SUBJECT]) . '" name="bt_ce_subject" id="bt_ce_subject"></div>';
        $ret .= '<div class="budatoll-form-group"><label for="bt_ce_body">Email törzse: </label>'
                . '<textarea  rows="4" cols="80" name="bt_ce_body" id="bt_ce_body">' . esc_textarea($email_arr[BUDATOLL_EMAIL_BODY]) . '</textarea></div>';
        $ret .= '</div>';
    }
    $sign = get_option(BT_CUSTOM_EMAIL_SIGN, BT_CUSTOM_EMAIL_DEFAULT_SIGN);
    $ret .= '<div class="budatoll-form-group"><label for="bt_ce_sign">Email aláírása: </label><textarea id="bt_ce_sign" name="bt_ce_sign">' . $sign . '</textarea></div>';
    $ret .= '<div class="budatoll-form-group">' . bt_text_button('email_save', 'Módosítások mentése') . bt_text_button('email_default', 'Eredeti visszaállítása') . '</div>';
    $ret .= '</form>';
    return $ret;
}

function bt_send_custom_email($user_id, $email_id, $params = []) {
    $email_arr = bt_get_custom_email($email_id);
    $subject = $email_arr[BUDATOLL_EMAIL_SUBJECT];
    $body = $email_arr[BUDATOLL_EMAIL_BODY];
    $formated_body = str_replace('\n', '<br>', $body);
    $final_body = str_replace(BT_REPLACE_USER_NAME, bt_get_full_name($user_id), $formated_body);
    if (!empty($params)) {
        foreach ($params as $key => $value) {
            $placeholder = '#' . ($key + 1) . '#'; // Generate placeholder like #1#, #2#
            $final_body = str_replace($placeholder, $value, $final_body);
        }
    }
    return bt_send_email($user_id, $subject, $final_body);
}

function bt_get_custom_email($email_id) {
    global $bt_default_emails;
    if (!isset($bt_default_emails[$email_id])) {
        return false;
    }
    $option_arr = get_option(bt_get_ce_option_name($email_id), $bt_default_emails[$email_id]);
    $option_arr[BUDATOLL_EMAIL_DESCRIPTION] = $bt_default_emails[$email_id][BUDATOLL_EMAIL_DESCRIPTION];
    $option_arr[BUDATOLL_EMAIL_TITLE] = $bt_default_emails[$email_id][BUDATOLL_EMAIL_TITLE];
    return $option_arr;
}

function bt_get_ce_option_name($email_id) {
    return 'budatoll_ce_' . $email_id;
}
