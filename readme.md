ikea tradfri gateway client for esp32

hello
if u have trouble setting it up
folow the readme here
https://github.com/Friedmonkey/ESP32-COAP-CLIENT

the menu config to set up the wifi and other settings
url and psk doesnt matter cus they're defined in code

u just need the wifi settings and some coap settings i think

u also need 2 of my libraries

fdata library for working with generic objects and json
https://github.com/Friedmonkey/fdata-esp-idf-esp32-CYD-

fried_coaps_client library for secure CoAPs to talk with the ikea tradfri gateway
https://github.com/Friedmonkey/fried-coaps-client-esp-idf-esp32-CYD-


personally i use the structure
documents/esp-projects/tradfri_controller/CmakeLists.txt
documents/esp-projects/.libraries/fdata/CmakeLists.txt
documents/esp-projects/.libraries/fried_coaps_client/CmakeLists.txt


then use esp idf 5.5
and inside the folder do
idf.py menuconfig and follow the earlier linked repo's readme

then do
idf.py flash -p {PORT} monitor

for me i use COM5 as {PORT}


if u REALLY need more help make an issue and perhaps i can help u if i see it
