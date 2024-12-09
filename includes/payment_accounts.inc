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
    error_log('charge_passed_trainings');
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE '
            . ' `' . BUDATOLL_TRAINING_PAID . '`=0 AND '
            . ' `' . BUDATOLL_TRAINING_CONFIRMED . '`=1 AND '
            . ' `' . BUDATOLL_TRAINING_DONE . '`=1 ';
    $res = bt_get_results($sql);
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
            BUDATOLL_ACCOUNT_EVENT_ID => $event->{BUDATOLL_EVENTS_ID},
            BUDATOLL_ACCOUNT_COMMENT => $event->{BUDATOLL_EVENTS_LONG},
            BUDATOLL_ACCOUNT_AMOUNT => -$amount,
        ];
        $format = ['%d', '%s', '%d'];
        $wpdb->insert(BUDATOLL_ACCOUNT_TABLE, $data, $format);
        $last_id = $wpdb->insert_id;
        $sql = 'UPDATE `' . BUDATOLL_TRAINING_TABLE . '` SET `' . BUDATOLL_TRAINING_PAID . '`='.$last_id
                . ' WHERE `' . BUDATOLL_TRAINING_ID . '`=' . $training->{BUDATOLL_TRAINING_ID};
 //       error_log('Charge training: ' . $sql);
        $wpdb->query($sql);
        logger(BUDATOLL_LOGGER_TYPE_DEBIT, $user_id, $event->{BUDATOLL_EVENTS_LONG} . ' kifizetve:' . bt_currency($amount));
        if(btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_NEGATIV_BALANCE)) {
            $balance = get_balance_of_user($user_id);
            if($balance<=0) {
                $message = 'Tisztelt '. bt_get_full_name($user_id).'!<br><br>Az egyenleged elfogyott. <br>'
                        . 'A jelenlegi egyenleged: '.bt_currency($balance).'<br>Kérünk egy befizetéssel töltsd fel.';
                bt_send_email($user_id, 'Elfogyott az egyenleg', $message);
            }
        }
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
