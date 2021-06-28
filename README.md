# TeeRec - Teensy recorder

Libraries for recording analog input data on [Teensy](https://www.pjrc.com/teensy/) 3.5 microcontroller.

Based on Pedro Villanueva's [ADC](https://github.com/pedvide/ADC) library, 
Paul Stoffregen's [DMAChannel](https://github.com/PaulStoffregen/cores/tree/master/teensy3),
Bill Greiman's [SdFat](https://github.com/greiman/SdFat) library, and on
Stefan Mucha's first [EOD logger sketch](https://github.com/muchaste/EOD-Logger/tree/master/eodlogger_v1) with contributions by [Lydia Federman](https://github.com/LydiaFe/EOD-Logger) and [Sebastian Volkmer](https://github.com/SebastianVol/EOD-Logger/blob/master/eodlogger_2channel_barebone/eodlogger_8channel.ino) (multiplexing support).


## Features

- Continuous DMA-based data acquisition into a single multiplexed circular buffer.
- Single channel or multiplexed acquisition from multiple channels, from one or both ADCs.
- Highspeed timed acquisition up to 500kHz.
- Conversion of data to signed 16bit for direct storage into WAV files.
- Detailed metadta in wave file header: sampling rate, number of
  channels and pin IDs, bit resolution, data and time, Teensy board
  version and its unique MAC address..
- Storing recorded data as wave files on SD cards.
- Display recorded data on a monitor.
- Generate test signals.
- React to push buttons.
- Feedback by blinking the LED.


## Libraries

- ContinousADC: Sample from multiple pins into a single cyclic buffer.
- SDWrite: Write data on SD card.
- WaveHeader: Setting up wave file header with metadata.
- Display: Display data on a monitor.
- RTClock: Time and date strings from the real time clock.
- Blink: Blinking LEDs.
- PushButtons: Manage and query push buttons with callback functions.
- TestSignals: Generate test signals on pulse-width modulation pins.
- TeensyBoard: Find out which Teensy board we are running on.


## Examples

- scope: Show acquired data on a display.
- logger: Continuously store data on SD card.
- recorder: Show acquired data on a display and store data on SD card upon user request.
- averaging: Test various averaging settings for acquisition.
- removefiles: Remove all files from a directory on SD card.
- blink: Demonstrate usage of Blink class.
- pushbuttons: Demonstrate usage of PushButtons class.
