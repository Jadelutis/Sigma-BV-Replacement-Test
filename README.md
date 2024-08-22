Sigma DBV-145 Replacement with ESP32:

All of the needed information is covered in the code comments:
  - Pin on Board | Signal | Pin on BV Connector
  - 1  BUSY  3
  - 2  ACK  4
  - 4  ENABLE  10
  - 6  VEND1  11
  - 8  VEND2  12
  - 10  VEND3  13

Currently the loop is configured to insert bills denomination $1, $5, $10, $20 on repeat I was unable to get it to take a $50 or $100, I think it is not supported

I haven't been able to overcome the 100 credit limit yet. At/Over 100 credits, it disables the BV and won't listen to any communications from it.

This code should be able to be run on any arduino compatible board. Currently configured for WEMOS Lolin S2 Mini you can edit the platformio.ini to change the board, or copy and paste the CPP file into ArduinoIDE
