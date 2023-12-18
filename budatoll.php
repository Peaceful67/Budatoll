<?php

/*
  Plugin Name: Budatoll Plugin for Wordpress
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

require_once plugin_dir_path(__FILE__) . 'events_list.inc';
require_once plugin_dir_path(__FILE__) . 'events_calendar.inc';
require_once plugin_dir_path(__FILE__) . 'ajax.inc';
require_once plugin_dir_path(__FILE__) . 'settings.inc';

require_once( ABSPATH . 'wp-admin/includes/upgrade.php' );

add_shortcode('list_balances', 'list_balance_of_users');
add_shortcode('budatoll-players', 'budatoll_manage_players');
add_shortcode('budatoll-list-event-types', 'budatoll_list_event_types');
add_shortcode('budatoll-event-types', 'budatoll_manage_event_types');
add_shortcode('budatoll-events-list', 'budatoll_manage_events_list');
add_shortcode('budatoll-events-calendar', 'budatoll_manage_events_calendar');

register_activation_hook(__FILE__, 'budatoll_activated');
register_deactivation_hook(__FILE__, 'budatoll_deactivated');

add_action('wp_enqueue_scripts', 'budatoll_scripts');
add_action('admin_enqueue_scripts', 'budatoll_admin_styles');

function budatoll_scripts() {
    wp_enqueue_style('budatoll-jquery-style', plugins_url('jquery-ui/jquery-ui.css', __FILE__));
    wp_enqueue_style('budatoll-fc-style', plugins_url('css/fullcalendar.css', __FILE__));
    wp_enqueue_style('budatoll-style', plugins_url('css/budatoll.css', __FILE__));
    wp_enqueue_script('jquery-ui-datepicker');
    wp_enqueue_script('budatoll-fc', plugins_url('js/fullcalendar/index.global.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-header-script', plugins_url('js/budatoll-header-script.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-jquery-ui-script', plugins_url('jquery-ui/jquery-ui.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-end-script', plugins_url('js/budatoll-end-script.js', __FILE__), array('jquery'), null, true);

    wp_localize_script('budatoll-header-script', 'budatoll_ajax_object', array('ajax_url' => admin_url('admin-ajax.php')));
}

function budatoll_activated() {
    global $wpdb;
    add_role(BUDATOLL_ROLE_COACH, 'Edző', ['read' => true, BUDATOLL_ACCESS_MANAGE_OPTIONS => true, BUDATOLL_ACCESS_MANAGE_COACH => true]);
    add_role(BUDATOLL_ROLE_PLAYER, 'Játékos', ['read' => true, BUDATOLL_ACCESS_MANAGE_PLAYER => true]);

    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_EVENT_TYPE_TABLE . '` (`'
            . BUDATOLL_EVENT_TYPE_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_EVENT_TYPE_SHORT . '` varchar(50) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_LONG . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_EVENT_TYPE_PRICE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_MAX_PLAYERS . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_START . '` time(1) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_END . '` time(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_EVENT_TYPE_ID . '` (`' . BUDATOLL_EVENT_TYPE_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_LOGGER_TABLE . '` (`'
            . BUDATOLL_LOGGER_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_LOGGER_TYPE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_LOGGER_USER . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_LOGGER_RELATED . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_LOGGER_COMMENT . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_LOGGER_TIME . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_LOGGER_ID . '` (`' . BUDATOLL_LOGGER_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_EVENTS_TABLE . '` (`'
            . BUDATOLL_EVENTS_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_EVENTS_SHORT . '` varchar(50) NOT NULL,`'
            . BUDATOLL_EVENTS_LONG . '` varchar(255) DEFAULT NULL,`'
            . BUDATOLL_EVENTS_DAY . '` DATE NOT NULL ,`'
            . BUDATOLL_EVENTS_PRICE . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENTS_EVENT_TYPE_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_EVENTS_MAX_PLAYERS . '` int(8) NOT NULL DEFAULT 0,`'
            . BUDATOLL_EVENT_TYPE_START . '` time(1) NOT NULL,`'
            . BUDATOLL_EVENT_TYPE_END . '` time(1) NOT NULL, `'
            . BUDATOLL_EVENTS_COMMENT . '` varchar(255) DEFAULT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_EVENTS_ID . '` (`' . BUDATOLL_EVENTS_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
}

function budatoll_deactivated() {
    remove_role(BUDATOLL_ROLE_COACH);
    remove_role(BUDATOLL_ROLE_PLAYER);
}
