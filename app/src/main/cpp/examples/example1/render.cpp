//
// Created by Griffin Michalak on 8/10/23.
//

#include "Log.h"
#include "LDSP.h"
#include <math.h> // for using sine

float frequency = 440.0;
float amplitude = 0.2;
float phase = 0;
float inverseSampleRate;

void render(float* floatData, int framesCount, int channelCount, float sampleRate) {
  for(int n = 0; n < framesCount; n++) {
    float out = amplitude * sinf(phase);
    phase += (2.0f * (float)M_PI * frequency) / sampleRate;
    while(phase > 2.0f *M_PI)
      phase -= 2.0f * (float)M_PI;

    for (auto channel = 0; channel < channelCount; ++channel) {
      floatData[n * channelCount + channel] = out;
    }
  }
}
