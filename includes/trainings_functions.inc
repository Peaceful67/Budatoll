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
    $training = bt_get_training_by_id($id);
    if (!isset($training->{BUDATOLL_TRAINING_EVENT_ID})) {
        return;
    }
    $event = get_event_by_id($training->{BUDATOLL_TRAINING_EVENT_ID});
    $player_id = $training->{BUDATOLL_TRAINING_PLAYER_ID} ?? -1;
    if ($player_id > 0 && $wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_ID => $id]) > 0) {
        bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés törölve: ' . print_r($training, true));
        $account_id = $training->{BUDATOLL_TRAINING_PAID};
        if ($account_id != 0) { // Ez már ki volt fizetve.
            $wpdb->delete(BUDATOLL_ACCOUNT_TABLE, [BUDATOLL_ACCOUNT_ID => $account_id]);
        }
        if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_USER_MODIFY)) {
            $current_user = wp_get_current_user();
            $current_user_name = $current_user ? ($current_user->display_name . '-t') : 'A klub edzőjét';
            bt_send_custom_email($player_id, BT_CUSTOM_EMAIL_TRAINING_DELETED,
                    [$event->{BUDATOLL_EVENTS_LONG}, $event->{BUDATOLL_EVENTS_DAY}, bt_time($event->{BUDATOLL_EVENTS_START})
                        , bt_time($event->{BUDATOLL_EVENTS_END}), $current_user_name]);
        }
    }
}

function bt_is_training_full($id) {
    $training = bt_get_training_by_id($id);
    if (!$training) {  // Nem is jó az id, nincs ilyen
        return true;
    }
    $event_id = $training->{BUDATOLL_TRAINING_EVENT_ID} ?? -1;
    return bt_is_event_full($event_id);
}

function bt_is_event_full($event_id) {
    global $wpdb;
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
        bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés visszaigazolva.');
        return true;
    }
    return false;
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
        if ($training->{BUDATOLL_TRAINING_DONE} == 1 OR $training->{BUDATOLL_TRAINING_PAID} == 1) { // Ha már teljesül és vagy ki van fizetve, akkor nem 
            return false;
        }
        if ($training->{BUDATOLL_TRAINING_DELETABLE} == 1 AND btIsOptionEnabled(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE)) { // Úgy lett felvéve, hogy törölni lehet és még mindig úgy van beállitva
            return true;
        }

        if (btIsOptionEnabled(BUDATOLL_OPTION_TRAINING_CANCELABLE)) { // Ha bármikor lehet törölni
            return true;
        } else { // Ha csak 24 óránál korábbit lehet törölni
            $event_id = $training->{BUDATOLL_TRAINING_EVENT_ID};
            $event = bt_get_event_by_id($event_id);
            if ($event) {
                $day = $event->{BUDATOLL_EVENTS_DAY};
                $time = $event->{BUDATOLL_EVENTS_START};
                $eventTime = new DateTime($day . ' ' . $time, new DateTimeZone(wp_timezone_string()));
                $currentTime = new DateTime('now', new DateTimeZone(wp_timezone_string()));
                $interval = $currentTime->diff($eventTime);
                if ($eventTime >= $currentTime && $interval->days == 0 && $interval->h <= 24) { // 24 órán belül vagyunk
                    return false;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

function bt_is_short_application_cancelable($event) {
    if (!isset($event->{BUDATOLL_EVENTS_DAY})) {
        return false;
    } else {
        $day = $event->{BUDATOLL_EVENTS_DAY};
    }
    return btIsOptionEnabled(BUDATOLL_OPTION_SHORT_APPLICATION_CANCELABLE) AND bt_is_day_before($day);
}

function get_trainings_by_event($event_id) {
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id;
    return bt_get_results($sql);
}

function bt_count_trainings($event_id) {
    $sql = 'SELECT COUNT(*) as `NUM` FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id;
    $res = bt_get_results($sql);
    if (isset($res[0]->NUM)) {
        return $res[0]->NUM;
    } else {
        return false;
    }
}

function bt_my_event_clicked($event_id) {
    /* Possibles states: 
     *  done, deleted, deleted-waiting, waiting, confirmed, confirmed-waiting, delete-refused
     */
    global $wpdb;
    $ret = [];
    $player_id = bt_get_current_userid();
    $leased = get_user_meta($player_id, BUDATOLL_USER_META_LEASED, true);
    $event = bt_get_event_by_id($event_id);
    if (!$event) {
        return json_encode(['status' => 'event_error', 'result' => 'error']);
    }
    if ($event->{BUDATOLL_EVENTS_DONE} == 1) {
        return json_encode(['status' => 'done', 'result' => 'success']);
    }
    if (!$leased) { // Ha nem bérletes, csak a következő napra foglalhat.
        $day = $event->{BUDATOLL_EVENTS_DAY};
        $time = $event->{BUDATOLL_EVENTS_START};
        $eventTime = new DateTime($day . ' ' . $time, new DateTimeZone(wp_timezone_string()));
        $currentTime = new DateTime('now', new DateTimeZone(wp_timezone_string()));
        $interval = $currentTime->diff($eventTime);
        if ($interval->days > 1) {
            return json_encode(['status' => 'not-leased', 'result' => 'success']);
        }
    }
    $training = bt_get_results('SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` '
            . ' WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id . ' AND `' . BUDATOLL_TRAINING_PLAYER_ID . '`=' . $player_id);
    $ret['result'] = 'success';
    if (isset($training[0])) { // Már van foglalás
        $training_id = $training[0]->{BUDATOLL_TRAINING_ID};
        if ($training[0]->{BUDATOLL_TRAINING_DONE} == 1) { // Már lezajlott, nem lehet módosítani
            $ret['status'] = 'done';
            return json_encode($ret);
        }

        if ($training[0]->{BUDATOLL_TRAINING_CONFIRMED} == 1) { // Le volt foglalva, tehát töröljük.
            if (!bt_is_training_deletable($training_id)) { // Ha nem törölhető, elutasitjuk
                $ret['status'] = 'delete-refused';
                return json_encode($ret);
            }
            if ($wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_ID => $training_id])) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés törölve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START}) . '-'
                        . bt_time($event->{BUDATOLL_EVENTS_END}));
            }
            if (btIsOptionEnabled(BUDATOLL_OPTION_WAITINGLIST_EMAIL)) {
                $waiting_list = bt_get_waiting_list($event_id);
                if (!empty($waiting_list)) {
                    foreach ($waiting_list as $waiting_id) {
                        bt_send_custom_email($waiting_id, BT_CUSTOM_EMAIL_WAITING,
                                [$event->{BUDATOLL_EVENTS_SHORT}, $event->{BUDATOLL_EVENTS_DAY}, bt_time($event->{BUDATOLL_EVENTS_START}), bt_time($event->{BUDATOLL_EVENTS_END})]);
                    }
                }
            }
            $ret['status'] = 'deleted';
            return json_encode($ret);
        } // Váró listán van
        if (bt_is_event_full($event_id)) { // Még tele van, töröljük
            if ($wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_ID => $training_id])) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés a várólistáról törölve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START}) . '-'
                        . bt_time($event->{BUDATOLL_EVENTS_END}));
            }
            $ret['status'] = 'deleted-waiting';
        } else { // Nincs tele, biztosan jelentkezni akart
            $cancelable = bt_is_short_application_cancelable($event);
            $data = [
                BUDATOLL_TRAINING_CONFIRMED => 1,
                BUDATOLL_TRAINING_DELETABLE => ($cancelable ? 1 : 0),
            ];
            if ($wpdb->update(BUDATOLL_TRAINING_TABLE, $data, [BUDATOLL_TRAINING_ID => $training_id]) > 0) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés várólistáról élesítve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START}) . '-'
                        . bt_time($event->{BUDATOLL_EVENTS_END}));
            }
            $ret['status'] = 'confirmed-waiting';
        }
        return json_encode($ret);
    } else { // Még nincs foglalás
        if (bt_is_event_full($event_id)) { // Még tele van, wainting listre tesszük
            $data = [
                BUDATOLL_TRAINING_EVENT_ID => $event_id,
                BUDATOLL_TRAINING_PLAYER_ID => $player_id,
                BUDATOLL_TRAINING_CONFIRMED => 0,
            ];
            if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
                bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $player_id, 'Jelentkezés várólistára felvéve: '
                        . $event->{BUDATOLL_EVENTS_SHORT} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ':' . bt_time($event->{BUDATOLL_EVENTS_START})
                        . '-' . bt_time($event->{BUDATOLL_EVENTS_END}));
            }

            $ret['status'] = 'waiting';
        } else { // Nincs tele, biztosan jelentkezni akart
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
            }
            $ret['status'] = 'confirmed';
        }
        return json_encode($ret);
    }
    $ret['status'] = 'unknown';
    return json_encode($ret);
}

function bt_is_player_already($player_id, $event_id) {
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE '
            . '`' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id . ' AND `' . BUDATOLL_TRAINING_PLAYER_ID . '`=' . $player_id;
    $res = bt_get_results($sql);
    return $res && !empty($res);
}
