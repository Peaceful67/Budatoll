<?php

/*
  Plugin Name: Budatoll Plugin for Wordpress
  Description: Kezeli a befizetéseket és a pályafoglalásokat.
  Version: 1.1
  Author: Baksa Zsolt
 */

if (!defined('ABSPATH')) {
    die();
}
setlocale(LC_TIME, 'hu_HU.UTF-8');
require_once plugin_dir_path(__FILE__) . 'config.inc';
require_once plugin_dir_path(__FILE__) . 'functions.inc';
require_once plugin_dir_path(__FILE__) . 'includes/view.inc';
require_once plugin_dir_path(__FILE__) . 'includes/settings_event_types.inc';
require_once plugin_dir_path(__FILE__) . 'includes/settings_logger.inc';
require_once plugin_dir_path(__FILE__) . 'includes/settings_manage_emails.inc';
require_once plugin_dir_path(__FILE__) . 'includes/settings_manage_players.inc';
require_once plugin_dir_path(__FILE__) . 'includes/settings_options.inc';
require_once plugin_dir_path(__FILE__) . 'includes/manage_payments.inc';
require_once plugin_dir_path(__FILE__) . 'includes/payment_accounts.inc';
require_once plugin_dir_path(__FILE__) . 'includes/events_list.inc';
require_once plugin_dir_path(__FILE__) . 'includes/events_calendar.inc';
require_once plugin_dir_path(__FILE__) . 'includes/events_template.inc';
require_once plugin_dir_path(__FILE__) . 'includes/trainings_functions.inc';
require_once plugin_dir_path(__FILE__) . 'includes/trainings_calendar.inc';
require_once plugin_dir_path(__FILE__) . 'includes/trainings_list.inc';
require_once plugin_dir_path(__FILE__) . 'includes/scheduler.inc';
require_once plugin_dir_path(__FILE__) . '/includes/password_change.inc';

require_once plugin_dir_path(__FILE__) . 'ajax.inc';
// require_once plugin_dir_path(__FILE__) . 'settings.inc';

require_once(ABSPATH . 'wp-admin/includes/upgrade.php');

add_shortcode('list_balances', 'list_balance_of_users');
add_shortcode('budatoll-players', 'budatoll_manage_players');
add_shortcode('budatoll-list-event-types', 'budatoll_list_event_types');
add_shortcode('budatoll-event-types', 'budatoll_manage_event_types');
add_shortcode('budatoll-events-list', 'budatoll_manage_events_list');
add_shortcode('budatoll-events-calendar', 'budatoll_manage_events_calendar');
add_shortcode('budatoll-events-template', 'budatoll_events_template');
add_shortcode('budatoll-my-trainings-list', 'budatoll_my_trainings_list');
add_shortcode('budatoll-my-trainings-calendar', 'budatoll_my_trainings_calendar');
add_shortcode('budatoll-trainings-list', 'budatoll_trainings_list');
add_shortcode('budatoll-trainings-calendar', 'budatoll_trainings_calendar');
add_shortcode('budatoll-test-page', 'budatoll_test_page');
add_shortcode('budatoll-payment-management', 'bt_payment_management');
add_shortcode('budatoll-balance-of-user', 'bt_balance_of_user');
add_shortcode('budatoll-my-balance', 'bt_my_balance');
add_shortcode('budatoll-rules', 'bt_rule_options');
add_shortcode('budatoll-welcome-message', 'bt_welcome_message');
//add_shortcode('budatoll_password_change', 'bt_change_password');

add_shortcode('budatoll-settings-options', 'budatoll_setting_options');
add_shortcode('budatoll-settings-logger', 'bt_logger_list');
add_shortcode('budatoll-settings-event-types', 'budatoll_manage_event_types');
add_shortcode('budatoll-settings-emails', 'budatoll_manage_emails');
add_shortcode('budatoll-settings-players', 'budatoll_manage_players');

register_activation_hook(__FILE__, 'budatoll_activated');
register_deactivation_hook(__FILE__, 'budatoll_deactivated');

add_action('wp_enqueue_scripts', 'budatoll_scripts');
add_action('template_redirect', 'budatoll_is_page_allowed');

add_action('wp_login', 'budatoll_redirect_after_login');
add_filter('wp_nav_menu_args', 'budatoll_menu_based_on_role');

add_action('budatoll_cron_hook', 'budatoll_crontab');
add_filter('cron_schedules', 'budatoll_cron_interval');
add_filter('auth_cookie_expiration', 'custom_remember_me_duration', 10, 3);
add_action('wp_footer', 'add_remember_me_to_divi_login');

add_action('wp_login', 'bt_log_user_login', 5, 1);
add_action('wp_logout', 'budatoll_logout');
add_action('clear_auth_cookie', 'bt_log_user_logout');
add_action('wp_login_failed', 'bt_log_login_failed');
add_action('user_register', 'bt_user_register', 10, 1);
add_action('delete_user', 'bt_user_delete', 10, 1);
add_action('init', 'bt_init');
add_action('shutdown', 'budatoll_crontab'); // Meghívjuk innen is, hátha nem működik a crontab
add_action('after_setup_theme', 'budatoll_remove_admin_bar');

add_action('user_register', 'bt_set_default_role');

/**
 * Disable admin notification on user password change.
 */
if (!function_exists('wp_password_change_notification')) {

    function wp_password_change_notification($user) {
        // do nothing
    }

}

function bt_enqueue_dashicons() {
    wp_enqueue_style('dashicons');
}

add_action('wp_enqueue_scripts', 'bt_enqueue_dashicons');

function budatoll_scripts() {
    global $post;
    wp_enqueue_style('budatoll-jquery-style', plugins_url('jquery-ui/jquery-ui.css', __FILE__));
    wp_enqueue_style('budatoll-fc-style', plugins_url('css/fullcalendar.css', __FILE__), [], '1.5.3', false);
    wp_enqueue_style('budatoll-bootstrap-style', plugins_url('css/bootstrap.css', __FILE__));
    wp_enqueue_style('budatoll-style', plugins_url('css/budatoll.css', __FILE__), [], '1.5.1', false);
    wp_enqueue_style('budatoll-mobile-style', plugins_url('css/mobile.css', __FILE__, [], '1.5.1'));

    wp_enqueue_script('budatoll-fc', plugins_url('fullcalendar/index.global.min.js', __FILE__), array('jquery'), false, false);
    wp_enqueue_script('budatoll-fc-lc-hu', plugins_url('fullcalendar/locales/hu.global.min.js', __FILE__), array('jquery'), false, false);
    wp_enqueue_script('budatoll-header-script', plugins_url('js/budatoll-header-script.js', __FILE__), array('jquery'), '1.5.1', false);
    if (!is_null($post->post_name)) {
        switch ($post->post_name) {
            case 'sablon-naptar':
                wp_enqueue_script('budatoll-events-script', plugins_url('js/events_template.js', __FILE__), array('jquery'), '1.5.1', true);
                break;
            case 'alkalom-naptar':
                wp_enqueue_script('budatoll-events-script', plugins_url('js/events_calendar.js', __FILE__), array('jquery'), '1.5.2', true);
                break;
            case 'edzesek-naptar':
                wp_enqueue_script('budatoll-trainings-script', plugins_url('js/trainings_calendar.js', __FILE__), array('jquery'), '1.5.1', true);
                break;
            case 'edzeseim-naptar':
                wp_enqueue_script('budatoll-my-trainings-script', plugins_url('js/my_trainings_calendar.js', __FILE__), array('jquery'), '1.5.1', true);
                break;
        }
    }
    wp_enqueue_script('budatoll-jquery-ui-script', plugins_url('jquery-ui/jquery-ui.js', __FILE__), array('jquery'), '1.5.1', false);
    wp_enqueue_script('budatoll-end-script', plugins_url('js/budatoll-end-script.js', __FILE__), array('jquery'), '1.5.1', true);
    wp_localize_script('budatoll-header-script', 'budatoll_ajax_object', array('ajax_url' => admin_url('admin-ajax.php')));
}

function budatoll_remove_admin_bar() {
    if (!current_user_can(BUDATOLL_ROLE_ADMIN)) {
        show_admin_bar(false);
    }
}
