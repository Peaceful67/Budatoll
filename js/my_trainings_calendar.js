var btAddedTrainingIds = [];
var btCurrentUserId = -1;
var calendarEl_trainings = document.getElementById('budatoll-edzes-calendar');
budatoll_trainings_calendar = new FullCalendar.Calendar(calendarEl_trainings, {
    datesSet: function (eventInfo) {
        var active_start = getDateOfEventDate(eventInfo.start);
        var active_end = getDateOfEventDate(eventInfo.end);
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
                    btCurrentUserId = response.current_user_id;
                    response.events.forEach(function (event) {
                        if (!btAddedTrainingIds.hasOwnProperty(event.id)) {
                            console.log(event);
                            btAddedTrainingIds[event.id] = event;
                            budatoll_trainings_calendar.addEvent({
                                'id': event.id,
                                'title': event.short,
                                'start': event.day + 'T' + event.start,
                                'end': event.day + 'T' + event.end,
                                'booked': true,
                                'confirmed': false
                            });
                        }
                    });
                } else {
                    console.log('Wrong action: ' + response.action);
                    console.log('SQL: ' + response.sql);
                }
            },
            error: function (response) {
                console.log('AJAX not succed');
                console.log(response);
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
        } else {
            title.classList.add('budatoll-available-event');
        }
        arrayOfDomNodes.push(title);
        return {domNodes: arrayOfDomNodes};
    },
    headerToolbar: {
        left: 'prev,next today',
        center: 'title',
        right: 'dayGridMonth,timeGridWeek'

    },
    initialView: ((window.innerWidth < 768) ? 'listWeek' : 'dayGridMonth'),
    locale: 'hu',
    firstDay: 1,
    editable: false,
    weekends: false,
    droppable: false,
    expandRows: true,
    forceEventDuration: true,
    defaultAllDay: false,
    dayMaxEvents: true, // allow "more" link when too many events
    showNonCurrentDates: false,
    eventClick: function (eventInfo) {
        btEventClick(eventInfo);
    },
    eventMouseEnter: function (eventInfo) {
        btMyTrainingMouseEnter(eventInfo);
    },
    eventMouseLeave: function (eventInfo) {
        btMyTrainingMouseLeave(eventInfo);
    }
});
budatoll_trainings_calendar.render();
function btEventClick(eventInfo) {
    let id = eventInfo.event.id;
    let event = btAddedTrainingIds[id];
    let trainings = event.trainings_of_event ?? [];
    console.log(trainings);
    var is_already_booked = false;
    var is_waiting = false;
    let trainings_text = '<h4>' + event.long + '</h4><hr>';
    trainings_text += '<input type="hidden" name="event_id" value="' + id + '">';
    trainings_text += '<span id="close-editor-popup" class="budatoll-popup-close">&times;</span>';
    trainings_text += '<p>Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '</p>';
    trainings_text += 'Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '<br>';
    trainings_text += 'Jelentkeztek: ';
    let waiting_list = '';
    if (trainings.length === 0) {
        trainings_text += 'Még senki';
    } else {
        trainings.forEach(function (training) {
            if (training.player_id === btCurrentUserId) {
                is_already_booked = true;
                if (training.confirmed !== '1') {
                    is_waiting = true;
                }
            }
            if (training.confirmed === '1') {
                trainings_text += training.player_name + ', ';
            } else {
                waiting_list += training.player_name + ', ';
            }
        });
    }
    if (waiting_list !== '') {
        trainings_text += '<br>' + 'Várólistás: ' + waiting_list;
    }
    trainings_text += '<div class="budatoll-row">';
    if (is_already_booked) {
        if (is_waiting && (event.max_players === 0 || event.max_players < trainings.length)) {
            trainings_text += '<button class="button budatoll-button" name="training_from_waiting"  value="-1" title="Jelentkezés aktiválása"><span class="dashicons dashicons-insert"></span></button>';
        } else {
            trainings_text += '<button class="button budatoll-button" name="training_remove"  value="-1" title="Lemondás"><span class="dashicons dashicons-remove"></span></button>';
        }
    } else {
        if (event.max_players > 0 && event.max_players <= trainings.length) {  // Csak várólistára fér fel
            trainings_text += '<button class="button budatoll-button budatoll-button-warning" name="training_wait"  value="-1" title="Várólistára"><span class="dashicons dashicons-insert-after"></span></button>';
        } else {
            trainings_text += '<button class="button budatoll-button" name="training_add"  value="-1" title="Jelentkezés"><span class="dashicons dashicons-insert"></span></button>';
        }
    }
    trainings_text += '</div>';
    $(function () {
        $('#close-editor-popup').click(function () {
            $('#budatoll-trainings-editor').html('trainings_text').hide();
        });
    });
    $("#budatoll-trainings-editor").html(trainings_text).show();
    $("#budatoll-trainings-info").hide();
}

function btMyTrainingMouseEnter(eventInfo) {
    if ($("#budatoll-trainings-editor").is(":hidden")) {
        let id = eventInfo.event.id;
        let event = btAddedTrainingIds[id];
        let trainings = event.trainings_of_event  ?? null;
        if (trainings === null) {
            return;
        }
        let trainings_text = '<h4>' + event.long + '</h4>';
        trainings_text += 'Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '<br>';
        trainings_text += 'Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '<br>';
        trainings_text += 'Jelentkeztek: ';
        let waiting_list = '';
        if (trainings.length === 0) {
            trainings_text += 'Még senki';
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
        training_info = $("#budatoll-trainings-info");
        popup_width = training_info.width();
        popup_height = training_info.height();
        training_info.html(trainings_text).show().css({
            top: budatoll_get_popup_y(popup_height),
            left: budatoll_get_popup_x(popup_width),
        });
    }

}
function btMyTrainingMouseLeave(eventInfo) {
    $("#budatoll-trainings-info").hide();
}


