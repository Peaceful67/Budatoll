var btAddedEventIds = [];

var eventListEl_types = document.getElementById('event-types-list');
new FullCalendar.Draggable(eventListEl_types, {
    itemSelector: '.fc-event',
    eventData: function (dropInfo) {
        type_data = JSON.parse(dropInfo.dataset.type);
        type_id = type_data.type_id;
        return {
            id: type_id,
        };
    }
});
var calendarEl_events = document.getElementById('budatoll-alkalom-calendar');
budatoll_events_calendar = new FullCalendar.Calendar(calendarEl_events, {

    datesSet: function (eventInfo) {
        var active_start = getDateOfEventDate(eventInfo.start);
        var active_end = getDateOfEventDate(eventInfo.end);
        $.ajax({
            url: budatoll_ajax_object.ajax_url,
            type: 'POST',
            dataType: 'json',
            data: {
                action: 'budatoll',
                'ajax-action': 'get-events-range',
                'event-start': active_start,
                'event-end': active_end,
            },
            success: function (response) {
                if (response.result === 'success') {
                    console.log(eventInfo);
                    response.events.forEach(function (event) {
                        if (!btAddedEventIds.includes(event.id)) {
                            btAddedEventIds.push(event.id);
                            budatoll_events_calendar.addEvent({
                                'id:': event.id,
                                'title': event.short,
                                'start': event.day + 'T' + event.start,
                                'end': event.day + 'T' + event.end,

                            });
                        }
                    });
                }
            },
            error: function (response) {
                console.log(response);
                $('#budatoll-message').html('Az edzések beolvasása sikertelen').addClass('budatoll-error');
                $('#budatoll-message').show(1000).delay(1500).hide(1000);
            }
        });
    },

    headerToolbar: {
        left: 'prev,next today',
        center: 'title',
        right: 'dayGridMonth,timeGridWeek'

    },
    initialView: 'dayGridMonth',
    locale: 'hu',
    firstDay: 1,
    editable: true,
    weekends: false,
    droppable: true,
    forceEventDuration: true,
    defaultAllDay: false,
    dayMaxEvents: true, // allow "more" link when too many events
    eventClick: function (eventInfo) {
        btEventClick(eventInfo);
    },
    eventReceive: function (eventInfo) {
        btEventReceive(eventInfo);
    },
    eventMouseEnter: function (eventInfo) {
        btEventMouseEnter(eventInfo);
    },
    eventMouseLeave: function (eventInfo) {
        btEventMouseLeave(eventInfo);
    },
    eventDrop: function (eventInfo) {
        btEventDrop(eventInfo);
    },

});
budatoll_events_calendar.render();



function btEventClick(eventInfo) {
 //   alert('Event info: ' + eventInfo.event.id + ' / ' + eventInfo.event.title + ' / ' + eventInfo.event.start);
}

function btEventReceive(eventInfo) {
    var droppedDate = getDateOfEventDate(eventInfo.event.start);
    eventInfo.event.setAllDay(false);
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'add-event',
            'event_type-id': eventInfo.event.id,
            'dropped-date': droppedDate,
        },
        success: function (response) {
            switch (response.result) {
                case 'already':
                    eventInfo.event.remove();
                    $('#budatoll-message').html('Ilyen edzés már van ezen a napon, nem történt mentés').removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
                case 'success':
                    start_time = droppedDate + 'T' + response.event.start;
                    end_time = droppedDate + 'T' + response.event.end;
                    event_id = response.event_id;
                    console.log('event id: ' + event_id);
                    eventInfo.event.setProp('id', event_id);
                    eventInfo.event.setDates(start_time, end_time);
                    eventInfo.event.setAllDay(false);
                    //                           console.log(eventInfo.event);
                    $('#budatoll-message').html('Mentés sikeres').removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
                case 'error':
                    eventInfo.event.remove();
                    $('#budatoll-message').html('A mentés sikeretelen').removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
            }
        },
        error: function (response) {
            eventInfo.event.remove();
            $('#budatoll-message').html('A mentés sikeretelen').addClass('budatoll-error');
            $('#budatoll-message').show(1000).delay(1500).hide(1000);
        }
    });
}

function btEventMouseEnter(eventInfo) {
    console.log(eventInfo.event);
}

function btEventMouseLeave(eventInfo) {
    console.log('Mouse leave: ' + eventInfo.event.title);

}

function btEventDrop(eventInfo) {
//    alert('Drop event:' + eventInfo.event.id);
    console.log('EventDrop: ' + eventInfo.event.start + ' - ' + eventInfo.event.start);
}
