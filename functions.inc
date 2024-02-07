<?php

function isBudatollPagePermited($page_name) {
    $page_acccess = [
        'manage_balances' => [BUDATOLL_ACCESS_MANAGE_BALANCES],
        'view_balance' => [BUDATOLL_ACCESS_VIEW_BALANCE],
    ];
    if (!isset($page_acccess[$page_name])) {
        return false;
    }
    if (current_user_can('administrator')) {
        return true;
    }
    $current_user = wp_get_current_user();
    foreach ($page_acccess[$page_name] as $required) {
        if ($current_user->has_cap($required)) {
            return true;
        }
    }
    return false;
}

function putPermissionNotGranted() {
    return '<div class="et-pb-promo-error">Az oldal megtekintéséhez nincs jogosultsága!</div>';
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

function bt_send_email($user_id, $subject, $message) {
    $user = get_userdata($user_id);
    if (!$user) {
        return false; // User not found
    }
    $user_email = sanitize_email($user->user_email);
    if (!is_email($user_email)) {
        return false;
    }
    $headers = array('Content-Type: text/html; charset=UTF-8');
    $message .= '<br><br>Üdvözlettel:<br>Budatoll csapata';
    return wp_mail($user_email, $subject, $message, $headers);
}

function budatoll_deactivated() {
    remove_role(BUDATOLL_ROLE_COACH);
    remove_role(BUDATOLL_ROLE_PLAYER);
}

function budatoll_menu_based_on_role($args) {

    if ('primary-menu' === $args['theme_location']) {
        $user = wp_get_current_user();
        if (in_array('administrator', (array) $user->roles) AND wp_get_nav_menu_object('Admin')) {
            $args['menu'] = 'Admin';
        } elseif (in_array('coach', (array) $user->roles) AND wp_get_nav_menu_object('Coach')) {
            $args['menu'] = 'Coach';
        } elseif (in_array('player', (array) $user->roles) AND wp_get_nav_menu_object('Player')) {
            $args['menu'] = 'Player';
        }
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
            . BUDATOLL_TRAINING_COMMENT . '` varchar(255) DEFAULT NULL, `'
            . BUDATOLL_TRAINING_CREATED . '` timestamp(1) NOT NULL, '
            . 'UNIQUE KEY `' . BUDATOLL_TRAINING_ID . '` (`' . BUDATOLL_TRAINING_ID . '`)) '
            . $wpdb->get_charset_collate() . ';';
    dbDelta($sql);
}
