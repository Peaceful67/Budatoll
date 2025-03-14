var btAddedTrainingIds = [];
let previousWidth = window.innerWidth;
var calendarEl_trainings = document.getElementById('budatoll-edzes-calendar');
budatoll_trainings_calendar = new FullCalendar.Calendar(calendarEl_trainings, {
    datesSet: function (info) {
        if ((info.view.type === 'timeGridWeek' || info.view.type==='dayGridMonth') && btIsTouchDevice()) {
            btRequestFullScreenAndLockOrientation();
        } else {
            btUnlockOrientation();
        }
    },

    events: function (eventInfo, successCallback, failureCallback) {
        var active_start = getDateOfEventDate(eventInfo.start);
        var active_end = getDateOfEventDate(eventInfo.end);
        btAddedTrainingIds = [];
        $.ajax({
            url: budatoll_ajax_object.ajax_url,
            type: 'POST',
            dataType: 'json',
            cache: false,
            data: {
                action: 'budatoll',
                'ajax-action': 'get-trainings-range',
                'trainings-start': active_start,
                'trainings-end': active_end
            },
            success: function (response) {
                if (response.result === 'success') {
                    //                   console.log(response.events);
                    arr_events = response.events;
                    if (!Array.isArray(arr_events)) {
                        console.error('Invalid events response:', arr_events);
                        failureCallback();
                        return;
                    }
                    var events = arr_events.map(function (event) {
                        if (event && !btAddedTrainingIds.hasOwnProperty(event.id)) {
                            btAddedTrainingIds[event.id] = event;
                            return {
                                id: event.id,
                                title: event.short,
                                start: event.day + 'T' + event.start,
                                end: event.day + 'T' + event.end,
                                extendedProps: {
                                    state: event.state, }
                            };
                        }
                    });
                    successCallback(events);
                } else {
                    failureCallback();
                }
            },
            error: function (response) {
                console.log('training AJAX not succeed');
            }
        });
    },
    eventContent: function (day) {
        var arrayOfDomNodes = [];
        var title = document.createElement('div');
        title.innerText = day.event.title;
        switch (day.event.extendedProps.state) {
            case 'full':
                title.classList.add('budatoll-event-state-full');
                break;
            default:
            case 'available':
                title.classList.add('budatoll-event-state-available');
                break;
            case 'waiting':
                title.classList.add('budatoll-event-state-waiting');
                break;
        }
        arrayOfDomNodes.push(title);
        return {domNodes: arrayOfDomNodes};
    },
    viewDidMount: function (info) {
        budatoll_trainings_calendar.setOption('height', getCalendarHeight());
    },
    headerToolbar: {
        left: 'prev,next today',
        center: 'title',
        right: 'dayGridMonth timeGridWeek listWeek'
    },
    initialView: ((window.innerWidth < 768) ? 'timeGridWeek' : 'dayGridMonth'),

    windowResize: function (view) {
        const currentWidth = window.innerWidth;
        const widthDifference = Math.abs(currentWidth - previousWidth);
        budatoll_trainings_calendar.setOption('height', getCalendarHeight());
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
    height: getCalendarHeight(), // Adjusts height dynamically
    firstDay: 1,
    editable: false,
    weekends: false,
    slotMinTime: bt_events_not_before,
    slotMaxTime: bt_events_not_after,
    droppable: false,
    expandRows: true,
    forceEventDuration: true,
    allDaySlot: false,
    defaultAllDay: false,
    dayMaxEvents: false, // allow "more" link when too many events
    showNonCurrentDates: false,
    eventClick: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventClick(eventInfo);
        } else {
            if (eventInfo.jsEvent.pointerType === 'touch') {
                btTouchX = eventInfo.jsEvent.clientX;
                btTouchY = eventInfo.jsEvent.clientY;
            }
        }
    },
    eventMouseEnter: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btTrainingMouseEnter(eventInfo);
        }
    },
    eventMouseLeave: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btTrainingMouseLeave(eventInfo);
        }
    },
    eventDidMount: function (eventInfo) {
        if (btIsTouchDevice()) {
            addLongPressListener(
                    eventInfo.el,
                    function () {
                        btEventClick(eventInfo);  // Open event editor
                    },
                    function () {
                        btTrainingMouseEnter(eventInfo);
                    }
            );
        }
    }

});
budatoll_trainings_calendar.render();
window.addEventListener('resize', function () {
    budatoll_trainings_calendar.render();
}, {passive: true});


function btEventClick(eventInfo) {
    var id = eventInfo.event.id;
    var event = btAddedTrainingIds[id];
    var trainings = event.trainings_of_event;
    var trainings_text = '<h4>' + event.long + '</h4>';
    trainings_text += '<input type="hidden" name="event_id" value="' + id + '">';
    trainings_text += '<span id="close-editor-popup" class="budatoll-popup-close">&times;</span>';
    if (event.done === '1') {
        trainings_text += '<p class="budatoll-warning">Az edzés lezajlott.</p>';
    }
    trainings_text += '<div class="budatoll-row">Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '</div>';
    trainings_text += '<div class="budatoll-row">Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '</div>';
    trainings_text += '<div class="budatoll-popup-row">';
    trainings_text += '<select name="select_players" id="select_players"><option value="-1" selected>Válassz!!!</option>';
    budatoll_players.forEach(function (player) {
        if (!trainings.some(obj => obj['player_id'] === player.ID)) {
            trainings_text += '<option value="' + player.ID + '">' + player.display_name + '</option>';
        }
    });
    trainings_text += '</select>';
    trainings_text += '<button class="button budatoll-button" name="training_add"  value="-1" title="Hozzáadás"><span class="dashicons dashicons-saved"></span></button>';
    trainings_text += '</div>';

    trainings.forEach(function (training) {
        trainings_text += '<div class="budatoll-popup-row">';
        trainings_text += '<div>' + training.player_name + '</div>' + '<button class="button budatoll-button" name="training_delete" ';
        trainings_text += ' value="' + training.id + '" title="Törlés" >';
        trainings_text += '<span class="dashicons dashicons-trash"></span></button>';
        trainings_text += '</div>';
    });

    training_editor = $("#budatoll-trainings-editor");
    var popupX, popupY;
    if (btIsTouchDevice()) {
        popupX = Math.max(btTouchX, 10);
        popupY = Math.max(btTouchY, 10);
    } else {
        [popupX, popupY] = getPopupPos(training_info);
    }
    $(function () {
        $('#close-editor-popup').click(function () {
            training_editor.fadeOut(budatoll_modal_speed);
        });
    });

    training_editor.html(trainings_text).css({
        left: popupX,
        top: popupY
    }).fadeIn(budatoll_modal_speed);
    $("#budatoll-trainings-info").fadeOut(budatoll_modal_speed);
}


function btTrainingMouseEnter(eventInfo) {

    if ($("#budatoll-trainings-editor").is(":hidden")) {
        let id = eventInfo.event.id;
        let event = btAddedTrainingIds[id];
        let trainings = event.trainings_of_event;
        let trainings_text = '<h4>' + event.long + '</h4>';
        var popupX, popupY;
        training_info = $("#budatoll-trainings-info");
        if (btIsTouchDevice()) {
            popupX = Math.max(btTouchX, 10);
            popupY = Math.max(btTouchY, 10);
        } else {
            [popupX, popupY] = getPopupPos(training_info);
        }
        trainings_text += 'Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '<br>';
        trainings_text += 'Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '<br>';
        trainings_text += showApplicants(trainings);
        training_info.html(trainings_text).css({
            left: popupX,
            top: popupY
        }).stop(true, true).fadeIn(budatoll_modal_speed);
    }
}

function btTrainingMouseLeave(eventInfo) {
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