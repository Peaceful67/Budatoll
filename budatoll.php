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
require_once plugin_dir_path(__FILE__) . 'includes/manage_payments.inc';
require_once plugin_dir_path(__FILE__) . 'includes/events_list.inc';
require_once plugin_dir_path(__FILE__) . 'includes/events_calendar.inc';
require_once plugin_dir_path(__FILE__) . 'includes/trainings_calendar.inc';
require_once plugin_dir_path(__FILE__) . 'includes/trainings_list.inc';
require_once plugin_dir_path(__FILE__) . 'ajax.inc';
require_once plugin_dir_path(__FILE__) . 'settings.inc';

require_once( ABSPATH . 'wp-admin/includes/upgrade.php' );

add_shortcode('list_balances', 'list_balance_of_users');
add_shortcode('budatoll-players', 'budatoll_manage_players');
add_shortcode('budatoll-list-event-types', 'budatoll_list_event_types');
add_shortcode('budatoll-event-types', 'budatoll_manage_event_types');
add_shortcode('budatoll-events-list', 'budatoll_manage_events_list');
add_shortcode('budatoll-events-calendar', 'budatoll_manage_events_calendar');
add_shortcode('budatoll-my-trainings-list', 'budatoll_my_trainings_list');
add_shortcode('budatoll-my-trainings-calendar', 'budatoll_my_trainings_calendar');
add_shortcode('budatoll-trainings-list', 'budatoll_trainings_list');
add_shortcode('budatoll-trainings-calendar', 'budatoll_trainings_calendar');
add_shortcode('budatoll-test-page', 'budatoll_test_page');
add_shortcode('budatoll-payment-management', 'bt_payment_management');

register_activation_hook(__FILE__, 'budatoll_activated');
register_deactivation_hook(__FILE__, 'budatoll_deactivated');

add_action('wp_enqueue_scripts', 'budatoll_scripts');
add_action('admin_enqueue_scripts', 'budatoll_admin_styles');

add_filter('wp_nav_menu_args', 'budatoll_menu_based_on_role');

function budatoll_scripts() {
    global $post;
    wp_enqueue_style('budatoll-jquery-style', plugins_url('jquery-ui/jquery-ui.css', __FILE__));
    wp_enqueue_style('budatoll-fc-style', plugins_url('css/fullcalendar.css', __FILE__));
    wp_enqueue_style('budatoll-style', plugins_url('css/budatoll.css', __FILE__));

    wp_enqueue_script('jquery-ui-datepicker');
    wp_enqueue_script('budatoll-fc', plugins_url('js/fullcalendar/index.global.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-fc-lc-hu', plugins_url('js/fullcalendar/locales/hu.global.min.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-header-script', plugins_url('js/budatoll-header-script.js', __FILE__), array('jquery'), null, false);
    switch ($post->post_name) {
        case 'alkalom-naptar':
            wp_enqueue_script('budatoll-events-script', plugins_url('js/events_calendar.js', __FILE__), array('jquery'), null, true);
            break;
        case 'edzesek-naptar':
            wp_enqueue_script('budatoll-trainings-script', plugins_url('js/trainings_calendar.js', __FILE__), array('jquery'), null, true);
            break;
        case 'edzeseim-naptar':
            wp_enqueue_script('budatoll-my-trainings-script', plugins_url('js/my_trainings_calendar.js', __FILE__), array('jquery'), null, true);
            break;
    }
    wp_enqueue_script('budatoll-jquery-ui-script', plugins_url('jquery-ui/jquery-ui.js', __FILE__), array('jquery'), null, false);
    wp_enqueue_script('budatoll-end-script', plugins_url('js/budatoll-end-script.js', __FILE__), array('jquery'), null, true);

    wp_localize_script('budatoll-header-script', 'budatoll_ajax_object', array('ajax_url' => admin_url('admin-ajax.php')));
}


