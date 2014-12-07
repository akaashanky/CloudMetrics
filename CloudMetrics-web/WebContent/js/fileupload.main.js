$(function(){

    $('#file-input-button').click(function(){
        // Simulate a click on the file input button
        // to show the file browser dialog
        $('#fileupload').click();
    });

    // Initialize the jQuery File Upload plugin
    $('#upload1').fileupload({       

        // This function is called when a file is added to the queue;
        // either via the browse button, or via drag/drop:
        add: function (e, data) {

            alert("doing");


            // Automatically upload the file once it is added to the queue
            var jqXHR = data.submit();
        },

        progress: function(e, data){
            
        },

        fail:function(e, data){
            alert("failed");
        },
        
        success:function(e, data){
            // Something has gone wrong!
            alert("success");
        }

    });


    // Helper function that formats the file sizes
    function formatFileSize(bytes) {
        if (typeof bytes !== 'number') {
            return '';
        }

        if (bytes >= 1000000000) {
            return (bytes / 1000000000).toFixed(2) + ' GB';
        }

        if (bytes >= 1000000) {
            return (bytes / 1000000).toFixed(2) + ' MB';
        }

        return (bytes / 1000).toFixed(2) + ' KB';
    }

});