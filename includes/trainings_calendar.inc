<?php

function budatoll_my_trainings_calendar() {
    $player_id =  get_current_user_id();
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
                BUDATOLL_TRAINING_PLAYER_ID => $player_id,
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
