<?php

define("BUDATOLL_OPTION_EVENT_CANCELABLE", 'event-cancelable');
define("BUDATOLL_OPTION_AUTO_CHARGE", 'auto-charge');
define("BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT", 'negativ-account');
define("BUDATOLL_OPTION_WAITINGLIST_EMAIL", 'waiting-list');
define("BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY", 'email-user-modify');
define("BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT", 'email-user-payment');
define("BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE", 'warning-negativ-balance');
define("BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE", 'short-application-cancelable');

define("BUDATOLL_OPTION_LAST_CRONTAB", 'bt_last_crontab');

define("BUDATOLL_CURRENT_VERSION", '1.0');

define("BUDATOLL_VERSION_ID", 'id');
define("BUDATOLL_VERSION_DATE", 'date');
define("BUDATOLL_VERSION_TITLE", 'title');
define("BUDATOLL_VERSION_DESCRIPTION", 'desc');

function budatoll_setting_options() {
    $bt_versions = [
        '1.0' => [
            BUDATOLL_VERSION_DATE => '2024-12-17',
            BUDATOLL_VERSION_TITLE => 'Első teszt verzió',
            BUDATOLL_VERSION_DESCRIPTION => 'Alapfunckciók elsőre',
        ],
        '1.1' => [
            BUDATOLL_VERSION_DATE => '2025-01-01',
            BUDATOLL_VERSION_TITLE => 'Apróbb javítások',
            BUDATOLL_VERSION_DESCRIPTION => '<ul>'
            . '<li>- Képek cseréje a kezdő oldalon.<li>'
            . '<li>- Az edző a múltat is tudja módosítani, az egyenleg automatikusan módosul.</li>'
            . '<li>- A nem bérletes csak előző nap jelentkezhet egy edzésre.</li>'
            . '<li>- A negatív egyenlegű csak előző nap jelentkezhet egy edzésre.( kapcsolható )</li>'
            . '<li>- Az edzésre jelentkezés egyszerűsítése, az állapot színekkel jelölve.</li>'
            . '<li>- Az egyenleg változásakor küldött emailben szerepel az új egyenleg.</li>'
            . '<li>- Van publikus edzés típus, amit minden aktív játékos lát, nem kell külön beállítani.</li>'
            . '<li>- Ha egy játékos 6 hónapja nem volt egyik edzésen sem, inaktívvá válik.</li>'
            . '<li>- Az olyan játékost, aki még semmilyen edzésen nem volt (fake), ki lehet törölni.</li>'
            . '<li>- Az új játékos kap egy üdvözlő emailt, azzal a kéréssel, hogy vegye fel az edzővel a kapcsolatot.</li>'
            . '<li>- Egy új játékos regisztrációjáról kap email értesítést az összes edző.</li>'
            . '<li>- Mobil és desktop megjelenítés feljavítása.</li>'
            . '</ul>',
        ],
        '1.2' => [
            BUDATOLL_VERSION_DATE => '2025--',
            BUDATOLL_VERSION_TITLE => 'További fejlesztések',
            BUDATOLL_VERSION_DESCRIPTION => '<ul>'
            . '<li>- Alkalom heti beosztása másolása a következő hétre.</li>'
            . '</ul>',
        ],
    ];

    $hidden_var_name = 'bt_save_options';
    if (!current_user_can(BUDATOLL_ACCESS_MANAGE_COACH)) {
        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
    }

    if (getPostVariable($hidden_var_name) == 'options') { // az oldal újratöltődik. Valószínüleg mentés történt
        ${BUDATOLL_OPTION_EVENT_CANCELABLE} = btGetCheckbox(BUDATOLL_OPTION_EVENT_CANCELABLE);
        ${BUDATOLL_OPTION_AUTO_CHARGE} = btGetCheckbox(BUDATOLL_OPTION_AUTO_CHARGE);
        ${BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT} = btGetCheckbox(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT);
        ${BUDATOLL_OPTION_WAITINGLIST_EMAIL} = btGetCheckbox(BUDATOLL_OPTION_WAITINGLIST_EMAIL);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE);
        ${BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE} = btGetCheckbox(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE);
    }
    echo '<div class="wrap">';
    echo '<h2>Budatoll beállítások</h2>';
    echo '<form id="BudatollForm" method="post">';
    echo '<input type="hidden" name="' . $hidden_var_name . '" value="options"/>';
    echo '<input type="hidden" name="page" value="options"/>';
    echo '<table class="form-table"><tbody>';

    btPutCheckbox(BUDATOLL_OPTION_EVENT_CANCELABLE, 'Edzés lemondható', 'Az edzés kezdetéig.', 'Csak 24 órával az edzés előtt mondtható le.');
//    btPutCheckbox(BUDATOLL_OPTION_AUTO_CHARGE, 'Edzés költsége', 'Automatikusan levonódik.', 'Kézzel kell lekönyvelni.');
    btPutCheckbox(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT, 'Negatív egyenleg esetén', 'Csak előző nap foglalhat.', 'Szabadon foglalhat edzést.');
    btPutCheckbox(BUDATOLL_OPTION_WAITINGLIST_EMAIL, 'Email lemondáskor', 'Értesítést kapnak a várólistán szereplők.', 'Nincs értesítés.');
    btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY, 'Játékos módosításáról', 'Értesítést kap a játékos.', 'Nincs értesítés.');
    btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT, 'A befizetés lekönyveléséről', 'Értesítést kap a játékos.', 'Nincs értesítés.');
    btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE, 'Az egyenleg elfogyásáról', 'Értesítést kap a játékos.', 'Nincs értesítés.');
    btPutCheckbox(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE, 'Az előző napi jelentkezés', 'Lemondható a játék kezdetéig.', 'Áltatlános szabály szerint mondható le.');
    echo '<tr><th scope="row"><label>Utolsó crontab:</label></th>';
    echo '<td>' . get_option(BUDATOLL_OPTION_LAST_CRONTAB) . '</td></tr>';
    echo '<tr><th></th><td>' . get_submit_button() . '</td></tr>';
    echo '<tr><th>Aktuális verzió:</th><td>' . BUDATOLL_CURRENT_VERSION . ', legutóbb frissítve: ' . $bt_versions[BUDATOLL_CURRENT_VERSION][BUDATOLL_VERSION_DATE] . '</td></tr>';
    $selected_version = getPostVariable('budatoll_versions', BUDATOLL_CURRENT_VERSION);
    echo '<tr><th>Verziók: </th>';
    echo '<td><select name="budatoll_versions" onChange="document.getElementById(\'submit\').click();">';
    foreach ($bt_versions as $ver_id => $version) {
        echo printf('<option value="%s" %s>%s</option>',
                esc_attr($ver_id),
                selected($selected_version, $ver_id, false),
                esc_html($ver_id . ' : ' . $version[BUDATOLL_VERSION_DATE] . ' - ' . $version[BUDATOLL_VERSION_TITLE]));
    }
    echo '</select></td></tr>';
    echo '<tr><th>Leírás:</th><td>' . $bt_versions[$selected_version][BUDATOLL_VERSION_DESCRIPTION] . '</td></tr>';
    echo '</tbody></table>';
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
        bt_logger(BUDATOLL_LOGGER_TYPE_SETTING_OPTIONS, -1, 'Beállítások, ' . $var_name . ' új érték: ' . $var);
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
