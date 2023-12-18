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
setlocale(LC_TIME, 'hu_HU.UTF-8');
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
add_shortcode('budatoll-test-page', 'budatoll_test_page');

register_activation_hook(__FILE__, 'budatoll_activated');
register_deactivation_hook(__FILE__, 'budatoll_deactivated');

add_action('wp_enqueue_scripts', 'budatoll_scripts');
add_action('admin_enqueue_scripts', 'budatoll_admin_styles');

add_filter('wp_nav_menu_args', 'budatoll_menu_based_on_role');

function budatoll_scripts() {
    wp_enqueue_style('budatoll-jquery-style', plugins_url('jquery-ui/jquery-ui.css', __FILE__));
    wp_enqueue_style('budatoll-fc-style', plugins_url('css/fullcalendar.css', __FILE__));
    wp_enqueue_style('budatoll-style', plugins_url('css/budatoll.css', __FILE__));

    wp_enqueue_script('jquery-ui-datepicker');
    wp_enqueue_script('budatoll-fc', plugins_url('js/fullcalendar/index.global.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-fc-lc-hu', plugins_url('js/fullcalendar/locales/hu.global.min.js', __FILE__), array('jquery'), null, false);
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

function budatoll_menu_based_on_role($args) {
    global $bt_nav_menu;
    $bt_nav_menu = 'Visitor';
    if ('primary-menu' === $args['theme_location']) {
        $user = wp_get_current_user();
        if (in_array('administrator', (array) $user->roles) AND wp_get_nav_menu_object('Admin')) {
            $args['menu'] = 'Admin';
            $bt_nav_menu = 'Admin';
        } elseif (in_array('coach', (array) $user->roles) AND wp_get_nav_menu_object('Coach')) {
            $args['menu'] = 'Coach';
            $bt_nav_menu = 'Coach';
        } elseif (in_array('player', (array) $user->roles) AND wp_get_nav_menu_object('Player')) {
            $args['menu'] = 'Player';
            $bt_nav_menu = 'Player';
        }
    }

    return $args;
}

$bt_nav_menu = 'Visistor';

function budatoll_test_page() {
    global $bt_nav_menu;
    $ret = 'Teszteljük az oldalt';
    $menus = wp_get_nav_menus();
    foreach ($menus as $menu) {
        $ret .= 'Menu Name: ' . $menu->name . '<br>';
    }
    $user = wp_get_current_user();
    $ret .= ' Current user: ' . $user->display_name . '<br>';
    $ret .= 'Menu:' . $bt_nav_menu;
    return $ret;
}
