<?php

define("BUDATOLL_ROLE_COACH", 'coach');
define("BUDATOLL_ROLE_PLAYER", 'player');

define("BUDATOLL_ACCESS_MANAGE_BALANCES", 'budatoll_manage_balances');
define("BUDATOLL_ACCESS_MANAGE_BOOKING", 'budatoll_manage_booking');
define("BUDATOLL_ACCESS_VIEW_BALANCE", 'budatoll_view_balance');
define("BUDATOLL_ACCESS_VIEW_BOOKINGS", 'budatoll_view_bookings');

define("BUDATOLL_PAYMENT_TABLE", $wpdb->prefix . 'budatoll_payments');
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

define("BUDATOLL_APPLY_TABLE", $wpdb->prefix . 'budatoll_applay');
define("BUDATOLL_APPLAY_ID", 'id');
define("BUDATOLL_APPLAY_EVENT_ID", 'event-id');
define("BUDATOLL_APPLAY_USER_ID", 'user-id');


/*
 * CREATE TABLE IF NOT EXISTS `wp_budatoll_event_type` (
  `id` int(9) NOT NULL AUTO_INCREMENT,
  `short` varchar(50) NOT NULL,
  `long` varchar(255) DEFAULT NULL,
  `price` int(8) NOT NULL DEFAULT 0,
  `max_players` int(8) NOT NULL DEFAULT 0,
  `start` time(1) NOT NULL,
  `end` time(1) NOT NULL,
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;

 * CREATE TABLE IF NOT EXISTS `wp_budatoll_accounts` (
  `id` int(8) NOT NULL AUTO_INCREMENT,
  `user_id` int(8) NOT NULL DEFAULT -1,
  `event_id` int(8) NOT NULL DEFAULT -1,
  `amount` int(8) NOT NULL DEFAULT 0,
  `payment_time` timestamp(1) NOT NULL DEFAULT current_timestamp(1) ON UPDATE current_timestamp(1),
  `comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;


CREATE TABLE IF NOT EXISTS `wp_budatoll_trainings` (
  `id` int(8) NOT NULL AUTO_INCREMENT,
  `type_id` int(8) NOT NULL DEFAULT -1,
  `name` varchar(255) DEFAULT NULL,
  `max_players` int(8) NOT NULL DEFAULT 0,
  `day` date NOT NULL,
  `start` time(1) NOT NULL,
  `end` time(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
 */