Sample code taken and adapted from various libraries and integrated into one sketch.

• Adafruit_HTU21DF

• Adafruit_BME280

• Adafruit_Sensor (the previous 2 depend on it)

• ClosedCube_OPT3001

Using an Arduino Mega 2560 clone, with an "Arduino Mega Click Shield" [3], and three Click Boards[4][5][6].
A pair of "RAK811 LPWAN Evaluation Boards (EU868)"[1][2] are used to send/receive LoRa P2P packets: one on Serial1 of the Mega, one connected to a computer via USB.

Code displays data from all 3 sensors on the Serial terminal, and sends data from the BME280 only as a LoRa packet.

My setup is pretty unique, but is easily reproducible with similar sensors and boards.

----

[1] https://doc.rakwireless.com/rak811-lora-evaluation-board/overview

[2] https://store.rakwireless.com/products/rak811-lpwan-evaluation-board

[3] https://www.mikroe.com/arduino-mega-click-shield

[4] HTU21D click: https://www.mikroe.com/htu21d-click

[5] Weather Click: https://www.mikroe.com/weather-click

[6] Ambient 2 Click: https://www.mikroe.com/ambient-2-click
