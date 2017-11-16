# esp32-thermal_printer
Hack an Epson TM-T88 thermal printer (the ones that print receipts in a supermarket till) and remotely control it through a WiFi enabled ESP32.

This code :
- connects to a given WiFi network
- starts a HTTP server on port 80
- serves a basic page that allows one to type some text in a text area
- once a button is pressed it sends that text through a serial link to the thermal printer and tells it to cut the paper at the end
- also the setup steps are printed on the integrated OLED display
