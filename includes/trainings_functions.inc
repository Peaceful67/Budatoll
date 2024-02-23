<?php

function bt_get_training_by_id($id) {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_ID . '`=' . $id . ';';
    $results = $wpdb->get_results($sql);
    if (empty($results)) {
        return false;
    } else {
        return $results[0];
    }
}

function bt_delete_training($id) {
    global $wpdb;
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
