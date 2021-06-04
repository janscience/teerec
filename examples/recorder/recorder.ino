#include <ContinuousADC.h>
#include <SDWriter.h>
#include <Display.h>
#include <RTClock.h>
#include <TestSignals.h>


// Settings: --------------------------------------------------------------------------------

int bits = 12;                     // resolution: 10bit 12bit, or 16bit 
uint32_t samplingRate = 100000;    // samples per second and channel in Hertz
int8_t channels0 [] =  {A2, A3, A4, A5, -1, A6, A7, A8, A9};      // input pins for ADC0
int8_t channels1 [] =  {-1, A16, A17, A18, A19, A20, A22, A10, A11};  // input pins for ADC1

uint updateScreen = 500;           // milliseconds
float displayTime = 0.005;
//float displayTime = 0.001*updateScreen;

bool logging = false;              // keep saving to files
char fileName[] = "SDATELNUM.wav"; // may include DATE, SDATE, TIME, STIME, DATETIME, SDATETIME, ANUM, NUM
float fileSaveTime = 10;

int startPin = 24;

int stimulusFrequency = 500;       // Hertz


// ------------------------------------------------------------------------------------------
 
ContinuousADC aidata;

SDWriter file;

Display screen(1);
int n_plots = 1;
elapsedMillis screenTime;

RTClock rtclock;


void setupADC() {
  aidata.setChannels(0, channels0);
  aidata.setChannels(1, channels1);
  aidata.setRate(samplingRate);
  aidata.setResolution(bits);
  aidata.setMaxFileTime(fileSaveTime);
  aidata.check();
  Serial.printf("buffer time: %.3fs\n", aidata.bufferTime());
}


void openNextFile() {
  String name = rtclock.makeStr(fileName, true);
  name = file.incrementFileName(name);
  if (name.length() == 0 )
    return;
  file.setupWaveHeader(aidata);
  file.openWave(name.c_str());
  screen.pushText(0, name.c_str());
  screen.writeText(1, "00:00");
  Serial.println(name);
}


void setupStorage() {
  if (file.available())
    file.dataDir("recordings");
  file.setWriteInterval(aidata);
}


void setupScreen() {
  screen.setTextArea(0, 0.0, 0.8, 0.65, 1.0);
  char msg[30];
  aidata.settingsStr(msg);
  screen.pushText(0, msg);
  screen.setTextArea(1, 0.71, 0.8, 1.0, 1.0);
  n_plots = aidata.nchannels();
  if (n_plots > 8)
    n_plots = 8;
  float h = 0.8/n_plots;
  for (int k=0; k<n_plots; k++)
    screen.setDataArea(n_plots-k-1, 0.0, k*h, 1.0, (k+0.9)*h);
}


void setupInput() {
  if (startPin >= 0)
    pinMode(startPin, INPUT_PULLUP);
}


void plotData() {
  if (screenTime > updateScreen) {
    screenTime -= updateScreen;
    screen.scrollText(0);
    /*
    char ts[20];
    rtclock.time(ts);
    screen.writeText(0, ts);
    */
    screen.clearData();
    size_t n = aidata.frames(displayTime);
    float data[n];
    size_t start = aidata.currentSample(n);
    for (int k=0; k<aidata.nchannels(); k++) {
      aidata.getData(k, start, data, n);
      screen.plotData(k%n_plots, data, n, k/n_plots);
    }
  }
}


void writeData() {
  aidata.writeData(file.file());
  char ts[6];
  aidata.fileTimeStr(ts);
  screen.writeText(1, ts);
}


void storeData() {
  if (startPin >= 0) {
    int push = digitalRead(startPin);
    if (push == 0 && file.available() && !file.isOpen()) {
      openNextFile();
      aidata.setMaxFileSamples(0);
      aidata.startWrite();
    }
    if (push == 1)
      aidata.setMaxFileTime(fileSaveTime);
  }
  if (file.needToWrite()) {
    writeData();
    if (aidata.endWrite()) {
      file.closeWave();
      screen.popText(0);
      screen.clearText(1);
      if (logging)
        openNextFile();
    }
  }
}


// ------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(100);
  rtclock.check();
  setupTestSignals(2, 6, stimulusFrequency);
  setupInput();
  setupADC();
  setupScreen();
  setupStorage();
  screenTime = 0;
  aidata.start();
}


void loop() {
  plotData();
  storeData();
} 
