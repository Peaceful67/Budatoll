<?php

function bt_add_button($id = -1) {
    return '<button class="button budatoll-button button-small" name="budatoll_add" id="budatoll_add" value="'.$id.'" title="Új hozzáadása">'
            . '<span class="dashicons dashicons-plus"></span></button>';
}


function bt_edit_button($id) {
    return  '<button class="button budatoll-button button-small" name="budatoll_edit" id="budatoll_edit" value="' . $id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button>';
}
function bt_delete_button($id) {
    return  '<button class="button budatoll-button button-small button-action" onclick="return confirm(\'Biztosan törölni akarod\');" name="budatoll_delete" id="budatoll_delete" value="' . $id . '" title="Törlés">'
                . '<span class="dashicons dashicons-trash" ></span></button>';
}

function bt_warning($msg) {
      return '<div class="alert alert-warning  my-1"'
        . '<strong >' . $msg . '</strong>'
        .'</div>';
}