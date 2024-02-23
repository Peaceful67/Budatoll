
/*
 
 
 var calendar_script = document.createElement('script');
 calendar_script.type = 'text/javascript';
 switch (getCalendarType()) {
 case 'events':
 calendar_script.src = '../js/events_calendar.js';
 break;
 case 'trainings':
 calendar_script.src = '../js/trainings_calendar.js';
 break;
 case 'my-trainings':
 calendar_script.src = '../js/my_trainings_calendar.js';
 break;
 }
 document.head.appendChild(calendar_script); // Betoltjuk a megfelelo naptarat
 */

function getCalendarType() { // Megmondja az URL alapján,melyik fajta naptárat töltsük be
    const calendarTypes = {
        'alkalom-naptar': 'events',
        'edzesek-naptar': 'trainings',
        'edzeseim-naptar': 'my-trainings'
    };
    var url = window.location.href;
    var mainUrl = url.split('?')[0];
    if (mainUrl.endsWith('/')) {
        mainUrl = mainUrl.substring(0, mainUrl.length - 1);
    }
    var slug = mainUrl.substring(mainUrl.lastIndexOf('/') + 1);
    for (const [pattern, type] of Object.entries(calendarTypes)) {
        if (slug.includes(pattern)) {
            return type;
        }
    }
    return false;
}

function submitForm(form_id) {
    document.getElementById(form_id).submit();
}

function getDateOfEventDate(event_date) {
    var date = new Date(event_date);
    var year = date.getFullYear(); // Gets the year
    var month = (date.getMonth() + 1).toString().padStart(2, '0'); // Gets the month, zero-padded
    var day = date.getDate().toString().padStart(2, '0'); // Gets the day of the month, zero-padded

    return  year + '-' + month + '-' + day;
}

