<?php

if (!defined('ABSPATH')) {
    die();
}

define('BUDATOLL_OPTIONS_MENU', "budatoll_options");

require_once plugin_dir_path(__FILE__) . 'setting/event_types.inc';
require_once plugin_dir_path(__FILE__) . 'setting/options.inc';
require_once plugin_dir_path(__FILE__) . 'setting/logger.inc';
require_once plugin_dir_path(__FILE__) . 'setting/manage_players.inc';
require_once plugin_dir_path(__FILE__) . 'setting/manage_emails.inc';

add_action('admin_init', 'budatoll_settings_init');
add_action('admin_menu', 'budatoll_add_settings_pages', 99);
add_action('after_setup_theme', 'budatoll_remove_admin_bar');

function budatoll_add_settings_pages() {

    add_menu_page(
            'Budatoll beállítások',
            'Budatoll',
            BUDATOLL_ACCESS_MANAGE_COACH,
            'options',
            '',
            'dashicons-buddicons-topics',
    );
    add_submenu_page(
            'budatoll-settings',
            'Beállítások',
            'Beállítások',
            BUDATOLL_ACCESS_MANAGE_COACH,
            'options',
            'budatoll_setting_options',
    );
    add_submenu_page(
            'options',
            'Naplók lekérdezése',
            'Naplók',
            BUDATOLL_ACCESS_MANAGE_COACH,
            'naplo-lista',
            'bt_logger_list',
    );
    add_submenu_page(
            'options',
            'Edzést típusok kezelése',
            'Edzés típusok',
            BUDATOLL_ACCESS_MANAGE_COACH,
            'edzes-tipusok',
            'budatoll_manage_event_types',
    );
    add_submenu_page(
            'options',
            'Játékosok kezelése',
            'Játékosok',
            BUDATOLL_ACCESS_MANAGE_COACH,
            'jatekosok',
            'budatoll_manage_players',
    );
    add_submenu_page(
            'options',
            'Emailek kezelése',
            'Emailek',
            BUDATOLL_ACCESS_MANAGE_COACH,
            'emailek',
            'budatoll_manage_emails',
    );
}

function budatoll_admin_styles() {
    wp_enqueue_style('budatoll-admin', plugins_url('css/admin.css', __FILE__));
}

function budatoll_settings_init() {
    if (!defined('DOING_AJAX') || !DOING_AJAX) {
        if (!current_user_can(BUDATOLL_ROLE_COACH) && !current_user_can(BUDATOLL_ROLE_ADMIN)) {
            wp_redirect(home_url());
            exit;
//        wp_die(__('Nincs megfelelő jogosultságod az oldal megtekintéséhez!'));
        }
    }
}

function budatoll_remove_admin_bar() {
    global $menu;
    if (!current_user_can(BUDATOLL_ROLE_ADMIN) && !current_user_can(BUDATOLL_ROLE_COACH)) {
        show_admin_bar(false);
    }
}
