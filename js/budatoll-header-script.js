var budatoll_modal_speed = 300;
var budatoll_message_speed = 500;
var btMouseX = -1, btMouseY = -1;
var btTouchX = btTouchY = -1;
const offsetModalX = 15;             // distance from cursor
const offsetModalY = 20;

var bt_events_not_before = '16:00:00';
var bt_events_not_after = '22:00:00';

function getCalendarType() { // Megmondja az URL alapján,melyik fajta naptárat töltsük be
    const calendarTypes = {
        'sablon-naptar': 'template',
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

function getCalendarHeight() {
    let headerHeight = document.querySelector('.fc-toolbar')?.offsetHeight || 50;
    let availableHeight = window.innerHeight - headerHeight;
    return Math.max(400, availableHeight); // Ensure a minimum height
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
        btTouchX = Math.round(touch.clientX);
        btTouchY = Math.round(touch.clientY);
        long_press = false;
//        console.log('touchstart');
        pressed = true;
        clearTimeout(bt_timer);
        bt_timer = setTimeout(function () {
            long_press = true;
        }, 800);
    }, {passive: true});

    // Clear timer if touch ends/cancels quickly
    element.addEventListener('touchend', function (event) {
        event.stopPropagation();
        console.log('touchend');
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
    let popupX, popupY;
    popup_width = Math.max(80, popupModal.outerWidth());
    popup_height = Math.max(50, popupModal.outerHeight());
    screenX = window.innerWidth;// $(window).width();
    screenY = window.innerHeight; // $(window).height();
 //   console.log('Window: X: ' + screenX + ', Y: ' + screenY);
 //   const $parent = $(popupModal).parent();
 //   const parent_width = $parent.outerWidth();   // includes padding + border
 //   const parent_height = $parent.outerHeight();
 //   console.log('Parent: X: ' + parent_width + ', Y: ' + parent_height);
 //   console.log('Mouse: X: ' + btMouseX + ', Y: ' + btMouseY);
    if (btMouseX < 0) {
        btMouseX = screenX / 2;
    }
    if (btMouseY < 0) {
        btMouseY = screenY / 2;
    }
 
    if (btMouseX > (screenX / 2)) { // Ha nagyon jobbra van már
        popupX = (screenX / 2) - offsetModalX - popup_width;
    } else {
        popupX = btMouseX + offsetModalX;
    }
    popupX = Math.max(offsetModalX, popupX);

    if (btMouseY > (screenY / 2)) { // Ha nagyon lent van már
        popupY = (screenY / 2) - offsetModalY - popup_height;
//        console.log('Lent van: ' + popupY);
    } else {
        popupY = btMouseY + offsetModalY;
    }
    popupY = Math.max(offsetModalY, popupY);
//    console.log('X/Y, Size: ' + popup_width + ' / ' + popup_height + ', pos: ' + popupX + ' / ' + popupY);
    return [popupX + 'px', popupY + 'px'];

}
function getPopupPosTouchDevice(eventInfo) {
    console.log(eventInfo.target);
    var popupX = eventInfo.jsEvent.clientX;
    var popupY = eventInfo.jsEvent.ClientY;
    return {x: popupX, y: popupY};

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

function getHourMinutes(fullCalendarTimeString) {
    const date = new Date(fullCalendarTimeString);
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    return `${hours}:${minutes}`;
}

function reloadCalendar(calendar) {
    let currentDate = calendar.getDate();
    calendar.removeAllEvents();
    calendar.refetchEvents();
    calendar.gotoDate(currentDate);

}

function btRequestFullScreenAndLockOrientation() {

    if (btIsTouchDevice()) {
        const elem = document.documentElement;
        if (elem.requestFullscreen) {
            elem.requestFullscreen().then(btLockOrientation).catch(console.error);
        } else if (elem.mozRequestFullScreen) { // Firefox
            elem.mozRequestFullScreen();
            btLockOrientation();
        } else if (elem.webkitRequestFullscreen) { // Safari
            elem.webkitRequestFullscreen();
            btLockOrientation();
        } else if (elem.msRequestFullscreen) { // IE11
            elem.msRequestFullscreen();
            btLockOrientation();
        } else {
            alert("A böngésződ nem támogatja az elforgatás rögzítését. Forgasd el a telefonod!");
        }
    }
}

function btLockOrientation() {
    if (btIsTouchDevice()) {
        if (screen.orientation && screen.orientation.lock) {
            screen.orientation.lock("landscape").catch(() => {
                alert("Fordítsd el a telefonod a jobb megjelnítés érdekében.");
            });
        } else {
            alert("A böngésződ nem támogatja az elforgatás rögzítését. Forgasd el a telefonod!");
        }
    }
}

function btUnlockOrientation() {
    if (btIsTouchDevice()) {
        if (screen.orientation && screen.orientation.unlock) {
            screen.orientation.unlock();
        } else {
            alert("Nem sikerült a telefon elforgatásának rögzítését megszüntetni.");
        }
    }
}

// Detect if in portrait mode and warn the user
function btCheckOrientation() {
    if (window.innerHeight > window.innerWidth) {
        btRequestFullScreenAndLockOrientation();
//        alert("Fordítsd el a telefonod a jobb megjelnítés érdekében.");
    }
}

