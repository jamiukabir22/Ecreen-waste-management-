// webserver.h

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESPAsyncWebServer.h>
#include "distance_sensor.h" // Include the header for reading distance

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// HTML content served by the web server
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dustbin-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Dustbin Level Monitor</h2>
  <p>
    <i class="fas fa-trash" style="color:#ff6347;"></i> 
    <span class="dustbin-labels">Dustbin Full</span> 
    <span id="dustbin_full">%DUSTBIN_FULL%</span>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var isFull = parseFloat(this.responseText) <= 20.0; // Adjust threshold as needed
      document.getElementById("dustbin_full").innerHTML = isFull ? "Yes" : "No";
    }
  };
  xhttp.open("GET", "/distance", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with the dustbin full status
String processor(const String& var){
  if(var == "DUSTBIN_FULL"){
    float distance = readDistance();
    return String(distance);
  }
  return String();
}

// Function to set up the web server
void setupWebServer() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Route to get the current distance
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request){
    float distance = readDistance();
    request->send_P(200, "text/plain", String(distance).c_str());
  });

  // Start server
  server.begin();
}

#endif // WEBSERVER
