<?php

function list_balances_of_users() {
    $ret = 'Ez a lista az egyenlegekről már a fejlesztésből.<br>';

    $users = get_users([
        'role' => ROLE_COACH,
        'number' => -1,
        'fields' => 'all',
    ]);

    foreach ($users as $user) {
        $ret .= 'ID: ' . $user->ID . ', user name:' . $user->user_login . '<br>';
    }
    return $ret;
}
