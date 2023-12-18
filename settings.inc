<?php

require_once plugin_dir_path(__FILE__) . 'setting/event_types.inc';
require_once plugin_dir_path(__FILE__) . 'setting/options.inc';
require_once plugin_dir_path(__FILE__) . 'setting/logger.inc';
require_once plugin_dir_path(__FILE__) . 'setting/manage_players.inc';

add_action('admin_init', 'budatoll_settings_init');
add_action('admin_menu', 'budatoll_add_settings_pages');

function budatoll_add_settings_pages() {
    add_menu_page(
            'Budatoll beállítások',
            'Budatoll',
            'manage_options',
            'options',
            '',
            'dashicons-buddicons-topics',
    );
    add_submenu_page(
            'budatoll-settings',
            'Beállítások',
            'Beállítások',
            'manage_options',
            'options',
            'budatoll_setting_options',
    );
    add_submenu_page(
            'options',
            'Naplók lekérdezése',
            'Naplók',
            'manage_options',
            'naplo-lista',
            'bt_logger_list',
    );
    add_submenu_page(
            'options',
            'Edzést típusok kezelése',
            'Edzés típusok',
            'manage_options',
            'edzes-tipusok',
            'budatoll_manage_event_types',
    );
    add_submenu_page(
            'options',
            'Játékosok kezelése',
            'Játékosok',
            'manage_options',
            'jatekosok',
            'budatoll_manage_players',
    );
}

function budatoll_admin_styles() {
    wp_enqueue_style('budatoll-admin', plugins_url('css/admin.css', __FILE__));
}
