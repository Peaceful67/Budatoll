<?php

define("BUDATOLL_PLAYERS_FIELD_EVENTS", 'players_events_assignment');
define("BUDATOLL_USER_META_LEASED", 'leased');
define("BUDATOLL_USER_META_LOGIN", 'login-name');
define("BUDATOLL_USER_META_FULLANME", 'display-name');
define("BUDATOLL_USER_META_EVENTS", 'events');
define("BUDATOLL_USER_META_PHONE", 'phone');
define("BUDATOLL_USER_META_ACTIVE", 'active');
define("BUDATOLL_USER_META_EMAIL", 'user_email');

function budatoll_manage_players() {
    if (!current_user_can(BUDATOLL_ACCESS_MANAGE_COACH)) {
        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
    }
    $ret = '';
//   echo 'Post: ' . print_r($_POST, true);
    $edit_id = getPostVariable('budatoll_player', -1);
    if (getPostVariable('budatoll_cancel', false)) {
        $edit_id = -1;
    }
    $delete = getPostVariable('budatoll_delete', false);
    if ($delete) {
        wp_delete_user($delete);
    }
    $save = getPostVariable('budatoll_save', false);
    $ret .= '<div class="wrap">';
    $ret .= '<form id="BudatollForm" method="POST" action="/jatekosok" >';
    $ret .= '<h2>Játékosok listája.</h2>';
    if ($save) {
        if (budatoll_manage_players_save($edit_id)) {
            $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-message">Sikeres</div>';
        } else {
            $ret .= '<div class="budatoll-message budatoll-error" id="error-message">Mentés sikertelen</div>';
        }
        $edit_id = -1;
    }
    $ret .= '<input type="hidden" name="budatoll_player" id="budatoll_edit" value="' . $edit_id . '">';
    $new_player = getPostVariable('new_player');
    if ($edit_id < 0 AND !$new_player) {
        $ret .= bt_add_button(-1, 'new_player', "Új játékos");
    }
    if ($new_player) {
        $ret .= budatoll_manage_players_edit(-1);
    }
    if ($edit_id >= 0) {
        $ret .= budatoll_manage_players_edit($edit_id);
    }
    $ret .= budatoll_manage_players_list();
    $ret .= '</form></div>';
    return $ret;
}

function budatoll_manage_players_edit($user_id) {
    $ret = '';
    if ($user_id < 0) {  // Új játékos veszünk fel
        $phone = '';
        $leased = 1;
        $active = 1;
        $events = [];
        $user_login = '';
        $display_name = '';
        $user_email = '';
    } else { //Szerkesztünk egy meglévőt
        $user = get_userdata($user_id);
        $phone = get_user_meta($user_id, BUDATOLL_USER_META_PHONE, true);
        $leased = get_user_meta($user_id, BUDATOLL_USER_META_LEASED, true);
        $active = get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, true);
        $events = get_user_meta($user_id, BUDATOLL_USER_META_EVENTS, false);
        if (!$events OR $events == '') {
            $events = [];
        }
        if (isset($events[0]) AND is_array($events[0])) {
            $events = $events[0];
        }
        $user_login = $user->user_login;
        $display_name = $user->display_name;
        $user_email = $user->user_email;
    }
    $ret .= '<div class="budatoll-keret">';
    $ret .= '<div class="budatoll-form-group"><label>Felhasználó név:</label>';
    $ret .= (($user_id > 0) ? $user_login : '<input type = "text" id = "budatoll-user-login" name = "budatoll-user-login" value = ""/>') . '</div>';
    $ret .= '<div class="budatoll-form-group"><label for="budatoll-display-name">Teljes név:</label>'
            . '<input type = "text" id = "budatoll-display-name" name = "budatoll-display-name" value = "' . $display_name . '"/></div>';
    $ret .= '<div class="budatoll-form-group"><label for="budatoll-user-email">Email cím:</label>'
            . '<input type = "email" id = "budatoll-user-email" name = "budatoll-user-email" value = "' . $user_email . '"/></div>';
    $ret .= '<div class="budatoll-form-group"><label for = "budatoll-tel">Telefon:</label>'
            . '<input type = "tel" id = "budatoll-tel" name = "budatoll-tel" value = "' . $phone . '"/></div>';
    $ret .= '<div class="budatoll-form-group"><label for = "budatoll-active">Aktív:</label>'
            . '<input type = "checkbox" id = "budatoll-active" name = "budatoll-active" value = "1"'
            . ($active == '1' ? 'checked' : '') . '></div>';
    $ret .= '<div class="budatoll-form-group"><label for = "budatoll-leased">Bérletes:</label>'
            . '<input type = "checkbox" id = "budatoll-leased" name = "budatoll-leased" value = "1"'
            . ($leased == '1' ? 'checked' : '') . '></div>';
    $ret .= '<hr><div class = "budatoll-form-group">';
    $ret .= '<label>Edzések:</label><div class="budatoll-column">';
    $first_row = true;
    foreach (btGetEventTypes() as $event_type) {
        /*
          if ($first_row) {
          $first_row = false;
          } else {
          $ret .= '<tr><th></th>';
          }
         * 
         */
        $event_type_id = $event_type->{BUDATOLL_EVENT_TYPE_ID};
        $checked = in_array($event_type_id, $events) ? 'checked' : '';
        $ret .= '<p><input type = "checkbox" name = "event_types_chk[]"  value = "' . $event_type_id
                . '" ' . $checked . '>' . $event_type->{BUDATOLL_EVENT_TYPE_LONG} . '</p>';
    }
    $ret .= '</div></div>';
    $ret .= '<div class="budatoll-form-group">' . bt_save_button(-1) . bt_cancel_button() . '</div>';
    $ret .= '</div>';
    return $ret;
}

function budatoll_manage_players_save($user_id) {
    $message_body = '';
    if ($user_id < 0) {
        $user_login = sanitize_user(getPostVariable('budatoll-user-login', ''));
        $display_name = getPostVariable('budatoll-display-name', '');
        $user_email = getPostVariable('budatoll-user-email', '');
        if (strlen($user_login) >= 3 AND is_email($user_email)) {
            $user_id = wp_insert_user([
                'user_login' => iconv('UTF-8', 'ASCII', $user_login),
                'display_name' => $display_name,
                'user_email' => $user_email,
                'role' => 'player',
            ]);
            if ($user_id > 0 AND !is_wp_error($user_id)) {
                bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, $user_login . ' nevű játékos létrehozva');
                if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY)) {
                    bt_send_custom_email($user_id, BT_CUSTOM_EMAIL_PLAYER_REGISTERED, [$user_login, $user_email]);
                }
            } else {
                echo '<p style = "color:red;">' . $user_id->get_error_message() . '</p>';
                return false;
            }
        } else {
            echo '<p style = "color:red;">Túl rövid felhasználó név vagy helytelen email cím!</p>';
            return false;
        }
        return true;
    }
    $user = get_userdata($user_id);

    $display_name = getPostVariable('budatoll-display-name', 'dummy');
    if ($display_name != $user->display_name) {
        if (wp_update_user(['ID' => $user_id, 'display_name' => $display_name])) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Az új teljes név' . $display_name);
            $message_body .= 'Az új adat a teljes név: ' . $display_name . '<br>';
        }
    }

    $leased = getPostVariable('budatoll-leased', '0');
    if ($leased != get_user_meta($user_id, BUDATOLL_USER_META_LEASED, true)) {
        if (update_user_meta($user_id, BUDATOLL_USER_META_LEASED, $leased)) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, ($leased == 1 ? 'Bérletes lett' : 'Már nem bérletes'));
            $message_body .= 'Az új adat: ' . ($leased == 1 ? 'bérletes lettél.' : 'már nem vagy bérletes.') . '<br>';
        }
    }
    $active = getPostVariable('budatoll-active', '0');
    if ($active != get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, true)) {
        if (update_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, $active)) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, ($active == 1 ? 'Aktív lett' : 'Inaktív lett'));
            $message_body .= 'Az új adat: ' . ($active == 1 ? 'aktív státuszú lettél.' : 'már nem vagy aktív státuszú.') . '<br>';
        }
    }
    $phone = getPostVariable('budatoll-tel');
    if ($phone != get_user_meta($user_id, BUDATOLL_USER_META_PHONE, true)) {
        if (update_user_meta($user_id, BUDATOLL_USER_META_PHONE, $phone)) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Az új telefonszám' . $phone);
            $message_body .= 'Az új adat a telefonszámod: ' . $phone . '<br>';
        }
    }
    $email = sanitize_email(getPostVariable('budatoll-user-email'));

    if (is_email($email) AND $email != $user->user_email) {
        if (wp_update_user(['ID' => $user_id, 'user_email' => $email])) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Az új email cím: ' . $email);
            $message_body .= 'Az új adat az email címed: ' . $email . '<br>';
        }
    }
    $events_new = getPostVariable('event_types_chk', false);
    if (!$events_new) {
        $events_new = [];
    }
    $events_old = get_user_meta($user_id, BUDATOLL_USER_META_EVENTS, false);
    if (!$events_old OR $events_old == '') {
        $events_old = [];
    }
    if (isset($events_old[0]) AND is_array($events_old[0])) {
        $events_old = $events_old[0];
    }
    $event_type_changed = false;
    foreach (btGetEventTypes() as $event_type) {
        $event_type_id = $event_type->{BUDATOLL_EVENT_TYPE_ID};
        $event_type_name = $event_type->{BUDATOLL_EVENT_TYPE_LONG};
        if (in_array($event_type_id, $events_new) AND !in_array($event_type_id, $events_old)) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Felvett edzés típus: ' . $event_type_name);
            $message_body .= 'Új edzés típusra jelentkezhetsz: ' . $event_type_name . '<br>';

            $event_type_changed = true;
        } elseif (!in_array($event_type_id, $events_new) AND in_array($event_type_id, $events_old)) {
            bt_logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Kitörölt edzés típus: ' . $event_type_name);
            $message_body .= 'Ezután már nem jelentkezhetsz erre az edzés típusra: ' . $event_type_name . '<br>';
            $event_type_changed = true;
        }
        if ($event_type_changed) {
            update_user_meta($user_id, BUDATOLL_USER_META_EVENTS, $events_new);
        }
    }
    if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY) AND !empty($message_body)) {
        $user = get_userdata($user_id);
        bt_send_custom_email($user_id, BT_CUSTOM_EMAIL_PLAYER_MODIFIED, [$message_body]);
    }
    return true;
}

function budatoll_manage_players_list() {

    $users = bt_get_active_players(false);
    $ret = '';
    $ret .= '<table class = "budatoll-table budatoll-table-strippe budatoll-table-wrapper">';
    $ret .= '<thead><tr>'
            . '<th scope = "col" id = "column1" class = "manage-column column-column1" style = "width:120px;">Felhasználónév</th>'
            . '<th scope = "col" id = "column2" class = "manage-column column-column2" >Teljes név</th>'
            . '<th scope = "col" id = "column3" class = "manage-column column-column3" >Email cím</th>'
            . '<th scope = "col" id = "column4" class = "manage-column column-column4" >Telefonszám</th>'
            . '<th scope = "col" id = "column5" class = "manage-column column-column5" >Választható edzések</th>'
            . '<th scope = "col" id = "column6" class = "manage-column column-column6" style = "width:50px;">Aktív</th>'
            . '<th scope = "col" id = "column7" class = "manage-column column-column6" style = "width:50px;">Bérletes</th>'
            . '<th scope = "col" id = "column8" class = "manage-column column-column7" style = "width:100px;">Művelet</th>'
            . '</tr></thead><tbody>';
    foreach ($users as $user) {
        $user_id = $user->ID;
        $phone = get_user_meta($user_id, BUDATOLL_USER_META_PHONE, true);
        $leased = get_user_meta($user_id, BUDATOLL_USER_META_LEASED, true);
        $active = get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, true);
        $events_arr = get_user_meta($user_id, BUDATOLL_USER_META_EVENTS, false);
        if (empty($events_arr)) {
            $events = 'Egyik sem';
        } else {
            $events = '';
            foreach ($events_arr[0] as $event_type_id) {
                $event_type = get_event_type_by_id($event_type_id);
                if (!empty($event_type)) {
                    $events .= $event_type->{BUDATOLL_EVENT_TYPE_LONG} . ', ';
                }
            }
            $events = substr($events, 0, -2);
        }
//     $events = print_r($events_arr, true);
        $ret .= '<tr>';
        $ret .= '<td class = "column-column1">' . $user->user_login . '</td>';
        $ret .= '<td class = "column-column2">' . $user->display_name . '</td>';
        $ret .= '<td class = "column-column3">' . $user->user_email . '</td>';
        $ret .= '<td class = "column-column4">' . $phone . '</td>';
        $ret .= '<td class = "column-column5">' . $events . '</td>';
        $ret .= '<td class = "column-column6"><span class = "dashicons dashicons-' . ($active == '1' ? 'yes' : 'no') . '"></span></td>';
        $ret .= '<td class = "column-column7"><span class = "dashicons dashicons-' . ($leased == '1' ? 'yes' : 'no') . '"></span></td>';
        $ret .= '<td class = "column-column8">' . bt_edit_button($user_id, 'budatoll_player');
        if (is_player_deletable($user_id)) {
            $ret .= bt_delete_button($user_id);
        }
        $ret .= '</td></tr>';
    }
    $ret .= '</tbody></table>';
    return $ret;
}

function isUserActivePlayer($user_id) {
    return 1 == get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE);
}

function put_select_player($var_name, $player_id = 0, $submitOnChange = true, $unselected_txt = 'Összes') {

    $ret = '<select class = "budatoll-dropdown" id = "select_' . $var_name . '" name = "select_' . $var_name . '" ' . ($submitOnChange ? 'onChange = "submit();" ' : '') . '>';
    $ret .= '<option value = "-1" ' . (($player_id == -1) ? 'selected' : '') . '>' . $unselected_txt . '</option>';
    foreach (bt_get_active_players() as $player) {
        $pl_id = $player->ID;
        $ret .= '<option value = "' . $pl_id . '" ' . (($player_id == $pl_id) ? 'selected' : '') . '>' . $player->display_name . '</option>';
    }
    $ret .= '</select>';
    return $ret;
}

function bt_get_active_players($only_active = true) {
    $ret = [];
    $selector = [
        'role' => BUDATOLL_ROLE_PLAYER,
        'number' => -1,
        'fields' => 'all',
        'orderby' => 'display_name',
        'count_total' => true,
    ];
    if ($only_active) {
        $selector['meta_key'] = BUDATOLL_USER_META_ACTIVE;
        $selector['meta_value'] = 1;
    }
    $active_players = new WP_User_Query($selector);
    foreach ($active_players->get_results() as $player) {
        $ret[] = $player->data;
    }
    return $ret;
}

function bt_get_user_name($user_id) {
    $user = get_userdata($user_id);
    if (empty($user)) {
        return 'Nem létező felhasználó';
    } else {
        return $user->user_login;
    }
}

function bt_get_full_name($user_id) {
    $user = get_userdata($user_id);
    if (empty($user)) {
        return 'Nem létező felhasználó';
    } else {
        return $user->display_name;
    }
}

function bt_user_register($user_id) {
    bt_send_custom_email($user_id, BT_CUSTOM_EMAIL_SELF_REGISTERED_PLAYER);
    bt_logger(BUDATOLL_LOGGER_TYPE_LOGIN, $user_id, 'Játékos regisztrált a rendszerben. ID: ' . $user_id);
    $coaches = bt_get_coaches();
    if (!empty($coaches)) {
        foreach ($coaches as $coach) {
            bt_send_custom_email($coach->ID, BT_CUSTOM_EMAIL_SELF_REGISTERED_COACH, [bt_get_user_name($user_id),]);
        }
    }
}

function bt_user_delete($user_id) {
    bt_logger(BUDATOLL_LOGGER_TYPE_LOGIN, $user_id, 'Játékos törölve a rendszerből. ID: ' . $user_id);
}

function is_player_deletable($player_id) {
    global $wpdb;
    $ret = true;
    $wpdb->query('SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_PLAYER_ID . '`=' . $player_id);
    if ($wpdb->num_rows > 0) {
        $ret = false;
    }
    $wpdb->query('SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=' . $player_id);
    if ($wpdb->num_rows > 0) {
        $ret = false;
    }
    return $ret;
}
