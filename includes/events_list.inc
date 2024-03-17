<?php

function budatoll_manage_events_list() {
    global $wpdb;
    $ret = '<H3>Alkalmak listája</H3>';
    $ret .= '<form id="BudatollForm" method="post"  enctype="multipart/form-data" autocomplete="on">';
    $edit_id = getPostVariable('budatoll_edit', -1);
    $ret .= putHiddenValue('budatoll_edit', $edit_id);
    $delete_id = getPostVariable('budatoll_delete', false);
    $save = getPostVariable('budatoll_save', false);
    if (getPostVariable('budatoll_cancel', false)) {
        $edit_id = -1;
    }
    if (getPostVariable('budatoll_add', false)) {
        $edit_id = 0;
    }
    if ($delete_id) {
        if ($wpdb->delete(BUDATOLL_EVENTS_TABLE, [BUDATOLL_EVENTS_ID => $delete_id]) > 0) {
            logger(BUDATOLL_LOGGER_TYPE_EVENTS, -1, 'Alkalom törölve: ');
        }
    }
    if ($save) {
        $data = [
            BUDATOLL_EVENTS_SHORT => getPostVariable('budatoll-short', ''),
            BUDATOLL_EVENTS_LONG => getPostVariable('budatoll-long', ''),
            BUDATOLL_EVENTS_PRICE => getPostVariable('budatoll-price', 0),
            BUDATOLL_EVENTS_START => getPostVariable('budatoll-start', '00:00'),
            BUDATOLL_EVENTS_END => getPostVariable('budatoll-end', '00:00'),
            BUDATOLL_EVENTS_MAX_PLAYERS => getPostVariable('budatoll-max', 0),
        ];
        if ($edit_id > 0) {
            if ($wpdb->update(BUDATOLL_EVENTS_TABLE, $data, [BUDATOLL_EVENTS_ID => $edit_id])) {
                logger(BUDATOLL_LOGGER_TYPE_EVENTS, $edit_id, 'Alkalom módosítva: ' . print_r($data, true));
            }
        } else {
            if ($wpdb->insert(BUDATOLL_EVENTS_TABLE, $data)) {
                logger(BUDATOLL_LOGGER_TYPE_EVENTS, $wpdb->insert_id, 'Alkalom létrehozva: ' . print_r($data, true));
            }
        }
        $edit_id = -1;
    }
    if ($edit_id >= 0) {
        $ret .= budatollEventEdit($edit_id);
    }
    $ret .= budatollEventsList();
    $ret .= '</form>';

    return $ret;
}

function budatollEventEdit($item_id) {
    global $wpdb; // <div class="shadow p-1 m-2 rounded">
    $ret = '<div class="container-fluid"><div class="budatoll-keret"><div class="budatoll-editor">';
    $short = '';
    $long = '';
    $price = 0;
    $start = '00:00';
    $end = '00:00';
    $max_players = 0;
    if ($item_id > 0) {
        $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENT_TYPE_ID . '`=' . $item_id . ';';
        $results = $wpdb->get_results($sql);
        $short = $results[0]->{BUDATOLL_EVENT_TYPE_SHORT} ?? '';
        $long = $results[0]->{BUDATOLL_EVENT_TYPE_LONG} ?? '';
        $start = $results[0]->{BUDATOLL_EVENT_TYPE_START} ?? '00:00';
        $end = $results[0]->{BUDATOLL_EVENT_TYPE_END} ?? '00:00';
        $price = $results[0]->{BUDATOLL_EVENT_TYPE_PRICE} ?? 0;
        $max_players = $results[0]->{BUDATOLL_EVENT_TYPE_MAX_PLAYERS} ?? 0;
    }
    $ret .= '<div class="budatoll-editor-group"><label>Rövid név:</label>'
            . '<input  type="text" id="budatoll-short" name="budatoll-short" value="' . $short . '"></div>';
    $ret .= '<div class="budatoll-editor-group"><label>Hosszú név:</label>'
            . '<input type="text" id="budatoll-long" name="budatoll-long" value="' . $long . '"></div>';
    $ret .= '<div class="budatoll-editor-group"><label>Kezdés:</label>'
            . '<input  type="time" id="budatoll-start" name="budatoll-start" value="' . $start . '"></div>';
    $ret .= '<div class="budatoll-editor-group"><label>Vége:</label>'
            . '<input  type="time" id="budatoll-end" name="budatoll-end" value="' . $end . '"></div>';
    $ret .= '<div class="budatoll-editor-group"><label>Ár:</label>'
            . '<input type="number" id="budatoll-price" name="budatoll-price" value="' . $price . '"></div>';
    $ret .= '<div class="budatoll-editor-group"><label>Max játékos:</label>'
            . '<input type="number" id="budatoll-max" name="budatoll-max" value="' . $max_players . '"></div>';
    $ret .= '</div>';
    $ret .= '<button class="button budatoll-button" name="budatoll_save"  id="budatoll_save" value="-1" title="Mentés">'
            . '<span class="dashicons dashicons-saved"></span></button>';
    $ret .= '<button class="button budatoll-button" name="budatoll_cancel" id="budatoll_cancel" value="-1" title="Mégse">'
            . '<span class="dashicons dashicons-undo"></span></button></div>';
    $ret .= '</div></div>';
    return $ret;
}

function budatollEventsList() {
    global $wpdb;
    $events_from = getPostVariable('events_from', date('Y-m-01'));
    $events_to = getPostVariable('events_to', date('Y-m-t'));
    $event_type_id = getPostVariable('bt_event_type', -1);
    $event_type_year = date("Y", strtotime($events_from));
    $event_type_month = date("m", strtotime($events_from));
    putScript('$(function() {'
            . '$("#events_from").datepicker({dateFormat: "yyyy-mm-dd" });'
            . '$("#events_to").datepicker({dateFormat: "yyyy-mm-dd" });'
            . '});');

    if (getPostVariable('month_left')) {
        if ($event_type_month > 1) {
            $event_type_month--;
        } else {
            $event_type_year--;
            $event_type_month = 12;
        }
        $events_from = date(sprintf("%04d", $event_type_year) . '-' . sprintf("%02d", $event_type_month) . '-01');
        $events_to = date('Y-m-t', strtotime($events_from));
    }
    if (getPostVariable('month_right')) {
        if ($event_type_month < 12) {
            $event_type_month++;
        } else {
            $event_type_year++;
            $event_type_month = 1;
        }
        $events_from = date(sprintf("%04d", $event_type_year) . '-' . sprintf("%02d", $event_type_month) . '-01');
        $events_to = date('Y-m-t', strtotime($events_from));
    }
    $ret .= '<input type="hidden" name="event_type_year" id="event_type_year" value="' . $event_type_year . '">';
    $ret .= '<input type="hidden" name="event_type_month" id="event_type_month" value="' . $event_type_month . '">';
    //   $ret .= 'Year:' . $event_type_year . ', Month: ' . $event_type_month;
    $ret = '<div class="budatoll-keret budatoll-search">';
    $ret .= '<div class="budatoll-search-group"><button class="button budatoll-button" name="budatoll_add" id="budatoll_add" value="-1" title="Új hozzáadása">'
            . '<span class="dashicons dashicons-plus"></span></button></div>';
    $ret .= '<div class="budatoll-search-group"><label for="bt_event_type">Edzés típus:</label>';
    $ret .= '<select class="budatoll-dropdown" id="bt_event_type" name="bt_event_type" onChange="submit();" >';
    $ret .= '<option value="-1" ' . ($event_type_id == -1 ? 'selected' : '') . '>Összes</option>';
    foreach (btGetEventTypes() as $event_type) {
        $et_id = $event_type->{BUDATOLL_EVENT_TYPE_ID};
        $ret .= '<option value="' . $et_id . '" ' . ($event_type_id == $et_id ? 'selected' : '') . '>' . $event_type->{BUDATOLL_EVENT_TYPE_SHORT} . '</option>';
    }
    $ret .= '</select></div>';
    $ret .= '<div class="budatoll-search-group"><label for="events_from">Időszak:</label>'
            . '<input type="date" id="events_from" name="events_from" value="' . $events_from . '" onchange="submit();"> - '
            . '<input type="date" id="events_to" name="events_to" value="' . $events_to . '" onchange="submit();"></div>';
    $ret .= '<div class="budatoll-search-group"><button class="button budatoll-button" name="month_left" id="month_left" title="Előző hónap" value="-1">'
            . '<span class="dashicons dashicons-arrow-left"></span></button>';
    $ret .= $event_type_year . ' ' . strftime("%B", strtotime($events_from));
    $ret .= '<button class="button budatoll-button" name="month_right" id="month_right" title="Következő hónap" value="-1">'
            . '<span class="dashicons dashicons-arrow-right"></span></button></div>';
    $ret .= '</div>';
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENTS_DAY . '` BETWEEN "' . $events_from . '" AND "' . $events_to . '" '
            . ($event_type_id >= 0 ? (' AND `' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=' . $event_type_id) : '')
            . ' ORDER BY `' . BUDATOLL_EVENTS_DAY . '`,`' . BUDATOLL_EVENTS_START . '` ;';

    $results = $wpdb->get_results($sql);
    $ret .= '<table class="table table-bordered budatoll-table">';
    $ret .= '<thead><tr><th>Rövid név</th><th>Dátum</th><th>Kezdés</th><th>Befejezés</th><th>Létszám</th><th>Művelet</th></tr></thead><tbody>';

    foreach ($results as $row) {
        $id = $row->{BUDATOLL_EVENTS_ID};
        $short = $row->{BUDATOLL_EVENTS_SHORT} ?? '';
        $day = $row->{BUDATOLL_EVENTS_DAY} ?? '';
        $day_of_week = strftime('%A', strtotime($day));
        $start = substr(($row->{BUDATOLL_EVENTS_START} ?? '00:00'), 0, 5);
        $end = substr(($row->{BUDATOLL_EVENTS_END} ?? '00:00'), 0, 5);
        $max_players = $row->{BUDATOLL_EVENTS_MAX_PLAYERS} ?? 0;
        $ret .= '<tr class="budatoll-table-row" id="budatoll-popup_' . $id . '" data-item="' . $id . '">';
        $ret .= '<td>' . $short . '</td>';
        $ret .= '<td>' . $day . ' (' . $day_of_week . ')</td>';
        $ret .= '<td>' . $start . ' Ft</td>';
        $ret .= '<td>' . $end . '</td>';
        $ret .= '<td>' . $max_players . '</td>';
        $disabled = bt_is_deletable($id, [BUDATOLL_TRAINING_TABLE => BUDATOLL_TRAINING_EVENT_ID]) ? '' : ' disabled';
        $ret .= '<td class="form-inline"><button class="button budatoll-button" name="budatoll_edit" id="budatoll_edit" value="' . $id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button>'
                . '<button class="button budatoll-button" name="budatoll_delete" id="budatoll_delete" value="' . $id . '" ' . $disabled
                . 'onclick="return confirm(\'Biztosan törölni akarod a \"' . $row->{BUDATOLL_EVENTS_LONG} . '\" nevű edzési időpontot?\');" title="Törlés">'
                . '<span class="dashicons dashicons-trash"></span></button>';
        $ret .= '</td></tr>';
    }
    $ret .= '</tbody></table>';
    return $ret;
}

function bt_get_event_by_id($event_id) {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENT_TYPE_ID . '`=' . $event_id . ';';
    $results = $wpdb->get_results($sql);
    if (empty($results)) {
        return false;
    } else {
        return $results[0];
    }
}
