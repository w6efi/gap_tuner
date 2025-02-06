# Gap Dipole antenna tuner firmware

The Gap Dipole is an antenna and remote tuner that allows both changing the length of the dipole in real time, via switched relays, and remote tuning.  The tuner hardware sits at the antenna feed point and is controlled over WiFi.   The ESP32 in the tuner provides a web server which supplies the control page for the tuner.  The control web page (accessed via phone,table, computer, etc.), provides a UI to operate the tuner functions.

We are currently using the esp32-s3-devkit-c-1-n16r8v board.  There is a esp32-s3-devkit-c-1-n16r8v.json file here.  Copy that to ~/.platformio/platforms/espressif32/boards/  if you are using platformio.
