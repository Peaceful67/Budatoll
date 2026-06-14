<?php

if (!defined('ABSPATH')) {
    die();
}

function budatoll_my_trainings_calendar() {
    $player_id = get_current_user_id();
    global $wpdb;
    $ret = '<form id="BudatollForm" method="post">';
    $ret .= '<div id="budatoll-trainings-info" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>';
    $ret .= '<div id="budatoll-trainings-editor" class="budatoll-popup">'
            . '<div id="budatoll-trainings-editor-popup" class="budatoll-popup-content">'
            . '</div></div>';
    $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-success-message"></div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="budatoll-error-message"></div>';
    $training_delete = getPostVariable('training_delete');
    if ($training_delete AND $training_delete > 0) {
        bt_delete_training($training_delete);
        $training = bt_get_training_by_id($training_delete);
        bt_confirm_waitinglist_of_event($training->{BUDATOLL_TRAINING_EVENT_ID} ?? -1);
    }
    $event_id = getPostVariable('event_id');
    if (getPostVariable('training_remove') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            if ($wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_EVENT_ID => $event_id, BUDATOLL_TRAINING_PLAYER_ID => $player_id])) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés törölve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START}) . '-'
                        . bt_time($event->{BUDATOLL_EVENTS_END}));
                bt_confirm_waitinglist_of_event($event_id);
                if (btIsOptionEnabled(BUDATOLL_OPTION_WAITINGLIST_EMAIL)) {
                    $waiting_list = bt_get_waiting_list($event_id);
                    foreach ($waiting_list as $waiting_id) {
                        bt_send_custom_email($waiting_id, BT_CUSTOM_EMAIL_WAITING,
                                [$event->{BUDATOLL_EVENTS_SHORT}, $event->{BUDATOLL_EVENTS_DAY}, bt_time($event->{BUDATOLL_EVENTS_START}), bt_time($event->{BUDATOLL_EVENTS_END})]);
                    }
                }
            }
        }
    }
    if (getPostVariable('training_from_waiting') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $cancelable = bt_is_short_application_cancelable($event);
            $data = [
                BUDATOLL_TRAINING_CONFIRMED => 1,
                BUDATOLL_TRAINING_DELETABLE => ($cancelable ? 1 : 0),
            ];
            if ($wpdb->update(BUDATOLL_TRAINING_TABLE, $data, [BUDATOLL_TRAINING_EVENT_ID => $event_id, BUDATOLL_TRAINING_PLAYER_ID => $player_id]) > 0) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés várólistáról élesítve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START}) . '-'
                        . bt_time($event->{BUDATOLL_EVENTS_END}));
            }
        }
    }
    if (getPostVariable('training_wait') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $data = [
                BUDATOLL_TRAINING_EVENT_ID => $event_id,
                BUDATOLL_TRAINING_PLAYER_ID => $player_id,
                BUDATOLL_TRAINING_CONFIRMED => 0,
            ];
            if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés várólistára felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . bt_time($event->{BUDATOLL_EVENTS_START})
                        . '-' . bt_time($event->{BUDATOLL_EVENTS_END}));
                if (!bt_is_training_full($wpdb->insert_id)) {
                    bt_training_confirm($wpdb->insert_id);
                }
            }
        }
    }
    if (getPostVariable('training_add') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $cancelable = bt_is_short_application_cancelable($event);
            $data = [
                BUDATOLL_TRAINING_EVENT_ID => $event_id,
                BUDATOLL_TRAINING_PLAYER_ID => $player_id,
                BUDATOLL_TRAINING_CONFIRMED => 1,
                BUDATOLL_TRAINING_DELETABLE => ($cancelable ? 1 : 0),
            ];
            if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START})
                        . '-' . bt_time($event->{BUDATOLL_EVENTS_END}));
                if (!bt_is_training_full($wpdb->insert_id)) {
                    bt_training_confirm($wpdb->insert_id);
                }
            }
        }
    }
    $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-message"></div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="error-message"></div>';
    $ret .= '<div id="budatoll-edzes-calendar"></div>';
    $ret .= '</form>';
    return $ret;
}

function budatoll_trainings_calendar() {

    global $wpdb;
    $goto_date = false;
    $ret = '<form id="BudatollForm" method="post">';
    $ret .= '<div id="budatoll-trainings-info" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>';
    $ret .= '<div id="budatoll-trainings-editor" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>';
    $set_max_players = getPostVariable('set_max_players');
    if ($set_max_players) {
        $max_players = getPostVariable('max_players');
        if ($max_players >= 1 AND $max_players <= 99) {
            if ($wpdb->update(BUDATOLL_EVENTS_TABLE, [BUDATOLL_EVENTS_MAX_PLAYERS => $max_players], [BUDATOLL_EVENTS_ID => $set_max_players]) > 0) {
                bt_logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'A ' . $set_max_players . ' azonositójú edzés módositott max játékos száma: ' . $max_players);
                bt_confirm_waitinglist_of_event($set_max_players);
            }
        }
    }
    $training_delete = getPostVariable('training_delete');
    if ($training_delete AND $training_delete > 0) {
        $training = bt_get_training_by_id($training_delete);
        $event = get_event_by_id($training->{BUDATOLL_TRAINING_EVENT_ID} ?? -1);
        $goto_date = $event->{BUDATOLL_EVENTS_DAY} ?? false;
        bt_delete_training($training_delete);
        bt_confirm_waitinglist_of_event($training->{BUDATOLL_TRAINING_EVENT_ID} ?? -1);
    }
    $add_player_id = getPostVariable('select_players');
    $event_id = getPostVariable('event_id');
    if ($add_player_id > 0 AND getPostVariable('training_add') AND $event_id AND !bt_is_player_already($add_player_id, $event_id)) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $data = [
                BUDATOLL_TRAINING_EVENT_ID => $event_id,
                BUDATOLL_TRAINING_PLAYER_ID => $add_player_id,
                BUDATOLL_TRAINING_CONFIRMED => 1,
            ];
            if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
                $goto_date = $event->{BUDATOLL_EVENTS_DAY} ?? false;
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $add_player_id, 'Jelentkezés felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START})
                        . '-' . bt_time($event->{BUDATOLL_EVENTS_END}));
                if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY)) {
                    $current_user = wp_get_current_user();
                    $current_user_name = $current_user ? ($current_user->display_name . '-t') : 'A klub edzőjét';
                    bt_send_custom_email($add_player_id, BT_CUSTOM_EMAIL_TRAINING_SAVED, [
                        $event->{BUDATOLL_EVENTS_LONG},
                        $event->{BUDATOLL_EVENTS_DAY},
                        bt_time($event->{BUDATOLL_EVENTS_START}),
                        bt_time($event->{BUDATOLL_EVENTS_END}),
                        $current_user_name]);
                }
            }
        }
    }
    $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-message"></div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="error-message"></div>';
    $ret .= '<div id="budatoll-edzes-calendar" ' . ($goto_date ? 'data-goto-date="' . $goto_date . '"' : '') . '></div>';
    $ret .= '</form>';
    return $ret;
}
