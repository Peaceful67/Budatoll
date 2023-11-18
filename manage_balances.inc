<?php

function list_balance_of_users() {
    if (!isBudatollPagePermited('manage_balances')) {
        return putPermissionNotGranted();
    }
    $ret = 'Ez a lista az egyenlegekről már a fejlesztésből.<br>';

    $users = get_users([
        'role' => BUDATOLL_ROLE_COACH,
        'number' => -1,
        'fields' => 'all',
    ]);

    foreach ($users as $user) {
        $ret .= 'ID: ' . $user->ID . ', user name:' . $user->user_login . '<br>';
    }
    return $ret;
}
