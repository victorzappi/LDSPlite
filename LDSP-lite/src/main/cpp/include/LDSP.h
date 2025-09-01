//
// Created by Griffin Michalak on 8/10/23.
//


#ifndef LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_
#define LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_

#include <stdint.h>
#include "LDSP_log.h"
//#include "LDSPlite.h"
#include "BelaUtilities.h"

//VIC these are here to compatibility with LDSP original codebase
#include <string>
#include <vector> // vector
using std::string;
using std::vector;

namespace ldsplite {
class LDSPlite;  // Forward declaration
}

//VIC this is a terrible kludge to keep the same API as LDSP...
#define LDSP_requestStop() context->ldspLite->stop()

struct LDSPinitSettings {
  // these items might be adjusted by the user:
  int periodSize;
  float samplerate;
  int captureOff;
  int sensorsOff;
  int parametersOff; //VIC LDSPlite only
  int verbose;
};

struct multiTouchInfo {
  float screenResolution[2];
  int touchSlots;
  int touchAxisMax;
  int touchWidthMax;
  bool anyTouchSupported;
};

struct LDSPcontext {
  const float * const audioIn;
  float * const audioOut;
  const uint32_t audioFrames;
  const uint32_t audioInChannels;
  const uint32_t audioOutChannels;
  const float audioSampleRate;
  float *sensors;
  const int * const ctrlInputs;
  const uint32_t sensorChannels;
  const bool * const sensorsSupported;
  const string * const sensorsDetails;
  const float controlSampleRate; // sensors and output devices
  const multiTouchInfo * const mtInfo;
  const string projectName;
  float * const sliders;
  ldsplite::LDSPlite * const ldspLite;
};

enum sensorChannel {
  chn_sens_accelX,
  chn_sens_accelY,
  chn_sens_accelZ,
  chn_sens_magX,
  chn_sens_magY,
  chn_sens_magZ,
  chn_sens_gyroX,
  chn_sens_gyroY,
  chn_sens_gyroZ,
  chn_sens_light,
  chn_sens_proximity,
  chn_sens_count
};

enum btnInputChannel {
  chn_btn_power,
  chn_btn_volUp,
  chn_btn_volDown,
  chn_btn_count
};

enum multiTouchInputChannel {
  chn_mt_anyTouch,
  chn_mt_x,
  chn_mt_y,
  chn_mt_majAxis,
  chn_mt_minAxis,
  chn_mt_orientation,
  chn_mt_hoverX,
  chn_mt_hoverY,
  chn_mt_majWidth,
  chn_mt_minWidth,
  chn_mt_pressure,
  chn_mt_id,
  chn_mt_count
};

void LDSP_initSensors(LDSPinitSettings *settings);
void LDSP_cleanupSensors();


bool setup(LDSPcontext *context, void *userData);
/**
 *
 * @param context pointer to structure containing audio buffers, sample rate...
 * @param userData
 */
void render(LDSPcontext *context, void *userData);
void cleanup(LDSPcontext *context, void *userData);

static inline float audioRead(LDSPcontext *context, int frame, int channel);
static inline void audioWrite(LDSPcontext *context, int frame, int channel, float value);

static inline int multiTouchRead(LDSPcontext *context, multiTouchInputChannel channel, int touchSlot=0);

static inline float sliderRead(LDSPcontext *context, int parameterNum);
static inline void sliderWrite(LDSPcontext *context, int parameterNum, float value);

//-----------------------------------------------------------------------------------------------
// inline

// audioRead()
//
// Returns the value of the given audio input at the given frame number
static inline float audioRead(LDSPcontext *context, int frame, int channel)
{
  return context->audioIn[frame * context->audioInChannels + channel];
}

// audioWrite()
//
// Sets a given audio output channel to a value for the current frame
static inline void audioWrite(LDSPcontext *context, int frame, int channel, float value)
{
  context->audioOut[frame * context->audioOutChannels + channel] = value;
}

static inline int multiTouchRead(LDSPcontext *context, multiTouchInputChannel channel, int touchSlot)
{
  if(channel==chn_mt_anyTouch)
    return context->ctrlInputs[chn_btn_count+chn_mt_anyTouch];
  else
    return context->ctrlInputs[chn_btn_count+1+(channel-1)*context->mtInfo->touchSlots + touchSlot];
}


// sliderRead()
//
// LDSPlite only - Returns the most recent value of the given GUI parameter
static inline float sliderRead(LDSPcontext *context, int parameterNum)
{
  return context->sliders[parameterNum];
}

// sliderWrite()
//
// LDSPlite only - Sets a given GUI parameter to a value (it updates the GUI)
static inline void sliderWrite(LDSPcontext *context, int parameterNum, float value)
{
  context->sliders[parameterNum] = value;
}

#endif //LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_

//macros ^