# Gap Dipole antenna tuner firmware

The Gap Dipole is an antenna and remote tuner that allows both changing the length of the dipole in real time, via switched relays, and remote tuning.   The tuner hardware sits at the antenna feed point and is controlled over WiFi.   The ESP32 in the tuner provides a web server which supplies the control page for the tuner.  The control web page (accessed via phone,table, computer, etc.), provides a UI to operate the tuner functions.

*A unique feature of the Gap dipole is that it is characterized once with a VNA, and that characterization is uploaded to the tuner over WiFi.  After that, when the user switches frequency that freqency is given to the tuner and it tunes the antenna to the best possible match based on the VNA characterizaton, without the traditional need to transmit a signal while the tuner tries various tuning network combinations.*

We are currently using the esp32-s3-devkit-c-1-n16r8v board.  There is a esp32-s3-devkit-c-1-n16r8v.json file here.  Copy that to ~/.platformio/platforms/espressif32/boards/  if you are using platformio.

Current Web UI image:

![image](https://github.com/user-attachments/assets/86f151cd-c9a9-4343-a31c-9c03c372dd1f)

