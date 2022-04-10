#include <Sensors.h>


Sensors::Sensors() :
  Configurable("Sensors"),
  NSensors(0),
  MaxDelay(0),
  Interval(10000),
  Time(0),
  State(0),
  DF(),
  Header(0),
  Data(0),
  NData(0),
  MData(0),
  RTC(0) {
  UseInterval = Interval;
}


Sensors::Sensors(RTClock &rtc) :
  Sensors() {
  RTC = &rtc;
}


Sensors::~Sensors() {
  if (Header != 0)
    delete [] Header;
  Header = 0;
  if (Data != 0)
    delete [] Data;
  Data = 0;
  NData = 0;
  MData = 0;
}


void Sensors::addSensor(Sensor &sensor) {
  if (NSensors >= MaxSensors) {
    Serial.println("Maximum number of supported sensors exceeded!");
    return;
  }
  Snsrs[NSensors++] = &sensor;
}


float Sensors::interval() const {
  return 0.001*Interval;
}


void Sensors::setInterval(float interval) {
  Interval = (unsigned long)(1000.0*interval);
}


void Sensors::report() {
  char ds[2] = {'\0', '\0'};
  if (NSensors > 1)
    ds[1] = 's';
  Serial.printf("Sensors: %d device%s, read every %gs\n",
		NSensors, ds, 0.001*Interval);
  int n = 0;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      Serial.printf("  ");
      Snsrs[k]->report();
      n++;
    }
  }
  if (n == 0)
    Serial.println("  no sensors avilable!");
}


void Sensors::start() {
  MaxDelay = 0;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available() && Snsrs[k]->delay() > MaxDelay)
      MaxDelay = Snsrs[k]->delay();
  }
  UseInterval = Interval;
  if (UseInterval < 2*MaxDelay)
    UseInterval = 2*MaxDelay;
  Time = UseInterval - MaxDelay;
  State = 0;
  if (Data != 0)
    *Data = '\0';
}


bool Sensors::update() {
  switch (State) {
  case 0: if (Time > UseInterval - MaxDelay) {
      for (uint8_t k=0; k<NSensors; k++)
	Snsrs[k]->request();
      State = 1;
    }
    break;
  case 1: if (Time > UseInterval) {
      for (uint8_t k=0; k<NSensors; k++)
	Snsrs[k]->read();
      makeCSVData();
      State = 0;
      Time -= UseInterval;
      return true;
    }
    break;
  }
  return false;
}


bool Sensors::pending() {
  if (DF && Data != 0)
    return (strlen(Data) > 0 && !DF.isBusy());
  return false;
}


void Sensors::print() {
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      Serial.printf("%s = %5.2f%s\n", Snsrs[k]->name(),
		    Snsrs[k]->value(), Snsrs[k]->unit());
    else
      Serial.printf("%s not available\n", Snsrs[k]->name());
  }
}


void Sensors::printHeader() {
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      Serial.printf("%s/%s\t", Snsrs[k]->name(), Snsrs[k]->unit());
  }
  Serial.println();
}


void Sensors::printValues() {
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      Serial.printf("%5.2f\t", Snsrs[k]->value());
  }
  Serial.println();
}


void Sensors::setRTClock(RTClock &rtc) {
  RTC = &rtc;
}


void Sensors::makeCSVHeader() {
  if (Header != 0)
    delete [] Header;
  if (Data != 0)
    delete [] Data;
  Data = 0;
  NData = 0;
  MData = 0;
  // size of header and data line:
  size_t m = 0;
  size_t n = 5;
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      n += strlen(Snsrs[k]->name()) + strlen(Snsrs[k]->unit()) + 2;
      m++;
    }
  }
  if (m == 0) {
    // no sensors:
    Header = new char[1];
    *Header = '\0';
    return;
  }
  // compose header line:
  Header = new char[n];
  char *hp = Header;
  hp += sprintf(hp, "time,");
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available())
      hp += sprintf(hp, "%s/%s,", Snsrs[k]->name(), Snsrs[k]->unit());
  }
  *(--hp) = '\n';
  // allocate data line:
  MData = 20 + m*10;
  NData = (100*MData/512+1)*512;   // store up to about 100 lines
  Data = new char[NData];
  *Data = '\0';
}


void Sensors::makeCSVData() {
  if (Data == 0 || NData == 0)
    return;
  if (strlen(Data) > NData - MData) {
    // buffer overflow!
    return;
  }
  // get time:
  char ts[20];
  if (RTC != 0)
    RTC->dateTime(ts);
  else
    ts[0] = '\0';
  // compose data line:
  char *sp = Data + strlen(Data);
  sp += sprintf(sp, "%s,", ts);
  for (uint8_t k=0; k<NSensors; k++) {
    if (Snsrs[k]->available()) {
      sp += Snsrs[k]->print(sp);
      *(++sp) = ',';
    }
  }
  *(--sp) = '\n';
  *(++sp) = '\0';
}


bool Sensors::openCSV(SDCard &sd, const char *path, bool append) {
  if (DF)
    closeCSV();
  if (Header == 0)
    makeCSVHeader();
  if (*Header == '\0') {
    // no sensors:
    return false;
  }
  // create file and write header:
  char fpath[strlen(path)+10];
  strcpy(fpath, path);
  strcat(fpath, ".csv");
  if (append && sd.exists(fpath))
    DF = sd.openAppend(fpath);
  else
    DF = sd.openWrite(fpath);
  if (DF) {
    DF.write(Header, strlen(Header));
    DF.flush();
    return true;
  }
  else
    return false;
}


bool Sensors::writeCSV() {
  if (Data == 0 || *Data == '\0' || !DF)
    return false;
  // write data:
  bool success = true;
  size_t n = DF.write(Data, strlen(Data));
  if (n < strlen(Data))
    success = false;
  DF.flush();
  *Data = '\0';        // clear buffer
  if (!DF)
    success = false;
  return success;
}


bool Sensors::closeCSV() {
  if (!DF)
    return false;
  return (!DF.close());
}


void Sensors::configure(const char *key, const char *val) {
  bool found = true;
  char pval[30];
  if (strcmp(key, "writeinterval") == 0) {
    setInterval(parseTime(val));
    sprintf(pval, "%gs", 0.001*Interval);
    Serial.printf("  set Sensors-%s to %s\n", key, pval);
  }
  else {
    found = false;
    for (uint8_t k=0; k<NSensors; k++) {
      if (Snsrs[k]->configure(key, val)) {
	Snsrs[k]->setConfigured();
	found = true;
	break;
      }
    }
  }
  if (!found)
    Serial.printf("  Sensors key \"%s\" not found.\n", key);
}
