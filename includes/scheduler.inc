<?php

define("BUDATOLL_CRONTAB_INTERVAL", 300);
if (!defined('ABSPATH')) {
    die();
}

function budatoll_cron_interval($schedules) {
    if (!isset($schedules['every_five_minutes'])) {
        $schedules['every_five_minutes'] = array(
            'interval' => BUDATOLL_CRONTAB_INTERVAL, // Interval in seconds
            'display' => esc_html__('Every Five Minutes'),
        );
    }
    return $schedules;
}

function budatoll_crontab($instant = false) {
    global $wpdb;

    // Get WordPress timezone
    $timezone = new DateTimeZone(get_option('timezone_string') ?: 'UTC');

    // Get current time in WordPress timezone
    $now = wp_date('Y-m-d H:i');
    $datetime = new DateTime($now, $timezone);

    // Get the last cron execution time or use a default value
    $last_option = get_option(BUDATOLL_OPTION_LAST_CRONTAB, '1970-01-01 00:00:00');
    $last_datetime = new DateTime($last_option, $timezone);
    $last = $last_datetime->getTimestamp();

    // Check if the interval has passed
    if ($instant OR (($datetime->getTimestamp() - $last) > BUDATOLL_CRONTAB_INTERVAL - 10)) {
        //       error_log('Crontab executed at: ' . $now);
        // Delete old logs
        $wpdb->query('DELETE FROM `' . BUDATOLL_LOGGER_TABLE . '` WHERE `' . BUDATOLL_LOGGER_TIME . '` < DATE_SUB(NOW(), INTERVAL 2 YEAR)');

        // Update last execution time
        update_option(BUDATOLL_OPTION_LAST_CRONTAB, $now);

        // Execute tasks
        if (!headers_sent()) {
            ob_start();
        }
        try {
            bt_trainings_make_done();
            bt_charge_passed_trainings();
            bt_inactive_players();
        } finally {
            if (ob_get_length()) {
                ob_end_flush();
            }
        }
    }
}

function bt_trainings_make_done() {
    global $wpdb;
    $sql = 'SELECT * FROM `' . BUDATOLL_EVENTS_TABLE . '` WHERE `' . BUDATOLL_EVENTS_DONE . '`=0 '
            . ' AND (CURDATE()>`' . BUDATOLL_EVENTS_DAY . '` OR '
            . '(CURDATE()=`' . BUDATOLL_EVENTS_DAY . '`  AND  CURTIME()>=`' . BUDATOLL_EVENTS_START . '`))';
    $events = bt_get_results($sql);
    if ($events) {
        foreach ($events as $event) {
            bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, -1, 'Edzés lezajlott: ' . $event->{BUDATOLL_EVENTS_LONG} . ' ('
                    . $event->{BUDATOLL_EVENTS_DAY} . ' / ' . $event->{BUDATOLL_EVENTS_START} . ')');
            $wpdb->update(BUDATOLL_EVENTS_TABLE, [BUDATOLL_EVENTS_DONE => 1], [BUDATOLL_EVENTS_ID => $event->{BUDATOLL_EVENTS_ID}]);
        }
    }
    $sql = 'UPDATE `' . BUDATOLL_TRAINING_TABLE . '` AS `bt`'
            . ' INNER JOIN `' . BUDATOLL_EVENTS_TABLE . '` AS `be` ON `bt`.`event_id` = `be`.`id`'
            . ' SET `bt`.`done` = 1 WHERE `be`.`done` = 1;';
    $wpdb->query($sql);
}

function bt_inactive_players() {
    global $wpdb;
    $month = 6;
    $players = bt_get_active_players();
    if (empty($players)) {
        return;
    }
    foreach ($players as $player) {
        $player_id = $player->ID;
        $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_PLAYER_ID . '`=' . $player_id . ' AND '
                . ' `' . BUDATOLL_TRAINING_CREATED . '` > DATE_SUB(NOW(), INTERVAL ' . $month . ' MONTH);';
        $wpdb->query($sql);
        $last = $wpdb->num_rows;
        $sql = 'SELECT * FROM `' . BUDATOLL_TRAINING_TABLE . '` WHERE `' . BUDATOLL_TRAINING_PLAYER_ID . '`=' . $player_id . ' AND '
                . ' `' . BUDATOLL_TRAINING_CREATED . '` <= DATE_SUB(NOW(), INTERVAL ' . $month . ' MONTH);';
        $wpdb->query($sql);
        if ($wpdb->num_rows > 0 AND $last == 0) { // Ha régen volt, de az utóbbi időben semmi.
            update_user_meta($player_id, BUDATOLL_USER_META_ACTIVE, 0);
            logger(BUDATOLL_LOGGER_TYPE_USER_DATA, $player_id, $month . ' hónapja nem jelentkezett, inaktiváltuk');
        }
    }
}
