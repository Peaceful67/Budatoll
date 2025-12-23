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
const budatoll_events_calendar = new FullCalendar.Calendar(calendarEl_events, {
    datesSet: function (info) {
        btAddedTrainingIds = [];
    },
    events: function (info, successCallback, failureCallback) {

        var active_start = getDateOfEventDate(info.start);
        var active_end = getDateOfEventDate(info.end);

        $.ajax({
            url: budatoll_ajax_object.ajax_url,
            type: 'POST',
            dataType: 'json',
            data: {
                action: 'budatoll',
                'ajax-action': 'get-events-range',
                'event-start': active_start, // Start date in custom format
                'event-end': active_end      // End date in custom format
            },
            success: function (response) {
                if (response.result === 'success') {
                    // Map response events to FullCalendar's event structure
                    const events = response.events.map(function (event) {
                        return {
                            id: event.id,
                            title: event.short,
                            start: event.day + 'T' + event.start, // Combine date and time
                            end: event.day + 'T' + event.end, // Combine date and time
                            classNames: ['budatoll-event-state-available'],
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
                console.log('Error fetching events:', response);
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
        left: 'prev,next today copyTemplate sendEmailWarningPlayers sendEmailWarningRenters',
        center: 'title',
        right: 'dayGridMonth timeGridWeek listWeek'

    },
    customButtons: {
        copyTemplate: {
            text: 'Sablon másolás',
            hint: 'Egy hetet bemásol',
            click: function () {
                if (confirm('Biztosan be akarod másolni a sablont az aktuális hétre?')) {
                    copyTemplateEvents();
                }
            }
        },
        sendEmailWarningPlayers: {
            text: 'Email játékosoknak',
            hint: 'Emaileket küld a játékosoknak, hogy jelentkezhetnek következő időszakra.',
            click: function () {
                if (confirm('Biztosan levelet akarsz küldeni az összes érintett játékosnak?')) {
                    emailToPlayers();
                }
            }
        },
        sendEmailWarningRenters: {
            text: 'Email pályabérlőknek',
            hint: 'Emaileket küld a pályabérlőknek, hogy jelentkezhetnek következő időszakra.',
            click: function () {
                if (confirm('Biztosan levelet akarsz küldeni az összes érintett játékosnak?')) {
                    emailToRenters();
                }
            }
        },

    },
    eventContent: function (day) {
        var arrayOfDomNodes = [];
        var title = document.createElement('div');
        title.innerText = day.event.title;
        arrayOfDomNodes.push(title);
        return {domNodes: arrayOfDomNodes};
    },
    viewDidMount: function (info) {
        const copyTemplateButtonEl = document.querySelector('.fc-copyTemplate-button');
        if (copyTemplateButtonEl) {
            if (info.view.type === 'dayGridMonth') {
                copyTemplateButtonEl.style.display = 'none'; // Hide in dayGridMonth
            } else {
                copyTemplateButtonEl.style.display = ''; // Show in other views
            }
        }
    },
    viewWillUnmount: function (info) {
        const copyTemplateButtonEl = document.querySelector('.fc-copyTemplate-button');
        if (copyTemplateButtonEl) {
            copyTemplateButtonEl.style.display = ''; // Reset visibility on unmount
        }
    },
    initialView: 'dayGridMonth',
    height: 'auto', // Adjusts height dynamically
    locale: 'hu',
    firstDay: 1,
    editable: false,
    slotMinTime: bt_events_not_before,
    slotMaxTime: bt_events_not_after,
    weekends: false,
    droppable: true,
    forceEventDuration: true,
    defaultAllDay: false,
    dayMaxEvents: false, // allow "more" link when too many events

    eventClick: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventRemove(eventInfo);
        }
    },
    eventReceive: function (eventInfo) {
        btEventReceive(eventInfo);
    },
    eventMouseEnter: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventMouseEnter(eventInfo);
        }
    },
    eventMouseLeave: function (eventInfo) {
        if (!btIsTouchDevice()) {
            btEventMouseLeave(eventInfo);
        }
    },
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

});
budatoll_events_calendar.render();
function btEventRemove(eventInfo) {
//       alert('Event info: ' + eventInfo.event.id + ' / ' + eventInfo.event.title + ' / ' + eventInfo.event.start);
    if (!confirm('Biztosan törölni akarod az edzés alkalmat?'))
        return;
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'remove-event',
            'event-id': eventInfo.event.id
        },
        success: function (response) {
            switch (response.result) {
                case 'deleted':
                    $('#budatoll-message').html('Törlés sikeres').removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    reloadCalendar(budatoll_events_calendar);
                    break;
                case 'error':
                    message = response.message;
                    $('#budatoll-message').html('A törlés sikeretelen. ' + message).removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
            }
        },
        error: function (response) {
            $('#budatoll-message').html('A törlés hibás').addClass('budatoll-error');
            $('#budatoll-message').show(1000).delay(2500).hide(1000);
        }
    });
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
            'dropped-date': droppedDate
        },
        success: function (response) {
            switch (response.result) {
                case 'already':
                    eventInfo.event.remove();
                    $('#budatoll-message').html('Ilyen edzés már van ezen a napon, nem történt mentés').removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
                case 'success':
                    message = response.message;
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
    reloadCalendar(budatoll_events_calendar);
}

function btEventMouseEnter(eventInfo) {
    const event_info = $("#budatoll-event-info");
    const event = eventInfo.event;
    const title = event.extendedProps.long_title;
    const start = event.startStr.substring(11, 16);
    const end = event.endStr.substring(11, 16);
    var event_text = '<h4>' + title + '</h4>';
    event_text += 'Idősáv: ' + start + ' - ' + end + '<br>';
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
    $("#budatoll-event-info").stop(true, true).fadeOut(budatoll_modal_speed);
}


function btEventDrop(eventInfo) {
    console.log('EventDrop: ' + eventInfo.event.start + ' - ' + eventInfo.event.end);
}

function copyTemplateEvents() {
    var startOfWeek = budatoll_events_calendar.view.activeStart; // Get the first day of the current week
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'copy-templates',
            'start-of-week': startOfWeek,
        },
        success: function (response) {
            switch (response.result) {
                case 'success':
                    const message = response.copied + ' / ' + response.total + ' alkalom bemásolva';
                    $('#budatoll-message').html('Másolás sikeres.<br>' + message).removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(1000).delay(2500).hide(1000);
                    reloadCalendar(budatoll_events_calendar);
                    break;
                case 'error':
                    $('#budatoll-message').html('A másolás sikeretelen<br>' + response.message).removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
            }
        },
        error: function (response) {
            $('#budatoll-message').html('A másolás sikeretelen').addClass('budatoll-error');
            $('#budatoll-message').show(1000).delay(1500).hide(1000);
        }
    });
}

function emailToPlayers() {
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'email-players'
        },
        success: function (response) {
            switch (response.result) {
                case 'success':
                    const message = response.emailed + ' levél kiküldve';
                    $('#budatoll-message').html('Levélküldés sikeres.<br>' + message).removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(1000).delay(2500).hide(1000);
                    setTimeout(function () {
                        window.location.reload(false);
                    }, 3000);
                    break;
                case 'error':
                    $('#budatoll-message').html('A levélküldés sikeretelen').removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
            }
        },
        error: function (response) {
            $('#budatoll-message').html('A levélküldés sikeretelen').addClass('budatoll-error');
            $('#budatoll-message').show(1000).delay(1500).hide(1000);
        }
    });
}

function emailToRenters() {
    $.ajax({
        url: budatoll_ajax_object.ajax_url,
        type: 'POST',
        dataType: 'json',
        data: {
            action: 'budatoll',
            'ajax-action': 'email-renters'
        },
        success: function (response) {
            switch (response.result) {
                case 'success':
                    const message = response.emailed + ' levél kiküldve';
                    $('#budatoll-message').html('Levélküldés sikeres.<br>' + message).removeClass('budatoll-error').addClass('budatoll-success');
                    $('#budatoll-message').show(1000).delay(2500).hide(1000);
                    setTimeout(function () {
                        window.location.reload(false);
                    }, 3000);
                    break;
                case 'error':
                    $('#budatoll-message').html('A levélküldés sikeretelen').removeClass('budatoll-success').addClass('budatoll-error');
                    $('#budatoll-message').show(1000).delay(1500).hide(1000);
                    break;
            }
        },
        error: function (response) {
            $('#budatoll-message').html('A levélküldés sikeretelen').addClass('budatoll-error');
            $('#budatoll-message').show(1000).delay(1500).hide(1000);
        }
    });
} 