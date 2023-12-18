<?php

function budatoll_manage_event_types() {
    global $wpdb;

    $ret = '<form id="BudatollForm" method="post" enctype="multipart/form-data" autocomplete="on">';
    $ret .= '<input type="hidden" name="page" value="edzes-tipusok">';
    $edit_id = getPostVariable('budatoll_edit', -1);
    $delete_id = getPostVariable('budatoll_delete', false);
    $save = getPostVariable('budatoll_save', false);
    if (getPostVariable('budatoll_cancel', false)) {
        $edit_id = -1;
    }
    if (getPostVariable('budatoll_add', false)) {
        $edit_id = 0;
    }
    $ret .= '<input type="hidden" name="budatoll_edit" id="budatoll_edit" value="' . $edit_id . '">';
    if ($delete_id) {
        logger(BUDATOLL_LOGGER_TYPE_EVENT_TYPE, -1, 'Edzés típus törlése: '.$delete_id);
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
            logger(BUDATOLL_LOGGER_TYPE_EVENT_TYPE, -1, 'Edzés típus módosítása:'.$edit_id, $data);
        } else {
            $edit_id = $wpdb->insert(BUDATOLL_EVENT_TYPE_TABLE, $data);
            logger(BUDATOLL_LOGGER_TYPE_EVENT_TYPE, -1, 'Edzés típus létrehozása:'.$edit_id, $data);
        }
        $edit_id = -1;
    }
    if ($edit_id >= 0) {
        $ret .= btEventTypeEdit($edit_id);
    }
    $ret .= btEventTypeList();
    $ret .= '</form>';
    echo $ret;
}

function btEventTypeEdit($item_id) {
    global $wpdb;
    $ret = '<div class="wrap"><div class="budatoll-keret">';
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
    $ret .= '<table class="form-table"><tbody>';
    $ret .= '<tr><th scope="row"><label>Rövid név:</label></th>'
            . '<td><input  type="text" id="budatoll-short" name="budatoll-short" value="' . $short . '"></td></tr>';
    $ret .= '<tr><th scope="row"><label>Hosszú név:</th>'
            . '<td><input type="text" id="budatoll-long" name="budatoll-long" value="' . $long . '"></td></tr>';
    $ret .= '<tr><th scope="row"><label>Idősáv:</th>'
            . '<td><input  type="time" id="budatoll-start" name="budatoll-start" value="' . $start . '">';
    $ret .= '<input  type="time" id="budatoll-end" name="budatoll-end" value="' . $end . '"></td></tr>';
    $ret .= '<tr><th scope="row"><label>Ár:</th>'
            . '<td><input type="number" id="budatoll-price" name="budatoll-price" value="' . $price . '"> Ft</td></tr>';
    $ret .= '<tr><th scope="row"><label>Max játékos:</th>'
            . '<td><input type="number" id="budatoll-max" name="budatoll-max" value="' . $max_players . '"></td></tr>';
    $ret .= '</tbody></table>';
    $ret .= get_submit_button('Mentés', 'primary small', 'budatoll_save');
    $ret .= '</div></div>';
   

    return $ret;
}

function btEventTypeList() {
    $ret = '<H3>Edzés típusok listája</H3>';
    $results = btGetEventTypes();
    $ret .= '<div class="wrap">'
            . '<button class="button button-primary button-large" name="budatoll_add" id="budatoll_add" value="-1" title="Új hozzáadása">'
            . '<span class="dashicons dashicons-plus"></span>'
            . '</button>';
    $ret .= '<table class="wp-list-table widefat fixed striped">';
    $ret .= '<thead><tr>'
            . '<th scope="col" id="column1" class="manage-column column-column1" style="width:120px;">Rövid név</th>'
            . '<th scope="col" id="column2" class="manage-column column-column2" style="width:280px;">Hosszú név</th>'
            . '<th scope="col" id="column3" class="manage-column column-column3" style="width:80px;">Ár</th>'
            . '<th scope="col" id="column4" class="manage-column column-column4" style="width:80px;">Idősáv</th>'
            . '<th scope="col" id="column5" class="manage-column column-column5" style="width:80px;">Max játékos</th>'
            . '<th scope="col" id="column6" class="manage-column column-column6" style="width:80px;">Művelet</th>'
            . '</tr></thead><tbody>';

    foreach ($results as $row) {
        $id = $row->{BUDATOLL_EVENT_TYPE_ID};
        $short = $row->{BUDATOLL_EVENT_TYPE_SHORT} ?? '';
        $long = $row->{BUDATOLL_EVENT_TYPE_LONG} ?? '';
        $start = substr(($row->{BUDATOLL_EVENT_TYPE_START} ?? '00:00'), 0, 5);
        $end = substr(($row->{BUDATOLL_EVENT_TYPE_END} ?? '00:00'), 0, 5);
        $price = number_format(($row->{BUDATOLL_EVENT_TYPE_PRICE} ?? 0), 0, ',', '.');
        $max = $row->{BUDATOLL_EVENT_TYPE_MAX_PLAYERS} ?? 0;
        //        $ret .= '<tr class="clickable-row" id="budatoll-popup_' . $id . '" data-item="' . $id . '">';
        $ret .= '<tr>';
        $ret .= '<td class="column-column1">' . $short . '</td>';
        $ret .= '<td class="column-column2">' . $long . '</td>';
        $ret .= '<td class="column-column3">' . $price . ' Ft</td>';
        $ret .= '<td class="column-column4">' . $start . ' - ' . $end . '</td>';
        $ret .= '<td class="column-column5">' . $max . '</td>';
        $ret .= '<td class="column-column6"><button class="button button-link button-small" name="budatoll_edit" id="budatoll_edit" value="' . $id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button>'
                . '<button class="button button-action button-small" name="budatoll_delete" id="budatoll_delete" value="' . $id . '" '
                . 'onclick="return confirm(\'Biztosan törölni akarod a \"' . $row->{BUDATOLL_EVENT_TYPE_LONG} . '\" nevű edzést?\');" title="Törlés">'
                . '<span class="dashicons dashicons-trash"></span></button>';
        $ret .= '</td></tr>';
    }
    $ret .= '</tbody></table></div>';
    return $ret;
}

function btGetEventTypes() {
    global $wpdb;
    return $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENT_TYPE_TABLE . '`;');
}

function btGetEventTypeById($id) {
    global $wpdb;
    $result = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_EVENT_TYPE_TABLE . '` WHERE `' . BUDATOLL_EVENT_TYPE_ID . '`=' . $id . ';');
    $ret = $result[0] ?? [];
    return $ret;
}
