<?php

function budatoll_my_trainings_list() {
    return budatoll_trainings_list();
}

function budatoll_trainings_list($player_id = -1) {
    global $wpdb;
    $ret = '<H3>Edzések listája</H3>';
    $ret .= '<form id="BudatollForm" method="post"  enctype="multipart/form-data" autocomplete="on">';
    $edit_id = getPostVariable('budatoll_edit', -1);
    $ret .= putHiddenValue('budatoll_edit', $edit_id);
    $delete_id = getPostVariable('budatoll_delete', false);
    $save = getPostVariable('budatoll_save', false);
    if (getPostVariable('budatoll_cancel', false)) {
        $edit_id = -1;
    }
    if (getPostVariable('budatoll_add', false)) {
        $ret .= budatoll_training_add();
    }
    if ($delete_id) {
        $wpdb->delete(BUDATOLL_TRAINING_TABLE, [BUDATOLL_TRAINING_ID => $delete_id]);
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
        if ($wpdb->insert(BUDATOLL_TRAINING_TABLE, $data)) {
            logger(BUDATOLL_LOGGER_TYPE_TRAININGS, $wpdb->insert_id, 'Edzés létrehozva: ' . print_r($data, true));
        }

        $edit_id = -1;
    }

    $ret .= budatoll_training_list($player_id);
    $ret .= '</form>';

    return $ret;
}

function budatoll_training_add() {
    global $wpdb; // <div class="shadow p-1 m-2 rounded">
    $ret = '<div class="container-fluid"><div class="budatoll-keret"><div class="budatoll-editor">';
    $short = '';
    $long = '';
    $price = 0;
    $start = '00:00';
    $end = '00:00';
    $max_players = 0;
    if ($item_id > 0) {
        $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_TABLE . '`=%d';
        $query = $wpdb->prepare($sql, $item_id);
        $item = $wpdb->get_row($query);
        $event_id = $item->{BUDATOLL_TRAINING_EVENT_ID} ?? -1;
        $player_id = $item->{BUDATOLL_TRAINING_PLAYER_ID} ?? -1;
        $done = $item->{BUDATOLL_TRAINING_DONE} ?? 0;
        $paid = $item->{BUDATOLL_TRAINING_PAID} ?? 0;
        $confirmed = $item->{BUDATOLL_TRAINING_CONFIRMED} ?? 0;
        $created = $item->{BUDATOLL_TRAINING_CREATED} ?? 0;
        $comment = $item->{BUDATOLL_TRAINING_COMMENT} ?? '';
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

function budatoll_training_list($player_id = -1) {
    global $wpdb;
    $trainings_from = getPostVariable('trainings_from', date('Y-m-01'));
    $trainings_to = getPostVariable('trainings_to', date('Y-m-t'));
    $event_type_id = getPostVariable('bt_event_type', -1);
    $trainings_year = date("Y", strtotime($trainings_from));
    $trainings_month = date("m", strtotime($trainings_from));
    if (getPostVariable('month_left')) {
        if ($trainings_month > 1) {
            $trainings_month--;
        } else {
            $trainings_year--;
            $trainings_month = 12;
        }
        $trainings_from = date(sprintf("%04d", $trainings_year) . '-' . sprintf("%02d", $trainings_month) . '-01');
        $trainings_to = date('Y-m-t', strtotime($trainings_from));
    }
    if (getPostVariable('month_right')) {
        if ($trainings_month < 12) {
            $trainings_month++;
        } else {
            $trainings_year++;
            $trainings_month = 1;
        }
        $trainings_from = date(sprintf("%04d", $trainings_year) . '-' . sprintf("%02d", $trainings_month) . '-01');
        $trainings_to = date('Y-m-t', strtotime($trainings_from));
    }
    $ret .= '<input type="hidden" name="trainings_type_year" value="' . $trainings_year . '">';
    $ret .= '<input type="hidden" name="trainings_month"  value="' . $trainings_month . '">';
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
    $ret .= '<div class="budatoll-search-group"><label for="trainings_from">Időszak:</label>'
            . '<input type="date" id="trainings_from" name="trainings_from" value="' . $trainings_from . '" onchange="submit();"> - '
            . '<input type="date" id="trainings_to" name="trainings_to" value="' . $trainings_to . '" onchange="submit();"></div>';
    $ret .= '<div class="budatoll-search-group"><button class="button budatoll-button" name="month_left" id="month_left" title="Előző hónap" value="-1">'
            . '<span class="dashicons dashicons-arrow-left"></span></button>';
    $ret .= $trainings_year . ' ' . strftime("%B", strtotime($trainings_from));
    $ret .= '<button class="button budatoll-button" name="month_right" id="month_right" title="Következő hónap" value="-1">'
            . '<span class="dashicons dashicons-arrow-right"></span></button></div>';
    $ret .= '</div>';
    return $ret;
}

