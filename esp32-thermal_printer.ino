/**
 * Copyright 2017 Dan Oprescu
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 *     
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

HardwareSerial serialPrinter(1);

#include <WiFi.h>
#include <ESP32WebServer.h>
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

ESP32WebServer server(80);

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);


void setup() {
  Serial.begin(500000);

  display.init();
  display.drawString(0, 0, "Starting...");
  display.display();
  

  serialPrinter.begin(19200, SERIAL_8N1, 25, 26);

  

  display.drawString(0, 10, "Connecting to WiFi...");
  display.display();
  WiFi.begin("SSID", "password");
  /*wait until ESP32 connect to WiFi*/
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  display.drawString(0, 20, "\nWiFi connected, IP address: "); 
  display.drawString(0, 30, WiFi.localIP().toString());
  display.display();


  server.onNotFound(handleNotFound);
  server.on("/", menu);
  server.on("/print", sendToPrinter);
  
  
  server.begin();
  display.drawString(0, 40, "HTTP started on port 80");
  display.display();
}



void loop() {
  // deals with the Wifi clients and responds, calls the callbacks, etc. ...
  server.handleClient();
  delay(1);
}



void menu() {
  Serial.println("Menu...");
  String message = "<h1>Thermal Printer</h1>";
  message += "<form action=\"/print\" id=\"frm\"><input type=\"submit\" value=\"Send to printer\"  style=\"height:200px;width:200px\"> </form>";
  message += "<textarea rows=\"10\" cols=\"50\" name=\"text\" form=\"frm\"> Enter text here...</textarea>";
  server.send(200, "text/html", message);
}

void sendToPrinter() {
  // we expect one and only 1 var which is the text to print
  String var = server.argName(0);
  String value = server.arg(0);


  if (var == "text") {
    Serial.print("Received to print: "); Serial.println(value);
    serialPrinter.println(value);
    cutPaper();
    
  } 
  else Serial.println("UNKNOWN var " + var );

  menu();
}


void cutPaper() {
  //extra empty rows to cut below the latest text
  serialPrinter.println("\n\n\n");
  byte cutCmd[] = {0x1B, 0x69, 0x0A};
  serialPrinter.write(cutCmd, sizeof(cutCmd));
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri();
  message += "\nMethod: " + (server.method() == HTTP_GET)?" GET":" POST";
  message += "\nArguments: " + server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



