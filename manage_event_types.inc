<?php

function budatoll_manage_event_types() {
    global $wpdb;

    $ret = '<form id="BudatollForm" method="post">';
    $ret .= print_r($_POST, true);
    $edit_id = filter_input(INPUT_POST, 'budatoll_edit') ?? -1;
    $delete_id = filter_input(INPUT_POST, 'budatoll_delete');
    $save = filter_input(INPUT_POST, 'budatoll_save');
    if (filter_input(INPUT_POST, 'budatoll_cancel')) {
        $edit_id = -1;
    }
    $clicked_row = filter_input(INPUT_POST, 'clicked_row') ?? -1;
    if (filter_input(INPUT_POST, 'budatoll_add')) {
        $edit_id = 0;
    }
    if ($delete_id) {
        $wpdb->delete(BUDATOLL_EVENT_TYPE_TABLE, [BUDATOLL_EVENT_TYPE_ID => $delete_id]);
    }
    if ($save) {
        $data = [
            BUDATOLL_EVENT_TYPE_SHORT => (filter_input(INPUT_POST, 'budatoll-short') ?? ''),
            BUDATOLL_EVENT_TYPE_LONG => (filter_input(INPUT_POST, 'budatoll-long') ?? ''),
            BUDATOLL_EVENT_TYPE_PRICE => (filter_input(INPUT_POST, 'budatoll-price') ?? 0),
            BUDATOLL_EVENT_TYPE_START => (filter_input(INPUT_POST, 'budatoll-start') ?? '00:00'),
            BUDATOLL_EVENT_TYPE_END => (filter_input(INPUT_POST, 'budatoll-end') ?? '00:00'),
            BUDATOLL_EVENT_TYPE_MAX_PLAYERS => (filter_input(INPUT_POST, 'budatoll-max') ?? 0),
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
        $ret .= budatollEventTypeEdit($edit_id);
    } elseif ($clicked_row > 0) { // Modal of clicked item
        $ret .= budatollEventTypeModal($clicked_row);
    }
    $ret .= budatollEventTypeList();
    $ret .= '<input type="hidden" name="budatoll_edit" id="budatoll_edit" value="' . $edit_id . '">';
    $ret .= '<input type="hidden" name="clicked_row" id="clicked_row" value="' . $clicked_row . '">';
    $ret .= '</form>';
    return $ret;
}

function budatollEventTypeModal($item_id) {
    global $wpdb;
    $ret = '';
    $results = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENT_TYPE_TABLE . '` WHERE `' . BUDATOLL_EVENT_TYPE_ID . '`=' . $item_id . ';');
    $short = $results[0]->{BUDATOLL_EVENT_TYPE_SHORT} ?? '';
    $long = $results[0]->{BUDATOLL_EVENT_TYPE_LONG} ?? '';
    $start = $results[0]->{BUDATOLL_EVENT_TYPE_START} ?? '00:00';
    $end = $results[0]->{BUDATOLL_EVENT_TYPE_END} ?? '00:00';
    $price = $results[0]->{BUDATOLL_EVENT_TYPE_PRICE} ?? 0;
    $max_players = $results[0]->{BUDATOLL_EVENT_TYPE_MAX_PLAYERS} ?? 0;
    $ret .= budatollNewModal('event_type', 'Edzés típus', 'lg');
    $ret .= '<div class="row"><label>Rövid név:</label>' . $short . '</div>';
    $ret .= '<div class="row"><label>Hosszú név:</label>' . $long . '</div>';
    $ret .= '<div class="row"><label>Kezdés:</label>' . $start . '</div>';
    $ret .= '<div class="row"><label>Vége:</label>' . $end . '</div>';
    $ret .= '<div class="row"><label>Ár:</label>' . number_format($price, 0, ',', '.') . ' Ft</div>';
    $ret .= '<div class="row"><label>Max játékos:</label>' . ($max_players > 0 ? $max_players : 'Nincs korlátozva') . '</div>';
    $ret .= budatollNewModalFooter();
    $ret .= budatollEndModal();
    return $ret;
}

function budatollEventTypeEdit($item_id) {
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
    $ret .= '<div class="row"><label class="col-6">Rövid név:'
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
    $ret .= '<div class="form-inline"><button name="budatoll_save" value="' . $item_id . '" title="Mentés"><span class="dashicons dashicons-saved"></span></button>';
    $ret .= '<button name="budatoll_cancel" id="budatoll_cancel" value="-1" title="Mégse"><span class="dashicons dashicons-undo"></span></button></div>';
    $ret .= '</div></div></div>';
    return $ret;
}

function budatollEventTypeList() {
    global $wpdb;
    $ret = '<H4>Edzés típusok</H4>';
    $results = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENT_TYPE_TABLE . '`;');
    $ret .= '<table class="table table-striped table-bordered budatoll-table">';
    $ret .= '<button class="button budatoll-button" name="budatoll_add" value="-1" title="Új hozzáadása"><span class="dashicons dashicons-plus"></span></button>';
    $ret .= '<thead><tr><th>Rövid név</th><th>Rövid név</th><th>Ár</th><th>Kezdet</th><th>Vég</th><th>Művelet</th></tr></thead><tbody>';
    foreach ($results as $row) {
        $id = $row->{BUDATOLL_EVENT_TYPE_ID};
        $ret .= '<tr class="clickable-row" data-itemm="' . $id . '">';
        $ret .= '<td>' . $row->{BUDATOLL_EVENT_TYPE_SHORT} . '</td>';
        $ret .= '<td>' . $row->{BUDATOLL_EVENT_TYPE_LONG} . '</td>';
        $ret .= '<td>' . number_format($row->{BUDATOLL_EVENT_TYPE_PRICE}, 0, ',', '.') . ' Ft</td>';
        $ret .= '<td>' . substr($row->{BUDATOLL_EVENT_TYPE_START}, 0, 5) . '</td>';
        $ret .= '<td>' . substr($row->{BUDATOLL_EVENT_TYPE_END}, 0, 5) . '</td>';
        $ret .= '<td class="form-inline"><button class="button budatoll-button" name="budatoll_edit" id="budatoll_edit" value="' . $id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button>'
                . '<button class="button budatoll-button" name="budatoll_delete" id="budatoll_delete" value="' . $id . '" '
                . 'onclick="return confirm(\'Biztosan törölni akarod a \"' . $row->{BUDATOLL_EVENT_TYPE_LONG} . '\" nevű edzést?\');" title="Törlés"><span class="dashicons dashicons-trash"></span></button></td>';
        $ret .= '</tr>';
    }
    $ret .= '</tbody></table>';
    return $ret;
}
