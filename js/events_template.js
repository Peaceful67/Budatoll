/* global bt_events_not_before */

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
var calendarEl_events = document.getElementById('budatoll-events-template');
const budatoll_events_calendar = new FullCalendar.Calendar(calendarEl_events, {
    events: function (info, successCallback, failureCallback) {
        $.ajax({
            url: budatoll_ajax_object.ajax_url,
            type: 'POST',
            dataType: 'json',
            data: {
                action: 'budatoll',
                'ajax-action': 'get-events-template',
                start: info.startStr, // Pass start date of visible range
                end: info.endStr      // Pass end date of visible range
            },
            success: function (response) {
                if (response.result === 'success') {
                    const events = Object.values(response.events).map(function (event) {
                        return {
                            id: event.template_id,
                            title: event.short,
                            daysOfWeek: event.day, // Use daysOfWeek for recurring events
                            startTime: event.start + ':00',
                            endTime: event.end + ':00',
                            extendedProps: {
                                long_title: event.long
                            }
                        };
                    });
                    successCallback(events); // Pass processed events to FullCalendar
                } else {
                    failureCallback(new Error('Failed to fetch events'));
                }
            },
            error: function (response) {
                console.log('error');
                console.log(response);
                $('#budatoll-message')
                        .html('Az edzések beolvasása sikertelen')
                        .addClass('budatoll-error')
                        .show(1000)
                        .delay(1500)
                        .hide(1000);
                failureCallback(new Error('AJAX error while fetching events'));
            }
        });
    },

    headerToolbar: {
        left: '',
        center: '',
        right: 'timeGridWeek, listWeek'
    },
    initialView: 'timeGridWeek',
    height: 'auto', // Adjusts height dynamically
    locale: 'hu',
    firstDay: 1,
    rerenderDelay: 500,
    slotMinTime: bt_events_not_before,
    slotMaxTime: bt_events_not_after,
    editable: false,
    weekends: false,
    droppable: true,
    forceEventDuration: true,
    defaultAllDay: false,
    dayHeaders: true,
    allDaySlot: false,
    dayMaxEvents: true, // allow "more" link when too many events
    dayHeaderContent: function (args) {
        return args.date.toLocaleDateString('hu', {weekday: 'long'}); // e.g., "Monday"
    }
    ,
    eventClick: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventRemove(eventInfo);
        }
    }
    ,
    eventReceive: function (eventInfo) {
        btEventReceive(eventInfo);
    }
    ,
    eventMouseEnter: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventMouseEnter(eventInfo);
        }
    }
    ,
    eventMouseLeave: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventMouseLeave(eventInfo);
        }
    }
    ,
    eventDrop: function (eventInfo) {
        btEventDrop(eventInfo);
    },
    eventDidMount: function (eventInfo) {
        if (btIsTouchDevice()) {
            addLongPressListener(
                    eventInfo.el,
                    function () {
                        btEventRemove(eventInfo); // Open event editor
                    },
                    function () {
                        btMouseEnter(eventInfo);
                    });
        }
    }

}
);
budatoll_events_calendar.render();
function btEventRemove(eventInfo) {
    if (!confirm('Biztosan törölni akarod az edzés alkalmat?'))
        return;
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'remove-event-template',
            'template-id': eventInfo.event.id
        },
        success: function (response) {
            switch (response.result) {
                case 'success':
                    $('#budatoll-message').html('Törlés sikeres').removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(budatoll_message_speed).delay(1500).hide(budatoll_message_speed);
                    reloadCalendar(budatoll_events_calendar);
                    break;
                case 'error':
                    message = response.message;
                    $('#budatoll-message').html('A törlés sikeretelen. ' + message).removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(budatoll_message_speed).delay(1500).hide(budatoll_message_speed);
                    break;
            }
        },
        error: function (response) {
            $('#budatoll-message').html('A törlés hibás').addClass('budatoll-error');
            $('#budatoll-message').show(budatoll_message_speed).delay(2500).hide(budatoll_message_speed);
        }
    });

}

function btEventReceive(eventInfo) {
    var droppedDate = getDateOfEventDate(eventInfo.event.start);
    eventInfo.event.setAllDay(false);
    const event = eventInfo.event;
    const dayOfWeek = event.start.getDay();
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'add-event-template',
            'event_type-id': event.id,
            'weekday': dayOfWeek
        },
        success: function (response) {
            switch (response.result) {
                case 'success':
                    message = response.event.long;
                    $('#budatoll-message').html('Mentés sikeres.<br>' + message).removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(1000).delay(2500).hide(1000);
                    reloadCalendar(budatoll_events_calendar);
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
    const event_info = $("#budatoll-template-info");
    const event = eventInfo.event;
    const title = event.extendedProps.long_title;
    const start = event.start;
    const end = event.end;
    var event_text = '<h4>' + title + '</h4>';
    event_text += 'Idősáv: ' + getHourMinutes(start) + ' - ' + getHourMinutes(end) + '<br>';
    const [popupX, popupY] = btIsTouchDevice()
            ? getPopupPosTouchDevice(event_info)
            : getPopupPos(event_info);
    event_info.html(event_text).css({
        left: popupX,
        top: popupY
    }).fadeIn(budatoll_modal_speed);
    if (btIsTouchDevice()) {
        event_info.off("click").on("click", function () {
            event_info.stop(true, true).fadeOut(budatoll_modal_speed);
        });
    }
}

function btEventMouseLeave(eventInfo)
{
    $("#budatoll-template-info").stop(true, true).fadeOut(budatoll_modal_speed);
}


function btEventDrop(eventInfo) {
    console.log('EventDrop: ' + eventInfo.event.start + ' - ' + eventInfo.event.end);
}

