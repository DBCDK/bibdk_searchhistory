/*
 * @file
 * JavaScript for bibdk_searchhistory.
 * Adding Memolist checkbox functionality
 *
 */

(function ($) {
    Drupal.ajax.prototype.commands.searchhistory_remove = function(ajax, response, status){
        var selector = response['selector'];
        $(selector).closest('tr').remove();
    },
    Drupal.behaviors.bibdk_searchhistory = {
        attach:function (context) {
            var basePath = Drupal.settings.basePath;

            /*Select/combine stuff*/

            //header row
            $('.select-all').change(function () {
                window.setTimeout(countSelected, 100);
            });

            //single rows
            $('td div .combine-select').change(function () {
                countSelected()
            });

            function countSelected() {
                var b = $('td div .combine-select');
                var count = b.filter(':checked').length;

                if (count >= 1) {
                    $('#edit-delete').removeAttr('disabled');
                    $('#edit-combine').attr('disabled', 1);
                    if (count >= 2) {
                        $('#edit-combine').removeAttr('disabled');
                    }
                } else {
                    $('#edit-delete').attr('disabled', 1);
                    $('#edit-combine').attr('disabled', 1);
                }
            }

        }
    };
})(jQuery);
