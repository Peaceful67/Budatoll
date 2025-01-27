<?php

if (!defined('ABSPATH')) {
    die();
}

function budatoll_manage_events_calendar() {
    $ret = '<h4>Alkalmak naptár szerint</h4>';
    $ret .= '<div id="calendar-wrap">'
            . '<div id="budatoll-alkalom-calendar">'
            . '</div></div>';
    return $ret;
}

function budatoll_list_event_types() {
    $ret = '<div id="external-events"><H4>Edzés típusok</H4>';
    $ret .= '<div id="event-types-list">';
    $event_types = btGetEventTypes();

    foreach ($event_types as $event) {
        $id = $event->{BUDATOLL_EVENT_TYPE_ID};
        $short = $event->{BUDATOLL_EVENT_TYPE_SHORT};
        $long = $event->{BUDATOLL_EVENT_TYPE_LONG};
        $start = $event->{BUDATOLL_EVENT_TYPE_START};
        $end = $event->{BUDATOLL_EVENT_TYPE_END};
        $ret .= '<div class="fc-event  fc-h-event fc-daygrid-event fc-daygrid-block-event" '
                . 'data-type=\'{"start":"' . $start . '", "end":"' . $end . '" , "type_id":"' . $id . '"}\'>'
                . '<div class="fc-event-main" id="type-' . $id . '">' . $short
                . '</div></div>';
    }
    $ret .= '</div></div>';
    $ret .= '<div id="budatoll-event-info" class="budatoll-popup">'
            . '<div class="budatoll-popup-content">'
            . '</div></div>';
    $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-message"></div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="error-message">Mentés sikertelen</div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="already-message">Ilyen edzés már van.</div>';
    return $ret;
}

function get_event_by_id($event_id) {
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENTS_ID . '`=' . $event_id . ';';
    $results = bt_get_results($sql);
    if (empty($results)) {
        return false;
    } else {
        return $results[0];
    }
}
