/*
  SDWriter - library for writing data from a DataWorker to SD card.
  Created by Jan Benda, May 26th, 2021.
*/

#ifndef SDWriter_h
#define SDWriter_h


#include <Arduino.h>
#include <DataWorker.h>
#include <WaveHeader.h>

// undefine if you want to use the SD library instead:
#define SDCARD_USE_SDFAT


#ifdef SDCARD_USE_SDFAT
  // Use SdFat library:
  #include <SdFat.h>
  #define SDFILE FsFile
#else
  // Use Teensy SD library:
  #include <SD.h>
  #include <SPI.h>
  #define SDFILE File
#endif


class SDCard {

 public:

  // Initialize. You need to call begin() for accessing the SD card.
  SDCard();
  // End usage of SD card.
  ~SDCard();

#ifdef BUILTIN_SDCARD
  // Initialize built in SD card.
  // Return true on success.
  bool begin();
#endif
  
  // Initialize SD card on specified SPI chip select pin.
  // Return true on success.
  bool begin(uint8_t csPin);

#ifdef SDCARD_USE_SDFAT
  // Initialize SD card with clock speed.
  bool begin(SdCsPin_t csPin, uint32_t maxSck);

  // Initialize SD card via SdioConfig.
  bool begin(SdioConfig sdioConfig);
  
  // Initialize SD card via SdSpiConfig.
  bool begin(SdSpiConfig spiConfig);
#endif
  
  // End usage of SD card.
  void end();

  // Availability of a SD card. 
  bool available() const { return Available; };

  // True if SD card is busy.
  bool isBusy();

#ifdef SDCARD_USE_SDFAT
  SdFs &sdcard() { return SD; };
#else
  SDClass &sdcard() { return SD; };
#endif

  // Make directory if it does not exist and
  // make it the currrent working directory.
  // Return true on succes.
  bool dataDir(const char *path);

  // Reset current working directory to root.
  // Return true on succes.
  bool rootDir();

  // Return true if path exists.
  bool exists(const char *path);

  // Replace NUM in fname by "01", "02", "03" etc., 'ANUM' by 'aa', 'ab', 'ac' etc. 
  // such that it specifies a non existing file. 
  // If no SD card is available, or if no unique file can be found, return an empty string.
  // Takes about 1-2ms.
  String incrementFileName(const String &fname);

  // Reset the counter that is used by incrementFileName().
  // Call it, whenever the filename changes, for example, because of a new date.
  void resetFileCounter();

  // Remove the specified file from the current working directory.
  // Return true on success.
  bool removeFile(const char *path);

  // Remove all files in path (non-recursively).
  void removeFiles(const char *path);

  // Open file on SD card for reading.
  SDFILE openRead(const char *path);

  // Open file on SD card for writing (not appending).
  SDFILE openWrite(const char *path);

  // Open file on SD card for appending to existing file.
  SDFILE openAppend(const char *path);

  
 protected:

#ifdef SDCARD_USE_SDFAT
  SdFs SD;
#endif
  
  bool Available;

  uint16_t NameCounter;
};


class SDWriter : public DataWorker {

 public:

  // Size of blocks for writing.
  // Must be an integer multiple of TeensyADC::MajorSize.
  static const size_t MajorSize = 512;

  // Initialize writer on default SD card.
  SDWriter(const DataWorker &data);
  // Initialize writer on SD card.
  SDWriter(SDCard &sd, const DataWorker &data);
  // Close file and end usage of SD card.
  ~SDWriter();

  // Availability of a SD card. 
  bool cardAvailable() const;

  // The SD card volume on which data are written.
  SDCard *sdcard() { return SDC; };

  // End usage of SD card if it was created by SDWriter.
  void end();

  // Set name of software to be saved in wave header by openWave().
  void setSoftware(const char *software);

  // Return write interval in seconds.
  float writeInterval() const;

  // Set write interval depending on settings of the DataWorker.
  void setWriteInterval();

  // Return time after last write in seconds.
  float writeTime() const;

  // True if data are pending that need to be written to file.
  // Check this regularly in loop() and call write() if true is returned.
  bool pending();

  // Open new file for writing.
  // fname is the name of the file inclusively extension.
  bool open(const char *fname);

  // True if file is open.
  bool isOpen() const;

  // Close file.
  void close();

  // Return file object.
  SDFILE &file();


  // Open new file for writing and write wave header with metadata
  // from all data producers.
  // For samples<0, take max file size.
  // For samples=0, initialize wave header with unspecified size.  You
  // then need to close the file with closeWave() and provide the
  // number of samples there.
  // If no file extension is provided, ".wav" is added.
  // Returns true if the file was successfully opened.
  bool openWave(const char *fname, int32_t samples=-1,
                const char *datetime=0);

  // Update wave header with proper file size and close file.
  // Returns true if the file was not open or the file was sucessfully
  // closed, including an update of the wave header with the actual
  // file size.
  bool closeWave();

  
  // Write available data to file (if the file is open).
  // If maxFileSamples() is set (>0), then stop writing after that
  // many samples.
  // Returns number of written samples or a negative number on error:
  // -1: file is not open.
  // -2: file is already full according to maxFileSamples().
  // -3: no data are available.
  ssize_t write();

  // Start writing to a file from the current sample minus decr samples on.
  void start(size_t decr=0);

  // Return current file size in samples.
  size_t fileSamples() const;

  // Return current file size in seconds.
  float fileTime() const;

  // Return current file size as a string displaying minutes and seconds.
  // str must hold at least 6 characters.
  void fileTimeStr(char *str) const;

  // Set maximum file size to a fixed number of samples modulo 256.
  void setMaxFileSamples(size_t samples);

  // Set maximum file size to approximately that many seconds.
  void setMaxFileTime(float secs);

  // Return actually used maximum file size in samples.
  size_t maxFileSamples() const;

  // Return maximum file size in seconds.
  float maxFileTime() const;

  // Return true if maximum number of samples have been written
  // and a new file needs to be opened.
  bool endWrite();
  
  // Data buffer has been initialized.
  virtual void reset();
  

 protected:

  SDCard *SDC;
  bool SDOwn;
  mutable SDFILE DataFile;   // mutable because File from FS.h has non-constant bool() function

  WaveHeader Wave;

  elapsedMillis WriteTime;
  uint WriteInterval;

  size_t FileSamples;    // current number of samples stored in the file.
  size_t FileMaxSamples; // maximum number of samples to be stored in a file.
  
};


#endif
