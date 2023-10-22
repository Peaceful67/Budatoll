<?php

function isBudatollPagePermited($page_name) {
    $page_acccess = [
        'manage_balances' => [ACCESS_MANAGE_BALANCES],
        'view_balance' => [ACCESS_VIEW_BALANCE],
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