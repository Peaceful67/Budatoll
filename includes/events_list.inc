<?php

function budatoll_manage_events_list() {
    global $wpdb;
    $ret = '<H3>Edzések listája</H3>';
    //   $ret .= print_r($_POST, true);
    $ret .= '<form id="BudatollForm" method="post" enctype="multipart/form-data" autocomplete="on">';
    $edit_id = getPostVariable('budatoll_edit', -1);
    $delete_id = getPostVariable('budatoll_delete', false);
    $save = getPostVariable('budatoll_save', false);
    if (getPostVariable('budatoll_cancel', false)) {
        $edit_id = -1;
    }
    $clicked_row = getPostVariable('budatoll_clicked_row', -1);
    if (getPostVariable('budatoll_add', false)) {
        $edit_id = 0;
    }
    $ret .= '<input type="hidden" name="budatoll_edit" id="budatoll_edit" value="' . $edit_id . '">';
    $ret .= '<input type="hidden" name="budatoll_clicked_row" id="budatoll_clicked_row" value="' . $clicked_row . '">';
    if ($delete_id) {
        $wpdb->delete(BUDATOLL_EVENT_TYPE_TABLE, [BUDATOLL_EVENT_TYPE_ID => $delete_id]);
    }
    if ($save) {
        $data = [
            BUDATOLL_EVENT_TYPE_SHORT => getPostVariable('budatoll-short', ''),
            BUDATOLL_EVENT_TYPE_LONG => getPostVariable('budatoll-long', ''),
            BUDATOLL_EVENT_TYPE_PRICE => getPostVariable('budatoll-price', 0),
            BUDATOLL_EVENT_TYPE_START => getPostVariable('budatoll-start', '00:00'),
            BUDATOLL_EVENT_TYPE_END => getPostVariable('budatoll-end', '00:00'),
            BUDATOLL_EVENT_TYPE_MAX_PLAYERS => getPostVariable('budatoll-max', 0),
        ];

        if ($edit_id > 0) {
            $wpdb->update(BUDATOLL_EVENT_TYPE_TABLE, $data, [BUDATOLL_EVENT_TYPE_ID => $edit_id]);
        } else {
            $wpdb->insert(BUDATOLL_EVENT_TYPE_TABLE, $data);
        }
        $edit_id = -1;
    }
    if ($edit_id >= 0) {
        $clicked_row = -1;
        $ret .= budatollEventEdit($edit_id);
    }
    $ret .= budatollEventsList();
    $ret .= '</form>';

    return $ret;
}

function budatollEventEdit($item_id) {
    global $wpdb;
    $ret = '<div class="container-fluid"><div class="shadow p-1 m-2 rounded"><div class="budatoll-keret budatoll-editor">';
    $short = '';
    $long = '';
    $price = 0;
    $start = '00:00';
    $end = '00:00';
    $max_players = 0;
    if ($item_id > 0) {
        $results = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENT_TYPE_TABLE . '` WHERE `' . BUDATOLL_EVENT_TYPE_ID . '`=' . $item_id . ';');
        $short = $results[0]->{BUDATOLL_EVENT_TYPE_SHORT} ?? '';
        $long = $results[0]->{BUDATOLL_EVENT_TYPE_LONG} ?? '';
        $start = $results[0]->{BUDATOLL_EVENT_TYPE_START} ?? '00:00';
        $end = $results[0]->{BUDATOLL_EVENT_TYPE_END} ?? '00:00';
        $price = $results[0]->{BUDATOLL_EVENT_TYPE_PRICE} ?? 0;
        $max_players = $results[0]->{BUDATOLL_EVENT_TYPE_MAX_PLAYERS} ?? 0;
    }
    $ret .= '<div class="et_pb_row"><label class="col-6">Rövid név:'
            . '<input  type="text" id="budatoll-short" name="budatoll-short" value="' . $short . '"></label>';
    $ret .= '<label class="col-6">Hosszú név:'
            . '<input type="text" id="budatoll-long" name="budatoll-long" value="' . $long . '"></label></div>';
    $ret .= '<div class="form-inline"><label>Kezdés:'
            . '<input  type="time" id="budatoll-start" name="budatoll-start" value="' . $start . '"></label>';
    $ret .= '<label>Vége:'
            . '<input  type="time" id="budatoll-end" name="budatoll-end" value="' . $end . '"></label></div>';
    $ret .= '<div class="form-inline"><label>Ár:'
            . '<input type="number" id="budatoll-price" name="budatoll-price" value="' . $price . '"> Ft</label>';
    $ret .= '<label>Max játékos:'
            . '<input type="number" id="budatoll-max" name="budatoll-max" value="' . $max_players . '"></label></div>';
    $ret .= '<div class="form-inline"><button class="button budatoll-button" name="budatoll_save"  id="budatoll_save" value="-1" title="Mentés">'
            . '<span class="dashicons dashicons-saved"></span></button>';
    $ret .= '<button class="button budatoll-button" name="budatoll_cancel" id="budatoll_cancel" value="-1" title="Mégse"><span class="dashicons dashicons-undo"></span></button></div>';
    $ret .= '</div></div></div>';
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
    $ret = '<table class="table" style="border:none;">';
    $ret .= '<tr><td style="width:10%;">';
    $ret .= '<button class="button budatoll-button" name="budatoll_add" id="budatoll_add" value="-1" title="Új hozzáadása">'
            . '<span class="dashicons dashicons-plus"></span></button>';
    $ret .= '</td><td style="30%;">';
    $ret .= '<label for="bt_event_type">Edzés típus:</label>';
    $ret .= '<select class="budatoll-dropdown" id="bt_event_type" name="bt_event_type" onChange="submit();" >';
    $ret .= '<option value="-1" ' . ($event_type_id == -1 ? 'selected' : '') . '>Összes</option>';
    foreach (btGetEventTypes() as $event_type) {
        $et_id = $event_type->{BUDATOLL_EVENT_TYPE_ID};
        $ret .= '<option value="' . $et_id . '" ' . ($event_type_id == $et_id ? 'selected' : '') . '>' . $event_type->{BUDATOLL_EVENT_TYPE_SHORT} . '</option>';
    }
    $ret .= '</select>';
    $ret .= '</td><td style="30%;">';
    $ret .= '<label for="events_from">Időszak:</label>'
            . '<input type="date" id="events_from" name="events_from" value="' . $events_from . '" onchange="submit();"> - '
            . '<input type="date" id="events_to" name="events_to" value="' . $events_to . '" onchange="submit();">';
    $ret .= '</td><td style="30%;">';
    $ret .= '<button class="button budatoll-button" name="month_left" id="month_left" title="Előző hónap" value="-1">'
            . '<span class="dashicons dashicons-arrow-left"></span></button>';
    $ret .= $event_type_year . ' ' . strftime("%B", strtotime($events_from));
    $ret .= '<button class="button budatoll-button" name="month_right" id="month_right" title="Következő hónap" value="-1">'
            . '<span class="dashicons dashicons-arrow-right"></span></button>';
    $ret .= '</td></tr>';
    $ret .= '</table>';
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENTS_DAY . '` BETWEEN "' . $events_from . '" AND "' . $events_to . '" '
            . ($event_type_id >= 0 ? (' AND `' . BUDATOLL_EVENTS_EVENT_TYPE_ID . '`=' . $event_type_id) : '')
            . ' ORDER BY `' . BUDATOLL_EVENTS_DAY . '`,`' . BUDATOLL_EVENTS_START . '` ;';

    $results = $wpdb->get_results($sql);
    $ret .= '<table class="table table-striped table-bordered budatoll-table">';
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
        $ret .= '<td class="form-inline"><button class="button budatoll-button" name="budatoll_edit" id="budatoll_edit" value="' . $id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button>'
                . '<button class="button budatoll-button" name="budatoll_delete" id="budatoll_delete" value="' . $id . '" '
                . 'onclick="return confirm(\'Biztosan törölni akarod a \"' . $row->{BUDATOLL_EVENTS_LONG} . '\" nevű edzési időpontot?\');" title="Törlés">'
                . '<span class="dashicons dashicons-trash"></span></button>';
        $ret .= '</td></tr>';
    }
    $ret .= '</tbody></table>';
    return $ret;
}
