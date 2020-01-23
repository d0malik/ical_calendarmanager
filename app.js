'use strict'

// C library API
const ffi = require('ffi');

let sharedlib = ffi.Library('./libparser', {
  'fileToCalToJSON': [ 'string', [ 'string' ] ],
  'fileToEventsToJSON': [ 'string', [ 'string' ] ],
  'fileToAlarmsToJSON': [ 'string', ['string', 'int'] ],
  'fileToPropsToJSON': [ 'string', ['string', 'int'] ],
  'createCalFromJSON': [ 'string', ['string', 'string', 'string', 'string', 'string', 'string', 'string', 'string'] ],
  'addEventFromJSON': [ 'string', ['string', 'string', 'string', 'string', 'string', 'string', 'string'] ],
  'findOrganizer': [ 'string', ['string', 'int'] ],
  'findLocation': [ 'string', ['string', 'int'] ],
});

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

// MySQL Connection Information
const mysql = require('mysql');
var connection = null;

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
  
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

// On page load endpoint
app.get('/pageload', function(req , res){
  fs.readdir('uploads/', (err, files) => {
    var listJSON = {};
    var i = 0;
    files.forEach(file => {
      var cal_string = sharedlib.fileToCalToJSON('uploads/' + file);
      listJSON[i] = JSON.parse(cal_string);
      listJSON[i]['filename'] = file;
      i++;
    });
    if (err == null) {
      if (i == 0) {
        // console.log("NO FILES ON SERVER");
        res.send('0');
      }else{
        // console.log(listJSON);
        res.send(listJSON);
      }
    }else{
      console.log(err);
      res.send('');
    }
  });
});

app.get('/calToJSON', function(req , res){
    var listJSON = {};
    var cal_string = sharedlib.fileToCalToJSON('uploads/' + req.query.file);
    listJSON = JSON.parse(cal_string);
    // console.log(listJSON);
    res.send(listJSON);
});

app.get('/getFiles', function(req, res) {
  var listJSON = {};
  fs.readdir('uploads/', (err, files) => {
    var i = 0;
    files.forEach(file => {
      listJSON[i] = file;
      i++;
    });
    if (err == null) {
      // console.log(listJSON);
      res.send(listJSON);
    }else{
      console.log(err);
      res.send('');
    }
  });
});

app.get('/getEvents', function(req, res) {
  var listJSON = {};
  var cal_string = sharedlib.fileToEventsToJSON('uploads/' + req.query.filename);
  if (cal_string != "") {
    listJSON = JSON.parse(cal_string);
  }else{
    listJSON = "";
  }
  // console.log(listJSON);
  res.send(listJSON);
});

app.get('/getAlarms', function(req, res) {
  var listJSON = {};
  var cal_string = sharedlib.fileToAlarmsToJSON(('uploads/' + req.query.file), req.query.num);
  if (cal_string != "") {
    listJSON = JSON.parse(cal_string);
  }else{
    listJSON = "";
  }
  // console.log(listJSON);
  res.send(listJSON);
});

app.get('/getProps', function(req, res) {
  var listJSON = {};
  var cal_string = sharedlib.fileToPropsToJSON(('uploads/' + req.query.file), req.query.num);
  if (cal_string != "") {
    listJSON = JSON.parse(cal_string);
  }else{
    listJSON = "";
  }
  // console.log(listJSON);
  res.send(listJSON);
});

app.post('/createCal', function(req, res) {
  var temp = ((JSON.stringify(req.body.cal_info)).split('\\').join(''));
  var temp_1 = temp.substr(1);
  var cal_info = temp_1.slice(0, -1);
  var code = sharedlib.createCalFromJSON(req.body.filename, cal_info, req.body.UID, req.body.dtstart, req.body._dtstart, req.body.dtstamp, req.body._dtstamp, req.body.summary);
  console.log(code);
  res.send(code);
});

app.post('/addEvent', function(req, res) {
  var code = sharedlib.addEventFromJSON(req.body.filename, req.body.UID, req.body.dtstart, req.body._dtstart, req.body.dtstamp, req.body._dtstamp, req.body.summary);
  console.log(code);
  res.send(code);
});

// ************* A4 functionality - MUST be implemented ***************
app.get('/dbConnect', function(req, res) {
  connection = mysql.createConnection({
    host     : 'dursley.socs.uoguelph.ca',
    user     : req.query.user,
    password : req.query.pw,
    database : req.query.db,
  });
  console.log("Attempting MySQL Connection...");
  connection.connect(function(err) {
    if (err) {
      if (err.code === 'ER_ACCESS_DENIED_ERROR') {
        console.log("Invalid credentials entered");
        res.send('-2');
      }else if (err.code === 'ETIMEDOUT') {
        console.log("Connection timed out");
        res.send('-1');
      }
      return;
    }

    let tbl_FILE = `CREATE TABLE IF NOT EXISTS FILE(
      cal_id INT AUTO_INCREMENT PRIMARY KEY,
      file_Name VARCHAR(60) NOT NULL,
      version INT NOT NULL,
      prod_id VARCHAR(256) NOT NULL
    )`;
    connection.query(tbl_FILE, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-3');
      }
    });

    let tbl_EVENT = `CREATE TABLE IF NOT EXISTS EVENT(
      event_id INT AUTO_INCREMENT PRIMARY KEY,
      summary VARCHAR(1024),
      start_time DATETIME NOT NULL,
      location VARCHAR(60),
      organizer VARCHAR(256),
      cal_file INT NOT NULL,
      FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE
    )`;
    connection.query(tbl_EVENT, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-4');
      }
    });

    let tbl_ALARM = `CREATE TABLE IF NOT EXISTS ALARM(
      alarm_id INT AUTO_INCREMENT PRIMARY KEY,
      action VARCHAR(256) NOT NULL,
      \`trigger\` VARCHAR(256) NOT NULL,
      event INT NOT NULL,
      FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE
    )`;
    connection.query(tbl_ALARM, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-5');
      }
    });

    console.log("Connection successful");
    res.send('0');
  });
});

app.get('/dbLogout', function(req, res) {
  if (connection) {
    connection.end();
    connection = null;
    console.log("Connection terminated");
  }
  res.send('0');
});

app.get('/dbStoreAllFiles', function(req, res) {
  var i = 1;
  var z = 1;
  if (connection) {
    fs.readdir('uploads/', (err, files) => {
      files.forEach(file => {
        var cal_string = sharedlib.fileToCalToJSON('uploads/' + file);
        var listJSON = JSON.parse(cal_string);
        if (listJSON['prodID'] !== 'invalid file') {
          var check_query = "SELECT file_Name FROM FILE WHERE file_Name = '" + file + "'";
          connection.query(check_query, function(err, rows, fields) {
            if (err) { console.log("Something went wrong. " + err); }
            else{
              if (!rows[0]) {
                var file_insert_query = "INSERT INTO FILE (file_Name, version, prod_id) VALUES ('" +
                                        file + "'," + parseInt(listJSON['version']) + ",'" +
                                        listJSON['prodID'] + "')";
                connection.query(file_insert_query, function(err, rows, fields) {
                  if (err) { console.log("Something went wrong. " + err); }
                  else {
                    var event_string = (sharedlib.fileToEventsToJSON('uploads/' + file));
                    var eventJSON = JSON.parse(event_string);
                    for (var j = 0; parseInt(j) < parseInt(listJSON['numEvents']); j++) {
                      var sum = eventJSON[j]['summary'];
                      var loc = sharedlib.findLocation(('uploads/' + file), j);
                      var org = sharedlib.findOrganizer(('uploads/' + file), j);
                      if (sum === '') { sum = 'NULL'; }else{ sum = `'` + eventJSON[j]['summary'] + `'`; }
                      if (loc === '') { loc = 'NULL'; }else{ loc = `'` + loc + `'`; }
                      if (org === '') { org = 'NULL'; }else{ org = `'` + org + `'`; }
                      var curr_event = eventJSON[j];
                      var event_insert_query = `INSERT INTO EVENT
                      (summary, start_time, location, organizer, cal_file)
                      VALUES (` + sum + `,'` + eventJSON[j]['startDT']['date'] + eventJSON[j]['startDT']['time'] + `',` +
                      loc + `,` + org + `,` + parseInt(i) + `)`;
                      connection.query(event_insert_query, function(err, rows, fields) {
                        if (err) { console.log("Something went wrong. " + err); }
                        else {
                          for (var k = 0; parseInt(k) < parseInt(curr_event['numAlarms']); k++) {
                            var alarm_string = sharedlib.fileToAlarmsToJSON(('uploads/' + file), 0);
                            var alarmJSON = JSON.parse(alarm_string);
                            var alarm_insert_query = `INSERT INTO ALARM
                            (action, \`trigger\`, event)
                            VALUES ('` + alarmJSON[k]['action'] + `','` + alarmJSON[k]['trigger'] + `',` + parseInt(z) + `)`;
                            connection.query(alarm_insert_query, function(err, rows, fields) {
                              if (err) { console.log("Something went wrong. " + err); }
                            });
                          }
                        }
                        z += 1;
                      });
                    }
                    i += 1;
                  }
                });
              }
            }
          });
        }
      });
      if (err == null) {
        res.send('0');
      }else{
        console.log(err);
        res.send('');
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbClearAllData', function(req, res) {
  if (connection) {
    connection.query(`DELETE FROM ALARM`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-3');
      }
    });
    connection.query(`ALTER TABLE ALARM AUTO_INCREMENT = 1;`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-3');
      }
    });
    connection.query(`DELETE FROM EVENT`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-2');
      }
    });
    connection.query(`ALTER TABLE EVENT AUTO_INCREMENT = 1;`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-2');
      }
    });
    connection.query(`DELETE FROM FILE`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }
    });
    connection.query(`ALTER TABLE FILE AUTO_INCREMENT = 1;`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }
    });
    res.send('0');
  }else{
    res.send('-9');
  }
});

app.get('/dbDisplayStatus', function(req, res) {
  var listJSON = {};
  if (connection) {
    connection.query(`SELECT COUNT(*) AS C FROM FILE`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        listJSON['file'] = results[0]['C'];
        connection.query(`SELECT COUNT(*) AS C FROM EVENT`, function(err, results, fields) {
          if (err) {
            console.log(err.message);
            res.send('-1');
          }else{
            listJSON['event'] = results[0]['C'];
            connection.query(`SELECT COUNT(*) AS C FROM ALARM`, function(err, results, fields) {
              if (err) {
                console.log(err.message);
                res.send('-1');
              }else{
                listJSON['alarm'] = results[0]['C'];
                // console.log(listJSON);
                res.send(listJSON);
              }
            });
          }
        });
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbQuery1', function(req, res) {
  if (connection) {
    connection.query(`SELECT * FROM EVENT ORDER BY start_time`, function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        // console.log(results);
        res.send(results);
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbQuery2', function(req, res) {
  if (connection) {
    connection.query(`SELECT * FROM EVENT e
                      INNER JOIN FILE f ON f.cal_id = e.cal_file
                      WHERE f.file_Name='` + req.query.filename + `';`,
                      function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        // console.log(results);
        res.send(results);
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbQuery3', function(req, res) {
  if (connection) {
    connection.query(`SELECT * FROM EVENT
                      WHERE start_time IN
                      (SELECT start_time FROM EVENT
                      GROUP BY start_time
                      HAVING COUNT(*) > 1);`,
                      function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        // console.log(results);
        res.send(results);
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbQuery4', function(req, res) {
  if (connection) {
    connection.query(`SELECT * FROM EVENT e
                      WHERE e.event_id IN
                      (SELECT event FROM ALARM GROUP BY event HAVING COUNT(*)=1);`,
                      function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        // console.log(results);
        res.send(results);
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbQuery5', function(req, res) {
  if (connection) {
    connection.query(`SELECT * FROM EVENT e
                      INNER JOIN ALARM a ON a.event = e.event_id
                      WHERE e.summary IS NOT NULL`,
                      function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        // console.log(results);
        res.send(results);
      }
    });
  }else{
    res.send('-9');
  }
});

app.get('/dbQuery6', function(req, res) {
  if (connection) {
    connection.query(`SELECT * FROM ALARM a
                      WHERE a.event IN
                      (SELECT event_id FROM EVENT e
                        INNER JOIN FILE f ON f.cal_id = e.cal_file
                        WHERE f.file_Name='` + req.query.filename + `');`,
                      function(err, results, fields) {
      if (err) {
        console.log(err.message);
        res.send('-1');
      }else{
        // console.log(results);
        res.send(results);
      }
    });
  }else{
    res.send('-9');
  }
});
// ********************************************************************

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);