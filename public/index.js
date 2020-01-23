// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    $('#status-area').attr('disabled', true);
    var today = new Date();
    var dd = String(today.getDate()).padStart(2, '0');
    var mm = String(today.getMonth() + 1).padStart(2, '0');
    var yyyy = today.getFullYear();
    today = yyyy + '-' + mm + '-' + dd;
    $('#create_DTSTART').attr('min', today);
    $('#add_DTSTART').attr('min', today);
    // $('#login-btn').attr('disabled', true);
    $('#storeall-btn').attr('disabled', true);
    $('#execute-btn').attr('disabled', true);

    $('#login_user').val('ddomalik');
    $('#login_pw').val('0933553');
    $('#login_db').val('test');

    $.ajax({
        type: 'GET',
        dataType: 'json', 
        url: '/pageload',
        success: function (data) {
            if (data !== 0) {
                for (i in data) {
                    var row = '<tr>' +
                        '<td>' + '<a href="/uploads/' + data[i]['filename'] + '">' + data[i]['filename'] + '</a>' + '</td>' +
                        '<td>' + data[i]['version'] + '</td>' +
                        '<td>' + data[i]['prodID'] + '</td>' +
                        '<td>' + data[i]['numEvents'] + '</td>' +
                        '<td>' + data[i]['numProps'] + '</td>' +
                    '</tr>';
                    $('#file_log-table').append(row);
                    if (data[i]['prodID'] !== 'invalid file') {
                        var list_row = '<option value ="' + data[i]['filename'] + '">' + data[i]['filename'] + '</option>'
                        $('#files-list').append(list_row);
                        $('#add-files-list').append(list_row);
                        $('#storeall-btn').attr('disabled', false);
                    }
                    i++;
                }
                
                $('#file_log-h5').html("");
                $('#btn-wrapper').css('display', 'block');
                $('#table-wrapper').css('display', 'inline-block');
                $('#_cal').css('display', 'block');
                $('#cal-btn-wrapper').css('display', 'block');
                $('#upload-btn').attr('disabled', true);
                $('#add-btn').attr('disabled', true);
                $('#create-btn').attr('disabled', true);

                updateStatus("Server: Successfully loaded files from uploads folder");
            }else{
                updateStatus("Server: No files on server...");
            }

            // console.log(data);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            updateStatus(error['responseText']);
            console.log(error['responseText']); 
        }
    });

    // ************* A3 functionality - MUST be implemented ***************
    $('#clear-btn').on('click', function(e){
        e.preventDefault();
        $('#status-area').val('');
    });

    $('#browse-btn').change(function(e){
        e.preventDefault();
        var data = $('#browse-btn')[0].files[0];

        var extension = data['name'].substr((data['name'].lastIndexOf('.') + 1));
        if (extension !== 'ics') {
            $('#upload-btn').attr('disabled', true);
            updateStatus("FileLog: Upload button disabled. \'" + data['name'] + "\' is an invalid file...");
        }else{
            fileCheck(data['name']);
        }
    });

    $('#file_log').submit(function(e){
        e.preventDefault();
        var file_data = $('#browse-btn')[0].files[0];
        var formData = new FormData();
        formData.append("uploadFile", file_data);
        $.ajax({
            method: 'POST',
            url: '/upload',
            data: formData,
            cache: false,
            contentType: false,
            processData: false,
            success: function(data){
                updateStatus("FileLog: Successfully uploaded file: \'" + file_data['name'] + "\'!");
                updateRow(file_data);
                $('#upload-btn').attr('disabled', true);
                $('#browse-btn-lbl').html("Choose a file");
            },
            fail: function(error) {
                // Non-200 return, do something with error
                updateStatus("FileLog: " + error['responseText']);
                console.log(error['responseText']); 
            }
        });
    });

    $('#calendar_view').submit(function(e){
        e.preventDefault();
        var file = $('#files-list').val();
        if (file !== "") {
            $.ajax({
                type: 'GET',
                contentType: 'application/json',
                data: {
                    filename: file
                },
                url: '/getEvents',
                success: function (data) {
                    if (data !== "") {
                        $('#calendar-table-wrapper').css('display', 'block');

                        var header = '<tr>' +
                            '<th>Event No</th>' +
                            '<th>Start date</th>' +
                            '<th>Start time</th>' +
                            '<th>Summary</th>' +
                            '<th>Props</th>' +
                            '<th>Alarms</th>' +
                        '</tr>';
                        
                        var rows = '';
                        rows += header;
                        for (i in data) {
                            var row;
                            var date = data[i]['startDT']['date'].substring(0, 4) + "/" +
                                    data[i]['startDT']['date'].substring(4, 6) + "/" +
                                    data[i]['startDT']['date'].substring(6, 8);
                            var time = data[i]['startDT']['time'].substring(0, 2) + ":" +
                                    data[i]['startDT']['time'].substring(2, 4) + ":" +
                                    data[i]['startDT']['time'].substring(4, 6);
                            var alarmRow;
                            var propRow;

                            if (data[i]['numAlarms'] > 0) {
                                alarmRow = '<a href=\"#\"onclick=\"javascript:getAlarms(\'' + file + '\',\'' + i + '\'); return false;\">';
                            }else{
                                alarmRow = '';
                            }

                            if (data[i]['numProps'] > 3) {
                                propRow = '<a href=\"#\"onclick=\"javascript:getProps(\'' + file + '\',\'' + i + '\'); return false;\">';
                            }else{
                                propRow = '';
                            }
                        
                            if (data[i]['startDT']['isUTC'] === true) {
                                row = '<tr>' +
                                    '<td>' + (parseInt(i) + parseInt(1)) + '</td>' +
                                    '<td>' + date + '</td>' +
                                    '<td>' + time + ' (UTC)' + '</td>' +
                                    '<td>' + data[i]['summary'] + '</td>' +
                                    '<td>' + propRow + data[i]['numProps'] + '</td>' +
                                    '<td>' + alarmRow + data[i]['numAlarms'] + '</td>' +
                                '</tr>';
                            }else{
                                row = '<tr>' +
                                    '<td>' + (parseInt(i) + parseInt(1)) + '</td>' +
                                    '<td>' + date + '</td>' +
                                    '<td>' + time + '</td>' +
                                    '<td>' + data[i]['summary'] + '</td>' +
                                    '<td>' + propRow + data[i]['numProps'] + '</td>' +
                                    '<td>' + alarmRow + data[i]['numAlarms'] + '</td>' +
                                '</tr>';
                            }

                            rows += row;
                            i++;
                        }
                        $('#calendar-table').html(rows);
                        updateStatus("CalendarView: Displaying '" + file + "'");
                    }else{
                        $('#calendar-table-wrapper').css('display', 'none');
                        updateStatus("CalendarView: Error with calendar file...");
                    }
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    updateStatus("CalendarView: " + error['responseText']);
                    console.log(error['responseText']); 
                }
            });
        }else{
            $('#calendar-table-wrapper').css('display', 'none');
        }
    });

    $('#create_filename').change(function () {
        var filename = $('#create_filename').val();
        var dots = (filename.split(".").length - 1);
        var squote = (filename.split("\'").length - 1);
        var dquote = (filename.split("\"").length - 1);
        if (dots === 1 && squote === 0 && dquote === 0) {
            var extension = filename.substr(filename.lastIndexOf('.') + 1);
            if (extension !== 'ics') {
                $('#create-btn').attr('disabled', true);
                updateStatus("CreateCalendar: Filename invalid!");
            }else{
                $('#create-btn').attr('disabled', false);
            }
        }else{
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: Filename invalid!");
        }
    });

    $('#create_version').change(function () {
        var version = $('#create_version').val();
        var dots = (version.split(".").length - 1);
        var squote = (version.split("\'").length - 1);
        var dquote = (version.split("\"").length - 1);
        if (dots === 0 && squote === 0 && dquote === 0) {
            if (isNaN(version)) {
                $('#create-btn').attr('disabled', true);
                updateStatus("CreateCalendar: Version invalid! (no quotes or periods)");
            }else{
                if (version > 0) {
                    $('#create-btn').attr('disabled', false);
                }else{
                    $('#create-btn').attr('disabled', true);
                    updateStatus("CreateCalendar: Version invalid! (higher than 0)");
                }
            }
        }else{
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: Version invalid!");
        }
    });

    $('#create_prodID').change(function () {
        var prodID = $('#create_prodID').val();
        var dots = (prodID.split(".").length - 1);
        var squote = (prodID.split("\'").length - 1);
        var dquote = (prodID.split("\"").length - 1);
        if (dots === 0 && squote === 0 && dquote === 0) {
            $('#create-btn').attr('disabled', false);
        }else{
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: Product ID invalid! (no quotes or periods)");
        }
    });

    $('#create_UID').change(function () {
        var UID = $('#create_UID').val();
        var dots = (UID.split(".").length - 1);
        var squote = (UID.split("\'").length - 1);
        var dquote = (UID.split("\"").length - 1);
        if (dots === 0 && squote === 0 && dquote === 0) {
            $('#create-btn').attr('disabled', false);
        }else{
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: UID invalid! (no quotes or periods)");
        }
    });

    $('#create_DTSTART').change(function (){
        var dtstart = $('#create_DTSTART').val();
        if (!dtstart) {
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: DTSTART(date) invalid!");
        }else{
            $('#create-btn').attr('disabled', false);
        }
    });

    $('#_create_DTSTART').change(function (){
        var dtstart = $('#_create_DTSTART').val();
        if (dtstart.length !== 6) {
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: DTSTART(time) invalid!");
        }else{
            $('#create-btn').attr('disabled', false);
        }
    });

    $('#create-btn').on('click', function(e){
        e.preventDefault();
        var filename = $('#create_filename').val();
        var version = parseInt($('#create_version').val());
        var prodID = $('#create_prodID').val();
        var UID = $('#create_UID').val();
        var dtstart = (($('#create_DTSTART').val()).replace('-', '')).replace('-', '');
        var _dtstart = $('#_create_DTSTART').val();
        var summary = $('#create_summary').val();
        if (filename !== '' && version !== '' && prodID !== '' &&
            UID !== '' && dtstart !== '' && _dtstart !== '') {
            $.ajax({
                type: 'GET',
                dataType: 'json', 
                url: '/getFiles',
                success: function (data) {
                    var found = 0;
                    for (i in data) {
                        if (data[i] === filename) {
                            found = 1;
                        }
                        i++;
                    }
                    if (found == 0) {
                        var today = new Date();
                        var DD = String(today.getDate()).padStart(2, '0');
                        var MM = String(today.getMonth() + 1).padStart(2, '0');
                        var YYYY = today.getFullYear();
                        var hh = String(today.getHours()).padStart(2, '0');
                        var mm = String(today.getMinutes()).padStart(2, '0');
                        var ss = String(today.getSeconds()).padStart(2, '0');
                        today = YYYY + '' + MM + '' + DD + hh + '' + mm + '' + ss;

                        var formData = new FormData();
                        var cal_info = {
                            version: version,
                            prodID: prodID
                        };
                        formData.append("filename", "uploads/" + filename);
                        formData.append("cal_info", JSON.stringify(cal_info));
                        formData.append("UID", UID);
                        formData.append("dtstart", dtstart);
                        formData.append("_dtstart", _dtstart);
                        formData.append("dtstamp", today.substr(0, 8));
                        formData.append("_dtstamp", today.substr(8, 6));
                        formData.append("summary", summary);

                        var updatefile = {
                            name: filename
                        }

                        $.ajax({
                            method: 'POST',
                            url: '/createCal',
                            data: formData,
                            cache: false,
                            contentType: false,
                            processData: false,
                            success: function(data){
                                updateStatus("CreateCalendar: File \'" + filename + "\' created!");
                                updateRow(updatefile);
                                $('#create_filename').val('');
                                $('#create_version').val('');
                                $('#create_prodID').val('');
                                $('#create_UID').val('');
                                $('#create_DTSTART').val('');
                                $('#_create_DTSTART').val('');
                                $('#create_summary').val('');
                                $('#create-btn').attr('disabled', true);

                                if ($('#file_log-h5').html() === "No files") {
                                    $('#file_log-h5').html("");
                                    $('#btn-wrapper').css('display', 'block');
                                    $('#table-wrapper').css('display', 'inline-block');
                                    $('#_cal').css('display', 'block');
                                    $('#cal-btn-wrapper').css('display', 'block');
                                    $('#upload-btn').attr('disabled', true);
                                    $('#add-btn').attr('disabled', true);
                                    $('#create-btn').attr('disabled', true);
                                }
                            },
                            fail: function(error) {
                                // Non-200 return, do something with error
                                updateStatus("CreateCalendar: " + error['responseText']);
                                console.log(error['responseText']); 
                            }
                        });
                    }else{
                        updateStatus("CreateCalendar: File \'" + filename + "\' already exists...");
                    }
                }
            });
        }else{
            var invalid = '';
            if (filename === '') { invalid += 'filename'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (version === '') { invalid += 'version'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (prodID === '') { invalid += 'prodID'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (UID === '') { invalid += 'UID'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (dtstart === '') { invalid += 'DTSTART(date)'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (_dtstart === '') { invalid += 'DTSTART(time)'; }
            if (invalid[invalid.length - 1] === ',') {
                updateStatus("CreateCalendar: Missing information [" + invalid.substring(0, invalid.length - 1) + "]");
            }else{
                updateStatus("CreateCalendar: Missing information [" + invalid + "]");
            }
        }
    });

    $('#create_UID').change(function () {
        var UID = $('#create_UID').val();
        var dots = (UID.split(".").length - 1);
        var squote = (UID.split("\'").length - 1);
        var dquote = (UID.split("\"").length - 1);
        if (dots === 0 && squote === 0 && dquote === 0) {
            $('#create-btn').attr('disabled', false);
        }else{
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: UID invalid! (no quotes or periods)");
        }
    });

    $('#create_DTSTART').change(function (){
        var dtstart = $('#create_DTSTART').val();
        if (!dtstart) {
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: DTSTART(date) invalid!");
        }else{
            $('#create-btn').attr('disabled', false);
        }
    });

    $('#_create_DTSTART').change(function (){
        var dtstart = $('#_create_DTSTART').val();
        if (dtstart.length !== 6) {
            $('#create-btn').attr('disabled', true);
            updateStatus("CreateCalendar: DTSTART(time) invalid!");
        }else{
            $('#create-btn').attr('disabled', false);
        }
    });

    $('#add_UID').change(function () {
        var UID = $('#add_UID').val();
        var dots = (UID.split(".").length - 1);
        var squote = (UID.split("\'").length - 1);
        var dquote = (UID.split("\"").length - 1);
        if (dots === 0 && squote === 0 && dquote === 0) {
            $('#add-btn').attr('disabled', false);
        }else{
            $('#add-btn').attr('disabled', true);
            updateStatus("AddEvent: UID invalid! (no quotes or periods)");
        }
    });

    $('#add_DTSTART').change(function (){
        var dtstart = $('#add_DTSTART').val();
        if (!dtstart) {
            $('#add-btn').attr('disabled', true);
            updateStatus("AddEvent: DTSTART(date) invalid!");
        }else{
            $('#add-btn').attr('disabled', false);
        }
    });

    $('#_add_DTSTART').change(function (){
        var dtstart = $('#_add_DTSTART').val();
        if (dtstart.length !== 6) {
            $('#add-btn').attr('disabled', true);
            updateStatus("AddEvent: DTSTART(time) invalid!");
        }else{
            $('#add-btn').attr('disabled', false);
        }
    });

    $('#add-btn').on('click', function(e){
        e.preventDefault();
        var filename = $('#add-files-list').val();
        var UID = $('#add_UID').val();
        var dtstart = (($('#add_DTSTART').val()).replace('-', '')).replace('-', '');
        var _dtstart = $('#_add_DTSTART').val();
        var summary = $('#add_summary').val();
        if (filename !== '' && UID !== '' && dtstart !== '' && _dtstart !== '') {
            var today = new Date();
            var DD = String(today.getDate()).padStart(2, '0');
            var MM = String(today.getMonth() + 1).padStart(2, '0');
            var YYYY = today.getFullYear();
            var hh = String(today.getHours()).padStart(2, '0');
            var mm = String(today.getMinutes()).padStart(2, '0');
            var ss = String(today.getSeconds()).padStart(2, '0');
            today = YYYY + '' + MM + '' + DD + hh + '' + mm + '' + ss;

            var formData = new FormData();
            formData.append("filename", "uploads/" + filename);
            formData.append("UID", UID);
            formData.append("dtstart", dtstart);
            formData.append("_dtstart", _dtstart);
            formData.append("dtstamp", today.substr(0, 8));
            formData.append("_dtstamp", today.substr(8, 6));
            formData.append("summary", summary);

            $.ajax({
                method: 'POST',
                url: '/addEvent',
                data: formData,
                cache: false,
                contentType: false,
                processData: false,
                success: function(data){
                    if (data === 'UID') {
                        updateStatus("AddEvent: UID is not unique...");
                        $('#add-btn').attr('disabled', true);
                    }else{
                        var header = '<tr>' +
                            '<th>File Name (click to download)</th>' +
                            '<th>Version</th>' +
                            '<th>Product ID</th>' +
                            '<th>Number of Events</th>' +
                            '<th>Number of Properties</th>' +
                        '</tr>';
                        
                        $('#file_log-table').html('');
                        $('#file_log-table').append(header);

                        $.ajax({
                            type: 'GET',
                            dataType: 'json', 
                            url: '/pageload',
                            success: function (data) {
                                if (data !== 0) {
                                    for (i in data) {
                                        var row = '<tr>' +
                                            '<td>' + '<a href="/uploads/' + data[i]['filename'] + '">' + data[i]['filename'] + '</a>' + '</td>' +
                                            '<td>' + data[i]['version'] + '</td>' +
                                            '<td>' + data[i]['prodID'] + '</td>' +
                                            '<td>' + data[i]['numEvents'] + '</td>' +
                                            '<td>' + data[i]['numProps'] + '</td>' +
                                        '</tr>';
                                        $('#file_log-table').append(row);
                                        if (data[i]['prodID'] !== 'invalid file') {
                                            var list_row = '<option value ="' + data[i]['filename'] + '">' + data[i]['filename'] + '</option>'
                                        }
                                        i++;
                                    }
                                }
                            },
                            fail: function(error) {
                                // Non-200 return, do something with error
                                updateStatus(error['responseText']);
                                console.log(error['responseText']); 
                            }
                        });
                    }
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    updateStatus("AddEvent: " + error['responseText']);
                    console.log(error['responseText']); 
                }
            });

            if ($('#files-list').val() === $('#add-files-list').val()) {
                $('#calendar-table').html('');
                var file = $('#files-list').val();
                $.ajax({
                    type: 'GET',
                    contentType: 'application/json',
                    data: {
                        filename: file
                    },
                    url: '/getEvents',
                    success: function (data) {
                        if (data !== "") {
                            var header = '<tr>' +
                                '<th>Event No</th>' +
                                '<th>Start date</th>' +
                                '<th>Start time</th>' +
                                '<th>Summary</th>' +
                                '<th>Props</th>' +
                                '<th>Alarms</th>' +
                            '</tr>';
                            
                            var rows = '';
                            rows += header;
                            for (i in data) {
                                var row;
                                var date = data[i]['startDT']['date'].substring(0, 4) + "/" +
                                        data[i]['startDT']['date'].substring(4, 6) + "/" +
                                        data[i]['startDT']['date'].substring(6, 8);
                                var time = data[i]['startDT']['time'].substring(0, 2) + ":" +
                                        data[i]['startDT']['time'].substring(2, 4) + ":" +
                                        data[i]['startDT']['time'].substring(4, 6);
                                var alarmRow;
                                var propRow;
    
                                if (data[i]['numAlarms'] > 0) {
                                    alarmRow = '<a href=\"#\"onclick=\"javascript:getAlarms(\'' + file + '\',\'' + i + '\'); return false;\">';
                                }else{
                                    alarmRow = '';
                                }
    
                                if (data[i]['numProps'] > 3) {
                                    propRow = '<a href=\"#\"onclick=\"javascript:getProps(\'' + file + '\',\'' + i + '\'); return false;\">';
                                }else{
                                    propRow = '';
                                }
                            
                                if (data[i]['startDT']['isUTC'] === true) {
                                    row = '<tr>' +
                                        '<td>' + (parseInt(i) + parseInt(1)) + '</td>' +
                                        '<td>' + date + '</td>' +
                                        '<td>' + time + ' (UTC)' + '</td>' +
                                        '<td>' + data[i]['summary'] + '</td>' +
                                        '<td>' + propRow + data[i]['numProps'] + '</td>' +
                                        '<td>' + alarmRow + data[i]['numAlarms'] + '</td>' +
                                    '</tr>';
                                }else{
                                    row = '<tr>' +
                                        '<td>' + (parseInt(i) + parseInt(1)) + '</td>' +
                                        '<td>' + date + '</td>' +
                                        '<td>' + time + '</td>' +
                                        '<td>' + data[i]['summary'] + '</td>' +
                                        '<td>' + propRow + data[i]['numProps'] + '</td>' +
                                        '<td>' + alarmRow + data[i]['numAlarms'] + '</td>' +
                                    '</tr>';
                                }
    
                                rows += row;
                                i++;
                            }
                            $('#calendar-table').html(rows);
                        }else{
                            $('#calendar-table-wrapper').css('display', 'none');
                            updateStatus("CalendarView: Error with calendar file...");
                        }
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("CalendarView: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
            }

            updateStatus("AddEvent: File \'" + filename + "\' updated!");
            $('#add-files-list').val('');
            $('#add_UID').val('');
            $('#add_DTSTART').val('');
            $('#_add_DTSTART').val('');
            $('#add_summary').val('');
            $('#add-btn').attr('disabled', true);
            $('#file_log-table').html('');
        }else{
            var invalid = '';
            if (UID === '') { invalid += 'UID'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (dtstart === '') { invalid += 'DTSTART(date)'; }
            if (invalid[invalid.length - 1] !== ',' && invalid !== '') { invalid += ','; }
            if (_dtstart === '') { invalid += 'DTSTART(time)'; }
            if (invalid[invalid.length - 1] === ',') {
                updateStatus("AddEvent: Missing information [" + invalid.substring(0, invalid.length - 1) + "]");
            }else{
                updateStatus("AddEvent: Missing information [" + invalid + "]");
            }
        }
    });

    $('#add-files-list').change(function(e){
        e.preventDefault();
        var cur = $(this).val();
        if (cur === '') {
            $('#add-btn').attr('disabled', true);
        }else{
            $('#add-btn').attr('disabled', false);
        }
    });
    // ********************************************************************

    // ************* A4 functionality - MUST be implemented ***************
    $('#login-btn').on('click', function(e){
        e.preventDefault();
        var user = $('#login_user').val();
        var pw = $('#login_pw').val();
        var db = $('#login_db').val();
        updateStatus("Login: Attempting database connection");
        $.ajax({
            type: 'GET',
            contentType: 'application/json',
            data: {
                user: user,
                pw: pw,
                db: db,
            },
            url: '/dbConnect',
            success: function (data) {
                if (data === '0') {
                    updateStatus("Login: Database connection established");
                    $('#_login').css('display', 'none');
                    $('#_dbwrapper').css('display', 'inline-block');
                }else if (data === '-1') {
                    updateStatus("Login: Error connecting to database (timeout)");
                }else if (data === '-2') {
                    updateStatus("Login: Error connecting to database (invalid credentials)");
                }else if (data === '-3') {
                    updateStatus("Login: Error creating FILE table");
                }else if (data === '-4') {
                    updateStatus("Login: Error creating EVENT table");
                }else if (data === '-5') {
                    updateStatus("Login: Error creating ALARM table");
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                updateStatus("Login: " + error['responseText']);
                console.log(error['responseText']); 
            }
        });
    });

    $('#login_user').change(function (){
        var pw = $('#login_pw').val();
        var db = $('#login_db').val();
        if (!pw || !db) {
            $('#login-btn').attr('disabled', true);
        }else{
            $('#login-btn').attr('disabled', false);
        }
    });

    $('#login_pw').change(function (){
        var user = $('#login_user').val();
        var db = $('#login_db').val();
        if (!user || !db) {
            $('#login-btn').attr('disabled', true);
        }else{
            $('#login-btn').attr('disabled', false);
        }
    });

    $('#login_db').change(function (){
        var pw = $('#login_pw').val();
        var user = $('#login_user').val();
        if (!pw || !user) {
            $('#login-btn').attr('disabled', true);
        }else{
            $('#login-btn').attr('disabled', false);
        }
    });

    $('#logout-btn').on('click', function(e){
        e.preventDefault();
        $.ajax({
            type: 'GET',
            dataType: 'JSON',
            url: '/dbLogout',
            success: function (data) {
                if (data === 0) {
                    updateStatus("Logout: Database connection terminated");
                    $('#login_user').val('');
                    $('#login_pw').val('');
                    $('#login_db').val('');
                    $('#_login').css('display', 'inline-block');
                    $('#_dbwrapper').css('display', 'none');
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                updateStatus("Logout: " + error['responseText']);
                console.log(error['responseText']); 
            }
        });
    })

    $('#storeall-btn').on('click', function(e){
        e.preventDefault();
        $.ajax({
            type: 'GET',
            dataType: 'JSON',
            url: '/dbStoreAllFiles',
            success: function (data) {
                if (data === 0) {
                    updateStatus("StoreAllFiles: Successfully stored all files");
                }else if (data === -9) {
                    updateStatus("StoreAllFiles: No Database Connection, Log In Again");
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                updateStatus("StoreAllFiles: " + error['responseText']);
                console.log(error['responseText']); 
            }
        });
    });

    $('#clearall-btn').on('click', function(e){
        e.preventDefault();
        $.ajax({
            type: 'GET',
            dataType: 'JSON',
            url: '/dbClearAllData',
            success: function (data) {
                if (data === 0) {
                    updateStatus("ClearAllData: Successfully cleared all data");
                }else{
                    if (data === -3) {
                        updateStatus("ClearAllData: Error deleting ALARM table");
                    }else if (data === -2) {
                        updateStatus("ClearAllData: Error deleting EVENT table");
                    }else if (data === -1) {
                        updateStatus("ClearAllData: Error deleting FILE table");
                    }else if (data === -9) {
                        updateStatus("ClearAllData: No Database Connection, Log In Again");
                    }
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                updateStatus("ClearAllData: " + error['responseText']);
                console.log(error['responseText']); 
            }
        });
    });

    $('#display-btn').on('click', function(e){
        e.preventDefault();
        $.ajax({
            type: 'GET',
            dataType: 'JSON',
            url: '/dbDisplayStatus',
            success: function (data) {
                if (data !== -1 && data !== -9) {
                    updateStatus("DisplayDBStatus: Database has " + data['file'] + " files, " +
                                data['event'] + " events, and " + data['alarm'] + " alarms");
                }else if (data === -9) {
                    updateStatus("DisplayDBStatus: No Database Connection, Log In Again");
                }else{
                    updateStatus("DisplayDBStatus: An error has occured");
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                updateStatus("DisplayDBStatus: " + error['responseText']);
                console.log(error['responseText']); 
            }
        });
    });

    $('#query-list').change(function(e){
        e.preventDefault();
        var cur = $(this).val();
        if (cur === '') {
            $('#execute-btn').attr('disabled', true);
        }else{
            if (cur === '2' || cur === '6') {
                $('#db2').css('display', 'inline-block');
                $('#file_db').val('');
                $('#execute-btn').attr('disabled', true);
            }else{
                $('#db2').css('display', 'none');
                $('#execute-btn').attr('disabled', false);
            }
        }
    });

    $('#file_db').change(function(e){
        e.preventDefault();
        var filename = $('#file_db').val();
        var dots = (filename.split(".").length - 1);
        var squote = (filename.split("\'").length - 1);
        var dquote = (filename.split("\"").length - 1);
        if (dots === 1 && squote === 0 && dquote === 0) {
            var extension = filename.substr(filename.lastIndexOf('.') + 1);
            if (extension !== 'ics') {
                $('#execute-btn').attr('disabled', true);
                updateStatus("Execute Query: Filename invalid!");
            }else{
                $('#execute-btn').attr('disabled', false);
            }
        }else{
            $('#execute-btn').attr('disabled', true);
            updateStatus("Execute Query: Filename invalid!");
        }
    });

    $('#execute-btn').on('click', function(e){
        e.preventDefault();
        var option = parseInt($('#query-list').val());
        $('#query-list').val('');
        $('#execute-btn').attr('disabled', true);
        $('#db2').css('display', 'none');
        switch (option) {
            case 1:
                $.ajax({
                    type: 'GET',
                    dataType: 'JSON',
                    url: '/dbQuery1',
                    success: function (data) {
                        if (data !== -1 && data !== -9) {
                            updateStatus("----- Query #1 Start -----");
                            if (data.length < 1) {
                                updateStatus("NO ROWS FOUND");    
                            }else{
                                for (var i = 0; i < data.length; i++) {
                                    updateStatus("EventID:" + data[i]['event_id'] + ", Summary:" + data[i]['summary'] + ", Start:" + data[i]['start_time']);
                                }
                            }
                            updateStatus("----- Query #1 End -----");
                        }else if (data === -9) {
                            updateStatus("Query #1: No Database Connection, Log In Again");
                        }else{
                            updateStatus("Query #1: An error has occured");
                        }
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("Query #1: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
                break;
            case 2:
                var filename = $('#file_db').val();
                $.ajax({
                    type: 'GET',
                    contentType: 'application/json',
                    data: {
                        filename: filename
                    },
                    url: '/dbQuery2',
                    success: function (data) {
                        if (data !== '-1' && data !== '-9') {
                            updateStatus("----- Query #2 Start -----");
                            if (data.length < 1) {
                                updateStatus("NO ROWS FOUND");    
                            }else{
                                for (var i = 0; i < data.length; i++) {
                                    updateStatus("Start:" + data[i]['start_time'] + ", Summary:" + data[i]['summary']);
                                }
                            }
                            updateStatus("----- Query #2 End -----");
                        }else if (data === '-9') {
                            updateStatus("Query #2: No Database Connection, Log In Again");
                        }else{
                            updateStatus("Query #2: An error has occured");
                        }
                        $('#file_db').val('');
                        $('#execute-btn').attr('disabled', true);
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("Query #2: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
                break;
            case 3:
                $.ajax({
                    type: 'GET',
                    dataType: 'JSON',
                    url: '/dbQuery3',
                    success: function (data) {
                        if (data !== -1 && data !== -9) {
                            updateStatus("----- Query #3 Start -----");
                            if (data.length < 1) {
                                updateStatus("NO ROWS FOUND");    
                            }else{
                                for (var i = 0; i < data.length; i++) {
                                    updateStatus("EventID:" + data[i]['event_id'] + ", Summary:" + data[i]['summary'] + ", Start:" + data[i]['start_time']);
                                }
                            }
                            updateStatus("----- Query #3 End -----");
                        }else if (data === -9) {
                            updateStatus("Query #3: No Database Connection, Log In Again");
                        }else{
                            updateStatus("Query #3: An error has occured");
                        }
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("Query #3: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
                break;
            case 4:
                $.ajax({
                    type: 'GET',
                    dataType: 'JSON',
                    url: '/dbQuery4',
                    success: function (data) {
                        if (data !== -1 && data !== -9) {
                            updateStatus("----- Query #4 Start -----");
                            if (data.length < 1) {
                                updateStatus("NO ROWS FOUND");    
                            }else{
                                for (var i = 0; i < data.length; i++) {
                                    updateStatus("EventID:" + data[i]['event_id'] + ", Summary:" + data[i]['summary'] + ", Start:" + data[i]['start_time']);
                                }
                            }
                            updateStatus("----- Query #4 End -----");
                        }else{
                            if (data === -9) {
                                updateStatus("Query #4: No Database Connection, Log In Again");
                            }else{
                                updateStatus("Query #4: An error has occured");
                            }
                        }
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("Query #4: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
                
                break;
            case 5:
                $.ajax({
                    type: 'GET',
                    dataType: 'JSON',
                    url: '/dbQuery5',
                    success: function (data) {
                        if (data !== -1 && data !== -9) {
                            updateStatus("----- Query #5 Start -----");
                            if (data.length < 1) {
                                updateStatus("NO ROWS FOUND");    
                            }else{
                                for (var i = 0; i < data.length; i++) {
                                    updateStatus("AlarmID:" + data[i]['alarm_id'] + ", Summary:" + data[i]['summary'] + ", Start:" + data[i]['start_time']);
                                }
                            }
                            updateStatus("----- Query #5 End -----");
                        }else if (data === -9) {
                            updateStatus("Query #5: No Database Connection, Log In Again");
                        }else{
                            updateStatus("Query #5: An error has occured");
                        }
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("Query #5: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
                break;
            case 6:
                var filename = $('#file_db').val();
                $.ajax({
                    type: 'GET',
                    contentType: 'application/json',
                    data: {
                        filename: filename
                    },
                    url: '/dbQuery6',
                    success: function (data) {
                        if (data !== '-1' && data !== '-9') {
                            updateStatus("----- Query #6 Start -----");
                            if (data.length < 1) {
                                updateStatus("NO ROWS FOUND");    
                            }else{
                                for (var i = 0; i < data.length; i++) {
                                    updateStatus("AlarmID:" + data[i]['alarm_id'] + ", Action:" + data[i]['action'] + ", Trigger:" + data[i]['trigger']);
                                }
                            }
                            updateStatus("----- Query #6 End -----");
                        }else if (data === '-9') {
                            updateStatus("Query #6: No Database Connection, Log In Again")
                        }else{
                            updateStatus("Query #6: An error has occured");
                        }
                        $('#file_db').val('');
                        $('#execute-btn').attr('disabled', true);
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        updateStatus("Query #6: " + error['responseText']);
                        console.log(error['responseText']); 
                    }
                });
                break;
            default:
                break;
        }
    });
    // ********************************************************************
});

function updateStatus(update) {
    var old = "\n" + $('#status-area').val();
    $('#status-area').val("   " + update + "   " + old);
}

function updateRow(filename) {
    $.ajax({
        type: 'GET',
        contentType: 'application/json',
        data: {
            file: filename['name']
        },
        url: '/calToJSON',
        success: function (data) {
            var row = '<tr>' +
                '<td>' + '<a href="/uploads/' + filename['name'] + '">' + filename['name'] + '</a>' + '</td>' +
                '<td>' + data['version'] + '</td>' +
                '<td>' + data['prodID'] + '</td>' +
                '<td>' + data['numEvents'] + '</td>' +
                '<td>' + data['numProps'] + '</td>' +
            '</tr>';
            $('#file_log-table').append(row);
            
            var list_row = '<option value ="' + filename['name'] + '">' + filename['name'] + '</option>'
            $('#files-list').append(list_row);
            $('#add-files-list').append(list_row);
        }
    });
}

function fileCheck(filename) {
    $.ajax({
        type: 'GET',
        dataType: 'json', 
        url: '/getFiles',
        success: function (data) {
            var found = 0;
            for (i in data) {
                if (data[i] === filename) {
                    found = 1;
                }
                i++;
            }
            if (found == 0) {
                updateStatus("FileLog: File \'" + filename + "\' selected");
                $('#upload-btn').attr('disabled', false);
                $('#browse-btn-lbl').html("\'" + filename + "\' selected");
            }else{
                updateStatus("FileLog: Upload button disabled. \'" + filename + "\' already exists...");
                $('#upload-btn').attr('disabled', true);
                $('#browse-btn-lbl').html("Choose a file");
            }
        }
    });
}

function getAlarms(filename, alarm) {
    $.ajax({
        type: 'GET',
        contentType: 'application/json',
        data: {
            file: filename,
            num: alarm
        }, 
        url: '/getAlarms',
        success: function (data) {
            var alarmList = '';
            for (i in data) {
                var addProps;
                if (data[i]['addProps']['length'] > 0) {
                    addProps = ', additionalProps:[{' + data[i]['addProps'][0]['propName'] + ':' + data[i]['addProps'][0]['propDescr'] + '}]';
                }else{
                    addProps = '';
                }

                if (parseInt(i) === (parseInt(data['length']) - parseInt(1))) {
                    alarmList += '{alarmNum:' + (parseInt(i) + parseInt(1)) + ', action:' + data[i]['action'] + ', trigger:' + data[i]['trigger'] +', duration:' + data[i]['duration'] +
                                ', repeat:' + data[i]['repeat'] + ', numProps:' + data[i]['numProps'] + addProps + '}';
                }else{
                    alarmList += '{alarmNum:' + (parseInt(i) + parseInt(1)) + ', action:' + data[i]['action'] + ', trigger:' + data[i]['trigger'] +', duration:' + data[i]['duration'] +
                                ', repeat:' + data[i]['repeat'] + ', numProps:' + data[i]['numProps'] + addProps + '}, ';
                }
            }
            updateStatus("CalendarView - Alarm List: [" + alarmList + "]");
        }
    });
}

function getProps(filename, prop) {
    $.ajax({
        type: 'GET',
        contentType: 'application/json',
        data: {
            file: filename,
            num: prop
        }, 
        url: '/getProps',
        success: function (data) {
            var propList = '';
            for (i in data) {
                if (parseInt(i) === (parseInt(data['length']) - parseInt(1))) {
                    propList += data[i]['propName'] + ':' + data[i]['propDescr'];
                }else{
                    propList += data[i]['propName'] + ':' + data[i]['propDescr'] + ', ';
                }
            }
            updateStatus("CalendarView - Additional Properties: [{" + propList + "}]");
        }
    });
}