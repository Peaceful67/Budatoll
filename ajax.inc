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
        default :
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

function bt_get_events_range($from, $to) {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE  `' . BUDATOLL_EVENTS_DAY . '` BETWEEN "' . $from . '" AND "' . $to . '";';
    $res = $wpdb->get_results($sql);
    $ret = [
        'result' => 'success',
        'events' => $res,
        'sql' => $sql,
    ];
    return json_encode($ret);
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
