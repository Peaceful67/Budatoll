<?php

define("BUDATOLL_ROLE_COACH", 'coach');
define("BUDATOLL_ROLE_PLAYER", 'player');

define("BUDATOLL_ACCESS_MANAGE_OPTIONS", 'budatoll-manage-options');
define("BUDATOLL_ACCESS_MANAGE_COACH", 'budatoll-manage-coach');
define("BUDATOLL_ACCESS_MANAGE_PLAYER", 'budatoll-manage-player');

define("BUDATOLL_PAYMENT_TABLE", 'wp_budatoll_payments');
define("BUDATOLL_PAYMENT_ID", 'id');
define("BUDATOLL_PAYMENT_USER_ID", 'user-id');
define("BUDATOLL_PAYMENT_AMOUNT", 'amount');
define("BUDATOLL_PAYMENT_EVENT_ID", 'event-id');
define("BUDATOLL_PAYMENT_TIME", 'time');

define("BUDATOLL_EVENT_TYPE_TABLE", 'wp_budatoll_event_type');
define("BUDATOLL_EVENT_TYPE_ID", 'id');
define("BUDATOLL_EVENT_TYPE_SHORT", 'short');
define("BUDATOLL_EVENT_TYPE_LONG", 'long');
define("BUDATOLL_EVENT_TYPE_PRICE", 'price');
define("BUDATOLL_EVENT_TYPE_MAX_PLAYERS", 'max_players');
define("BUDATOLL_EVENT_TYPE_START", 'start');
define("BUDATOLL_EVENT_TYPE_END", 'end');

define("BUDATOLL_EVENTS_TABLE", 'wp_budatoll_events');
define("BUDATOLL_EVENTS_ID", 'id');
define("BUDATOLL_EVENTS_EVENT_TYPE_ID", 'type_id');
define("BUDATOLL_EVENTS_SHORT", 'short');
define("BUDATOLL_EVENTS_LONG", 'long');
define("BUDATOLL_EVENTS_PRICE", 'price');
define("BUDATOLL_EVENTS_MAX_PLAYERS", 'max_players');
define("BUDATOLL_EVENTS_DAY", 'day');
define("BUDATOLL_EVENTS_START", 'start');
define("BUDATOLL_EVENTS_END", 'end');
define("BUDATOLL_EVENTS_COMMENT", 'comment');

define("BUDATOLL_LOGGER_TABLE", 'wp_budatoll_logger');
define("BUDATOLL_LOGGER_ID", 'id');
define("BUDATOLL_LOGGER_TYPE", 'type');
define("BUDATOLL_LOGGER_USER", 'user');
define("BUDATOLL_LOGGER_RELATED", 'related');
define("BUDATOLL_LOGGER_COMMENT", 'comment');
define("BUDATOLL_LOGGER_TIME", 'time');

$budatoll_months = [
    1 => 'Január',
    2 => 'Február',
    3 => 'Március',
    4 => 'Április',
    5 => 'Május',
    6 => 'Június',
    7 => 'Július',
    8 => 'Augusztus',
    9 => 'Szeptember',
    10 => 'Október',
    11 => 'November',
    12 => 'December',
];
/*
if (current_user_can(BUDATOLL_ACCESS_MANAGE_OPTIONS)) {
    
}

if (current_user_can(BUDATOLL_ACCESS_MANAGE_COACH)) {
   
}
if (current_user_can(BUDATOLL_ACCESS_MANAGE_PLAYER)) {
    
}