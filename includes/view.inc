<?php

function bt_add_button($id = -1, $var_name = 'budatoll_add', $title = 'Új hozzásadása') {
    return '<button class="budatoll-button-icon" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="' . $title . '">'
            . '<span class="dashicons dashicons-plus"></span></button>';
}

function bt_edit_button($id, $var_name = 'budatoll_edit', $title = 'Szerkesztés') {
    return '<button class="budatoll-button-icon" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="' . $title . '">'
            . '<span class="dashicons dashicons-edit"></span></button>';
}

function bt_save_button($id, $var_name = 'budatoll_save', $title = 'Mentés') {
    return '<button class="budatoll-button-icon" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="' . $title . '">'
            . '<span class="dashicons dashicons-saved"></span></button>';
}

function bt_cancel_button($var_name = 'budatoll_cancel', $title = 'Mégsem') {
    return '<button class="budatoll-button-icon" name="' . $var_name . '" id="' . $var_name . '" value="-1" title="' . $title . '">'
            . '<span class="dashicons dashicons-undo"></span></button>';
}

function bt_delete_button($id, $var_name = 'budatoll_delete', $title = 'Törlés', $confirm = '') {
    return '<button class="budatoll-button-icon"  '
            . 'name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" '
            . ((empty($confirm)) ? '' : 'onclick="return confirm(\'Biztosan törölni akarod a ' . $confirm . '\');" ')
            . 'title="' . $title . '">'
            . '<span class="dashicons dashicons-trash" ></span></button>';
}

function bt_warning($msg) {
    return '<div class="alert alert-warning  my-1"'
            . '<strong >' . $msg . '</strong>'
            . '</div>';
}

function bt_text_button($var_name = 'budatoll_button', $text = 'Gomb', $title = '') {
    return '<button class="budatoll-button" name="' . $var_name . '" id="' . $var_name . '" value="-1" title="' . $title . '">'
            . $text . '</button>';
}
