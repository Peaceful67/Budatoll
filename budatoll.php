<?php

/*
  Plugin Name: Budatoll Plugin
  Description: Kezeli a befizetéseket és a pályafoglalásokat.
  Version: 1.0
  Author: Baksa Zsolt
 */

require_once plugin_dir_path(__FILE__) . 'config.inc';
require_once plugin_dir_path(__FILE__) . 'functions.inc';
require_once plugin_dir_path(__FILE__) . 'manage_balances.inc';
require_once plugin_dir_path(__FILE__) . 'settings.inc';

add_shortcode('list_balances', 'list_balance_of_users');

add_action('admin_menu', 'budatoll_settings_page');

register_activation_hook(__FILE__, 'budatoll_activated');
register_deactivation_hook(__FILE__, 'budatoll_deactivated');

function budatoll_activated() {
    add_role(ROLE_COACH, 'Edző', [ACCESS_MANAGE_BALANCES => true, ACCESS_VIEW_BOOKINGS => true]);
    add_role(ROLE_PLAYER, 'Játékos', [ACCESS_MANAGE_BOOKING => true, ACCESS_VIEW_BOOKINGS => true, ACCESS_VIEW_BALANCE => true]);
}

function budatoll_deactivated() {
    $coach = get_role(ROLE_COACH);
    if ($coach) {
        $coach->remove_cap(ACCESS_MANAGE_BALANCES);
        $coach->remove_cap(ACCESS_VIEW_BOOKINGS);
    }
    remove_role(ROLE_COACH);
    $player = get_role(ROLE_PLAYER);
    if ($player) {
        $player->remove_cap(ACCESS_MANAGE_BOOKING);
        $player->remove_cap(ACCESS_VIEW_BOOKINGS);
        $player->remove_cap(ACCESS_VIEW_BALANCE);
    }
    remove_role(ROLE_PLAYER);
}
