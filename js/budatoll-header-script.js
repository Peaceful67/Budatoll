

btAddedEventIds = [];

function isFullcalendar() {
    var url = window.location.href;
    var mainUrl = url.split('?')[0];
    if (mainUrl.endsWith('/')) {
        mainUrl = mainUrl.substring(0, mainUrl.length - 1);
    }
    var slug = mainUrl.substring(mainUrl.lastIndexOf('/') + 1);
    return slug.includes('edzes-naptar');
}

function submitForm(form_id) {
    document.getElementById(form_id).submit();
}

function btEventClick(eventInfo) {
    alert('Event info: ' + eventInfo.event.id + ' / ' + eventInfo.event.title + ' / ' + eventInfo.event.start);
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

function getDateOfEventDate(event_date) {
    var date = new Date(event_date);
    var year = date.getFullYear(); // Gets the year
    var month = (date.getMonth() + 1).toString().padStart(2, '0'); // Gets the month, zero-padded
    var day = date.getDate().toString().padStart(2, '0'); // Gets the day of the month, zero-padded

    return  year + '-' + month + '-' + day;
}