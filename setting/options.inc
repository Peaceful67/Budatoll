<?php

define("BUDATOLL_OPTION_EVENT_CANCELABLE", 'event-cancelable');
define("BUDATOLL_OPTION_AUTO_CHARGE", 'auto-charge');
define("BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT", 'negativ-account');
define("BUDATOLL_OPTION_WAITINGLIST_EMAIL", 'waiting-list');

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
    }
    echo '<div class="wrap">';
    echo '<h2>Budatoll beállítások</h2>';
    echo '<form id="BudatollForm" method="post">';
    echo '<input type="hidden" name="' . $hidden_var_name . '" value="options"/>';
    echo '<input type="hidden" name="page" value="options"/>';
    echo '<table class="form-table"><tbody>';

    ${BUDATOLL_OPTION_EVENT_CANCELABLE} = get_option(BUDATOLL_OPTION_EVENT_CANCELABLE);
    echo '<tr><th scope="row"><label for="' . BUDATOLL_OPTION_EVENT_CANCELABLE . '">Edzés lemondható:</label></th>';
    echo '<td><input type="checkbox" name="' . BUDATOLL_OPTION_EVENT_CANCELABLE . '" id="' . BUDATOLL_OPTION_EVENT_CANCELABLE . '" '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_EVENT_CANCELABLE}) ? 'checked' : '')
    . ' value="1" onChange="page_changed=true;"> '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_EVENT_CANCELABLE}) ? ' az edzés kezdetéig lemondható.' : ' csak 24 órával az edzés előtt mondtható le.') . '</td></tr>';

    ${BUDATOLL_OPTION_AUTO_CHARGE} = get_option(BUDATOLL_OPTION_AUTO_CHARGE);
    echo '<tr><th scope="row"><label for="' . BUDATOLL_OPTION_AUTO_CHARGE . '">Edzés költsége:</label></th>';
    echo '<td><input type="checkbox" name="' . BUDATOLL_OPTION_AUTO_CHARGE . '" id="' . BUDATOLL_OPTION_AUTO_CHARGE . '" '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_AUTO_CHARGE}) ? 'checked' : '')
    . ' value="1" onChange="page_changed=true;"> '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_AUTO_CHARGE}) ? ' automatikusan levonódik.' : ' kézzel kell lekönyvelni.') . '</td></tr>';

    ${BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT} = get_option(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT);
    echo '<tr><th scope="row"><label for="' . BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT . '">Negatív egyenleg esetén:</label></th>';
    echo '<td><input type="checkbox" name="' . BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT . '" id="' . BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT . '" '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT}) ? 'checked' : '')
    . ' value="1" onChange="page_changed=true;"> '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT}) ? ' nem lehet edzést foglalni.' : ' szabadon foglalhat edzést.') . '</td></tr>';

    ${BUDATOLL_OPTION_WAITINGLIST_EMAIL} = get_option(BUDATOLL_OPTION_WAITINGLIST_EMAIL);
    echo '<tr><th scope="row"><label for="' . BUDATOLL_OPTION_WAITINGLIST_EMAIL . '">Email értesítés lemondáskor:</label></th>';
    echo '<td><input type="checkbox" name="' . BUDATOLL_OPTION_WAITINGLIST_EMAIL . '" id="' . BUDATOLL_OPTION_WAITINGLIST_EMAIL . '" '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_WAITINGLIST_EMAIL}) ? 'checked' : '')
    . ' value="1" onChange="page_changed=true;"> '
    . (btIsCheckboxOn(${BUDATOLL_OPTION_WAITINGLIST_EMAIL}) ? 'értesítést kapnak a várólistán szereplők.':' Nincs értesítés.' ) . '</td></tr>';

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
