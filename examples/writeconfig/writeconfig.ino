// select a data source:
//#define INPUT_ADC     // data are recorded from Teensy internal ADCs
#define INPUT_TDM     // data are recorded by TI PCM186x chip via TDM

// select SD card:
#define SDCARD_BUILTIN
//#define SDCARD_SPI0
//#define SDCARD_SPI1

#include <RTClock.h>
#include <SDCard.h>
#include <Configurator.h>
#include <ToolActions.h>
#include <ToolMenus.h>
#include <Settings.h>
#if defined(INPUT_ADC)
  #include <InputADC.h>
  #include <InputADCSettings.h>
#elif defined(INPUT_TDM)
  #include <InputTDMSettings.h>
#endif
#include <Blink.h>
#include <TeensyBoard.h>
#include <TeeRecBanner.h>

// Adapt the following parameter values to your needs:

#define CFG_FILE        "teerec.cfg"   // name of configuration file

// Settings:
#define PATH            "recordings"   // folder where to store the recordings
#define DEVICEID        0                 // device identifier
#define FILENAME        "recID-NUM4.wav"  // may include ID, IDA, DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
#define FILE_SAVE_TIME  10   // seconds
#define RANDOM_BLINKS   false
#define INITIAL_DELAY   10.0  // seconds
#define PULSE_FREQUENCY 230   // Hertz
#define DISPLAY_TIME  0.005   // seconds
#define SENSORS_TIME   10.0   // seconds

// Input Settings:
#define SAMPLING_RATE  48000 // samples per second and channel in Hertz
#if defined(INPUT_ADC)
  #define BITS           12    // resolution: 10bit 12bit, or 16bit
  #define AVERAGING       8    // number of averages per sample: 0, 4, 8, 16, 32
  #define CONVERSION     ADC_CONVERSION_SPEED::HIGH_SPEED
  #define SAMPLING       ADC_SAMPLING_SPEED::HIGH_SPEED
  #define REFERENCE      ADC_REFERENCE::REF_3V3
#elif defined(INPUT_TDM)
  #define GAIN           20.0  // dB
#endif
  #define PREGAIN        1.0


RTClock rtclock;
SDCard sdcard;

Configurator config;
Settings settings(PATH, DEVICEID, FILENAME, FILE_SAVE_TIME, INITIAL_DELAY,
                  RANDOM_BLINKS, PULSE_FREQUENCY, DISPLAY_TIME, SENSORS_TIME);
#if defined(INPUT_ADC)
InputADCSettings aisettings(SAMPLING_RATE, BITS, AVERAGING,
		  	    CONVERSION, SAMPLING, REFERENCE, PREGAIN);
#elif defined(INPUT_TDM)
InputTDMSettings aisettings(SAMPLING_RATE, 8, false, GAIN, PREGAIN);
#endif
DateTimeMenu datetime_menu(rtclock);
ConfigurationMenu configuration_menu(sdcard);
SDCardMenu sdcard0_menu(sdcard, settings);
#ifdef FIRMWARE_UPDATE
FirmwareMenu firmware_menu(sdcard);
#endif
DiagnosticMenu diagnostic_menu("Diagnostics", sdcard);
HelpAction help_act(config, "Help");

Blink blink(LED_BUILTIN);


void setup() {
  // enable parameter that also should go into the configuration file:
  settings.enable("InitialDelay");
  settings.enable("RandomBlinks");
  settings.enable("PulseFreq");
  settings.enable("DisplayTime");
  settings.enable("SensorsInterval");
  aisettings.enable("Pregain");

  Serial.begin(9600);
  while (!Serial && millis() < 2000) {};
  printTeeRecBanner();
  blink.switchOn();
#if defined(SDCARD_BUILTIN)
  sdcard.begin();
#elif defined(SDCARD_SPI0)
  pinMode(10, OUTPUT);
  SPI.begin();
  sdcard.begin(10, DEDICATED_SPI, 40, &SPI);
#elif defined(SDCARD_SPI1)
  pinMode(0, OUTPUT);
  SPI.begin();
  sdcard.begin(0, DEDICATED_SPI, 40, &SPI1);
#endif
  sdcard.check();
  config.setConfigFile(CFG_FILE);
  config.load(sdcard);
  if (Serial)
    config.configure(Serial, 10000);
  config.report();
  Serial.println();
  blink.switchOff();
  sdcard.end();
  halt();   // from TeensyBoard.h
}


void loop() {
  delay(100);
}
