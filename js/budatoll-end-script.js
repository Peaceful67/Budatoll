budatoll_players = false;
budatoll_center_x = window.innerWidth / 2;
budatoll_center_y = window.innerHeight / 2;
budatoll_mouse_x = budatoll_center_x;
budatoll_mouse_y = budatoll_center_y;

$.ajax({
    url: budatoll_ajax_object.ajax_url,
    type: 'POST',
    dataType: 'json',
    data: {
        action: 'budatoll',
        'ajax-action': 'get-players',
    },
    success: function (response) {
        if (response.result === 'success') {
            budatoll_players = response.players;
        } else {
            console.log('Wrong action: get-players: '.response);
        }
    },
    error: function (response) {
        console.log('AJAX not succed');
        console.log(response);
    }
});

/*
$(document).on('mousemove', function (event) {
    budatoll_mouse_x = event.pageX;
    budatoll_mouse_y = event.pageY;
    console.log('X: ' + budatoll_mouse_x + ', Y: ' + budatoll_mouse_y);
});
*/
function budatoll_get_popup_x(width) {
    x = (budatoll_mouse_x > budatoll_center_x) ? budatoll_center_x - width - 50 : budatoll_center_x + 50;
    x = (budatoll_mouse_x > budatoll_center_x) ? 0 : budatoll_center_x * 2 - width;
    x = budatoll_mouse_x;
    return x;
}

function budatoll_get_popup_y(height) {
    y = (budatoll_mouse_y > budatoll_center_y) ? budatoll_center_y - height - 50 : budatoll_center_y + 50;
    y = budatoll_mouse_y;
    return y;
}
