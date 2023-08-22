//
// Created by Griffin Michalak on 8/10/23.
//


#ifndef LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_
#define LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_

#include <stdint.h>

struct LDSPcontext {
  float * const audioOut;
  const uint32_t audioFrames;
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

static inline void audioWrite(LDSPcontext *context, int frame, int channel, float value);



// audioWrite()
//
// Sets a given audio output channel to a value for the current frame
static inline void audioWrite(LDSPcontext *context, int frame, int channel, float value)
{
  context->audioOut[frame * context->audioOutChannels + channel] = value;
}


#endif //LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_LITE_H_

//macros ^
