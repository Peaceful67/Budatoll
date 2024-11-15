<?php

if (!defined('ABSPATH')) {
    die();
}

function bt_get_unpaid_trainings($user_id) {
    global $wpdb;
    $sql = 'SELECT * FROM `'.BUDATOLL_TRAINING_TABLE.'` WHERE '
            . ' `'.BUDATOLL_TRAINING_PAID.'`=0 AND '
            . ' `'.BUDATOLL_TRAINING_DONE.'`=1 AND '
            . '`'.BUDATOLL_TRAINING_PLAYER_ID.'`='.$user_id;
    $res = bt_get_results($sql);
    return $res ? $res : [];
}

