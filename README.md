# TeeRec - Teensy recorder

Libraries for recording analog input data on [Teensy](https://www.pjrc.com/teensy/) 3.2/3.5/3.6 microcontrollers.

Based on Pedro Villanueva's [ADC](https://github.com/pedvide/ADC)
library, Paul Stoffregen's
[DMAChannel](https://github.com/PaulStoffregen/cores/tree/master/teensy3),
Bill Greiman's [SdFat](https://github.com/greiman/SdFat) library, and
on Stefan Mucha's first
[EOD logger sketch](https://github.com/muchaste/EOD-Logger/tree/master/eodlogger_v1)
with contributions by
[Lydia Federman](https://github.com/LydiaFe/EOD-Logger)
(simultaneous recordings from both ADCs) and
[Sebastian Volkmer](https://github.com/SebastianVol/EOD-Logger/blob/master/eodlogger_2channel_barebone/eodlogger_8channel.ino)
(multiplexing).


## Features

- Flexible producer/consumer data model based on a single multiplexed circular buffer.
- Continuous DMA-based data acquisition.
- Single channel or multiplexed acquisition from multiple channels, from one or both ADCs.
- Highspeed timed acquisition up to 500kHz.
- Conversion of data to signed 16bit for direct storage into wave files.
- Continuous storing recorded data as wave files on SD cards.
- Detailed metadata in wave file header: sampling rate, number of
  channels and pin IDs, bit resolution, date and time, Teensy board
  version, and its unique MAC address..
- Display recorded data on a monitor.
- Generate test signals.
- React to push buttons.
- Feedback by blinking the LED.
- Configurable via config file on SD card.

For slow acquisition of environmental sensor readings see [TeeSense
library](https://github.com/janscience/TeeSense).


## Libraries

The features provided by TeeRec are provided by many C++ classes,
defined in the following libraries:

### Data acquisition

- [DataBuffer](src/DataBuffer.h): A single cyclic, multiplexed buffer holding acquired data.
- [DataWorker](src/DataWorker.h): Producer/consumer working on a DataBuffer.
- [ContinousADC](src/ContinuousADC.h): Sample from multiple pins into a DataBuffer.
- [SDWrite](src/SDWrite.h): Write data from a DataWorker to SD card.
- [WaveHeader](src/WaveHeader.h): Setting up wave file header with metadata.

### Configuration

- [Configurable](src/Configurable.h): Base class for all configurable classes.
- [Settings](src/Settings.h): Common configurable settings (file name, path, etc.)
- [Configurator](src/Configuration.h): Configure configurable class instances from a configuration file.

### Utilities

- [Display](src/Display.h): Display data on a monitor.
- [RTClock](src/RTClock.h): Time and date strings from the real time clock.
- [Blink](src/Blink.h): Blinking LEDs.
- [PushButtons](src/PushButtons.h): Manage and query push buttons with callback functions.
- [TestSignals](src/TestSignals.h): Generate test signals on pulse-width modulation and DAC pins.
- [TeensyBoard](src/TeensyBoard.h): Find out which Teensy board we are running on.


## Examples

In [examples/](examples) you find sketches demonstrating the use of
the TeeRec libraries.

### Data acquisition

These examples could be used as the basis for you data acquisition application.

- [scope](examples/scope): Show acquired data on a display.
- [logger](examples/logger): Continuously store data on SD card.
- [recorder](examples/recorder): Show acquired data on a display and store data on SD card upon user request.

### Testing data acquisition

Useful sketches for checking out the performance of the data acquisition.

- [maxrate](examples/maxrate): Test for maximum possible sampling rate.
- [averaging](examples/averaging): Test various averaging settings for acquisition.

### Utilities

Some useful utilities.

- [sinegen](examples/sinegen): Sine-wave generator.
- [teensyboard](examples/teensyboard): Print Teensy board version, serial number and MAC adress.
- [writeconfig](examples/writeconfig): Write a default configuration file on SD card.
- [removefiles](examples/removefiles): Remove all files from a directory on SD card.

### Demos

Demonstrate the usage of some of the provided libraries.

- [temperature](examples/temperature): Read temperature from 1-wire device.
- [blink](examples/blink): Demonstrate usage of Blink class.
- [pushbuttons](examples/pushbuttons): Demonstrate usage of PushButtons class.


## Utilities

In [utils/](utils) you find some useful python scripts.

- [viewwave](utils/viewwave): display the traces in a wave file.
- [continuity](utils/continuity): check whether pulse signals recorded into wave file have consistent periods overmany wave files.
- [cycles](utils/cycles): plot failures in pulse traces? - needs update.
- [noise](utils/noise): plot and analyse baseline noise levels from wave files.
- [spectra](utils/spectra): power spectra of traces in wave files.


## Dependencies

TeeRec is based on the following libraries:

- [Arduino Time Library](https://github.com/PaulStoffregen/Time)
- [ADC](https://github.com/pedvide/ADC)
- [SdFat version2](https://github.com/greiman/SdFat)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)

The Display library currently uses:

- [Adafruit-GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library)
- [Adafruit ILI9341 Arduino Library](https://github.com/adafruit/Adafruit_ILI9341)


The [Arduino Time Library](https://github.com/PaulStoffregen/Time),
[ADC](https://github.com/pedvide/ADC), [SdFat
version2](https://github.com/greiman/SdFat), and
[Bounce2](https://github.com/thomasfredericks/Bounce2) libraries are
already included in
[Teensyduino](https://www.pjrc.com/teensy/teensyduino.html). When
installing Teensyduino make sure you selected them.

_Note_: you need Arduino 1.8.19 and Teensyduoino 1.56 or higher! When
installing Teensyduoino simply select all libraries for installation.
Remove `SdFat` and `Bounce2` from your `Arduino/libraries` folder,
they are now supplied by Teensyduoino.

For installing the libraries for the environmental sensors and the TFT
monitor, open in the Arduino IDE: Tools - Manage libraries. Search for
the libraries and install them.


## Installation

Clone the [TeeRec](https://github.com/janscience/TeeRec) repository
directly into 'Arduino/libraries':
```sh
cd Arduino/libraries
git clone https://github.com/janscience/TeeRec.git
```

Alternatively, download the whole repository as a zip archive (open
https://github.com/janscience/TeeRec in your browser and click on the
green "Code" button). Unpack the zip file:
```sh
cd Arduino/libraries
unzip ~/Downloads/TeeRec-main.zip
```

If you want to edit the TeeRec files, mark the library as developmental:
```sh
cd Arduino/libraries/TeeRec
touch .development
```

Close the Arduino IDE and open it again. Then the Arduino IDE knows
about the TeeRec library and its examples.


## Applications of the TeeRec libraries

TeeRec is used in:

- [EOD-Logger](https://github.com/muchaste/EOD-Logger): A 2-electrode
  logger for recording electric-organ discharges of electric fish.

- [TeeGrid](https://github.com/janscience/TeeGrid): Electrode arrays
  based on 8-16channel recording devices for recording electric fish
  behavior in natural habitats (see [Henninger et
  al. 2018](https://doi.org/10.1523/JNEUROSCI.0350-18.2018) and
  [Henninger et al. 2020](https://doi.org/10.1242/jeb.206342)).

Further planed applications are:

- Fishfinder: A replacement for the usual and horrible RadioShack/MP3
  player combination for recording electric fish EODs in their natural
  habitats with lot's of additional features (display, online
  analysis, environmental parameters).


## Links

- [Audio](https://github.com/PaulStoffregen/Audio) library
- [microSoundRecorder](https://github.com/WMXZ-EU/microSoundRecorder) - Environmental Sound Recorder for Teensy 3.6:
  [[github]](https://github.com/WMXZ-EU/microSoundRecorder)
  [[wiki]](https://github.com/WMXZ-EU/microSoundRecorder/wiki/Hardware-setup)
  [[forum]](https://forum.pjrc.com/threads/52175?p=185386&viewfull=1#post185386)