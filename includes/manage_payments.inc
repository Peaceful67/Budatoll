<?php

function list_balance_of_users() {
    if (!isBudatollPagePermited('manage_balances')) {
        return putPermissionNotGranted();
    }
    $ret = 'Ez a lista az egyenlegekről már a fejlesztésből.<br>';


    foreach (bt_get_active_players() as $user) {
        $ret .= 'ID: ' . $user->ID . ', user name:' . $user->user_login . '<br>';
    }
    return $ret;
}

function bt_payment_management() {
    global $wpdb;
    $ret = '<h4>Befizetések kezelése</h4>';
    $ret .= '<div class="wrap">';
    $ret .= print_r($_POST, true);

    $budatoll_delete = getPostVariable('budatoll_delete');
    $budatoll_edit = getPostVariable('budatoll_edit');
    $budatoll_save = getPostVariable('budatoll_save');
    $ret .= '<form id="BudatollForm" method="post">';
    if (getPostVariable('budatoll_add')) {
        $ret .= bt_payment_edit(-1);
    } elseif ($budatoll_edit) {
        $ret .= bt_payment_edit($budatoll_edit);
    } elseif ($budatoll_delete) {
        $payment = getPaymentById($budatoll_delete);
        $amount = $payment->{BUDATOLL_ACCOUNT_AMOUNT};
        $user_id = $payment->{BUDATOLL_ACCOUNT_PLAYER_ID};
        $wpdb->delete(BUDATOLL_ACCOUNT_TABLE, [BUDATOLL_ACCOUNT_ID => $budatoll_delete]);
        logger(BUDATOLL_LOGGER_TYPE_PAYMENT, $user_id, 'Befizetés (id: ' . $budatoll_delete . ') törlése:' . $amount . ' Ft');
    } elseif ($budatoll_save) {
        $user_id = getPostVariable('select_edited_player', -1);
        $amount = getPostVariable('payment_amount', 0);
        $data = [
            BUDATOLL_ACCOUNT_PLAYER_ID => $user_id,
            BUDATOLL_ACCOUNT_COMMENT => getPostVariable('payment_comment', ''),
            BUDATOLL_ACCOUNT_AMOUNT => $amount,
        ];
        if ($budatoll_save > 0) {
            $wpdb->update(BUDATOLL_ACCOUNT_TABLE, $data, [BUDATOLL_ACCOUNT_ID => $budatoll_save]);
            logger(BUDATOLL_LOGGER_TYPE_PAYMENT, $user_id, 'Befizetés (id: ' . $budatoll_save . ') módosítása:' . $amount . ' Ft');
        } else {
            $edit_id = $wpdb->insert(BUDATOLL_ACCOUNT_TABLE, $data);
            logger(BUDATOLL_LOGGER_TYPE_PAYMENT, -1, 'Befizetés (id: ' . $edit_id . ') érkezett:' . $amount . ' Ft');
        }
        /*
          if ($user_id <= 0) {
          $ret .= 'Válasszd ki a játékost, aki befizetett!';
          $ret .= bt_payment_edit($edit_id);
          } elseif ($amount <= 0) {
          $ret .= 'Állítsd be a befizetés összegét!';
          $ret .= bt_payment_edit($edit_id);
          } else {
          $edit_id = -1;
          }
         * 
         */
    } elseif (getPostVariable('budatoll_cancel')) {
        $edit_id = -1;
    }

    $ret .= bt_payment_list();
    $ret .= '</form></div>';
    return $ret;
}

function bt_payment_edit($edit_id) {
    if ($edit_id < 0) { // Újat veszünk fel
        $user_id = -1;
        $amount = 0;
        $comment = '';
    } else {
        $payment = getPaymentById($edit_id);
        $user_id = $payment[BUDATOLL_ACCOUNT_PLAYER_ID] ?? -1;
        $amount = $payment[BUDATOLL_ACCOUNT_AMOUNT] ?? 0;
        $comment = $payment[BUDATOLL_ACCOUNT_COMMENT] ?? '';
    }
    $ret = '<div class="budatoll-keret">';
    $ret .= '<table class="table budatoll-no-border">';
    $ret .= '<tr><th style="width:10%;">Játékos</th><td>';
    $ret .= put_select_player('edited_player', $user_id, false, 'Válasssz!');
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
