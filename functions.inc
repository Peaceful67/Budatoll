<?php

if (!defined('ABSPATH')) {
    die();
}

function budatoll_is_page_allowed() {

    $permalink = rtrim($_SERVER['REQUEST_URI'], '/');
    if (bt_user_has_role('administrator')) { // Ennek bármit engedünk
        return;
    }

    $page_acccess = [// Ezeket az oldalakat csak a megadott szerepkörrel érhetjük el
        'anybody' => ['', '/nincs-jogosultsag', '/belepes', '/gyik', '/rolunk'],
        'coach' => ['/alkalom-naptar', '/alkalom-lista', '/edzesek-naptar', '/edzesek-lista', '/egyenlegek', '/egyenleg'],
        'player' => ['/edzeseim-lista', '/edzeseim-naptar', '/egyenlegem',],
    ];

    if (is_user_logged_in()) {
        $user = wp_get_current_user();
        $roles = $user->roles;
    } else {
        $roles = [];
    }
    $roles[] = 'anybody';
    foreach ($roles as $role) {  // Vesszük az összes szerepet 
        if (in_array($permalink, $page_acccess[$role])) { // Ha benne van az aktuális, örülünk
            return;
        }
    }
    wp_redirect(site_url('/nincs-jogosultsag'));
    exit();
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
//    $ret = filter_input(INPUT_POST, $var_name, FILTER_NULL_ON_FAILURE);
//    error_log('getPostVariable '.$var_name.': '.$_POST[$var_name]);
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
        $ret = 'Belépve: ' . $user->display_name;
    }
    return $ret;
}

function bt_get_current_userid() {
    $user = wp_get_current_user();
    return ($user->ID == 0) ? false : $user->ID;
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
        'From: ' . get_option('blogname') . ' <from@budatoll.hu>',
        'Reply-to: no-reply@budatoll.hu',
    ];
    $message .= '<br><br>Üdvözlettel:<br>Budatoll csapata';
    logger(BUDATOLL_LOGGER_TYPE_EMAILS, $user_id, $subject.', '.substr($message, 0, 128));
    wp_mail($user_email, $subject, $message, $headers);
}

function budatoll_deactivated() {
    remove_role(BUDATOLL_ROLE_COACH);
    remove_role(BUDATOLL_ROLE_PLAYER);
    wp_clear_scheduled_hook('budatoll_cron_hook');
}

function budatoll_menu_based_on_role($args) {
    if (is_user_logged_in() AND ('primary-menu' === $args['theme_location'])) {
        $user = wp_get_current_user();
        if (in_array(BUDATOLL_ROLE_ADMIN, (array) $user->roles) AND wp_get_nav_menu_object('Admin')) {
            $args['menu'] = 'Admin';
        } elseif (in_array(BUDATOLL_ROLE_COACH, (array) $user->roles) AND wp_get_nav_menu_object('Coach')) {
            $args['menu'] = 'Coach';
        } elseif (in_array(BUDATOLL_ROLE_PLAYER, (array) $user->roles) AND wp_get_nav_menu_object('Player')) {
            $args['menu'] = 'Player';
        }
    } elseif (wp_get_nav_menu_object('Visitor')) {
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
    add_role(BUDATOLL_ROLE_COACH, 'Edző', ['read' => true, BUDATOLL_ACCESS_MANAGE_OPTIONS => false, BUDATOLL_ACCESS_MANAGE_COACH => true]);
    add_role(BUDATOLL_ROLE_PLAYER, 'Játékos', ['read' => true, BUDATOLL_ACCESS_MANAGE_PLAYER => true]);
    if (get_role(BUDATOLL_ROLE_PLAYER)) {
        update_option('default_role', BUDATOLL_ROLE_PLAYER);
    }
    if (!wp_next_scheduled('budatoll_cron_hook')) {
        wp_schedule_event(time(), 'every_five_minutes', 'budatoll_cron_hook');
    }
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

function bt_get_option($category, $name) {
    $opt_arr = unserialize(get_option($category));
    return $opt_arr[$name] ?? false;
}

function bt_set_option($category, $name, $value) {
    $opt_arr = unserialize(get_option($category));
    $opt_arr[$name] = $value;
    update_option($category, serialize($opt_arr));
}

function bt_set_default_role($user_id) {
    $user = new WP_User($user_id);
    foreach ($user->roles as $role) {
        $user->remove_role($role);
    }
    $user->add_role(BUDATOLL_ROLE_PLAYER);
}

function bt_get_results($sql, $args = false) {
    global $wpdb;
    //   error_log('SQL: ' . $sql . ', args: ' . print_r($args, true));
    if ($args) {
        $prep = $wpdb->prepare($sql, $args);
    } else {
        $prep = $sql;
    }
    $res = $wpdb->get_results($prep);
    if (is_null($res) || !empty($wpdb->last_error)) {
        return false;
    } else {
        return $res;
    }
}

function bt_currency($num) {
    return number_format($num, 0, '.', ' ') . ' Ft';
}

function bt_redirect($url, $arr_post) {
    $ret = '<form id="btRedirectForm" method="POST" action="' . esc_url(home_url($url)) . '">';
    foreach ($arr_post as $key => $value) {
        $ret .= '<input type="hidden" name="' . $key . '" value="' . $value . '">';
    }
    $ret .= '</form><script>  document.getElementById("btRedirectForm").submit(); </script>';
    return $ret;
}

function bt_get_all_posts() {
    $ret = '';
    foreach ($_POST as $name => $value) {
        if (is_array($value)) {
            $ret .= print_r($value, true) . '<br>';
        } else {
            $ret .= $name . ' => ' . $value . '<br>';
        }
    }
    return $ret;
}

function bt_init() {
    if (!headers_sent()) {
        ob_start();
    }
}
