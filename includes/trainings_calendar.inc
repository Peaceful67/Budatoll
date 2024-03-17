<?php

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
    $training_delete = getPostVariable('training_delete');
    if ($training_delete AND $training_delete > 0) {
        bt_delete_training($training_delete);
    }
    $event_id = getPostVariable('event_id');
    if (getPostVariable('training_remove') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            if ($wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_EVENT_ID => $event_id, BUDATOLL_TRAINING_PLAYER_ID => $player_id])) {
                logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés törölve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . $event->{BUDATOLL_EVENTS_START} . '-' . $event->{BUDATOLL_EVENTS_END});
            }
        }
    }
    if (getPostVariable('training_from_waiting') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $data = [
                BUDATOLL_TRAINING_CONFIRMED => 1,
            ];
            if ($wpdb->update(BUDATOLL_TRAINING_TABLE, $data, [BUDATOLL_TRAINING_EVENT_ID => $event_id, BUDATOLL_TRAINING_PLAYER_ID => $player_id]) > 0) {
                logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés várólistáról élesítve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . $event->{BUDATOLL_EVENTS_START} . '-' . $event->{BUDATOLL_EVENTS_END});
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
                logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés várólistára felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . $event->{BUDATOLL_EVENTS_START} . '-' . $event->{BUDATOLL_EVENTS_END});
                if (!bt_is_training_full($wpdb->insert_id)) {
                    bt_training_confirm($wpdb->insert_id);
                }
            }
        }
    }
    if (getPostVariable('training_add') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $data = [
                BUDATOLL_TRAINING_EVENT_ID => $event_id,
                BUDATOLL_TRAINING_PLAYER_ID => $player_id,
                BUDATOLL_TRAINING_CONFIRMED => 1,
            ];
            if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
                logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . $event->{BUDATOLL_EVENTS_START} . '-' . $event->{BUDATOLL_EVENTS_END});
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
    $ret = '<form id="BudatollForm" method="post">';
    $ret .= '<div id="budatoll-trainings-info" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>';
    $ret .= '<div id="budatoll-trainings-editor" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>';
    $training_delete = getPostVariable('training_delete');
    if ($training_delete AND $training_delete > 0) {
        bt_delete_training($training_delete);
    }
    $add_player_id = getPostVariable('select_players');
    $event_id = getPostVariable('event_id');
    if ($add_player_id > 0 AND getPostVariable('training_add') AND $event_id) {
        $event = bt_get_event_by_id($event_id);
        if ($event) {
            $data = [
                BUDATOLL_TRAINING_EVENT_ID => $event_id,
                BUDATOLL_TRAINING_PLAYER_ID => $add_player_id,
                BUDATOLL_TRAINING_CONFIRMED => 1,
            ];
            if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
                logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $add_player_id, 'Jelentkezés felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . $event->{BUDATOLL_EVENTS_START} . '-' . $event->{BUDATOLL_EVENTS_END});
            }
        }
    }
    $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-message"></div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="error-message"></div>';
    $ret .= '<div id="budatoll-edzes-calendar"></div>';
    $ret .= '</form>';
    return $ret;
}
