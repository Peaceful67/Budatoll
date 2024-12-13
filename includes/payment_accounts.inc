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
            . ' `' . BUDATOLL_TRAINING_PAID . '`=0 AND '
            . ' `' . BUDATOLL_TRAINING_CONFIRMED . '`=1 AND '
            . ' `' . BUDATOLL_TRAINING_DONE . '`=1 ';
    $res = bt_get_results($sql);
    foreach ($res as $training) {

        $player_id = $training->{BUDATOLL_TRAINING_PLAYER_ID};
        $event_id = $training->{BUDATOLL_TRAINING_EVENT_ID};
        $training_id = $training->{BUDATOLL_TRAINING_ID};

        $event = bt_get_event_by_id($event_id);
        if (!$event) {
            continue;
        }
        $amount = $event->{BUDATOLL_EVENTS_PRICE};
        $data = [
            BUDATOLL_ACCOUNT_PLAYER_ID => $player_id,
            BUDATOLL_ACCOUNT_EVENT_ID => $event->{BUDATOLL_EVENTS_ID},
            BUDATOLL_ACCOUNT_COMMENT => $event->{BUDATOLL_EVENTS_LONG},
            BUDATOLL_ACCOUNT_AMOUNT => -$amount,
        ];
        error_log('Charge: '.print_r($data, true));
        $format = ['%d','%d', '%s', '%d'];
        $wpdb->insert(BUDATOLL_ACCOUNT_TABLE, $data, $format);
        $last_id = $wpdb->insert_id;
        $sql = 'UPDATE `' . BUDATOLL_TRAINING_TABLE . '` SET `' . BUDATOLL_TRAINING_PAID . '`=' . $last_id
                . ' WHERE `' . BUDATOLL_TRAINING_ID . '`=' . $training_id;
        //       error_log('Charge training: ' . $sql);
        bt_get_results($sql);
        bt_logger(BUDATOLL_LOGGER_TYPE_CHARGE, $player_id, $event->{BUDATOLL_EVENTS_LONG} . ' kifizetve: ' . bt_currency($amount));
        bt_warn_user_negativ_balance($player_id);
    }
}

function bt_warn_user_negativ_balance($user_id) {
    if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE)) {
        $balance = get_balance_of_user($user_id);
        $already_warned = unserialize(get_option(BUDATOLL_NEGATIV_BALANCE_ALREADY_WARNED, ''));
        if ($balance <= 0) {
            if (!isset($already_warned[$user_id])) {
                $message = 'Tisztelt ' . bt_get_full_name($user_id) . '!<br><br>Az egyenleged elfogyott. <br>'
                        . 'A jelenlegi egyenleged: ' . bt_currency($balance) . '<br>Kérünk egy befizetéssel töltsd fel.';
                bt_send_email($user_id, 'Negatív egyenleg', $message);
                $already_warned[$user_id] = 1;
            } else {
                $already_warned[$user_id]++;
            }
        } else {
            unset($already_warned[$user_id]);
        }
        update_option(BUDATOLL_NEGATIV_BALANCE_ALREADY_WARNED, serialize($already_warned));
    }
}

function bt_get_account($id) {
    $sql = 'SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_ID . '`=' . $id . ';';
    $results = bt_get_results($sql);
    if (empty($results)) {
        return false;
    } else {
        return $results[0];
    }
}
