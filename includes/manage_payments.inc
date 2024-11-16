<?php

if (!defined('ABSPATH')) {
    die();
}

function bt_payment_management() {
    $ret = '<form id="BudatollForm" method="post">';
    $budatoll_mod = getPostVariable('budatoll_mod', 'all_balances');
    switch ($budatoll_mod) {
        default:
        case 'all_balances':
            $ret .= list_balance_of_all_users(true, true);
            break;
        case 'balance_of_user':
            $user_id = getPostVariable('budatoll_user_id, -1');
            $ret .= bt_list_balance_of_user($user_id);
            $ret .= putHiddenValue('budatoll_mod', 'balance_of_user');
            break;
    }

    $ret .= '</form></div>';
    return $ret;
}

function bt_list_balance_of_user($user_id) {
    global $wpdb;
    $ret = '';
    if ($user_id < 0) { // Valamiért nincs megadva játékos, bekérjük
        $ret .= bt_warning('Belső hiba. Valamiért nincs megadva játékos');
        return $ret;
    }
    $ret .= '<h4>' . bt_get_full_name($user_id) . ' egyenlegének tételei</h4>';
    $sql = 'SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE  `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=' . $user_id
            . ' ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';
    $result = $wpdb->get_results($sql);
    $ret .= '<div class="wrap">';
    $ret .= '<table class="budatoll-table">';
    $ret .= '<thead><tr><th>Időpont</th><th>Összeg</th><th>Hivatkozás</th><th>Megjegyzés</th><th>Művelet</th></tr></thead>';
    $ret .= '<tbody>';
    foreach ($result as $item) {
        if ($item->{BUDATOLL_ACCOUNT_AMOUNT} >= 0) {
            $ref = 'Befizetés';
        } else {
            $ref = $item->{BUDATOLL_ACCOUNT_EVENT_PLAYER_ID};
        }
        $ret .= '<tr><td>' . $item->{BUDATOLL_ACCOUNT_TIME} . '</td>';
        $ret .= '<td>' . $item->{BUDATOLL_ACCOUNT_AMOUNT} . ' Ft</td>';
        $ret .= '<td>' . $ref . '</td>';
        $ret .= '<td>' . $item->{BUDATOLL_ACCOUNT_COMMENT} . '</td>';
        $ret .= '<td>' . bt_edit_button($item->{BUDATOLL_ACCOUNT_ID}) . ' ' . bt_delete_button($item->{BUDATOLL_ACCOUNT_ID}) . '</td>';
        $ret .= '</tr>';
    }
    $ret .= '</tbody></table>';
    $ret .= '</div>';
    return $ret;
}

function list_balance_of_all_users($list = false, $add_new = false) {
    global $wpdb;
    $ret = '';
    $budatoll_edit = getPostVariable('budatoll_edit');
    $budatoll_save = getPostVariable('budatoll_save');
    $budatoll_add = getPostVariable('budatoll_add');
    if ($budatoll_add) {
        $ret .= bt_payment_edit($budatoll_add, -1);
    } elseif ($budatoll_edit) {
        $ret .= putHiddenValue('budatoll_mod', 'balance_of_user');
        $ret .= putHiddenValue('budatoll_user_id', $budatoll_edit);
        $ret .= bt_list_balance_of_user($budatoll_edit);
        return $ret;
    } elseif ($budatoll_save) {
        $user_id = getPostVariable('user_id', -1);
        $amount = getPostVariable('payment_amount', 0);
        $data = [
            BUDATOLL_ACCOUNT_PLAYER_ID => $user_id,
            BUDATOLL_ACCOUNT_COMMENT => getPostVariable('payment_comment', ''),
            BUDATOLL_ACCOUNT_AMOUNT => $amount,
        ];
        if ($user_id > 0) {
            $wpdb->insert(BUDATOLL_ACCOUNT_TABLE, $data);
            logger(BUDATOLL_LOGGER_TYPE_PAYMENT, $user_id, 'Befizetés érkezett:' . $amount . ' Ft');
            //           }
        } else {
            $ret .= bt_warning('Belső hiba. Befizetés rögzítése nem történt meg!');
        }
    }

    $players = bt_get_active_players();
    $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) AS `total` FROM `' . BUDATOLL_ACCOUNT_TABLE . '`';
    $res = bt_get_results($sql);
    $ret .= '<h4>Egyenlegek</h4>';
    if ($res) {
        $ret .= 'Teljes egyenleg: ' . $res[0]->total . ' Ft';
    }
    $ret .= '<table class="budatoll-table">';
    $ret .= '<thead><tr><th>Felhasználónév</th><th>Egyenleg</th><th>Utolsó változás</th><th>Művelet</th></tr></thead>';
    $ret .= '<tbody>';
    foreach ($players as $player) {
        $user_id = $player->ID;
        $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) AS `SUM`, MAX(`' . BUDATOLL_ACCOUNT_TIME . '`) AS `LAST` FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_AMOUNT . '`>=%d '
                . ' AND `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=%d';
        $result = bt_get_results($sql, [0, $user_id]);
        $balance = $result[0]->SUM ?? 0;
        $last_mod = $result[0]->LAST ?? '---';
        $ret .= '<tr>';
        $ret .= '<td>' . $player->display_name . ' (' . $user_id . ')</td>';
        $ret .= '<td>' . $balance . ' Ft</td>';
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

function bt_payment_edit($user_id, $edit_id) {
    $ret = '';
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
    $ret .= putHiddenValue('user_id', $user_id);
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
    $ret .= '<tr><td>';
    $ret .= '<button class="button budatoll-button" name="budatoll_save" title="Mentés" value="' . $edit_id . '">'
            . '<span class="dashicons dashicons-saved"></span></button></td>';
    $ret .= '<td><button class="button budatoll-button" name="budatoll_cancel" title="Mégsem" value="-1">'
            . '<span class="dashicons dashicons-undo"></span></button></td>';
    $ret .= '</tr>';
    $ret .= '</table></div>';
    return $ret;
}

function bt_payment_list() {
    global $wpdb;

    $payments_to = getPostVariable('payments_to', date('Y-m-d'));
    $payments_from = getPostVariable('payments_from', date('Y-m-d', strtotime($payments_to . ' -2 month')));
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
    $sql = 'SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_AMOUNT . '`>=0 '
            . ($player_id >= 0 ? (' AND `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=' . $player_id) : '')
            . ' AND `' . BUDATOLL_ACCOUNT_TIME . '` BETWEEN "' . $payments_from . ' 00:00:00" AND "' . $payments_to . ' 23:59:59" '
            . 'ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';

    $ret .= '<table class="wp-list-table widefat fixed striped">';
    $ret .= '<thead><tr>'
            . '<th scope="col" id="column1" class="manage-column column-column1" style="width:20%;">Játékos</th>'
            . '<th scope="col" id="column2" class="manage-column column-column2" style="width:10%;">Összeg</th>'
            . '<th scope="col" id="column3" class="manage-column column-column3" style="width:20%;">Időpont</th>'
            . '<th scope="col" id="column4" class="manage-column column-column4" style="width:30%;">Megjegyzés</th>'
            . '<th scope="col" id="column5" class="manage-column column-column5" style="width:20%;">Művelet</th>'
            . '</tr></thead><tbody>';
    foreach ($wpdb->get_results($sql) as $payment) {
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
    global $wpdb;
    $result = $wpdb->get_results('SELECT * FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_ID . '`=' . $id . ';');
    $ret = get_object_vars($result[0] ?? []);
    return $ret;
}

function get_balance_of_user($user_id) {
    global $wpdb;
    $sql = 'SELECT SUM(`' . BUDATOLL_ACCOUNT_AMOUNT . '`) FROM `' . BUDATOLL_ACCOUNT_TABLE . '` WHERE `' . BUDATOLL_ACCOUNT_AMOUNT . '`>=0 '
            . ' AND `' . BUDATOLL_ACCOUNT_PLAYER_ID . '`=' . $user_id
            . 'ORDER BY `' . BUDATOLL_ACCOUNT_TIME . '` DESC;';
    $result = $wpdb->get_results($sql);
    return get_object_vars($result[0] ?? []);
}
