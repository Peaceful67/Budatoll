<?php

define("BUDATOLL_EMAIL_DESCRIPTION", 'desc');
define("BUDATOLL_EMAIL_SUBJECT", 'subject');
define("BUDATOLL_EMAIL_BODY", 'body');

define("BT_CUSTOM_EMAIL_PAYMENT_CREATE", 'payment_create');
define("BT_CUSTOM_EMAIL_PAYMENT_MODIFY", 'payment_modify');
define("BT_CUSTOM_EMAIL_CHARGE_MODIFY", 'charge_modify');
define("BT_CUSTOM_EMAIL_WARN_NEGATIV", 'warn_negativ');

define("BT_REPLACE_USER_NAME", "#%#");

$bt_default_emails = [
    BT_CUSTOM_EMAIL_PAYMENT_CREATE => [
        BUDATOLL_EMAIL_DESCRIPTION => 'Befizetés lekönyveléséről',
        BUDATOLL_EMAIL_SUBJECT => 'Befizetés lekönyvelése',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>#1# összegű befizetésed az egyenlegeden jóváírásra került.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_PAYMENT_MODIFY => [
        BUDATOLL_EMAIL_DESCRIPTION => 'Befizetés módosításáról',
        BUDATOLL_EMAIL_SUBJECT => 'Befizetés módosítása',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy korábbi befizetésed #1# összegre módosult.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_CHARGE_MODIFY => [
        BUDATOLL_EMAIL_DESCRIPTION => 'Terhelés módosításáról',
        BUDATOLL_EMAIL_SUBJECT => 'Terhelés módosítása',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Egy korábbi terhelésed #1# összegre módosult.<br>Az új egyenleged: #2#',
    ],
    BT_CUSTOM_EMAIL_WARN_NEGATIV => [
        BUDATOLL_EMAIL_DESCRIPTION => 'Negatív egyenlegről',
        BUDATOLL_EMAIL_SUBJECT => 'Negatív egyenleg',
        BUDATOLL_EMAIL_BODY => 'Tisztelt ' . BT_REPLACE_USER_NAME . '!<br><br>Az egyenleged elfogyott. <br>'
        . 'A jelenlegi egyenleged: #1#<br>Kérünk egy befizetéssel töltsd fel.',
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
    if (getPostVariable('email_save') AND isset($bt_default_emails[$selected_custom_email])) {
        $subject = (getPostVariable('bt_ce_subject'));
        $body = (getPostVariable('bt_ce_body'));
        if (strcmp($subject, $email_arr[BUDATOLL_EMAIL_SUBJECT]) != 0 OR strcmp($body, $email_arr[BUDATOLL_EMAIL_BODY]) != 0) {
            update_option(bt_get_ce_option_name($selected_custom_email), [
                BUDATOLL_EMAIL_SUBJECT => $subject,
                BUDATOLL_EMAIL_BODY => $body,
            ]);
            bt_logger(BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY, -1, $selected_custom_email . ' azonosítójú módosítva. Tárgy: ' . $subject . ', Törzs: ' . $body);
        }
    }
    if (getPostVariable('email_default') AND isset($bt_default_emails[$selected_custom_email])) {
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
    echo '<table class="form-table"><tbody>';
    echo $selected_custom_email;
    echo '<tr><th>Email esemény: </th>';
    echo '<td><select name="budatoll_custom_email" id="BudatollSelect" onChange="document.getElementById(\'BudatollForm\').submit();">';
    echo '<option value="choose" ' . selected($selected_custom_email, 'choose', false) . '>Válassz!</option>';
    foreach ($bt_default_emails as $email_id => $email) {
        $email_arr = bt_get_custom_email($email_id);
        echo '<option value="' . esc_attr($email_id) . '" ' . selected($selected_custom_email, $email_id, false) . '>'
        . esc_html($email_arr[BUDATOLL_EMAIL_DESCRIPTION]) . '</option>';
    }
    echo '</select></td></tr>';
    if (isset($bt_default_emails[$selected_custom_email])) {
        $email_arr = bt_get_custom_email($selected_custom_email);
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
    error_log(print_r($params, true));
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
    return $option_arr;
}

function bt_get_ce_option_name($email_id) {
    return 'budatoll_ce_' . $email_id;
}
