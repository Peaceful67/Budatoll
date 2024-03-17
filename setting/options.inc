<?php

define("BUDATOLL_OPTION_EVENT_CANCELABLE", 'event-cancelable');
define("BUDATOLL_OPTION_AUTO_CHARGE", 'auto-charge');
define("BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT", 'negativ-account');
define("BUDATOLL_OPTION_WAITINGLIST_EMAIL", 'waiting-list');
define("BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY", 'email-user-modify');

function budatoll_setting_options() {

    $hidden_var_name = 'bt_save_options';
    if (!current_user_can('manage_options')) {
        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
    }

    if (getPostVariable($hidden_var_name) == 'options') { // az oldal újratöltődik. Valószínüleg mentés történt
        ${BUDATOLL_OPTION_EVENT_CANCELABLE} = btGetCheckbox(BUDATOLL_OPTION_EVENT_CANCELABLE);
        ${BUDATOLL_OPTION_AUTO_CHARGE} = btGetCheckbox(BUDATOLL_OPTION_AUTO_CHARGE);
        ${BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT} = btGetCheckbox(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT);
        ${BUDATOLL_OPTION_WAITINGLIST_EMAIL} = btGetCheckbox(BUDATOLL_OPTION_WAITINGLIST_EMAIL);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY);
    }
    echo '<div class="wrap">';
    echo '<h2>Budatoll beállítások</h2>';
    echo '<form id="BudatollForm" method="post">';
    echo '<input type="hidden" name="' . $hidden_var_name . '" value="options"/>';
    echo '<input type="hidden" name="page" value="options"/>';
    echo '<table class="form-table"><tbody>';

    btPutCheckbox(BUDATOLL_OPTION_EVENT_CANCELABLE, 'Edzés lemondható', 'Az edzés kezdetéig.', 'Csak 24 órával az edzés előtt mondtható le.');
    btPutCheckbox(BUDATOLL_OPTION_AUTO_CHARGE, 'Edzés költsége', 'Automatikusan levonódik.', 'Kézzel kell lekönyvelni.');
    btPutCheckbox(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT, 'Negatív egyenleg esetén', 'Nem lehet edzést foglalni.', 'Szabadon foglalhat edzést.');
    btPutCheckbox(BUDATOLL_OPTION_WAITINGLIST_EMAIL, 'Email lemondáskor', 'Értesítést kapnak a várólistán szereplők.', 'Nincs értesítés.');
    btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY, 'Értesítés módosításról', 'Értesítést kap a módosított játékos.', 'Nincs értesítés.');

    echo '</tbody></table>';
    submit_button();
    echo '</form>';
    echo '</div>';
}

function btIsCheckboxOn($chk_var) {
    if (isset($chk_var) && $chk_var == '1') {
        return true;
    } else {
        return false;
    }
}

function btIsOptionEnabled($var_name) {
    $var = get_option($var_name);
    return btIsCheckboxOn($var);
}

function btGetCheckbox($var_name) {
    $var = getPostVariable($var_name);
    if ($var === false) {
        $var = '0';
    }
    if ($var != get_option($var_name)) {
        update_option($var_name, $var);
        logger(BUDATOLL_LOGGER_TYPE_SETTING_OPTIONS, -1, 'Beállítások, ' . $var_name . ' új érték: ' . $var);
    }
}

function btPutCheckbox($var_name, $title, $msg_on, $msg_off) {
    global $$var_name;
    $$var_name = get_option($var_name);
    echo '<tr><th scope="row"><label for="' . $var_name . '">' . $title . ':</label></th>';
    echo '<td><input type="checkbox" name="' . $var_name . '" id="' . $var_name . '" '
    . (btIsCheckboxOn($$var_name) ? 'checked' : '')
    . ' value="1" onChange="page_changed=true;"> '
    . (btIsCheckboxOn($$var_name) ? $msg_on : $msg_off ) . '</td></tr>';
}
