var btAddedTrainingIds = [];
let previousWidth = window.innerWidth;
var calendarEl_trainings = document.getElementById('budatoll-edzes-calendar');
var goto_date = calendarEl_trainings.getAttribute('data-goto-date');

budatoll_trainings_calendar = new FullCalendar.Calendar(calendarEl_trainings, {
    datesSet: function (info) {
        if ((info.view.type === 'timeGridWeek' || info.view.type === 'dayGridMonth')) {
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
    eventSourceSuccess: function (content) {  // Ez van, amikor minden betoltodott.
    },
    eventContent: function (day) {
        var event_content = '<div class="';
        let start = new Date(day.event.start);
        let time = start.getHours() + ':' + start.getMinutes();
        switch (day.event.extendedProps.state) {
            case 'full':
                event_content += 'budatoll-event-state-full';
                break;
            default:
            case 'available':
                event_content += 'budatoll-event-state-available';
                break;
            case 'waiting':
                event_content += 'budatoll-event-state-waiting';
                break;
        }
        event_content += '" data-time="' + time + '"><span>' + day.event.title + '</span></div>';
        return {
            html: event_content
        };
    },
    viewDidMount: function (info) {
        budatoll_trainings_calendar.setOption('height', getCalendarHeight());
    },
    dayCellDidMount: function (info) {
    },
    headerToolbar: {
        left: ((window.innerWidth < 768) ? 'prev,next' : 'prev,next today'),
        center: 'title',
        right: 'dayGridMonth timeGridWeek listWeek'
    },
    initialView: ((window.innerWidth < 768) ? 'listWeek' : 'dayGridMonth'),
    initialDate: (goto_date == null) ? new Date() : goto_date,
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
    eventOverlap: true,
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
    trainings_text += '<div class="budatoll-popup-row">Max játékos: ';
    if (event.max_players > 0) {
        trainings_text += '<input type="number" size="2" min="1" max="20" name="max_players" id="max_players" value="' + event.max_players + '">';
        trainings_text += '<button class="button budatoll-button" name="set_max_players"  value="' + id + '" title="Módosítás"><span class="dashicons dashicons-saved"></span></button>';
    } else {
        trainings_text += 'Korlátlan';
    }
    trainings_text += '</div>'
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
    [popupX, popupY] = getPopupPos(training_editor);

    $(function () {
        $('#close-editor-popup').click(function () {
            training_editor.fadeOut(budatoll_modal_speed);
        });
    });

    training_editor.html(trainings_text).css({
        left: popupX,
        top: popupY
    }).addClass("is-open").fadeIn(budatoll_modal_speed);
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
        
        trainings_text += 'Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '<br>';
        trainings_text += 'Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '<br>';
        trainings_text += showApplicants(trainings);
        training_info.html(trainings_text);
        [popupX, popupY] = getPopupPos(training_info);
        training_info.css({
            left: popupX,
            top: popupY
        }).addClass("is-open").fadeIn(budatoll_modal_speed);
    }
}

function btTrainingMouseLeave(eventInfo) {
    $("#budatoll-trainings-info").fadeOut(budatoll_modal_speed).removeClass("is-open");
}


function showApplicants(trainings) {

    waiting_list = '';
    if (trainings.length === 0) {
        trainings_text = 'Nem jelentkezett senki';
    } else {
        trainings_text = 'Jelentkezett ' + trainings.length + ' játékos: ';
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