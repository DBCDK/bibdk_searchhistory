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
            var shouldSave;
            
            //memolist stuff
            $('.memo-list-btn').click(function(){
                
                currentItem = $(this);
                $('.memo-list-btn').attr('disabled', 1);
                var string = currentItem.attr('data-string');
                var count = currentItem.attr('data-count');
                var timestamp = currentItem.attr('data-timestamp');
                var saved = currentItem.attr('data-saved');
                
                shouldSave = 1;
                
                if(saved){
                    shouldSave = 0;
                }
                
                var url = "bibdk_searchhistory/memoitemchanged/" + string + "/" + shouldSave + "/" + count + "/" + timestamp;
                
                console.log(url);
                jQuery.ajax({
                    type: 'GET',
                    url:basePath + url,
                    success: onResponse
                });
                
                return false;
            });
            
            function onResponse(data){
                console.log(data);
                if(data){
                    alert(data);
                }
                Drupal.t
                if(shouldSave){
                    currentItem.attr('data-saved', 1);
                    currentItem.attr('value', Drupal.t('Remove'));
                } else {
                    currentItem.removeAttr('data-saved');
                    currentItem.attr('value', Drupal.t('Add'));
                }
                $('.memo-list-btn').removeAttr('disabled');
                
                
                currentItem = null;
                selected = null;
            }
        }
    };
})(jQuery);