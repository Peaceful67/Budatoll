<?php
/*
Plugin Name: Budatoll Plugin
Description: Kezeli a befizetéseket és a pályafoglalásokat.
Version: 1.0
Author: Baksa Zsolt
*/

require_once plugin_dir_path(__FILE__) . 'config.inc';
require_once plugin_dir_path(__FILE__) . 'init.inc';
require_once plugin_dir_path(__FILE__) . 'manage_balances.inc';
require_once plugin_dir_path(__FILE__) . 'settings.inc';


add_shortcode('list_balances', 'list_balances_of_users');


add_action('admin_menu', 'budatoll_settings_page');