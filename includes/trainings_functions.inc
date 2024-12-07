<?php

if (!defined('ABSPATH')) {
    die();
}

function bt_get_training_by_id($id) {
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_ID . '`=' . $id . ';';
    $results = bt_get_results($sql);
    if (empty($results)) {
        return false;
    } else {
        return $results[0];
    }
}

function bt_delete_training($id) {
    global $wpdb;
    echo 'Delete training:' . $id;
    $training = bt_get_training_by_id($id);
    $player_id = $training->{BUDATOLL_TRAINING_PLAYER_ID} ?? -1;
    if ($player_id > 0 && $wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_ID => $id]) > 0) {
        logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés törölve: ' . print_r($training, true));
    }
}

function bt_is_training_full($id) {
    global $wpdb;
    $training = bt_get_training_by_id($id);
    if (!$training) {  // Nem is jó az id, nincs ilyen
        return true;
    }
    $event_id = $training->{BUDATOLL_TRAINING_EVENT_ID} ?? -1;
    $event = bt_get_event_by_id($event_id);
    $max_players = $event->{BUDATOLL_EVENTS_MAX_PLAYERS} ?? 0;
    if ($max_players == 0) { // Nincs is korlátozva
        return false;
    }
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id
            . ' AND `' . BUDATOLL_TRAINING_CONFIRMED . '`=1';
    $wpdb->get_results($sql);
    return $max_players <= $wpdb->num_rows;
}

function bt_training_confirm($id) {
    global $wpdb;
    $training = bt_get_training_by_id($id);
    if (!$training) {  // Nem is jó az id, nincs ilyen
        return false;
    }
    $player_id = $training->{BUDATOLL_TRAINING_PLAYER_ID} ?? -1;
    $sql = 'UPDATE `' . BUDATOLL_TRAINING_TABLE . '` SET ';
    if ($wpdb->get_results($sql)) {
        logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés visszaigazolva.');
        return true;
    }
    return false;
}

function bt_training_charge($id) {
    $already_warned = get_option(BUDATOLL_EMAIL_WARNING_NEGATIV_BALANCE);
    $training = bt_get_training_by_id($id);
    $player_id = $training->BUDATOLL_TRAINING_PLAYER_ID;

    if (!isset($already_warned[$player_id])) {
        $already_warned[] = 3;
        update_option(BUDATOLL_EMAIL_WARNING_NEGATIV_BALANCE, $already_warned);
        bt_send_email($player_id, 'Negatív egyenleg', 'Az egyenleged negatív lett.');
    }
}

function bt_get_waiting_list($event_id) {
    $sql = 'SELECT `' . BUDATOLL_TRAINING_PLAYER_ID . '` FROM `' . BUDATOLL_TRAINING_TABLE . '` '
            . 'WHERE `' . BUDATOLL_TRAINING_CONFIRMED . '`=0 AND `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id;
    $result = bt_get_results($sql);
    $ret = [];
    if ($result) {

        foreach ($result as $item) {
            $ret[] = $item->{BUDATOLL_TRAINING_PLAYER_ID};
        }
    }
    return $ret;
}

function bt_is_training_deletable($id) {
    $training = bt_get_training_by_id($id);
    if ($training) {
        if ($training->{BUDATOLL_TRAINING_DELETABLE} == 1) { // Úgy lett felvéve, hogy törölni lehet
            return true;
        }
        if ($training->{BUDATOLL_TRAINING_DONE} == 1 OR $training->{BUDATOLL_TRAINING_PAID} == 1) { // Ha már teljesül és vagy ki van fizetve, akkor nem 
            return false;
        }

        if (btIsOptionEnabled(BUDATOLL_OPTION_EVENT_CANCELABLE)) { // Ha törölni lehet 24 órán belül
            $event_id = $training->{BUDATOLL_TRAINING_EVENT_ID};
            $event = bt_get_event_by_id($event_id);
            if ($event) {
                $day = $event->{BUDATOLL_EVENTS_DAY};
                $time = $event->{BUDATOLL_EVENTS_START};
                $eventTime = new DateTime($day . ' ' . $time, new DateTimeZone(wp_timezone_string()));
                $currentTime = new DateTime('now', new DateTimeZone(wp_timezone_string()));
                $interval = $currentTime->diff($eventTime);
                if ($eventTime <= $currentTime && $interval->days == 0 && $interval->h <= 24) {
                    return true;
                }
            }
        }
    }
    return false;
}

function bt_is_short_application_cancelable($event) {

    if ($event AND btIsOptionEnabled(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE)) {
        $day = $event->{BUDATOLL_EVENTS_DAY};
        $eventTime = new DateTime($day, new DateTimeZone(wp_timezone_string()));
        $eventTime->modify('-1 day')->setTime(0, 0);
        $currentTime = new DateTime('now', new DateTimeZone(wp_timezone_string()));
        return ($currentTime >= $eventTime);
    }
    return false;
}
