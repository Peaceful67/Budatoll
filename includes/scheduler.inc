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
    $now = (new DateTime('now'))->getTimestamp();
    $last = (new DateTime(get_option(BUDATOLL_OPTION_LAST_CRONTAB)))->getTimestamp();

    if (($now - $last) > BUDATOLL_CRONTAB_INTERVAL - 10) {
        ob_start();
        bt_trainings_make_done();
        bt_charge_passed_trainings();
        update_option(BUDATOLL_OPTION_LAST_CRONTAB, date('Y-m-d H:i'));
    }
}

function bt_trainings_make_done() {
    global $wpdb;
    $events_done = [];
    $sql = 'SELECT `' . BUDATOLL_TRAINING_TABLE . '`.*, `' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_LONG . '`, `' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_DAY . '`,  `' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_START . '` '
            . ' FROM `' . BUDATOLL_TRAINING_TABLE . '` INNER JOIN `' . BUDATOLL_EVENTS_TABLE . '` ON ('
            . '`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_ID . '`=`' . BUDATOLL_TRAINING_TABLE . '`.`' . BUDATOLL_TRAINING_ID . '` AND '
            . ' CURDATE()>=`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_DAY . '` AND  CURTIME()>=`' . BUDATOLL_EVENTS_TABLE . '`.`' . BUDATOLL_EVENTS_START . '`)'
            . ' WHERE `' . BUDATOLL_TRAINING_TABLE . '`.`' . BUDATOLL_TRAINING_DONE . '`=0 AND `' . BUDATOLL_TRAINING_TABLE . '`.`' . BUDATOLL_TRAINING_CONFIRMED . '`=1;';
    $trainings = $wpdb->get_results($sql);
    $data = [BUDATOLL_TRAINING_DONE => 1,];
    foreach ($trainings as $training) {
        if (!in_array($training->{BUDATOLL_TRAINING_EVENT_ID}, $events_done)) {
            $events_done[] = $training->{BUDATOLL_TRAINING_EVENT_ID};
            logger(BUDATOLL_LOGGER_TYPE_TRAININGS, -1, 'Edzés lezajlott: ' . $training->{BUDATOLL_EVENTS_LONG} . ' ('
                    . $training->{BUDATOLL_EVENTS_DAY} . ' / ' . $training->{BUDATOLL_EVENTS_START} . ')');
        }
        $wpdb->update(BUDATOLL_TRAINING_TABLE, $data, [BUDATOLL_TRAINING_ID => $training->{BUDATOLL_TRAINING_ID}]);
        if (btIsOptionEnabled(BUDATOLL_OPTION_AUTO_CHARGE)) {
            bt_training_charge($training->{BUDATOLL_TRAINING_ID});
        }
    }
}
