
$(".clickable-row").click(function () {
    var row_item = $(this).data('item');
    $('#clicked_row').val(row_item);
    $('#BudatollForm').submit();
});


