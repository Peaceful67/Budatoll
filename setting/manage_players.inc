<?php

define("BUDATOLL_PLAYERS_FIELD_EVENTS", 'players_events_assignment');
define("BUDATOLL_USER_META_LEASED", 'leased');
define("BUDATOLL_USER_META_EVENTS", 'events');
define("BUDATOLL_USER_META_PHONE", 'phone');
define("BUDATOLL_USER_META_ACTIVE", 'active');

function budatoll_manage_players() {
    if (!current_user_can('manage_options')) {
        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
    }
    //   echo 'Post: ' . print_r($_POST, true);
    $edit_id = getPostVariable('budatoll_player', -1);
    if (getPostVariable('budatoll_cancel', false)) {
        $edit_id = -1;
    }
    $save = getPostVariable('budatoll_save', false);
    echo '<div class="wrap">';
    echo '<form id="BudatollForm" method="post">';
    echo '<h2>Játékosok listája.</h2>';
    if ($save) {
        if (budatoll_manage_players_save($edit_id)) {
            echo '<div class="budatoll-message budatoll-success" id="budatoll-message">Sikeres</div>';
        } else {
            echo '<div class="budatoll-message budatoll-error" id="error-message">Mentés sikertelen</div>';
        }
        $edit_id = -1;
    }
    echo '<input type="hidden" name="budatoll_player" id="budatoll_edit" value="' . $edit_id . '">';
    $new_player = getPostVariable('new_player');
    if ($edit_id < 0 AND !$new_player) {
        submit_button("Új játékos", 'primary', 'new_player');
    }
    if ($new_player) {
        echo budatoll_manage_players_edit(-1);
    }
    if ($edit_id >= 0) {
        echo budatoll_manage_players_edit($edit_id);
    }
    echo budatoll_manage_players_list();
    echo '</form></div>';
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
    $ret .= '<table class="form-table"><tbody>';
    $ret .= '<tr><th scope="row"><label>Felhasználó név:</label></th>'
            . '<td><input type="text" id="budatoll-user-login" name="budatoll-user-login" value="' . $user_login . '"/></td></tr>';
    $ret .= '<tr><th scope="row"><label>Teljes név:</label></th>'
            . '<td><input type="text" id="budatoll-display-name" name="budatoll-display-name" value="' . $display_name . '"/></td></tr>';
    $ret .= '<tr><th scope="row"><label>Email cím:</label></th>'
            . '<td><input type="email" id="budatoll-user-email" name="budatoll-user-email" value="' . $user_email . '"/></td></tr>';
    $ret .= '<tr><th scope="row"><label for="budatoll-tel">Telefon:</th>'
            . '<td><input type="tel" id="budatoll-tel" name="budatoll-tel" value="' . $phone . '"/></td></tr>';
    $ret .= '<tr><th scope="row"><label for="budatoll-active">Aktív:</label></th>'
            . '<td><input type="checkbox" id="budatoll-active" name="budatoll-active" value="1"'
            . ($active == '1' ? 'checked' : '') . '></td></tr>';
    $ret .= '<tr><th scope="row"><label for="budatoll-leased">Bérletes:</label></th>'
            . '<td><input type="checkbox" id="budatoll-leased" name="budatoll-leased" value="1"'
            . ($leased == '1' ? 'checked' : '') . '></td></tr>';
    $ret .= '</tbody><tbody class="budatoll-options-group">';
    $ret .= '<tr><th scope="row"><label>Választható edzések:</label></th>';
    $first_row = true;
    foreach (btGetEventTypes() as $event_type) {
        if ($first_row) {
            $first_row = false;
        } else {
            $ret .= '<tr><th></th>';
        }
        $event_type_id = $event_type->{BUDATOLL_EVENT_TYPE_ID};
        $checked = in_array($event_type_id, $events) ? 'checked' : '';
        $ret .= '<td><input type="checkbox" name="event_types_chk[]" value="' . $event_type_id . '" ' . $checked . '>' . $event_type->{BUDATOLL_EVENT_TYPE_LONG} . '</td>';
    }
    $ret .= '</td></tr>';
    $ret .= '</tbody></table>';
    $ret .= get_submit_button('Mentés', 'primary small', 'budatoll_save', false, ['style' => 'margin: 10px;']);
    $ret .= get_submit_button('Mégsem', 'primary small', 'budatoll_cancel', false, ['style' => 'margin: 10px;']);
    $ret .= '</div></div><hr>';
    return $ret;
}

function budatoll_manage_players_save($user_id) {
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
                logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, $user_login . ' nevű játékos létrehozva');
            } else {
                echo 'Nem sikerült 1';
                return false;
            }
        } else {
            echo 'Nem sikerült 2';
            return false;
        }
    }
    $leased = getPostVariable('budatoll-leased', '0');
    if ($leased != get_user_meta($user_id, BUDATOLL_USER_META_LEASED, true)) {
        if (update_user_meta($user_id, BUDATOLL_USER_META_LEASED, $leased)) {
            logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, ($leased == 1 ? 'Bérletes lett' : 'Már nem bérletes'));
        }
    }
    $active = getPostVariable('budatoll-active', '0');
    if ($active != get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, true)) {
        if (update_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, $active)) {
            logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, ($active == 1 ? 'Aktív lett' : 'Inaktív lett'));
        }
    }
    $phone = getPostVariable('budatoll-tel');
    if ($phone != get_user_meta($user_id, BUDATOLL_USER_META_PHONE, true)) {
        if (update_user_meta($user_id, BUDATOLL_USER_META_PHONE, $phone)) {
            logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Az új telefonszám' . $phone);
        }
    }
    $events_new = getPostVariable('event_types_chk', []);
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
            logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Felvett edzés típus: ' . $event_type_name);
            $event_type_changed = true;
        } elseif (!in_array($event_type_id, $events_new) AND in_array($event_type_id, $events_old)) {
            logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $user_id, 'Kitörölt edzés típus: ' . $event_type_name);
            $event_type_changed = true;
        }
    }
    if ($event_type_changed) {
        update_user_meta($user_id, BUDATOLL_USER_META_EVENTS, $events_new);
    }
    return true;
}

function budatoll_manage_players_list() {
    $users = get_users([
        'role' => BUDATOLL_ROLE_PLAYER,
        'number' => -1,
        'fields' => 'all',
        'orderby' => 'display_name',
    ]);
    $ret .= '<div class="wrap">';
    $ret .= '<table class="wp-list-table widefat fixed striped">';
    $ret .= '<thead><tr>'
            . '<th scope="col" id="column1" class="manage-column column-column1" style="width:120px;">Felhasználónév</th>'
            . '<th scope="col" id="column2" class="manage-column column-column2" >Teljes név</th>'
            . '<th scope="col" id="column3" class="manage-column column-column3" >Email cím</th>'
            . '<th scope="col" id="column4" class="manage-column column-column4" >Telefonszám</th>'
            . '<th scope="col" id="column5" class="manage-column column-column5" >Választható edzések</th>'
            . '<th scope="col" id="column6" class="manage-column column-column6" style="width:50px;">Aktív</th>'
            . '<th scope="col" id="column7" class="manage-column column-column6" style="width:50px;">Bérletes</th>'
            . '<th scope="col" id="column8" class="manage-column column-column7" style="width:100px;">Művelet</th>'
            . '</tr></thead><tbody>';
    foreach ($users as $user) {
        $user_id = $user->ID;
        $phone = get_user_meta($user_id, BUDATOLL_USER_META_PHONE, true);
        $leased = get_user_meta($user_id, BUDATOLL_USER_META_LEASED, true);
        $active = get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE, true);
        $events_arr = get_user_meta($user_id, BUDATOLL_USER_META_EVENTS, false);
        if (empty($events_arr)) {
            $events = 'Inaktív';
        } else {
            $events = '';
            foreach ($events_arr[0] as $event_type_id) {
                $events .= btGetEventTypeById($event_type_id)->{BUDATOLL_EVENT_TYPE_LONG} . ', ';
            }
            $events = substr($events, 0, -2);
        }
        //     $events = print_r($events_arr, true);
        $ret .= '<tr>';
        $ret .= '<td class="column-column1">' . $user->user_login . '</td>';
        $ret .= '<td class="column-column2">' . $user->display_name . '</td>';
        $ret .= '<td class="column-column3">' . $user->user_email . '</td>';
        $ret .= '<td class="column-column4">' . $phone . '</td>';
        $ret .= '<td class="column-column5">' . $events . '</td>';
        $ret .= '<td class="column-column6"><span class="dashicons dashicons-' . ($active == '1' ? 'yes' : 'no' ) . '"></span></td>';
        $ret .= '<td class="column-column7"><span class="dashicons dashicons-' . ($leased == '1' ? 'yes' : 'no' ) . '"></span></td>';
        $ret .= '<td class="column-column8"><button class="button button-link button-small" name="budatoll_player" id="budatoll_player" value="'
                . $user_id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button></td>';
        $ret .= '</tr>';
    }
    $ret .= '</tbody></table></div>';
    return $ret;
}

function isUserActivePlayer($user_id) {
    return 1 == get_user_meta($user_id, BUDATOLL_USER_META_ACTIVE);
}

function put_select_player($var_name, $player_id = 0, $submitOnChange = true, $unselected_txt = 'Összes') {

    $ret = '<select class="budatoll-dropdown" id="select_' . $var_name . '" name="select_' . $var_name . '" ' . ($submitOnChange ? 'onChange="submit();" ' : '') . '>';
    $ret .= '<option value="-1" ' . (($player_id == -1) ? 'selected' : '') . '>' . $unselected_txt . '</option>';
    foreach (get_active_players() as $player) {
        $pl_id = $player->ID;
        $ret .= '<option value="' . $pl_id . '" ' . (($player_id == $pl_id) ? 'selected' : '') . '>' . $player->display_name . '</option>';
    }
    $ret .= '</select>';
    return $ret;
}

function get_active_players() {
    $active_players = new WP_User_Query([
        'role' => BUDATOLL_ROLE_PLAYER,
        'number' => -1,
        'fields' => 'all',
        'orderby' => 'display_name',
        'count_total' => false,
        'meta_key' => BUDATOLL_USER_META_ACTIVE,
        'meta_value' => 1,
    ]);
    return $active_players->get_results();
}