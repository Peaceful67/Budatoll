var budatoll_modal_speed = 300;
var btMouseX = -1, btMouseY = -1;
var btTouchX = btTouchY = -1;

function getCalendarType() { // Megmondja az URL alapján,melyik fajta naptárat töltsük be
    const calendarTypes = {
        'alkalom-naptar': 'events',
        'edzesek-naptar': 'trainings',
        'edzeseim-naptar': 'my-trainings'
    };
    var url = window.location.href;
    var mainUrl = url.split('?')[0];
    if (mainUrl.endsWith('/')) {
        mainUrl = mainUrl.substring(0, mainUrl.length - 1);
    }
    var slug = mainUrl.substring(mainUrl.lastIndexOf('/') + 1);
    for (const [pattern, type] of Object.entries(calendarTypes)) {
        if (slug.includes(pattern)) {
            return type;
        }
    }
    return false;
}


function getDateOfEventDate(event_date) {
    var date = new Date(event_date);
    var year = date.getFullYear(); // Gets the year
    var month = (date.getMonth() + 1).toString().padStart(2, '0'); // Gets the month, zero-padded
    var day = date.getDate().toString().padStart(2, '0'); // Gets the day of the month, zero-padded

    return  year + '-' + month + '-' + day;
}


function addLongPressListener(element, onLongPress, onTap, delay = 1800) {

    var long_press = false;
    var pressed = false;
    var bt_timer = 0;
    // Start timer on touchstart
    element.addEventListener('touchstart', function (event) {
        event.stopPropagation();
        const touch = event.touches[0];
        long_press = false;
//        console.log('touchstart');
        pressed = true;
        clearTimeout(bt_timer);
        bt_timer = setTimeout(function () {
            long_press = true;
        }, 1000);
    }, {passive: true});

    // Clear timer if touch ends/cancels quickly
    element.addEventListener('touchend', function (event) {
        event.stopPropagation();
//        console.log('touchend');
        clearTimeout(bt_timer);
        if (!pressed) {
            return;
        }
//        console.log('Long press: ' + (long_press ? 'true' : 'false'));

        if (long_press) {
            long_press = false;
            //           console.log('onLongPress');
            onLongPress(event);
        } else {
            //           console.log('onTap');
            onTap(event);  // Trigger tap only if long press wasn't triggered
        }
        pressed = false;
    }, {passive: true});

    element.addEventListener('touchcancel', function (event) {
        event.stopPropagation();
        clearTimeout(bt_timer);
        if (!pressed) {
            return;
        }
        if (long_press) {
            long_press = false;
            onLongPress(event);
        } else {
            onTap(event);  // Trigger tap only if long press wasn't triggered
        }
        pressed = false;
    }, {passive: true});
}


function btIsTouchDevice() {
    return 'ontouchstart' in window || navigator.maxTouchPoints > 0;
}

function getPopupPos(popupModal) {

    popup_width = popupModal.outerWidth();
    popup_height = popupModal.outerHeight();
    popup_width = Math.max(80, popup_width);
    popup_height = Math.max(50, popup_height);
    screenX = $(window).width();
    screenY = $(window).height();
    if (btMouseX < 0) {
        btMouseX = screenX / 2;
    }
    if (btMouseY < 0) {
        btMouseY = screenY / 2;
    }
    // Adjust horizontal position
    let popupX = btMouseX + 80;
    if (btMouseX > screenX / 2) {
        popupX = btMouseX - (popup_width / 2) - 80;
    }
    // Ensure the modal stays within the screen horizontally
    popupX = Math.max(10, Math.min(popupX, screenX - popup_width - 10));

    // Adjust vertical position
    let popupY = btMouseY + 80;
    if (btMouseY > screenY / 2) {
        popupY = btMouseY - (popup_height / 2) - 80;
    }
    // Ensure the modal stays within the screen vertically
    popupY = Math.max(10, Math.min(popupY, screenY - popup_height - 10));

    return [popupX + 'px', popupY + 'px'];

}
function getPopupPosTouchDevice(popupModal) {
    var popup_width = popupModal.outerWidth();
    var popup_height = popupModal.outerHeight();
    var popupX = popup_width / 2 + 100;
    var popupY = popup_height / 2 + 150;
    return [popupX + 'px', popupY + 'px'];

}

function isBeforeTomorrow(dateInput) {
    let today = new Date();
    today.setHours(0, 0, 0, 0);
    let tomorrow = new Date(today);
    tomorrow.setDate(tomorrow.getDate() + 1);
    let inputDate = new Date(dateInput);
    inputDate.setHours(0, 0, 0, 0); // Consider only the date part for comparison
    return inputDate <= tomorrow;
}
