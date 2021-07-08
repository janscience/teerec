/*
  AudioShield - playing acquired data on audio shield.
  Created by Jan Benda, July 2nd, 2021.
*/

#ifndef AudioShield_h
#define AudioShield_h


#include <Arduino.h>
#include <Audio.h>
#include <ContinuousADC.h>


class AudioPlayBuffer : public DataConsumer, public AudioStream {
  
 public:
  
  AudioPlayBuffer(const DataBuffer &data);
  virtual void update();
  
};


class AudioShield {

 public:

  AudioShield(const DataBuffer &data);
  ~AudioShield();

  void setup();


 protected:

  AudioPlayBuffer *AudioInput;
  AudioOutputI2S AudioOutput;
  AudioConnection *PatchCord1;
  AudioConnection *PatchCord2;
  AudioControlSGTL5000 Shield;

};


#endif