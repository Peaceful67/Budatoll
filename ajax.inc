<?php

add_action('wp_ajax_budatoll', 'budatoll_ajax');
add_action('wp_ajax_nopriv_budatoll', 'budatoll_ajax');

function budatoll_ajax() {
    header('Content-Type: application/json');
    $action = filter_input(INPUT_POST, 'ajax-action');
    switch ($action) {
        case 'add-event':
            $date = filter_input(INPUT_POST, 'dropped-date');
            $type_id = filter_input(INPUT_POST, 'event_type-id');
            $ret = btAddEvent($type_id, $date);
            break;
        case 'get-all-events':
            $ret = bt_get_events();
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
        default :
            $ret = json_encode([
                'action' => $action,
                'result' => 'undefined action',
                'message' => 'error: undefined action'
            ]);
            break;
    }
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
    global $wpdb;
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
        $sql_filter = '';
        foreach ($visible as $visible_id) {
            $sql_filter .= ' `' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=' . $visible_id . ' OR';
        }
        if (!empty($sql_filter)) {
            $sql_filter = ' AND (' . substr($sql_filter, 0, -2) . ')';
        }
        $sql_events = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  (`' . BUDATOLL_EVENTS_DAY . '` BETWEEN "' . $start . '" AND "' . $end . '") ' . $sql_filter;
        $res_events = $wpdb->get_results($sql_events);
        $events = bt_get_trainings_of_events($res_events);
        $ret = [
            'result' => 'success',
            'events' => $events,
            'current_user_id' => $user->ID,
        ];
    }
    return json_encode($ret);
}

function bt_get_events_range($from, $to) {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '` BETWEEN "' . $from . '" AND "' . $to . '";;';
    $res = $wpdb->get_results($sql);
    $ret = [
        'result' => 'success',
        'events' => $res,
        'sql' => $sql,
    ];
    return json_encode($ret);
}

function bt_get_trainings_range($from, $to) {
    global $wpdb;
    $sql = $wpdb->prepare('SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '` BETWEEN %s AND %s;', $from, $to);
    $events = $wpdb->get_results($sql);
    $ret = ['result' => 'success'];
    if (empty($events)) {
        $ret['events'] = [];
    } else {
        foreach ($events as $event) {
            $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` '
                    . ' WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event->{BUDATOLL_EVENTS_ID} . ' ORDER BY `' . BUDATOLL_TRAINING_CREATED . '`;';
            $trainings = $wpdb->get_results($sql);
            foreach ($trainings as $training) {
                $training->player_name = bt_get_full_name($training->player_id);
            }
            $event->trainings_of_event = $trainings;
            $ret['events'][] = $event;
        }
    }
    return json_encode($ret);
}

function bt_get_trainings_of_events($events) {
    global $wpdb;
    $ret = [];
    foreach ($events as $event) {
        $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` '
                . ' WHERE `' . BUDATOLL_TRAINING_EVENT_ID . '`=' . $event->{BUDATOLL_EVENTS_ID} . ' ORDER BY `' . BUDATOLL_TRAINING_CREATED . '`;';
        $trainings = $wpdb->get_results($sql);
        foreach ($trainings as $training) {
            $training->player_name = bt_get_full_name($training->player_id);
        }
        $event->trainings_of_event = $trainings;
        $ret[] = $event;
    }
    return $ret;
}

function btAddEvent($type_id, $date) {
    global $wpdb;
    $ret = json_encode(['result' => 'error']);
    $event = btGetEventTypeById($type_id);
    if (empty($event->{BUDATOLL_EVENT_TYPE_ID}) OR $event->{BUDATOLL_EVENT_TYPE_ID} != $type_id) {
        return $ret;
    }
    if (!$date) {
        return $ret;
    }
    $res = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '`="' . $date . '" '
            . 'AND `' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=' . $type_id . ';');
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
        logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'Edzés felvéve : ' . $wpdb->insert_id, $data);
        $ret = json_encode([
            'result' => 'success',
            'event' => $data,
            'event_id' => $wpdb->insert_id,
        ]);
    }
    return $ret;
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
