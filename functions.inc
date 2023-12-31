<?php

function isBudatollPagePermited($page_name) {
    $page_acccess = [
        'manage_balances' => [BUDATOLL_ACCESS_MANAGE_BALANCES],
        'view_balance' => [BUDATOLL_ACCESS_VIEW_BALANCE],
    ];
    if (!isset($page_acccess[$page_name])) {
        return false;
    }
    if (current_user_can('administrator')) {
        return true;
    }
    $current_user = wp_get_current_user();
    foreach ($page_acccess[$page_name] as $required) {
        if ($current_user->has_cap($required)) {
            return true;
        }
    }
    return false;
}

function putPermissionNotGranted() {
    return '<div class="et-pb-promo-error">Az oldal megtekintéséhez nincs jogosultsága!</div>';
}

function budatollNewModal($modal_name, $title, $size = 'md') {
    $ret = '<script type="text/javascript">$(function(){$("#' . $modal_name . '").modal("show");}); '
            . 'function closeModal(){ $("#' . $modal_name . '").modal("hide");}</script>';
    $ret .= '<div class="modal fade" id="' . $modal_name . '" tabindex="-1"><div class="modal-dialog modal-dialog-centered modal-' . $size . '"><div class="modal-content">';
    $ret .= '<div class="modal-header">';
    $ret .= '<h4 class="modal-title">' . $title . '</h4>';
    $ret .= '<button type="button" name="' . $modal_name . '_close" class="close" data-dismiss="modal" onClick="closeModal();">&times;</button>';
    $ret .= '</div>';
    $ret .= '<div class="modal-body"><div class="input-group input-group-' . $size . '"><div class="container-fluid">';
    return $ret;
}

function budatollNewModalFooter() {
    return '</div></div><div clas="modal-footer">';
}

function budatollEndModal() {
    return '</div></div></div></div></div>';
}

function getPostVariable($var_name, $default = false) {
    $ret = $_POST[$var_name] ?? $default;
    if (is_null($ret) OR !$ret) {
        $ret = $default;
    }
    return $ret;
}

function putScript($script) {
    return '<script type="text/javascript">' . $script . '</script>';
}

function bt_send_email($user_id, $subject, $message) {
    $user = get_userdata($user_id);
    if (!$user) {
        return false; // User not found
    }
    $user_email = sanitize_email($user->user_email);
    if (!is_email($user_email)) {
        return false;
    }
    $headers = array('Content-Type: text/html; charset=UTF-8');
    $message .= '<br><br>Üdvözlettel:<br>Budatoll csapata';
    return wp_mail($user_email, $subject, $message, $headers);
}
