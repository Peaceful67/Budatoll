<?php

if (!defined('ABSPATH')) {
    die();
}

function bt_get_unpaid_trainings($user_id) {
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE '
            . ' `' . BUDATOLL_TRAINING_PAID . '`=%d AND '
            . ' `' . BUDATOLL_TRAINING_DONE . '`=%d AND '
            . '`' . BUDATOLL_TRAINING_PLAYER_ID . '`=%d';
    $res = bt_get_results($sql, [0, 1, $user_id]);
    return $res ? $res : [];
}

function bt_charge_passed_trainings() {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE '
            . ' `' . BUDATOLL_TRAINING_PAID . '`=%d AND '
            . ' `' . BUDATOLL_TRAINING_CONFIRMED . '`=%d AND '
            . ' `' . BUDATOLL_TRAINING_DONE . '`=%d ';
    $res = bt_get_results($sql, [0, 0, 1]);
    foreach ($res as $training) {
        $user_id = $training->{BUDATOLL_TRAINING_PLAYER_ID};
        $event_id = $training->{BUDATOLL_TRAINING_EVENT_ID};
        $event = bt_get_event_by_id($event_id);
        if (!$event) {
            continue;
        }
        $amount = $event->{BUDATOLL_EVENTS_PRICE};
        $data = [
            BUDATOLL_ACCOUNT_PLAYER_ID => $user_id,
            BUDATOLL_ACCOUNT_COMMENT => $event->{BUDATOLL_EVENTS_LONG},
            BUDATOLL_ACCOUNT_AMOUNT => -$amount,
        ];
        $format = ['%d', '%s', '%d'];
        $wpdb->insert(BUDATOLL_ACCOUNT_TABLE, $data, $format);
        logger(BUDATOLL_LOGGER_TYPE_PAYMENT, $user_id, $event->{BUDATOLL_EVENTS_LONG} . ' kifizetve:' . $amount . ' Ft');
    }
}
