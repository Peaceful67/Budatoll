<?php

function budatoll_manage_players() {
    $ret = '<h3>Játékosok listája.</h3>';
    $users = get_users([
        'role' => BUDATOLL_ROLE_PLAYER,
        'number' => -1,
        'fields' => 'all',
    ]);
    foreach ($users as $user) {
        $ret .= 'ID: ' . $user->ID . ', user name:' . $user->user_login . '<br>';
    }
    return $ret;
}
