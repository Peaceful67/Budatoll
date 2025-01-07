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
define("BT_CUSTOM_EMAIL_TRAINING_DELETED", 'training-deleted');
define("BT_CUSTOM_EMAIL_TRAINING_SAVED", 'training-saved');
define("BT_CUSTOM_EMAIL_PLAYER_REGISTERED", 'player-registered');
define("BT_CUSTOM_EMAIL_PLAYER_MODIFIED", 'player-modified');
define("BT_CUSTOM_EMAIL_SELF_REGISTERED_PLAYER", 'self-registered-player');
define("BT_CUSTOM_EMAIL_SELF_REGISTERED_COACH", 'self-registered-coach');

define("BT_CUSTOM_EMAIL_SIGN", 'email-sign');
define("BT_CUSTOM_EMAIL_DEFAULT_SIGN", 'Üdvözlettel:<br>Budatoll csapata');
define("BT_REPLACE_USER_NAME", "#%#");

$bt_default_emails = [
    BT_CUSTOM_EMAIL_PAYMENT_CREATE => [
        BUDATOLL_EMAIL_TITLE => 'Befizetés lekönyveléséről',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor a befizetése a rendszerbe bekerül. '
        . '<p>#1# paraméter a befizetett összeget helyettesíti,</p><p>#2# helyére az új egyenleg fog kerülni.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Befizetés lekönyvelése',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>#1# összegű befizetésed az egyenlegeden jóváírásra került.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_PAYMENT_MODIFY => [
        BUDATOLL_EMAIL_TITLE => 'Befizetés módosításáról',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy korábbi befizetése az egyenlegének tételei szerkesztése során módosult.'
        . '<p>#1# paraméter az új összeget helyettesíti,</p><p>#2# helyére az új egyenleg fog kerülni.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Befizetés módosítása',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy korábbi befizetésed #1# összegre módosult.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_CHARGE_MODIFY => [
        BUDATOLL_EMAIL_TITLE => 'Terhelés módosításáról',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy korábbi terhelése az egyenlegének tételei szerkesztése során módosult.'
        . '<p>#1# paraméter az új összeget helyettesíti,</p><p>#2# helyére az új egyenleg fog kerülni.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Terhelés módosítása',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy korábbi terhelésed #1# összegre módosult.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_WARN_NEGATIV => [
        BUDATOLL_EMAIL_TITLE => 'Negatív egyenlegről',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egyenlege negatívvá válik. <br>Erről csak egy emailt kap.'
        . '<p>#1# helyére az új egyenleg fog kerülni.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Negatív egyenleg',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Az egyenleged elfogyott. <br>'
        . 'A jelenlegi egyenleged: #1#<br>Kérünk egy befizetéssel töltsd fel!',
    ],
    BT_CUSTOM_EMAIL_WAITING => [
        BUDATOLL_EMAIL_TITLE => 'Várólista felszabadult',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy olyan edzésen lett szabad hely, ahova várólistára jelentkezett.<br>'
        . 'Ha többen is vannak a várólistán, mindenki kap erről értesítést.<p>#1# helyére az edzés hosszú neve fog kerülni.</p>'
        . '<p>#2# helyére az edzés dátuma fog kerülni.</p><p>#3# helyére az edzés kezdési ideje fog kerülni.</p><p>#4# helyére az edzés vége fog kerülni.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Várólista felszabadult',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésről egy játékos törölte a jelentkezését, így lehetőséged van a várólistás jelentkezésedet megerősíteni. '
        . '<br>Siess, mert más várólistások is hasonló értesítést kaphattak!.<br>',
    ],
    BT_CUSTOM_EMAIL_TRAINING_DELETED => [
        BUDATOLL_EMAIL_TITLE => 'Edzés törölve',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy edzésre jelentkezését egy edző törölte.'
        . '<p>#1# az edzés hosszú neve fog kerülni.</p>'
        . '<p>#2# az edzés dátuma fog kerülni.</p>'
        . '<p>#3# az edzés kezdési ideje fog kerülni.</p>'
        . '<p>#4# az edzés vége fog kerülni.</p>'
        . '<p>#5# az edző neve, aki törölte a jelentkezést.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Edzés törölve',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésre jelentkezesédet letöröltük.<br>Amennyiben ezzel kapcsolatban kérdésed van, keresd: #5#!',
    ],
    BT_CUSTOM_EMAIL_TRAINING_SAVED => [
        BUDATOLL_EMAIL_TITLE => 'Edzés rögzítve',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy edzésre jelentkezését egy edző felvette.'
        . '<p>#1# az edzés hosszú neve fog kerülni.</p>'
        . '<p>#2# az edzés dátuma fog kerülni.</p>'
        . '<p>#3# az edzés kezdési ideje fog kerülni.</p>'
        . '<p>#4# az edzés vége fog kerülni.</p>'
        . '<p>#5# az edző neve, aki rögzítette a jelentkezést.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Edzés rögzítve',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>A #1#, #2# #3#-#4#'
        . ' edzésre jelentkezesédet rögzítettük.<br>Amennyiben ezzel kapcsolatban kérdésed van, keresd: #5#!',
    ],
    BT_CUSTOM_EMAIL_PLAYER_REGISTERED => [
        BUDATOLL_EMAIL_TITLE => 'Játékos regisztráció',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor egy edző a játékost regisztrálta.'
        . '<p>#1# a játékos felhasználó nevének felel meg.</p>'
        . '<p>#2# a játékos megadott email címét helyettesíti.',
        BUDATOLL_EMAIL_SUBJECT => 'Játékos regisztráció',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br>Üdvözöllek a tollaslabda klubbunk oldalán.<br>'
        . 'A belépéshez tartozó felhasználó neved: #1#<br>'
        . 'Jelszót a megadott #2# email címre tudsz kérni.',
    ],
    BT_CUSTOM_EMAIL_PLAYER_MODIFIED => [
        BUDATOLL_EMAIL_TITLE => 'Adatok módosultak',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt akkor kapja a játékos, amikor az adatit egy edző módosította.<p>#1# helyetesíti a modosítások listáját.</p>',
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
        . 'Egy rövid beszélgetés közben felteheted a kérdéseidet és én is kérdezek pár dolgot, hogy a neked megfelelő információval tudjak szolgálni.',
    ],
    BT_CUSTOM_EMAIL_SELF_REGISTERED_COACH => [
        BUDATOLL_EMAIL_TITLE => 'Ismeretlen játékos regisztrált',
        BUDATOLL_EMAIL_DESCRIPTION => 'Ezt az összes edzőkapja, amikor egy játékos magát regisztrálja az oldalon.'
        . '<p>#1# helyén a játékos felhasználó neve fog megjelenni.</p>',
        BUDATOLL_EMAIL_SUBJECT => 'Új játékos regisztrált',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy új játékos regisztrált a tollasklubban. A felhasználó neve:#1#<br>'
        . 'A vezérlőpulton álltsd be számára a megfelelő játékokat, edzéseket!',
    ],
];

function budatoll_manage_emails() {
    global $bt_default_emails;
    echo '<div class="wrap">';
    echo '<h2>Kimenő emailek szerkesztése</h2>';
    echo '<form id="BudatollForm" method="post">';
    echo '<input type="hidden" name="page" value="emailek"/>';
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
            if (strcmp($subject, $email_arr[BUDATOLL_EMAIL_SUBJECT]) != 0 OR strcmp($body, $email_arr[BUDATOLL_EMAIL_BODY]) != 0) {
                $subject = $bt_default_emails[$selected_custom_email][BUDATOLL_EMAIL_SUBJECT];
                $body = $bt_default_emails[$selected_custom_email][BUDATOLL_EMAIL_BODY];
                update_option(bt_get_ce_option_name($selected_custom_email), [
                    BUDATOLL_EMAIL_SUBJECT => $subject,
                    BUDATOLL_EMAIL_BODY => $body,
                ]);
                bt_logger(BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY, -1, $selected_custom_email . ' azonosítójú visszaállítva. Tárgy: ' . $subject . ', Törzs: ' . $body);
            }
        }
    }
    echo '<table class="form-table"><tbody>';
    $sign = get_option(BT_CUSTOM_EMAIL_SIGN, BT_CUSTOM_EMAIL_DEFAULT_SIGN);
    echo '<tr><th>Email aláírása: </th><td><textarea id="bt_ce_sign" name="bt_ce_sign">' . $sign . '</textarea></td></tr>';
    echo '<tr><th>Email esemény: </th>';
    echo '<td><select name="budatoll_custom_email" id="BudatollSelect" onChange="document.getElementById(\'BudatollForm\').submit();">';
    echo '<option value="choose" ' . selected($selected_custom_email, 'choose', false) . '>Válassz!</option>';
    foreach ($bt_default_emails as $email_id => $email) {
        $email_arr = bt_get_custom_email($email_id);
        echo '<option value="' . esc_attr($email_id) . '" ' . selected($selected_custom_email, $email_id, false) . '>'
        . esc_html($email_arr[BUDATOLL_EMAIL_TITLE]) . '</option>';
    }
    echo '</select></td></tr>';
    if (isset($bt_default_emails[$selected_custom_email])) {
        $email_arr = bt_get_custom_email($selected_custom_email);
        echo '<tr><th>Email leírása: </th><td><p>' . $email_arr[BUDATOLL_EMAIL_DESCRIPTION] . '</p><p>A ' . BT_REPLACE_USER_NAME . ' minden esetben a játékos teljes nevére cserélődik le.</p></td></tr>';
        echo '<tr><th>Email tárgya: </th><td><input type="text" value="' . esc_attr($email_arr[BUDATOLL_EMAIL_SUBJECT]) . '" name="bt_ce_subject"></td></tr>';
        echo '<tr><th>Email törzse: </th><td><textarea  rows="4" cols="80" name="bt_ce_body" id="bt_ce_body">' . esc_textarea($email_arr[BUDATOLL_EMAIL_BODY]) . '</textarea></td></tr>';
    }
    echo '<tr><th>' . get_submit_button('Módosítások mentése', 'primary', 'email_save') . '</th>'
    . '<td>' . get_submit_button('Eredeti visszaállítása', 'primary', 'email_default') . '</td></tr>';
    echo '</tbody></table>';
    echo '</form>';
    echo '</div>';
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
    bt_send_email($user_id, $subject, $final_body);
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
