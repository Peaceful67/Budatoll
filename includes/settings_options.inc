<?php

define("BUDATOLL_OPTION_TRAINING_CANCELABLE", 'event-cancelable');
define("BUDATOLL_OPTION_AUTO_CHARGE", 'auto-charge');
define("BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT", 'negativ-account');
define("BUDATOLL_OPTION_WAITINGLIST_EMAIL", 'waiting-list');
define("BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY", 'email-user-modify');
define("BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT", 'email-user-payment');
define("BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE", 'warning-negativ-balance');
define("BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE", 'short-application-cancelable');

define("BUDATOLL_OPTION_LAST_CRONTAB", 'bt_last_crontab');

define("BUDATOLL_CURRENT_VERSION", '1.1');

define("BUDATOLL_VERSION_ID", 'id');
define("BUDATOLL_VERSION_DATE", 'date');
define("BUDATOLL_VERSION_TITLE", 'title');
define("BUDATOLL_VERSION_DESCRIPTION", 'desc');

function budatoll_setting_options() {
    budatoll_crontab(true); // Ezt itt lefutattjuk
    $ret = '';
    $bt_versions = [
        '1.0' => [
            BUDATOLL_VERSION_DATE => '2024-12-17',
            BUDATOLL_VERSION_TITLE => 'Első teszt verzió',
            BUDATOLL_VERSION_DESCRIPTION => '<strong>Alapfunckciók:</strong><br><ul>'
            . '<li>- Két féle szerepkörű felhasználót kezel a rendszer: edző, játékos.</li>'
            . '<li>- Regisztráció után mindenki automatikusan játékos szerepkört kap.</li>'
            . '<li>- Az edzői szerepkört a rendszergazda tud beállítani.</li>'
            . '<li>- Változásokról az érintettek email értesítőt kapnak</li>'
            . '<li>- Minden aktivitás naplózva van két évre visszamenően.</li>'
            . '</ul>'
            . '<dl><strong>Edző szerepkör funkciói:</strong>'
            . '<dt>- Vezérlőpult'
            . '<dd>- Rendszer beállítások</dd>'
            . '<dd>- Műveletek naplóinak listázása</dd>'
            . '<dd>- Edzés sablonok szerekesztése: név, ár, idősáv, max játékos, publikus-e</dd>'
            . '<dd>- Játékosok kezelése: Név, tel.szám, email, választható edézsek, aktív, bérletes</dd>'
            . '</dt>'
            . '<dt>- Edzés alkalmak szerkesztése naptár felületen</dt>'
            . '<dt>- Játékosok edzéseinek áttekintése, szerkesztése naptár felületen</dt>'
            . '<dt>- Játékosok egyenlégeinek áttekintése, szerekesztése táblázatos formában</dt>'
            . '</dl><dl><strong>Játékos szerepkör funkciói:</strong>'
            . '<dt>- Az elérhető edzések áttekintése, jelentkezések kezelése naptár felületen</dt>'
            . '<dt>- Befizetések és terhelések áttekintése táblázatos formában</dt>'
            . '</dl>'
            . '',
        ],
        '1.1' => [
            BUDATOLL_VERSION_DATE => '2025-01-01',
            BUDATOLL_VERSION_TITLE => 'Apróbb javítások',
            BUDATOLL_VERSION_DESCRIPTION => '<ul>'
            . '<li>- Képek cseréje a kezdő oldalon.</li>'
            . '<li>- Az edző a múltat is tudja módosítani, az egyenleg automatikusan módosul.</li>'
            . '<li>- A nem bérletes csak előző nap jelentkezhet egy edzésre.</li>'
            . '<li>- A negatív egyenlegű csak előző nap jelentkezhet egy edzésre.( kapcsolható )</li>'
            . '<li>- Az edzésre jelentkezés egyszerűsítése, az állapot színekkel jelölve: zöld-szabad, kék-jelentkezve, piros-betelt, sárga-várólistán</li>'
            . '<li>- Az egyenleg változásakor küldött emailben szerepel az új egyenleg.</li>'
            . '<li>- Van publikus edzés típus, amit minden aktív játékos lát, nem kell külön beállítani.</li>'
            . '<li>- Ha egy játékos 6 hónapja nem volt egyik edzésen sem, inaktívvá válik.</li>'
            . '<li>- Az olyan játékost, aki még semmilyen edzésen nem volt (fake), ki lehet törölni.</li>'
            . '<li>- Az új játékos kap egy üdvözlő emailt, azzal a kéréssel, hogy vegye fel az edzővel a kapcsolatot.</li>'
            . '<li>- Egy új játékos regisztrációjáról kap email értesítést az összes edző.</li>'
            . '<li>- Mobil és desktop megjelenítés feljavítása.</li>'
            . '<li>- Belépett játékos egyenlege a neve alatt mindig.</li>'
            . '<li>- Az összes kimenő levél szerkeszthetősége.</li>'
            . '<li>- Beállítási odalak áthelyezése a vezérlőpultról a menübe.</li>'
            . '<li>- Edzői naptárnál szinek bevezetése: kék->még van hely, zöld->megtelt, sárga->vannak várólistán</li>'
            . '</ul>',
        ],
        '1.2' => [
            BUDATOLL_VERSION_DATE => '2025--',
            BUDATOLL_VERSION_TITLE => 'További fejlesztések',
            BUDATOLL_VERSION_DESCRIPTION => '<ul>'
            . '<li>- Heti beosztás sablon készítés és másolása.</li>'
            . '<li>- Az edzés időpontok beállítása után értesítő levél a játékosoknak, hogy jelentkezhetnek.</li>'
            . '</ul>',
        ],
    ];

    $hidden_var_name = 'bt_save_options';
    if (!current_user_can(BUDATOLL_ACCESS_MANAGE_COACH)) {
        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
    }

    if (getPostVariable($hidden_var_name) == 'options') { // az oldal újratöltődik. Valószínüleg mentés történt
        ${BUDATOLL_OPTION_TRAINING_CANCELABLE} = btGetCheckbox(BUDATOLL_OPTION_TRAINING_CANCELABLE);
        ${BUDATOLL_OPTION_AUTO_CHARGE} = btGetCheckbox(BUDATOLL_OPTION_AUTO_CHARGE);
        ${BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT} = btGetCheckbox(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT);
        ${BUDATOLL_OPTION_WAITINGLIST_EMAIL} = btGetCheckbox(BUDATOLL_OPTION_WAITINGLIST_EMAIL);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT);
        ${BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE} = btGetCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE);
        ${BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE} = btGetCheckbox(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE);
    }
    $ret .= '<div class="wrap">';
    $ret .= '<h2>Budatoll beállítások</h2>';
    $ret .= '<form id="BudatollForm" method="post" action="/beallitasok">';
    $ret .= '<input type="hidden" name="' . $hidden_var_name . '" value="options"/>';
    $ret .= btPutCheckbox(BUDATOLL_OPTION_TRAINING_CANCELABLE, 'Edzés lemondható', 'Az edzés kezdetéig.', 'Csak 24 órával az edzés előtt mondtható le.');
    $ret .= btPutCheckbox(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT, 'Negatív egyenleg esetén', 'Csak előző nap foglalhat.', 'Szabadon foglalhat edzést.');
    $ret .= btPutCheckbox(BUDATOLL_OPTION_WAITINGLIST_EMAIL, 'Email lemondáskor', 'Értesítést kapnak a várólistán szereplők.', 'Nincs értesítés.');
    $ret .= btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY, 'Játékos módosításáról', 'Értesítést kap a játékos.', 'Nincs értesítés.');
    $ret .= btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT, 'A befizetés lekönyveléséről', 'Értesítést kap a játékos.', 'Nincs értesítés.');
    $ret .= btPutCheckbox(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE, 'Az egyenleg elfogyásáról', 'Értesítést kap a játékos.', 'Nincs értesítés.');
    $ret .= btPutCheckbox(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE, 'Az előző napi jelentkezés', 'Lemondható a játék kezdetéig.', 'Áltatlános szabály szerint mondható le.');
    $ret .= '<div class="budatoll-form-group"><label>Utolsó crontab:</label>';
    $ret .= get_option(BUDATOLL_OPTION_LAST_CRONTAB) . '</div>';
    $ret .= '<div class="budatoll-form-actions"><input type="submit" style="display:none;" id="submit" name="submit" /></div>';
    $ret .= '<div class="budatoll-form-group"><label>Aktuális verzió:</label>' . BUDATOLL_CURRENT_VERSION . ', legutóbb frissítve: ' . $bt_versions[BUDATOLL_CURRENT_VERSION][BUDATOLL_VERSION_DATE] . '</div>';
    $selected_version = getPostVariable('budatoll_versions', BUDATOLL_CURRENT_VERSION);
    $ret .= '<div class="budatoll-form-group"><label>Verziók tartalma: </label>';
    $ret .= '<select name="budatoll_versions" onChange="document.getElementById(\'submit\').click();">';
    foreach ($bt_versions as $ver_id => $version) {
        $ret .= sprintf('<option value="%s" %s>%s</option>',
                esc_attr($ver_id),
                selected($selected_version, $ver_id, false),
                esc_html($ver_id . ' : ' . $version[BUDATOLL_VERSION_DATE] . ' - ' . $version[BUDATOLL_VERSION_TITLE]));
    }
    $ret .= '</select></div>';
    $ret .= '<div class="budatoll-keret"><label>Leírás:</label>' . $bt_versions[$selected_version][BUDATOLL_VERSION_DESCRIPTION] . '</div>';
    $ret .= '</form>';
    $ret .= '</div>';
    return $ret;
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
    return $var;
}

function btPutCheckbox($var_name, $title, $msg_on, $msg_off) {
    global $$var_name;
    $$var_name = get_option($var_name);
    $ret = '<div class="budatoll-form-group"><label for="' . $var_name . '">' . $title . ':</label>';
    $ret .= '<input type="checkbox" name="' . $var_name . '" id="' . $var_name . '" '
            . (btIsCheckboxOn($$var_name) ? 'checked' : '')
            . ' value="1" onChange="document.getElementById(\'submit\').click();"> '
            . (btIsCheckboxOn($$var_name) ? $msg_on : $msg_off ) . '</div>';
    return $ret;
}
