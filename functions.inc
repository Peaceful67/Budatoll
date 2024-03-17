<?php

function budatoll_pages_restricted() {
    /*
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
     */
    if (bt_user_has_role('administrator')) {
        return;
    }

    $permalink = home_url(FILTER_INPUT(INPUT_SERVER, 'REQUEST_URI'));
    $page_acccess = [// Ezeket az oldalakat csak a megadott szerepkörrel érhetjük el
        'manage_balances' => ['coach'],
        'view_balance' => ['player'],
    ];
    if (!isset($page_acccess[$permalink])) {
        return;
    }
    if (is_user_logged_in()) {
        foreach ($page_acccess[$permalink] as $required) {
            if (user_has_role($required)) {
                return;
            }
        }
    }
    wp_redirect(home_url('/nincs-jogosultsag'));
}

function bt_is_deletable($item_id, $related_arr) {
    global $wpdb;
    foreach ($related_arr as $table => $field) {
        $sql = 'SELECT * FROM `' . $table . '` WHERE `' . $field . '`=' . $item_id . ' LIMIT 1;';
        $wpdb->get_results($sql);
        if ($wpdb->num_rows > 0) {
            return false;
        }
    }
    return true;
}

function getPostVariable($var_name, $default = false) {
    $ret = $_POST[$var_name] ?? $default;
    return $ret;
}

function putHiddenValue($var_name, $value) {
    return '<input type="hidden" name="' . $var_name . '" id="' . $var_name . '" value="' . $value . '">';
}

function putScript($script) {
    return '<script type="text/javascript">' . $script . '</script>';
}

function bt_welcome_message() {
    $user = wp_get_current_user();
    if ($user->ID == 0) {
        $ret = 'Lépj be vagy regisztrálj játékosként!';
    } else {
        $ret .= 'Belépve: ' . $user->display_name;
    }
    return $ret;
}

function bt_send_email($user_id, $subject, $message) {
    $user = get_userdata($user_id);
    if (!$user) {
        return false; // User not found
    }
    $user_email = sanitize_email($user->user_email);
    if (!is_email($user_email)) {
        return false;
    }
    $headers = [
        'Content-Type: text/html; charset=UTF-8',
        'From: from@budatoll.hu',
        'Reply-to: no-reply@budatoll.hu',
    ];
    $message .= '<br><br>Üdvözlettel:<br>Budatoll csapata';
    logger(BUDATOLL_LOGGER_TYPE_EMAILS, $user_id, $subject);
    return wp_mail($user_email, $subject, $message, $headers);
}

function budatoll_deactivated() {
    remove_role(BUDATOLL_ROLE_COACH);
    remove_role(BUDATOLL_ROLE_PLAYER);
}

function budatoll_menu_based_on_role($args) {
    if (is_user_logged_in() AND ('primary-menu' === $args['theme_location'])) {
        $user = wp_get_current_user();
        if (in_array('administrator', (array) $user->roles) AND wp_get_nav_menu_object('Admin')) {
            $args['menu'] = 'Admin';
        } elseif (in_array('coach', (array) $user->roles) AND wp_get_nav_menu_object('Coach')) {
            $args['menu'] = 'Coach';
        } elseif (in_array('player', (array) $user->roles) AND wp_get_nav_menu_object('Player')) {
            $args['menu'] = 'Player';
        }
    } elseif(wp_get_nav_menu_object('Visitor')) {
        $args['menu'] = 'Visitor';
    } else {
        $args['menu'] = '';
    }

    return $args;
}

function budatoll_test_page() {
    $ret = 'Teszteljük az oldalt';
    $menus = wp_get_nav_menus();
    foreach ($menus as $menu) {
        $ret .= 'Menu Name: ' . $menu->name . '<br>';
    }
    $user = wp_get_current_user();
    $ret .= ' Current user: ' . $user->display_name . '<br>';
    return $ret;
}

function budatoll_redirect_after_login() {
    wp_redirect(home_url());
    exit();
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
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_ACCOUNT_TABLE . '` (`'
            . BUDATOLL_ACCOUNT_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_ACCOUNT_AMOUNT . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_ACCOUNT_EVENT_PLAYER_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_ACCOUNT_PLAYER_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_ACCOUNT_COMMENT . '` varchar(255) DEFAULT NULL, `'
            . BUDATOLL_ACCOUNT_TIME . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_ACCOUNT_ID . '` (`' . BUDATOLL_ACCOUNT_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
    $sql = 'CREATE TABLE IF NOT EXISTS `' . BUDATOLL_TRAINING_TABLE . '` (`'
            . BUDATOLL_TRAINING_ID . '` int(9) NOT NULL AUTO_INCREMENT,`'
            . BUDATOLL_TRAINING_PLAYER_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_TRAINING_EVENT_ID . '` int(8) NOT NULL DEFAULT -1,`'
            . BUDATOLL_TRAINING_DONE . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_CONFIRMED . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_PAID . '` int(1) NOT NULL DEFAULT 0,`'
            . BUDATOLL_TRAINING_COMMENT . '` varchar(255) DEFAULT NULL, `'
            . BUDATOLL_TRAINING_CREATED . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_TRAINING_ID . '` (`' . BUDATOLL_TRAINING_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
}

function bt_user_has_role($role) {
    if (is_user_logged_in()) {
        $user = wp_get_current_user();
        return in_array($role, $user->roles);
    } else {
        return false;
    }
}
