// Neopixel
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>

const char* ssid = "network";
const char* password = "pass";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

// Neopixel Config
#define NeoPIN D4
#define NUM_LEDS 30
int brightness = 150;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NeoPIN, NEO_RGB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);

  Serial.println();
  strip.setBrightness(brightness);
  strip.begin();
  strip.show();
  delay(50);
  Serial.println("NeoPixel started");

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 254);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait for connection
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  //Match the request
  if (request.indexOf("/colorUpdate/") != -1) {

    String color_update = request;
    color_update.replace("/colorUpdate/", "");
    color_update.replace("GET ", "");
    color_update.replace(" HTTP/1.1", "");

    if (color_update != "") {
      setNeoColor(color_update);
    }

    Serial.print("Color is " + color_update);
  }

  // Return the response, build the html page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one

  client.print("<!DOCTYPE html><html lang='en'> <head> <meta charset='utf-8'> <link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css' integrity='sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO' crossorigin='anonymous'> <script src='https://code.jquery.com/jquery-3.3.1.slim.min.js' integrity='sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo' crossorigin='anonymous'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js' integrity='sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49' crossorigin='anonymous'></script> <script src='https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js' integrity='sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy' crossorigin='anonymous'></script> <script src='https://cdn.jsdelivr.net/npm/farbstastic@1.3.0/farbtastic.min.js'></script> <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/farbstastic@1.3.0/farbtastic.css'> <script type=text/javascript> $(document).ready(function(e){$('#colorpicker').farbtastic(function(color){applyColor(color);});}); function applyColor(color){console.log(color); $('.jscolor').val(color); $('.jscolor').css('background-color', color); var rgb=hexToRgb(color); if (color){var color_clean=color.replace('#',''); $('#transfer').attr('href', '/colorUpdate/' + color_clean);}}function hexToRgb(hex){var result=/^#?([a-fd]{2})([a-fd]{2})([a-fd]{2})$/i.exec(hex); return result ?{r: parseInt(result[1], 16), g: parseInt(result[2], 16), b: parseInt(result[3], 16)}: null;}</script> </head> <style>#heading{text-align: center;}#colorPicker{width: 100%; height: 100px; font-size: 20px; text-align: center;}#transfer{width: 100%; height: 50px; font-size: 25px;}.alert{text-align: center;}.picker{margin-top: 10px; margin-left: auto; margin-right: auto;}.container{width: 100%;}</style> <body> <div class='container'> <form> <div class='form-group'> <h1 class='display-4' id='heading'>RGB COLOR STRIP</h1> <div class='alert alert-primary' role='alert'> Select RGB color below to transfer it onto strip </div><div class='form-group'> <div class='input-group'> <input class='jscolor' id='colorPicker' value=''> </div><div class='input-group'> <div id='colorpicker' class='picker'></div></div></div><div class='input-group'> <a href='' id='transfer' class='btn btn-primary'>Transfer</a> </div></div></form> </div></body></html>");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}


void setNeoColor(String value) {
  Serial.print("Setting Neopixel...");

  // splitting into three parts
   long number = (long) strtol( &value[0], NULL, 16);
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

  // setting whole strip to the given color
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(  g, r, b ) );
  }
  // init
  strip.show();
  Serial.println("on.");
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print("\"" + WiFi.SSID(thisNet) + "\"");
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
  }
}
