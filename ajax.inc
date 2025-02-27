<?php

if (!defined('ABSPATH')) {
    die();
}
add_action('wp_ajax_budatoll', 'budatoll_ajax');
add_action('wp_ajax_nopriv_budatoll', 'budatoll_ajax_nopriv');

function budatoll_ajax_nopriv() {
    //   error_log('Non-logged user tried AJAX.');
    wp_die();
}

function budatoll_ajax() {
    header('Content-Type: application/json');
    $action = filter_input(INPUT_POST, 'ajax-action');
//   error_log('AJAX action: ' . $action);
    switch ($action) {
        case 'logout':
            wp_logout();
            wp_send_json_success(['redirect' => home_url()]); // Redirect to homepage or your custom URL
            break;
        case 'add-event':
            $date = filter_input(INPUT_POST, 'dropped-date');
            $type_id = filter_input(INPUT_POST, 'event_type-id');
            $ret = bt_add_event($type_id, $date);
            break;
        case 'remove-event':
            $event_id = filter_input(INPUT_POST, 'event-id');
            $ret = bt_remove_event($event_id);
            break;
        case 'get-all-events':
            $ret = bt_get_events();
            break;
        case 'copy-templates':
            $start_date = filter_input(INPUT_POST, 'start-of-week');
            if ($start_date) {
                $template = bt_copy_templates($start_date);
                $template['result'] = 'success';
                $ret = json_encode($template);
            } else {
                $ret = json_encode(['result' => 'error']);
            }
            break;
        case 'email-players':
            $emails = bt_warn_players();
            if ($emails && $emails > 0) {
                $ret = [
                    'result' => 'success',
                    'emailed' => $emails,
                ];
            } else {
                $ret = ['result' => 'error'];
                $ret['message'] = $emails ? 'Nem történt levél küldés' : 'Belső hiba';
            }
            $ret = json_encode($ret);
            break;
        case 'get-events-template':
            $ret = bt_get_events_template();
            break;
        case 'add-event-template':
            $type_id = filter_input(INPUT_POST, 'event_type-id');
            $weekday = filter_input(INPUT_POST, 'weekday');
            $template = bt_event_template_insert($type_id, $weekday);
            $template['result'] = 'success';
            $ret = json_encode($template);
            break;
        case 'remove-event-template':
            $template_id = filter_input(INPUT_POST, 'template-id');
            $template = bt_event_template_delete($template_id);
            $ret = json_encode(['result' => ($template ? 'success' : 'error')]);
            break;
        case 'move-event-template':
            $template_id = filter_input(INPUT_POST, 'template-id');
            $weekday = filter_input(INPUT_POST, 'weekday');
            $start = filter_input(INPUT_POST, 'start');
            $end = filter_input(INPUT_POST, 'end');
            $template = bt_event_template_move($template_id, $weekday, $start, $end);
            $ret = json_encode(['result' => ($template ? 'success' : 'error')]);
            break;
        case 'get-events-range':
            $start = filter_input(INPUT_POST, 'event-start');
            $end = filter_input(INPUT_POST, 'event-end');
            $ret = bt_get_events_range($start, $end);
            break;
        case 'get-trainings-range-of-user':
            $start = filter_input(INPUT_POST, 'trainings-start');
            $end = filter_input(INPUT_POST, 'trainings-end');
            $ret = bt_get_trainings_range_of_user($start, $end);
            break;
        case 'get-trainings-range':
            $start = filter_input(INPUT_POST, 'trainings-start');
            $end = filter_input(INPUT_POST, 'trainings-end');
            $ret = bt_get_trainings_range($start, $end);
            break;
        case 'get-trainings-of_event':
            $event_id = filter_input(INPUT_POST, 'event_id');
            $ret = bt_get_trainings_of_event($event_id);
            break;
        case 'get-players':
            $ret = json_encode([
                'players' => bt_get_active_players(),
                'result' => 'success',
            ]);
            break;
        case 'my-event-clicked':
            $my_event_id = getPostVariable('my-event-id', -1);
            $ret = bt_my_event_clicked($my_event_id);
            break;
        default :
            $ret = json_encode([
                'action' => $action,
                'result' => 'undefined action',
                'message' => 'error: undefined action'
            ]);
            break;
    }
//    error_log($ret);
    echo $ret;
    wp_die();
}

function bt_get_events() {
    global $wpdb;
    $res = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '`  ;');
    $ret = [
        'result' => 'success',
        'events' => $res,
    ];
    return json_encode($ret);
}

function bt_get_trainings_range_of_user($start, $end) {
    $user = wp_get_current_user();
    if ($user->ID == 0) {
        $ret = [
            'result' => 'not logged',
            'events' => [],
        ];
        return json_encode($ret);
    }
    if (array_intersect(array('coach', 'administrator'), (array) $user->roles)) {
        return bt_get_events_range($start, $end);
    } else {
        $visible = get_user_meta($user->ID, BUDATOLL_USER_META_EVENTS, false);
        if (!$visible OR $visible == '') {
            $visible = [];
        }
        if (isset($visible[0]) AND is_array($visible[0])) {
            $visible = $visible[0];
        }
        $visible_filter = ' `et`.`' . BUDATOLL_EVENT_TYPE_PUBLIC . '`=1 OR';
        if (!empty($visible)) {
            foreach ($visible as $visible_id) {
                $visible_filter .= ' `e`.`' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=' . $visible_id . ' OR';
            }
        }
        $sql_filter = substr($visible_filter, 0, -2);

        $sql_events = 'SELECT `e`.* FROM `' . BUDATOLL_EVENTS_TABLE . '` AS `e` '
                . ' JOIN `' . BUDATOLL_EVENT_TYPE_TABLE . '` AS `et` ON (`e`.`' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=`et`.`' . BUDATOLL_EVENTS_ID . '`) '
                . ' WHERE  (`' . BUDATOLL_EVENTS_DAY . '` BETWEEN "' . $start . '" AND "' . $end . '") ' .
                ' AND (' . $sql_filter . ')';
        //       error_log($sql_events);
        $res_events = bt_get_results($sql_events);
        $events = bt_get_trainings_of_events($res_events, $user->ID);
        if (btIsOptionEnabled(BUDATOLL_OPTION_DISABLE_NAGATIV_ACCOUNT)) {
            $allowed = (bt_get_balance_of_user($user->ID) >= 0);
        } else {
            $allowed = true;
        }
        $ret = [
            'result' => 'success',
            'events' => $events,
            'current_user_id' => $user->ID,
            'booking_allowed' => $allowed ? 1 : 0,
        ];
    }
    return json_encode($ret);
}

function bt_get_events_range($from, $to) {
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '` BETWEEN "%s" AND "%s";';
    $res = bt_get_results($sql, [$from, $to]);
    $ret = [
        'result' => 'success',
        'events' => $res,
    ];
    return json_encode($ret);
}

function bt_get_trainings_range($from, $to) {
    $events = bt_get_results('SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '` BETWEEN "%s" AND "%s";', [$from, $to]);
    $ret = ['result' => 'success'];
    if (empty($events)) {
        $ret['events'] = [];
    } else {
        foreach ($events as $event) {
            $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` '
                    . ' WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=%d ORDER BY `' . BUDATOLL_TRAINING_CREATED . '`;';
            $trainings = bt_get_results($sql, [$event->{BUDATOLL_EVENTS_ID}]);
            $waiting = false;
            $num_confirmed = 0;
            foreach ($trainings as $training) {
                $training->player_name = bt_get_full_name($training->player_id);
                if ($training->{BUDATOLL_TRAINING_CONFIRMED} == 1) {
                    $num_confirmed++;
                } else {
                    $waiting = true;
                }
            }
            $event->trainings_of_event = $trainings;
            $state = 'full';  // Az alapeset, hogy megtelt
            if ($waiting) { // Vannak a várólistán
                $state = 'waiting';
            }
            if ($event->{BUDATOLL_EVENTS_MAX_PLAYERS} == 0 OR $num_confirmed < $event->{BUDATOLL_EVENTS_MAX_PLAYERS}) { // Meg van hely
                $state = 'available';
            }
            $event->state = $state;
            $ret['events'][] = $event;
        }
    }
    return json_encode($ret);
}

function bt_get_trainings_of_events($events, $user_id = -1) {

    $ret = [];
    foreach ($events as $event) {
        $event_id = $event->{BUDATOLL_EVENTS_ID};
        $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` '
                . ' WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=%d ORDER BY `' . BUDATOLL_TRAINING_CREATED . '`;';
        $trainings = bt_get_results($sql, [$event_id]);
        $confirmed = false;
        $booked = false;
        foreach ($trainings as $training) {
            $training->player_name = bt_get_full_name($training->player_id);
            $training->removeable = bt_is_training_deletable($training->id);
            if ($user_id == $training->player_id) {
                $booked = true;
                $confirmed = $training->{BUDATOLL_TRAINING_CONFIRMED} == 1;
            }
        }
        $event->trainings_of_event = $trainings;
        $event->confirmed = $confirmed;
        $event->booked = $booked;
        if (empty($trainings)) {
            $event->full = false;
        } else {
            $event->full = bt_is_event_full($event_id);
        }
        $ret[] = $event;
    }
    return $ret;
}

function bt_add_event($type_id, $date) {
    global $wpdb;
    $ret = json_encode(['result' => 'error']);
    $event = get_event_type_by_id($type_id);
    if (empty($event->{BUDATOLL_EVENT_TYPE_ID}) OR $event->{BUDATOLL_EVENT_TYPE_ID} != $type_id) {
        return $ret;
    }
    if (!$date) {
        return $ret;
    }
    $res = bt_get_results('SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '`="%s" '
            . 'AND `' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=%d;', [$date, $type_id]);
    if ($res AND count($res) > 0) {
        $ret = json_encode(['result' => 'already']);
        return $ret;
    }
    $data = [
        BUDATOLL_EVENTS_SHORT => $event->{BUDATOLL_EVENT_TYPE_SHORT},
        BUDATOLL_EVENTS_LONG => $event->{BUDATOLL_EVENT_TYPE_LONG},
        BUDATOLL_EVENTS_DAY => $date,
        BUDATOLL_EVENTS_EVENT_TYPE_ID => $type_id,
        BUDATOLL_EVENTS_PRICE => $event->{BUDATOLL_EVENT_TYPE_PRICE},
        BUDATOLL_EVENTS_MAX_PLAYERS => $event->{BUDATOLL_EVENT_TYPE_MAX_PLAYERS},
        BUDATOLL_EVENTS_START => $event->{BUDATOLL_EVENT_TYPE_START},
        BUDATOLL_EVENTS_END => $event->{BUDATOLL_EVENT_TYPE_END},
    ];
    if ($wpdb->insert(BUDATOLL_EVENTS_TABLE, $data)) {
        bt_logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'Edzés felvéve : ' . $wpdb->insert_id, $data);
        $ret = json_encode([
            'result' => 'success',
            'message' => $event->{BUDATOLL_EVENT_TYPE_LONG} . '<br>' . $date . ' : ' . $event->{BUDATOLL_EVENT_TYPE_START} . ' - ' . $event->{BUDATOLL_EVENT_TYPE_END},
            'event' => $data,
            'event_id' => $wpdb->insert_id,
        ]);
    }
    return $ret;
}

function bt_remove_event($event_id) {
    $event = bt_get_event_by_id($event_id);
    if (!$event) {
        bt_logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'Edzés törlése sikertelen, mert nem találtuk!');
        return json_encode(['result' => 'error', 'message' => 'Nem találtuk az edzése']);
    }
    $trainings = bt_count_trainings($event_id);
    if ($trainings AND $trainings > 0) {
        bt_logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'Edzés törlése sikertelen, mert már vannak foglalások!');
        return json_encode(['result' => 'error', 'message' => 'Már vannak foglalások az edzésre']);
    }
    bt_get_results('DELETE FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_ID . '`=' . $event_id);
    bt_logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'Edzés törölve : '
            . $event->{BUDATOLL_EVENTS_LONG} . ', ' . $event->{BUDATOLL_EVENTS_DAY} . ' ' . bt_time($event->{BUDATOLL_EVENTS_START}));
    return json_encode(['result' => 'deleted']);
}

function bt_get_trainings_of_event($event_id) {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` '
            . ' WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event_id . ' ORDER BY `' . BUDATOLL_TRAINING_CREATED . '`;';
    $trainings = $wpdb->get_results($sql);
    $ret = json_encode([
        'result' => 'success',
        'trainings' => $trainings,
    ]);
    return $ret;
}
