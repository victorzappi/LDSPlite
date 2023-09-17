//
// Created by vic on 7/13/23.
//
#include "LDSP-lite.h"
#include <cmath> // sin

float frequency = 230.0;
float amplitude = 0.2;

float frequency2 = 1.0;

//------------------------------------------------
float phase;
float inverseSampleRate;

float phase2;



bool setup(LDSPcontext *context, void *userData)
{
  inverseSampleRate = 1.0 / context->audioSampleRate;
  phase = 0.0;
  phase2 = 0.0;

  return true;
}

void render(LDSPcontext *context, void *userData)
{
  for(int n=0; n<context->audioFrames; n++)
  {
    float out = amplitude * sinf(phase) * sinf(phase2);
    phase += 2.0f * (float)M_PI * frequency * inverseSampleRate;
    while(phase > 2.0f *M_PI)
      phase -= 2.0f * (float)M_PI;
    phase2 += 2.0f * (float)M_PI * frequency2 * inverseSampleRate;
    while(phase2 > 2.0f *M_PI)
      phase2 -= 2.0f * (float)M_PI;

    //float in = audioRead(context, n, 0);

    for(int chn=0; chn<context->audioOutChannels; chn++)
      audioWrite(context, n, chn, /*in*/out);
  }
}

void cleanup(LDSPcontext *context, void *userData)
{

}