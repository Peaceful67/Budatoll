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

function budatoll_crontab() {
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
    if (($datetime->getTimestamp() - $last) > BUDATOLL_CRONTAB_INTERVAL - 10) {
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
        } finally {
            if (ob_get_length()) {
                ob_end_flush();
            }
        }
    }
}

function bt_trainings_make_done() {
    global $wpdb;
    $events_done = [];
    $sql = 'SELECT `' . BUDATOLL_TRAINING_TABLE . '`.*, `' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_LONG . '`, `' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_DAY . '`,  `' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_START . '` '
            . ' FROM `' . BUDATOLL_TRAINING_TABLE . '` INNER JOIN `' . BUDATOLL_EVENTS_TABLE . '` ON ('
            . '`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_ID . '`=`' . BUDATOLL_TRAINING_TABLE . '`.`' . BUDATOLL_TRAINING_EVENT_ID . '` AND '
            . ' (CURDATE()>`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_DAY . '` OR '
            . '(CURDATE()=`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_DAY . '`  AND  CURTIME()>=`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_START . '`)))'
            . ' WHERE `' . BUDATOLL_TRAINING_TABLE . '`.`' . BUDATOLL_TRAINING_DONE . '`=0 AND `' . BUDATOLL_TRAINING_TABLE . '`.`' . BUDATOLL_TRAINING_CONFIRMED . '`=1;';
    $trainings = $wpdb->get_results($sql);
    error_log($sql);
    $data = [BUDATOLL_TRAINING_DONE => 1,];
    foreach ($trainings as $training) {
        if (!in_array($training->{BUDATOLL_TRAINING_EVENT_ID}, $events_done)) {
            $events_done[] = $training->{BUDATOLL_TRAINING_EVENT_ID};
            bt_logger(BUDATOLL_LOGGER_TYPE_TRAININGS, -1, 'Edzés lezajlott: ' . $training->{BUDATOLL_EVENTS_LONG} . ' ('
                    . $training->{BUDATOLL_EVENTS_DAY} . ' / ' . $training->{BUDATOLL_EVENTS_START} . ')');
        }
        $wpdb->update(BUDATOLL_TRAINING_TABLE, $data, [BUDATOLL_TRAINING_ID => $training->{BUDATOLL_TRAINING_ID}]);
//        bt_training_charge($training->{BUDATOLL_TRAINING_ID});
    }
    foreach ($events_done as $event_id) {
        $wpdb->update(BUDATOLL_EVENTS_TABLE, [BUDATOLL_EVENTS_DONE => 1], [BUDATOLL_EVENTS_ID => $event_id]);
    }
}
