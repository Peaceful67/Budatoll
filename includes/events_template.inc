<?php

if (!defined('ABSPATH')) {
    die();
}

define("BUDATOLL_OPTION_EVENTS_TEMPLATE", 'events_template');

function budatoll_events_template() { // Megjeleníti a naptárat
    $ret = 'Naptár sablon';
    $ret .= '<div id="calendar-wrap"><div id="budatoll-template-info" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>'
            . '<div id="budatoll-events-template">'
            . '</div></div>';
    return $ret;
}

function bt_get_events_template() { // Visszaadja a sablon eseményeit
    $templates = unserialize(get_option(BUDATOLL_OPTION_EVENTS_TEMPLATE));
//    error_log('Templates: ' . print_r($templates, true));
    return json_encode([
        'result' => 'success',
        'events' => $templates ? $templates : [],
    ]);
}

function bt_event_template_insert($type_id, $weekday) {
    $events = unserialize(get_option(BUDATOLL_OPTION_EVENTS_TEMPLATE));
    $new_template = get_event_type_by_id($type_id);
    $found = -1;
    $max_id = -1;
    if ($events && !empty($events)) {
        foreach ($events as $key => $event) {  // Megnézzük, hogy van-e már ilyen ezen a napon
            $max_id = max($max_id, $event[BUDATOLL_EVENTS_TEMPLATE_ID]);
            if ($event[BUDATOLL_EVENTS_TEMPLATE_TYPE_ID] == $type_id AND $event[BUDATOLL_EVENTS_TEMPLATE_DAY] == $weekday) {
                $found = $key;
            }
        }
    }
    if ($found >= 0) { // Ha már volt ilyen, kitöröljük
        unset($events[$found]);
    }
    $template_id = $max_id + 1;
    $events[] = [
        BUDATOLL_EVENTS_TEMPLATE_ID => $template_id,
        BUDATOLL_EVENTS_TEMPLATE_TYPE_ID => $type_id,
        BUDATOLL_EVENTS_TEMPLATE_SHORT => $new_template->{BUDATOLL_EVENT_TYPE_SHORT},
        BUDATOLL_EVENTS_TEMPLATE_LONG => $new_template->{BUDATOLL_EVENT_TYPE_LONG},
        BUDATOLL_EVENTS_TEMPLATE_DAY => $weekday,
        BUDATOLL_EVENTS_TEMPLATE_START => bt_time($new_template->{BUDATOLL_EVENT_TYPE_START}),
        BUDATOLL_EVENTS_TEMPLATE_END => bt_time($new_template->{BUDATOLL_EVENT_TYPE_END}),
    ];
    bt_logger(BUDATOLL_LOGGER_TYPE_EVENT_TEMPLATE, -1, 'Új alkalom sablon felvéve.');
    update_option(BUDATOLL_OPTION_EVENTS_TEMPLATE, serialize($events));
    return ['template_id' => $template_id, 'event' => $new_template,];
}

function bt_event_template_delete($template_id) {
    $events = unserialize(get_option(BUDATOLL_OPTION_EVENTS_TEMPLATE));
    if (!isset($events[$template_id])) {
        return false;
    }
    unset($events[$template_id]);
    bt_logger(BUDATOLL_LOGGER_TYPE_EVENT_TEMPLATE, -1, 'Új alkalom sablon törölve.');
    update_option(BUDATOLL_OPTION_EVENTS_TEMPLATE, serialize($events));
    return true;
}

function bt_event_template_move($template_id, $weekday, $start, $end) {  //true, ha sikerült
    $events = unserialize(get_option(BUDATOLL_OPTION_EVENTS_TEMPLATE));
    if (!isset($events[$template_id])) {
        return false;
    }
    $events[$template_id][BUDATOLL_EVENTS_TEMPLATE_DAY] = $weekday;
    $events[$template_id][BUDATOLL_EVENTS_TEMPLATE_START] = $start;
    $events[$template_id][BUDATOLL_EVENTS_TEMPLATE_END] = $end;
    bt_logger(BUDATOLL_LOGGER_TYPE_EVENT_TEMPLATE, -1, 'Alkalom sablon módosítva.');
    update_option(BUDATOLL_OPTION_EVENTS_TEMPLATE, serialize($events));
    return true;
}

function bt_find_template_id($template_id) {
    $templates = unserialize(get_option(BUDATOLL_OPTION_EVENTS_TEMPLATE));
    if ($templates && !empty($templates)) {
        foreach ($templates as $key => $template) {  // Megnézzük, hogy van-e már ilyen ezen a napon
            if ($template[BUDATOLL_EVENTS_TEMPLATE_ID] == $template_id) {
                return $key;
            }
        }
    }
    return false;
}

function bt_copy_templates($start_date) {
    $templates = unserialize(get_option(BUDATOLL_OPTION_EVENTS_TEMPLATE));
    $total_templates = count($templates);
    $copied_templates = 0;
    $monday = new DateTime(preg_replace('/\s*\(.*\)$/', '', $start_date));
    if ($templates && !empty($templates)) {
        foreach ($templates as $template) {
            $day = max(0, $template[BUDATOLL_EVENTS_TEMPLATE_DAY] - 1);
            $current_day = clone $monday;
            $current_day->modify('+' . $day . ' days');
            $event = json_decode(bt_add_event($template[BUDATOLL_EVENTS_TEMPLATE_TYPE_ID], $current_day->format('Y-m-d')));
//            error_log('Copied event:' . print_r($event, true));
            if ($event->result == 'success') {
                $copied_templates++;
            }
        }
        $ret = ['total' => $total_templates, 'copied' => $copied_templates];
    } else {
        $ret = ['total' => 0, 'copied' => 0];
    }
//    error_log('copy: ' . print_r($ret, true));
    return $ret;
}
