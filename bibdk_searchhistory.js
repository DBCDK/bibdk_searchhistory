/*
 * @file
 * JavaScript for bibdk_searchhistory.
 * Adding Memolist checkbox functionality
 *
 */

(function ($) {
    Drupal.behaviors.bibdk_searchhistory = {        
        attach: function(context) {
            var basePath = Drupal.settings.basePath;
            var currentItem;
            var checked
            
            $('.memolist-check').change(function(){ 
                currentItem = $(this);
                $('.memolist-check').attr('disabled', true);
                
                var string = $(this).attr('string');
                var count = $(this).attr('count');
                var timestamp = $(this).attr('timestamp');
                
                checked = 0;
                if($(this).attr('checked')){
                    checked = 1;
                }
                
                var url = "bibdk_searchhistory/memoitemchanged/" + string + "/" + count + "/" + checked + "/" + timestamp;
                jQuery.ajax({
                    type: 'GET',
                    url:basePath + url,
                    success: onResponse
                });
            });
            
            function onResponse(data){
                if(data){
                    alert(data);
                    if(checked){
                        currentItem.attr('checked', false);
                    } else {
                        currentItem.attr('checked', true);
                    }
                }
                $('.memolist-check').removeAttr('disabled');
                currentItem = null;
                checked = null;
            }
        }
    };
})(jQuery);