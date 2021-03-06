#include <WiFi.h>

#define POWER_SW 22
#define STATUS_LED 23
#define ADDRESS 110

const char* ssid     = "";
const char* password = "";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);

  pinMode(POWER_SW, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(POWER_SW, LOW);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(STATUS_LED, HIGH);
    delay(250);
    digitalWrite(STATUS_LED, LOW);
    delay(250);
    Serial.print(WiFi.status());
  }

  WiFi.config(IPAddress(192,168,1,ADDRESS), WiFi.gatewayIP(), WiFi.subnetMask());
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(STATUS_LED, HIGH);

  server.begin();

}

int value = 0;

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/ON\">here</a> to turn the PC on.<br>");
            client.print("Click <a href=\"/OFF\">here</a> to turn the PC off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /ON")) {
          digitalWrite(POWER_SW, HIGH);               // GET /H turns the LED on
          delay(500);
          digitalWrite(POWER_SW, LOW);
        }
        if (currentLine.endsWith("GET /OFF")) {
          digitalWrite(POWER_SW, HIGH);
          delay(5000);
          digitalWrite(POWER_SW, LOW);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
