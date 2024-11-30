<?php

function bt_add_button($id = -1, $var_name = 'budatoll_add', $title = 'Új hozzásadása') {
    return '<button class="button budatoll-button button-small" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="' . $title . '">'
            . '<span class="dashicons dashicons-plus"></span></button>';
}

function bt_edit_button($id, $var_name = 'budatoll_edit', $title='Szerkesztés') {
    return '<button class="button budatoll-button button-small" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="' . $title . '">'
            . '<span class="dashicons dashicons-edit"></span></button>';
}

function bt_save_button($id, $var_name = 'budatoll_save', $title='Mentés') {
    return '<button class="button budatoll-button button-small" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="'.$title.'">'
            . '<span class="dashicons dashicons-saved"></span></button>';
}

function bt_cancel_button($id, $var_name = 'budatoll_cancel', $title='Mégsem') {
    return '<button class="button budatoll-button button-small" name="' . $var_name . '" id="' . $var_name . '" value="' . $id . '" title="'.$title.'">'
            . '<span class="dashicons dashicons-undo"></span></button>';
}

function bt_delete_button($id, $var_name='budatoll_delete', $title='') {
    return '<button class="button budatoll-button button-small button-action" onclick="return confirm(\'Biztosan törölni akarod\');" '
    . 'name="'.$var_name.'" id="'.$var_name.'" value="' . $id . '" title="'.$title.'">'
            . '<span class="dashicons dashicons-trash" ></span></button>';
}

function bt_warning($msg) {
    return '<div class="alert alert-warning  my-1"'
            . '<strong >' . $msg . '</strong>'
            . '</div>';
}
