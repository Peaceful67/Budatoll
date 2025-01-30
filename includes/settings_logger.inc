<?php

define("BUDATOLL_LOGGER_TYPE_EVENT_TYPE", 1);
define("BUDATOLL_LOGGER_TYPE_EVENTS", 2);
define("BUDATOLL_LOGGER_TYPE_SETTING_OPTIONS", 3);
define("BUDATOLL_LOGGER_TYPE_USER_DATA", 4);
define("BUDATOLL_LOGGER_TYPE_PAYMENT", 5);
define("BUDATOLL_LOGGER_TYPE_TRAININGS", 6);
define("BUDATOLL_LOGGER_TYPE_EMAILS", 7);
define("BUDATOLL_LOGGER_TYPE_CHARGE", 8);
define("BUDATOLL_LOGGER_TYPE_LOGIN", 9);
define("BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY", 10);
define("BUDATOLL_LOGGER_TYPE_EVENT_TEMPLATE", 11);

define("BUDATOLL_LOGGER_DATA_DELIMITER", '#&@');

$bt_logger_types = [
    BUDATOLL_LOGGER_TYPE_EVENT_TYPE => 'Edzés típus',
    BUDATOLL_LOGGER_TYPE_EVENTS => 'Alkalom',
    BUDATOLL_LOGGER_TYPE_TRAININGS => 'Edzés',
    BUDATOLL_LOGGER_TYPE_SETTING_OPTIONS => 'Beállítás',
    BUDATOLL_LOGGER_TYPE_USER_DATA => 'Játékos',
    BUDATOLL_LOGGER_TYPE_PAYMENT => 'Befizetés',
    BUDATOLL_LOGGER_TYPE_CHARGE => 'Terhelés',
    BUDATOLL_LOGGER_TYPE_EMAILS => 'Email küldés',
    BUDATOLL_LOGGER_TYPE_LOGIN => 'Belépés',
    BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY => 'Email módosítás',
    BUDATOLL_LOGGER_TYPE_EVENT_TEMPLATE => 'Sablon kezelés',
];
$bt_logger_colors = [
    BUDATOLL_LOGGER_TYPE_EVENT_TYPE => 'lightsteelblue',
    BUDATOLL_LOGGER_TYPE_EVENTS => 'lightgrey',
    BUDATOLL_LOGGER_TYPE_TRAININGS => 'wheat',
    BUDATOLL_LOGGER_TYPE_SETTING_OPTIONS => 'lightgreen',
    BUDATOLL_LOGGER_TYPE_USER_DATA => 'lightpink',
    BUDATOLL_LOGGER_TYPE_PAYMENT => 'lightcoral',
    BUDATOLL_LOGGER_TYPE_CHARGE => 'lightsteelblue',
    BUDATOLL_LOGGER_TYPE_EMAILS => 'lightblue',
    BUDATOLL_LOGGER_TYPE_LOGIN => 'khaki',
    BUDATOLL_LOGGER_TYPE_EMAIL_MODIFY => 'grey',
    BUDATOLL_LOGGER_TYPE_EVENT_TEMPLATE => 'white',
];

function bt_logger_list() {
    if (!current_user_can(BUDATOLL_ACCESS_MANAGE_COACH)) {
        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
    }
    $ret = '';
    $bt_logger_user = getPostVariable('bt_logger_user', -1);
    $bt_logger_related = getPostVariable('bt_logger_related', -1);
    $bt_logger_cat = getPostVariable('bt_logger_cat', -1);
    $bt_logger_to_date = getPostVariable('bt_logger_to_date', wp_date('Y-m-d'));
    $bt_logger_from_date = getPostVariable('bt_logger_from_date', wp_date('Y-m-d', strtotime($bt_logger_to_date . ' -1 month')));
    $ret .= bt_logger_filter($bt_logger_user, $bt_logger_related, $bt_logger_cat, $bt_logger_from_date, $bt_logger_to_date);
    $ret .= bt_logger_display($bt_logger_user, $bt_logger_related, $bt_logger_cat, $bt_logger_from_date, $bt_logger_to_date);
    return $ret;
}

function bt_logger_filter($bt_logger_user, $bt_logger_related, $bt_logger_cat, $bt_logger_from_date, $bt_logger_to_date) {
    global $bt_logger_types;
    $ret = '';
    $all_bt_users = get_users([
        'orderby' => 'username',
        'number' => -1,
        'fields' => 'all',]);
    $ret .= '<h1>Naplók listázása</h1></div>';
    $ret .= '<form id="loggerForm" method="post" action="/naplok">';
    $ret .= '<div class="budatoll-form-group"><label for="bt_logger_user">Elkövető</label>'
            . '<select class="budatoll-dropdown" id="bt_logger_user" name="bt_logger_user" onchange="submit();">';
    $ret .= '<option value="-1" ' . ($bt_logger_user == -1 ? 'selected' : '') . '>Összes</option>';
    foreach ($all_bt_users as $user) {
        $ret .= '<option value="' . $user->ID . '" ' . ($bt_logger_user == $user->ID ? 'selected' : '') . '>' . $user->user_login . '</option>';
    }
    $ret .= '</select></div>';
    $ret .= '<div class="budatoll-form-group"><label for="bt_logger_related">Érintett</label></th><td>'
            . '<select class="budatoll-dropdown" id="bt_logger_related" name="bt_logger_related" onchange="submit();">';
    $ret .= '<option value="-1" ' . ($bt_logger_user == -1 ? 'selected' : '') . '>Összes</option>';
    foreach ($all_bt_users as $user) {
        $ret .= '<option value="' . $user->ID . '" ' . ($bt_logger_related == $user->ID ? 'selected' : '') . '>' . $user->user_login . '</option>';
    }
    $ret .= '</select></div>';
    $ret .= '<div class="budatoll-form-group"><label for="bt_logger_cat">Kategória</label>'
            . '<select id="bt_logger_cat" name="bt_logger_cat" class="budatoll-dropdown" onchange="submit();">';
    $ret .= '<option value="-1" ' . ($bt_logger_cat == -1 ? 'selected' : '') . '>Összes</option>';
    foreach ($bt_logger_types as $type_id => $type_name) {
        $ret .= '<option value="' . $type_id . '" ' . ($bt_logger_cat == $type_id ? 'selected' : '') . '>' . $type_name . '</option>';
    }
    $ret .= '</select></div>';
    $ret .= '<div class="budatoll-form-group"><label for="bt_logger_from_date">Időszak:</label>
      <input type="date" name="bt_logger_from_date" id="bt_logger_from_date" value="' . $bt_logger_from_date . '" onchange="submit();"> 
      <input type="date" name="bt_logger_to_date" id="bt_logger_to_date" value="' . $bt_logger_to_date . '" onchange="submit();"></div>';
    $ret .= '</form>';
    return $ret;
}

function bt_logger_display($bt_logger_user, $bt_logger_related, $bt_logger_cat, $bt_logger_from_date, $bt_logger_to_date) {
    global $wpdb, $bt_logger_types, $bt_logger_colors;
    $ret = '';
    $sql = 'SELECT * FROM `' . BUDATOLL_LOGGER_TABLE . '` WHERE '
            . ($bt_logger_user > 0 ? ('`' . BUDATOLL_LOGGER_USER . '`=' . $bt_logger_user . ' AND ') : '')
            . ($bt_logger_related > 0 ? ('`' . BUDATOLL_LOGGER_RELATED . '`=' . $bt_logger_related . ' AND ') : '')
            . ($bt_logger_cat > 0 ? ('`' . BUDATOLL_LOGGER_TYPE . '`=' . $bt_logger_cat . ' AND ') : '')
            . ' `' . BUDATOLL_LOGGER_TIME . '` BETWEEN "' . $bt_logger_from_date . ' 00:00:00" AND "' . $bt_logger_to_date . ' 23:59:59" '
            . 'ORDER BY `' . BUDATOLL_LOGGER_TIME . '` DESC LIMIT 200;';
    $ret .= '<table class="budatoll-table">';
    $ret .= '<thead><tr><th scope="col" id="column1" class="manage-column column-column1" style="width:70px;">Elkövető</th>'
            . '<th scope="col" id="column2" class="manage-column column-column2" style="width:70px;">Típus</th>'
            . '<th scope="col" id="column3" class="manage-column column-column3"  style="width:70px;">Érintett</th>'
            . '<th scope="col" id="column3" class="manage-column column-column4"  style="width:150px;">Időpont</th>'
            . '<th scope="col" id="column4" class="manage-column column-colum5">Megjegyzés</th>'
            . '</tr></thead><tbody id="logger-list">';
    foreach ($wpdb->get_results($sql) as $row) {
        $user = get_userdata($row->{BUDATOLL_LOGGER_USER});
        $user_name = $user->display_name ?? '---';
        $related_id = $row->{BUDATOLL_LOGGER_RELATED};
        $comment = $row->{BUDATOLL_LOGGER_COMMENT};
        $obj_pos = strpos($comment, BUDATOLL_LOGGER_DATA_DELIMITER);
        if ($obj_pos !== false) { // Van benne kibontandó object
            $obj_str = substr($comment, $obj_pos + strlen(BUDATOLL_LOGGER_DATA_DELIMITER));
            $obj_arr = unserialize($obj_str);
            if ($obj_arr !== false) {
                $comment = substr($comment, 0, $obj_pos) . ', Adatok: ' . arr_to_string($obj_arr, true);
            }
        }
        $related_name = '---';
        switch ($row->{BUDATOLL_LOGGER_TYPE}) {
            case BUDATOLL_LOGGER_TYPE_PAYMENT:
            case BUDATOLL_LOGGER_TYPE_USER_DATA:
            case BUDATOLL_LOGGER_TYPE_TRAININGS:
            case BUDATOLL_LOGGER_TYPE_EMAILS:
            case BUDATOLL_LOGGER_TYPE_CHARGE:
            case BUDATOLL_LOGGER_TYPE_LOGIN:
                $related = (get_userdata($related_id));
                $related_name = $related->display_name ?? '---';
                break;
        }
        $ret .= '<tr style="background:' . $bt_logger_colors[$row->{BUDATOLL_LOGGER_TYPE}] . ';"><td class="column-column1">' . $user_name . '</td>';
        $ret .= '<td class="column-column2">' . $bt_logger_types[$row->{BUDATOLL_LOGGER_TYPE}] . '</td>';
        $ret .= '<td class="column-column3">' . $related_name . '</td>';
        $ret .= '<td class="column-column4">' . $row->{BUDATOLL_LOGGER_TIME} . '</td>';
        $ret .= '<td class="column-column5">' . $comment . '</td></tr>';
    }
    $ret .= '</tbody></table>';
    return $ret;
}

function bt_log_user_login($user_login) {
    $user = get_user_by('login', $user_login);
    $user_id = ($user && $user->exists()) ? $user->ID : -1;
    bt_logger(BUDATOLL_LOGGER_TYPE_LOGIN, $user_id, $user_login . ' belépett');
}

function bt_log_user_logout() {
    $user = wp_get_current_user(); // Get the current logged-in user

    if ($user && $user->exists()) {
        bt_logger(BUDATOLL_LOGGER_TYPE_LOGIN, $user->ID, $user->user_login . ' kilépett');
    } else {
        error_log('Nem volt belépve senki: ' . print_r($user, true));
    }
}

function bt_log_login_failed($username) {
    bt_logger(BUDATOLL_LOGGER_TYPE_LOGIN, -1, sanitize_text_field($username) . ' sikeretelen belépés');
}

function bt_logger($type, $related, $comment, $log_data = null) {
    global $wpdb;
    $user = wp_get_current_user();
    if (!is_null($log_data)) {
        $comment .= BUDATOLL_LOGGER_DATA_DELIMITER . maybe_serialize($log_data);
    }
    $user_id = isset($user->ID) ? absint($user->ID) : -1;
    $data = [
        BUDATOLL_LOGGER_USER => $user_id,
        BUDATOLL_LOGGER_RELATED => $related,
        BUDATOLL_LOGGER_TYPE => $type,
        BUDATOLL_LOGGER_COMMENT => $comment,
    ];
//    error_log('Logger: ' . print_r($data, true) . ', comment:' . $comment);
    $wpdb->insert(BUDATOLL_LOGGER_TABLE, $data, ['%d', '%d', '%d', '%s']);
}

function arr_to_string($arr) {
    $ret = '';
    foreach ($arr as $key => $value) {
        $ret .= $key . ':' . $value . ', ';
    }
    return $ret;
}
