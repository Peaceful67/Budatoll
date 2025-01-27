var btAddedTrainingIds = [];
var btCurrentUserId = -1;
var btBookingAllowed = false;
let previousWidth = window.innerWidth;
var calendarEl_trainings = document.getElementById('budatoll-edzes-calendar');
budatoll_trainings_calendar = new FullCalendar.Calendar(calendarEl_trainings, {
    datesSet: function (info) {
        btAddedTrainingIds = [];
    },

    events: function (eventInfo, successCallback, failureCallback) {
        var active_start = getDateOfEventDate(eventInfo.start);
        var active_end = getDateOfEventDate(eventInfo.end);
        btAddedTrainingIds = [];
        $.ajax({
            url: budatoll_ajax_object.ajax_url,
            type: 'POST',
            dataType: 'json',
            data: {
                action: 'budatoll',
                'ajax-action': 'get-trainings-range-of-user',
                'trainings-start': active_start,
                'trainings-end': active_end
            },
            success: function (response) {
                if (response.result === 'success') {
                    if (!Array.isArray(response.events)) {
                        console.error('Invalid events response:', response.events);
                        failureCallback();
                        return;
                    }
                    btCurrentUserId = response.current_user_id;
                    btBookingAllowed = response.booking_allowed;
                    var events = response.events.map(function (event) {
                        if (event && !btAddedTrainingIds.hasOwnProperty(event.id)) {
                            btAddedTrainingIds[event.id] = event;
                            return {
                                id: event.id,
                                title: event.short,
                                start: event.day + 'T' + event.start,
                                end: event.day + 'T' + event.end,
                                extendedProps: {
                                    booked: event.booked,
                                    full: event.full,
                                    confirmed: event.confirmed
                                }
                            };
                        }
                        return null;
                    }).filter(Boolean);
                    successCallback(events);
                } else {
                    failureCallback();
                }
            },
            error: function (response) {
                failureCallback();
                console.log('my trainings AJAX not succed');
                //       console.log(response);
            }
        });
    },
    eventContent: function (day) {
        var arrayOfDomNodes = [];
        var title = document.createElement('div');
        title.innerText = day.event.title;
        if (day.event.extendedProps.booked) {
            if (day.event.extendedProps.confirmed) {
                title.classList.add('budatoll-booked-event');
            } else {
                title.classList.add('budatoll-waiting-event');
            }
        } else if (day.event.extendedProps.full) {
            title.classList.add('budatoll-full-event');
        } else {
            title.classList.add('budatoll-available-event');
        }
        arrayOfDomNodes.push(title);
        return {domNodes: arrayOfDomNodes};
    },
    headerToolbar: {
        left: 'prev,next today',
        center: 'title',
        right: 'dayGridMonth,listWeek'
    },
    initialView: ((window.innerWidth < 768) ? 'listWeek' : 'dayGridMonth'),
    windowResize: function (view) {
        const currentWidth = window.innerWidth;
        const widthDifference = Math.abs(currentWidth - previousWidth);
        if (widthDifference > 10) {
            if (window.innerWidth < 768) {
                budatoll_trainings_calendar.changeView('listWeek');
            } else {
                budatoll_trainings_calendar.changeView('dayGridMonth');
            }
            previousWidth = currentWidth;
        }
    },
    locale: 'hu',
    firstDay: 1,
    editable: false,
    weekends: false,
    droppable: false,
    expandRows: true,
    slotMinTime: bt_events_not_before,
    slotMaxTime: bt_events_not_after,
    forceEventDuration: true,
    height: 'auto', // Adjusts height dynamically
    defaultAllDay: false,
    dayMaxEvents: true, // allow "more" link when too many events
    showNonCurrentDates: false,
    eventClick: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventClicked(eventInfo);
        } else {
            if (eventInfo.jsEvent.pointerType === 'touch') {
                btTouchX = eventInfo.jsEvent.clientX;
                btTouchY = eventInfo.jsEvent.clientY;
            }
        }
    },
    eventMouseEnter: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btMyTrainingMouseEnter(eventInfo);
        }
    },
    eventMouseLeave: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btMyTrainingMouseLeave(eventInfo);
        }
    },
    eventDidMount: function (eventInfo) {
        if (btIsTouchDevice()) {
            addLongPressListener(
                    eventInfo.el,
                    function () {
                        btEventClicked(eventInfo); // Open event editor
                    },
                    function () {
                        btMyTrainingMouseEnter(eventInfo);
                    });
        }
    }
});
budatoll_trainings_calendar.render();
window.addEventListener('resize', function () {
    budatoll_trainings_calendar.render();
}, {passive: true});
function btEventClicked(eventInfo) {
    var id = eventInfo.event.id;
    var event = btAddedTrainingIds[id];
    $("#budatoll-trainings-info").fadeOut(budatoll_modal_speed);
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'my-event-clicked',
            'my-event-id': id
        },
        success: function (response) {
            btAddedTrainingIds = [];
            budatoll_trainings_calendar.refetchEvents();
            success_msg = $("#budatoll-success-message");
            error_msg = $("#budatoll-error-message");
            switch (response.status) { // done, deleted, deleted-waiting, waiting, confirmed, confirmed-waiting
                case 'done':
                    error_msg.html('Az edzés már lezajlott, nem lehet változtatni.').show(1000).delay(2500).hide(1000);
                    break;
                case 'not-leased':
                    error_msg.html('Nem vagy bérletes, csak egy napon belüli edzésekre jelentkezhetsz.').show(1000).delay(2500).hide(1000);
                    break;
                case 'deleted':
                    success_msg.html('Jelentkezésed az edzésre sikeresen törölted.').show(1000).delay(2500).hide(1000);
                    break;
                case 'deleted-waiting':
                    success_msg.html('Jelentkezésed az edzés várólistájáról sikeresen törölted.').show(1000).delay(2500).hide(1000);
                    break;
                case 'waiting':
                    success_msg.html('Az edzés betelt, jelentkezésed várólistára került.').show(1000).delay(2500).hide(1000);
                    break;
                case 'confirmed-waiting':
                    success_msg.html('Jelentkezésed várólistáról érvényesre váltott.').show(1000).delay(2500).hide(1000);
                    break;
                case 'confirmed':
                    success_msg.html('Jelentkezésed az edzésre érvényes.').show(1000).delay(2500).hide(1000);
                    break;
                default:
                    error_msg.html('Belső ismeretlen hiba: ' + response.status + ', nem történt változtatás').show(1000).delay(2500).hide(1000);
                    break;
            }

        },
        error: function (response) {
            console.log('my-event-clicked AJAX not succed');
            console.log(response);
        }
    });
}

function btMyTrainingMouseEnter(eventInfo) {

    if ($("#budatoll-trainings-editor").is(":hidden")) {
        const training_info = $("#budatoll-trainings-info");
        let id = eventInfo.event.id;
        let event = btAddedTrainingIds[id];
        let trainings = event.trainings_of_event  ?? null;
        if (trainings === null) {
            return;
        }
        let trainings_text = '<h4>' + event.long + '</h4>';
        if (btIsTouchDevice()) {
            popupX = Math.max(btTouchX, 10);
            popupY = Math.max(btTouchY, 10);
        } else {
            [popupX, popupY] = getPopupPos(training_info);
        }
        trainings_text += 'Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '<br>';
        trainings_text += 'Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '<br>';
        trainings_text += showApplicants(trainings);
        training_info.html(trainings_text);
        training_info.css({
            left: popupX,
            top: popupY
        });
        training_info.stop(true, true).fadeIn(budatoll_modal_speed);
    }

}
function btMyTrainingMouseLeave(eventInfo) {
    $("#budatoll-trainings-info").stop(true, true).fadeOut(budatoll_modal_speed);
}



function showApplicants(trainings) {
    trainings_text = 'Jelentkeztek: ';
    waiting_list = '';
    if (trainings.length === 0) {
        trainings_text += 'Senki';
    } else {
        trainings.forEach(function (training) {
            if (training.confirmed === '1') {
                trainings_text += training.player_name + ', ';
            }
        });
    }
    trainings.forEach(function (training) {
        if (training.confirmed === '0') {
            waiting_list += training.player_name + ', ';
        }
    });
    if (waiting_list !== '') {
        trainings_text += '<br>' + 'Várólistás: ' + waiting_list;
    }
    return trainings_text;
}

