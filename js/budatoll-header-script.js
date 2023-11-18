

document.addEventListener('DOMContentLoaded', function () {
    var calendarEl = document.getElementById('calendar');
    let currentDate = new Date();
    let year = currentDate.getFullYear();
    let month = currentDate.getMonth() + 1; // getMonth() is zero-indexed
    let day = currentDate.getDate();
    month = month < 10 ? '0' + month : month;
    day = day < 10 ? '0' + day : day;
    let today = year + '-' + month + '-' + day;
    
    var budatoll_calendar = new FullCalendar.Calendar(calendarEl, {
         headerToolbar: {
        left: 'prev,next today',
        center: 'title',
        right: 'dayGridMonth,timeGridWeek'
      },
        initialDate: today,
        editable: true,
        selectable: true,
        businessHours: true,
        droppable: true,
        dayMaxEvents: true, // allow "more" link when too many events
        events: [
            {
                title: 'All Day Event',
                start: '2023-11-01'
            },
            {
                title: 'Long Event',
                start: '2023-11-07',
                end: '2023-11-10'
            },

            {
                title: 'Conference',
                start: '2023-11-11T14:30:00',
                end: '2023-11-11T14:30:00'
            },
        ]
    });

    budatoll_calendar.render();
});
