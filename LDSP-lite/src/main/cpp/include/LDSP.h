//
// Created by Griffin Michalak on 8/10/23.
//


#ifndef LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_
#define LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_

#include <stdint.h>
#include "LDSP_log.h"
#include "BelaUtilities.h"

struct LDSPcontext {
  const float * const audioIn;
  float * const audioOut;
  const uint32_t audioFrames;
  const uint32_t audioInChannels;
  const uint32_t audioOutChannels;
  const float audioSampleRate;
};


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

#endif //LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_

//macros ^
