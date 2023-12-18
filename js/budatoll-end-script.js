
$(".clickable-row").click(function () {
    var row_item = $(this).data('item');
    $('.budatoll-popup').hide();
    $('#budatoll-popup-item_' + row_item).show();
});

$(document).ready(function () {
  
    if (isFullcalendar()) {
        var eventListEl = document.getElementById('event-types-list');
        new FullCalendar.Draggable(eventListEl, {
            itemSelector: '.fc-event',
            eventData: function (dropInfo) {
                type_data = JSON.parse(dropInfo.dataset.type);
                type_id = type_data.type_id;
                return {
                    id: type_id,
                };
            }
        });
        var calendarEl = document.getElementById('calendar');
        budatoll_calendar = new FullCalendar.Calendar(calendarEl, {

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
                                    budatoll_calendar.addEvent({
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
            firstDay: 1,
            editable: true,
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
        budatoll_calendar.render();
    }
});