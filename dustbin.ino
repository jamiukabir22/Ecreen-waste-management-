// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Define HC-SR04 sensor pins
const int trigPin = 27; // Trigger pin
const int echoPin = 26; // Echo pin

// Threshold to consider the dustbin full (in centimeters)
const float fullThreshold = 20.0; // Adjust this value as needed

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Function to read the distance from HC-SR04 sensor
float readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  // Calculate distance in centimeters
  float distance = (duration / 2.0) * 0.0343; // Speed of sound in cm/us

  return distance;
}

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

// Replaces placeholder with dustbin full status
String processor(const String& var){
  if(var == "DUSTBIN_FULL"){
    float distance = readDistance();
    return String(distance);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request){
    float distance = readDistance();
    request->send_P(200, "text/plain", String(distance).c_str());
  });

  // Start server
  server.begin();
}

void loop(){
  // Add any additional code you need to run in the loop here
}
