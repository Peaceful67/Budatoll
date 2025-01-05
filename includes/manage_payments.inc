<?php

if (!defined('ABSPATH')) {
    die();
}
define('BUDATOLL_PAYMENT_LIST_LENGTH', 32);

function bt_payment_management() {

    $ret = '<form id="BudatollForm" method="post">';
    //   $ret .= bt_get_all_posts();
    $ret .= '<div class="budatoll-message budatoll-success" id="budatoll-message">A mentés sikeres volt</div>';
    $ret .= '<div class="budatoll-message budatoll-error" id="error-message">Mentés sikertelen volt</div>';
    $user_id = getPostVariable('budatoll_edit');
    $budatoll_save = getPostVariable('budatoll_save');
    if (($user_id) && get_userdata($user_id)) {
        wp_redirect(home_url('/egyenleg?player_id=' . intval($user_id)));
        exit;
    }
    if (($add_payment = getPostVariable('budatoll_add')) > 0) {
        if (get_userdata($add_payment)) {
            return bt_payment_edit($add_payment, -1);
        }
    }
    if ($budatoll_save) {
        bt_payment_save($budatoll_save);
        wp_redirect(home_url('/egyenleg?player_id=' . intval($budatoll_save)));
        exit;
    }
    $ret .= list_balance_of_all_users(true, true);
    $ret .= '</form></div>';
    return $ret;
}

function bt_balance_of_user() {
    //   echo bt_get_all_posts();
    if (getPostVariable('budatoll_cancel')) {
        wp_redirect(home_url('/egyenlegek'));
        exit;
    }
    if (($user_id_add = getPostVariable('budatoll_add'))) {
        return bt_payment_edit($user_id_add);
    }
    $user_id = intval(filter_input(INPUT_GET, 'player_id'));
    if ($user_id > 0) {
        if (($edit_id = getPostVariable('budatoll_edit')) > 0) {
            if (getPostVariable('budatoll_save')) {
                bt_payment_save($user_id);
                wp_redirect(home_url('/egyenlegek'));
                exit;
            } else {
                return bt_payment_edit($user_id, $edit_id);
            }
        } else {
            return bt_list_balance_of_user($user_id);
        }
    } else {
        wp_redirect(home_url('/egyenlegek/'));
        exit;
    }
}

function bt_get_balance_of_user($user_id) {
    $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) AS `total` FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=' . $user_id;
    $sum = bt_get_results($sql);
    return intval($sum[0]->total);
}

function bt_my_balance() {
    $user_id = bt_get_current_userid();
    $ret = '';
    if (!get_userdata($user_id)) { // Valamiért nincs megadva játékos, bekérjük
        $ret .= bt_warning('Belső hiba. Valamiért nincs megadva játékos: ' . $user_id);
        return $ret;
    }
    $total = bt_get_balance_of_user($user_id);
//    $ret .= '<h4 ' . ($total < 0 ? 'style="color:red;"' : '') . '>' . bt_get_full_name($user_id) . ' Egyenlege: ' . bt_currency($total) . '</h4>';
    $sql = 'SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE  `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=%d'
            . ' ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';
    $result = bt_get_results($sql, [$user_id]);
    $ret .= '<div class="wrap">';
    $ret .= '<table class="budatoll-table">';
    $ret .= '<thead><tr><th>Időpont</th><th>Összeg</th><th>Egyenleg</th><th>Hivatkozás</th><th>Megjegyzés</th></tr></thead>';
    $ret .= '<tbody>';
    $num = BUDATOLL_PAYMENT_LIST_LENGTH;
    $balance = $total;
    foreach ($result as $item) {
        if ($item->{BUDATOLL_ACCOUNT_AMOUNT} >= 0) {
            $ref = 'Befizetés';
        } else {
            $ref = get_event_description($item->{BUDATOLL_ACCOUNT_EVENT_ID});
        }
        $amount = $item->{BUDATOLL_ACCOUNT_AMOUNT};
        $ret .= '<tr><td>' . $item->{BUDATOLL_ACCOUNT_TIME} . '</td>';
        $ret .= '<td>' . bt_currency($amount) . '</td>';
        $ret .= '<td>' . bt_currency($balance) . '</td>';
        $ret .= '<td>' . $ref . '</td>';
        $ret .= '<td>' . $item->{BUDATOLL_ACCOUNT_COMMENT} . '</td>';
        $ret .= '</tr>';
        $balance -= $amount;
        if ($num-- <= 1) {
            break;
        }
    }
    $ret .= '</tbody></table>';
    $ret .= '</div>';
    return $ret;
}

function bt_list_balance_of_user($user_id) {

    $ret = '<form id="BudatollForm" method="post">';
    if (!get_userdata($user_id)) { // Valamiért nincs megadva játékos, bekérjük
        $ret .= bt_warning('Belső hiba. Valamiért nincs megadva játékos: ' . $user_id);
        return $ret;
    }
    $ret .= putHiddenValue('budatoll_user_id', $user_id);
    $total = bt_get_balance_of_user($user_id);
    $ret .= '<h4 ' . ($total < 0 ? 'style="color:red;"' : '') . '>' . bt_get_full_name($user_id) . ' egyenlege. Összesen: '
            . bt_currency($total) . /* ', ' . bt_add_button($user_id, 'budatoll_add', 'Új befizetés') . */ '</h4>';
    $sql = 'SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE  `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=%d'
            . ' ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';
    $result = bt_get_results($sql, [$user_id]);
    $ret .= '<div class="wrap">';
    $ret .= '<table class="budatoll-table">';
    $ret .= '<thead><tr><th>Időpont</th><th>Összeg</th><th>Egyenleg</th><th>Hivatkozás</th><th>Megjegyzés</th><th>Művelet</th></tr></thead>';
    $ret .= '<tbody>';
    $num = BUDATOLL_PAYMENT_LIST_LENGTH;
    $balance = $total;
    foreach ($result as $item) {
        if ($item->{BUDATOLL_ACCOUNT_AMOUNT} >= 0) {
            $ref = 'Befizetés';
        } else {
            $ref = get_event_description($item->{BUDATOLL_ACCOUNT_EVENT_ID});
        }
        $amount = $item->{BUDATOLL_ACCOUNT_AMOUNT};
        $ret .= '<tr><td>' . $item->{BUDATOLL_ACCOUNT_TIME} . '</td>';
        $ret .= '<td>' . bt_currency($amount) . '</td>';
        $ret .= '<td>' . bt_currency($balance) . '</td>';
        $ret .= '<td>' . $ref . '</td>';
        $ret .= '<td>' . $item->{BUDATOLL_ACCOUNT_COMMENT} . '</td>';
        $ret .= '<td>' . bt_edit_button($item->{BUDATOLL_ACCOUNT_ID}) . ' </td>';
        $ret .= '</tr>';
        $balance -= $amount;
        if ($num-- <= 1) {
            break;
        }
    }
    $ret .= '</tbody></table>';
    $ret .= '</div></form>';
    return $ret;
}

function list_balance_of_all_users($list = false, $add_new = false) {
    $ret = '';
    $players = bt_get_active_players();
    $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) AS `total` FROM `' . BUDATOLL_ACCOUNT_TABLE . '`';
    $res = bt_get_results($sql);
    $ret .= '<h4>Egyenlegek</h4>';
    if ($res) {
        $ret .= '<p>Összesen: ' . bt_currency($res[0]->total) . '</p>';
    }
    $ret .= '<table class="budatoll-table">';
    $ret .= '<thead><tr><th>Felhasználónév</th><th>Egyenleg</th><th>Utolsó változás</th><th>Művelet</th></tr></thead>';
    $ret .= '<tbody>';
    foreach ($players as $player) {
        $user_id = $player->ID;
        $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) AS `SUM`, MAX(`' . BUDATOLL_ACCOUNT_TIME . '`) AS `LAST` FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=' . $user_id;
        $result = bt_get_results($sql);
        $balance = $result[0]->SUM ?? 0;
        $last_mod = $result[0]->LAST ?? '---';
        $ret .= '<tr>';
        $ret .= '<td>' . $player->display_name . ' (' . $user_id . ')</td>';
        $ret .= '<td ' . ($balance < 0 ? 'style="color:red;"' : '') . '>' . bt_currency($balance) . '</td>';
        $ret .= '<td>' . $last_mod . '</td>';
        $ret .= '<td>';
        if ($list) {
            $ret .= bt_edit_button($user_id);
        }
        if ($add_new) {
            $ret .= bt_add_button($user_id);
        }
        $ret .= '</td>';
        $ret .= '</tr>';
    }
    $ret .= '</tbody>';

    return $ret;
}

function bt_payment_edit($user_id, $edit_id = -1) {
    $ret = '<form id="BudatollForm" method="post">';
    if ($edit_id < 0) { // Újat veszünk fel
        $amount = 0;
        $comment = '';
    } else {
        $payment = getPaymentById($edit_id);
        if ($user_id != ($payment[BUDATOLL_ACCOUNT_PLAYER_ID] ?? -1)) {
            return bt_warning('Belső hiba! Érvénytelen játékos azonosító!');
        }
        $amount = $payment[BUDATOLL_ACCOUNT_AMOUNT] ?? 0;
        $comment = $payment[BUDATOLL_ACCOUNT_COMMENT] ?? '';
    }
    $ret .= putHiddenValue('budatoll_edit', $edit_id);
    $ret .= '<div class="budatoll-keret">';
    $ret .= '<table class="table budatoll-no-border">';
    $ret .= '<tr><th style="width:10%;">Játékos: </th><td>';
    $ret .= bt_get_full_name($user_id);
    $ret .= '</td></tr>';
    $ret .= '<tr><th>Összeg</th><td>';
    $ret .= '<input type="number" name="payment_amount" value="' . $amount . '"> Ft';
    $ret .= '</td></tr>';
    $ret .= '<tr><th>Megjegyzés</th><td>';
    $ret .= '<input type="text" name="payment_comment" value="' . $comment . '">';
    $ret .= '</td></tr>';
    $ret .= '<tr><td></td><td>';
    $ret .= bt_save_button($user_id);
    $ret .= bt_cancel_button(-1);
    $ret .= '</td></tr>';
    $ret .= '</table></div></form>';

    return $ret;
}

function bt_payment_save($user_id) {
    $amount = getPostVariable('payment_amount', 0);
    $account_id = getPostVariable('budatoll_edit', -1);
    $data = [
        BUDATOLL_ACCOUNT_PLAYER_ID => $user_id,
        BUDATOLL_ACCOUNT_COMMENT => getPostVariable('payment_comment', ''),
        BUDATOLL_ACCOUNT_AMOUNT => $amount,
    ];
    $format = ['%d', '%s', '%d'];
    if ($user_id > 0) {
        global $wpdb;
        if ($account_id > 0) {
            $wpdb->update(BUDATOLL_ACCOUNT_TABLE, $data, [BUDATOLL_ACCOUNT_ID => $account_id]);
            bt_logger(($amount >= 0 ? BUDATOLL_LOGGER_TYPE_PAYMENT : BUDATOLL_LOGGER_TYPE_CHARGE), $user_id,
                    ($amount >= 0 ? 'Befizetés módosult:' : 'Terhelés módosult:') . bt_currency($amount));
            if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT)) {
                bt_send_custom_email($user_id, ($amount >= 0 ? BT_CUSTOM_EMAIL_PAYMENT_MODIFY : BT_CUSTOM_EMAIL_CHARGE_MODIFY), [bt_currency($amount), bt_currency(get_balance_of_user($user_id))]);
            }
        } else {
            $wpdb->insert(BUDATOLL_ACCOUNT_TABLE, $data, $format);
            bt_logger(BUDATOLL_LOGGER_TYPE_PAYMENT, $user_id, 'Befizetés érkezett:' . bt_currency($amount));
            if (btIsOptionEnabled(BUDATOLL_OPTION_EMAIL_NOTICE_PAYMENT)) {
                bt_send_custom_email($user_id, BT_CUSTOM_EMAIL_PAYMENT_CREATE, [bt_currency($amount), bt_currency(get_balance_of_user($user_id))]);
            }
        }
    } else {
        return false;
    }
    return true;
}

function bt_payment_list() {

    $payments_to = getPostVariable('payments_to', wp_date('Y-m-d'));
    $payments_from = getPostVariable('payments_from', wp_date('Y-m-d', strtotime($payments_to . ' -2 month')));
    $player_id = getPostVariable('select_listed_player', -1);
    $ret = '<table class="table" style="border:none;">';
    $ret .= '<tr><td style="width:10%;">';
    $ret .= '<button class="button budatoll-button" name="budatoll_add" id="budatoll_add" value="-1" title="Új hozzáadása">'
            . '<span class="dashicons dashicons-plus"></span></button>';
    $ret .= '</td><td style="30%;">';
    $ret .= '<label for="select_listed_player">Játékos:</label>';
    $ret .= put_select_player('listed_player', $player_id, true);
    $ret .= '</td><td style="30%;">';
    $ret .= '<label for="payment_from">Időszak:</label>'
            . '<input type="date" id="payment_from" name="payment_from" value="' . $payments_from . '" onchange="submit();"> - '
            . '<input type="date" id="payment_to" name="payment_to" value="' . $payments_to . '" onchange="submit();">';
    $ret .= '</td>';
    $ret .= '</tr></table>';
    $sql = 'SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_AMOUNT . '`>=%d '
            . ($player_id >= 0 ? (' AND `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=%d') : '')
            . ' AND `' . BUDATOLL_ACCOUNT_TIME . '` BETWEEN "%s 00:00:00" AND "%s 23:59:59" '
            . ' ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';
    $format = [0, $player_id, $payments_from, $payments_to];
    $ret .= '<table class="wp-list-table widefat fixed striped">';
    $ret .= '<thead><tr>'
            . '<th scope="col" id="column1" class="manage-column column-column1" style="width:20%;">Játékos</th>'
            . '<th scope="col" id="column2" class="manage-column column-column2" style="width:10%;">Összeg</th>'
            . '<th scope="col" id="column3" class="manage-column column-column3" style="width:20%;">Időpont</th>'
            . '<th scope="col" id="column4" class="manage-column column-column4" style="width:30%;">Megjegyzés</th>'
            . '<th scope="col" id="column5" class="manage-column column-column5" style="width:20%;">Művelet</th>'
            . '</tr></thead><tbody>';
    foreach (bt_get_results($sql, $format) as $payment) {
        $user = get_userdata($payment->{BUDATOLL_ACCOUNT_PLAYER_ID});
        $user_name = $user ? $user->display_name : 'Nem létező játékos';
        $amount = $payment->{BUDATOLL_ACCOUNT_AMOUNT};
        $comment = $payment->{BUDATOLL_ACCOUNT_COMMENT};
        $time = $payment->{BUDATOLL_ACCOUNT_TIME};
        $payment_id = $payment->{BUDATOLL_ACCOUNT_ID};
        $ret .= '<tr>';
        $ret .= '<td class="column-column1">' . $user_name . '</td>';
        $ret .= '<td class="column-column2">' . $amount . '</td>';
        $ret .= '<td class="column-column3">' . $time . '</td>';
        $ret .= '<td class="column-column4">' . $comment . '</td>';
        $ret .= '<td class="column-column5">';
        $ret .= '<button class="button budatoll-button button-small" name="budatoll_edit" id="budatoll_edit" value="' . $payment_id . '" title="Szerkesztés">'
                . '<span class="dashicons dashicons-edit"></span></button>'
                . '<button class="button budatoll-button button-small" name="budatoll_delete" id="budatoll_delete" value="' . $payment_id . '" '
                . 'onclick="return confirm(\'Biztosan törölni akarod a befizetést?\');" title="Törlés">'
                . '<span class="dashicons dashicons-trash"></span></button>';
        $ret .= '</td>';
        $ret .= '</tr>';
    }
    $ret .= '</tbody></table>';
    return $ret;
}

function getPaymentById($id) {
    $result = bt_get_results('SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_ID . '`=%d;', [$id]);
    $ret = get_object_vars($result[0] ?? []);
    return $ret;
}

function get_balance_of_user($user_id) {
    $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) AS `amount` FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_AMOUNT . '`>=%d '
            . ' AND `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=%d '
            . 'ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';
    $result = bt_get_results($sql, [0, $user_id]);
    return $result[0]->amount ?? 0;
}

function get_event_description($event_id) {
    $event = get_event_by_id($event_id);
    $ret = '---';
    if ($event) {
        $ret = $event->{BUDATOLL_EVENTS_SHORT};
        $ret .= '<br>' . $event->{BUDATOLL_EVENTS_DAY} . '';
    }
    return $ret;
}
