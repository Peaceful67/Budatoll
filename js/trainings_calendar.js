var btAddedTrainingIds = [];

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
                'ajax-action': 'get-trainings-range',
                'trainings-start': active_start,
                'trainings-end': active_end
            },
            success: function (response) {
                if (response.result === 'success') {
                    response.events.forEach(function (event) {
                        if (!btAddedTrainingIds.hasOwnProperty(event.id)) {
                            btAddedTrainingIds[event.id] = event;
                            budatoll_trainings_calendar.addEvent({
                                'id': event.id,
                                'title': event.short,
                                'start': event.day + 'T' + event.start,
                                'end': event.day + 'T' + event.end

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
        btTrainingMouseEnter(eventInfo);
    },
    eventMouseLeave: function (eventInfo) {
        btTrainingMouseLeave(eventInfo);
    },
});
budatoll_trainings_calendar.render();



function btEventClick(eventInfo) {
    let id = eventInfo.event.id;
    let event = btAddedTrainingIds[id];
    let trainings = event.trainings_of_event;
    let trainings_text = '<h4>' + event.long + '</h4>';
    trainings_text += '<input type="hidden" name="event_id" value="' + id + '">';
    trainings_text += '<span id="close-editor-popup" class="budatoll-popup-close">&times;</span>';
    trainings_text += '<p>Idősáv: ' + event.start.substring(0, 5) + ' - ' + event.end.substring(0, 5) + '</p>';
    trainings_text += 'Max játékos: ' + (event.max_players > 0 ? event.max_players : 'Korlátlan') + '<br>';
    trainings_text += '<div class="budatoll-row">';
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
        trainings_text += '<div class="budatoll-row">';
        trainings_text += training.player_name + '<button class="button budatoll-button" name="training_delete" ';
        trainings_text += ' value="' + training.id + '" title="Törlés" >';
        trainings_text += '<span class="dashicons dashicons-trash"></span></button>';
        trainings_text += '</div>';
    });
    $(function () {
        $('#close-editor-popup').click(function () {
            $('#budatoll-trainings-editor').html('trainings_text').hide();
        });
    });

    $("#budatoll-trainings-editor").html(trainings_text).show().css({top: 100, left: 100});
    $("#budatoll-trainings-info").hide();
}


function btTrainingMouseEnter(eventInfo) {
    if ($("#budatoll-trainings-editor").is(":hidden")) {
        let id = eventInfo.event.id;
        let event = btAddedTrainingIds[id];
        let trainings = event.trainings_of_event;
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
        $("#budatoll-trainings-editors").hide();
    }
}

function btTrainingMouseLeave(eventInfo) {
    $("#budatoll-trainings-info").hide();
}



