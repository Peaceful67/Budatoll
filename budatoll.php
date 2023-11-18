<?php

/*
  Plugin Name: Budatoll Plugin
  Description: Kezeli a befizetéseket és a pályafoglalásokat.
  Version: 1.0
  Author: Baksa Zsolt
 */

if (!defined('ABSPATH')) {
    die();
}

require_once plugin_dir_path(__FILE__) . 'config.inc';
require_once plugin_dir_path(__FILE__) . 'functions.inc';
require_once plugin_dir_path(__FILE__) . 'manage_balances.inc';
require_once plugin_dir_path(__FILE__) . 'manage_players.inc';
require_once plugin_dir_path(__FILE__) . 'manage_event_types.inc';
require_once plugin_dir_path(__FILE__) . 'settings.inc';

require_once( ABSPATH . 'wp-admin/includes/upgrade.php' );

add_shortcode('list_balances', 'list_balance_of_users');
add_shortcode('budatoll-players', 'budatoll_manage_players');
add_shortcode('budatoll-event-types', 'budatoll_manage_event_types');

add_action('admin_menu', 'budatoll_settings_page');

register_activation_hook(__FILE__, 'budatoll_activated');
register_deactivation_hook(__FILE__, 'budatoll_deactivated');

function budatoll_scripts() {
    wp_enqueue_style('budatoll-style-bs', plugins_url('css/bootstrap.min.css', __FILE__));
    wp_enqueue_style('budatoll-style', plugins_url('css/budatoll.css', __FILE__));
    wp_enqueue_script('budatoll-bs-min', plugins_url('js/bootstrap.min.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-fullcalendar', plugins_url('js/fullcalendar/index.global.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-header-script', plugins_url('js/budatoll-header-script.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-end-script', plugins_url('js/budatoll-end-script.js', __FILE__), array('jquery'), null, true);
}

add_action('wp_enqueue_scripts', 'budatoll_scripts');

function budatoll_activated() {
 global $wpdb;
    add_role(BUDATOLL_ROLE_COACH, 'Edző', [BUDATOLL_ACCESS_MANAGE_BALANCES => true, BUDATOLL_ACCESS_VIEW_BOOKINGS => true]);
    add_role(BUDATOLL_ROLE_PLAYER, 'Játékos', [BUDATOLL_ACCESS_MANAGE_BOOKING => true, BUDATOLL_ACCESS_VIEW_BOOKINGS => true, BUDATOLL_ACCESS_VIEW_BALANCE => true]);

    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_EVENT_TYPE_TABLE . '` (`'
            . BUDATOLL_EVENT_TYPE_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_EVENT_TYPE_SHORT . '` varchar(50) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_LONG . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_EVENT_TYPE_PRICE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_MAX_PLAYERS . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_START . '` time(1) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_END . '` time(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_EVENT_TYPE_ID . '` (`' . BUDATOLL_EVENT_TYPE_ID . '`)) '
            . $wpdb->get_charset_collate().';';
    dbDelta($sql);
}

function budatoll_deactivated() {
    
}


/*
 * CREATE TABLE IF NOT EXISTS `wp_budatoll_accounts` (
  `id` int(8) NOT NULL AUTO_INCREMENT,
  `user_id` int(8) NOT NULL DEFAULT -1,
  `event_id` int(8) NOT NULL DEFAULT -1,
  `amount` int(8) NOT NULL DEFAULT 0,
  `payment_time` timestamp(1) NOT NULL DEFAULT current_timestamp(1) ON UPDATE current_timestamp(1),
  `comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;

CREATE TABLE IF NOT EXISTS `wp_budatoll_event_type` (
  `id` int(9) NOT NULL AUTO_INCREMENT,
  `short` varchar(50) NOT NULL,
  `long` varchar(255) DEFAULT NULL,
  `price` int(8) NOT NULL DEFAULT 0,
  `max_players` int(8) NOT NULL DEFAULT 0,
  `start` time(1) NOT NULL,
  `end` time(1) NOT NULL,
  UNIQUE KEY `id` (`id`)
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