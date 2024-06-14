//
// Created by Jason Hoopes on 2/15/24.
//
/*
 * [2-Clause BSD License]
 *
 * Copyright 2022 Victor Zappi
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <chrono>
#include "LDSP.h"
#include <cmath> // sin
#include "OrtModel.h"
#include <fstream> // ofstream


float frequency = 220.0;
float amplitude = 0.2;
float phase;
float inverseSampleRate;

//------------------------------------------------


#define NUM_SAMPLE_POINTS 441000
unsigned long long inferenceTimes[NUM_SAMPLE_POINTS];
int logPtr = 0;

std::string modelType = "onnx";
std::string modelName = "GuitarLSTM";
int numInputSamples = 1;

OrtModel ortModel;
std::vector<std::vector<float>> input;
float output[1];


bool setup(LDSPcontext *context, void *userData)
{
  inverseSampleRate = 1.0 / context->audioSampleRate;
  phase = 0.0;

  std::string modelAssetPath = modelName+"."+modelType;
  if (!ortModel.setup("session1", modelAssetPath.c_str())) {
    LDSP_log("unable to setup ortModel");
  }

  // Generate random tensors to use as inputs
  input = ortModel.randInputs();

  return true;
}

void render(LDSPcontext *context, void *userData)
{

  for(int n=0; n<context->audioFrames; n++) {

    float out = amplitude * sinf(phase);
    phase += 2.0f * (float)M_PI * frequency * inverseSampleRate;
    while(phase > 2.0f *M_PI)
      phase -= 2.0f * (float)M_PI;


    if (n%numInputSamples==0) {
      // Start the Clock
      auto start_time = std::chrono::high_resolution_clock::now();

      // Run the model
      ortModel.run(input, output);

      // Stop the clock
      auto end_time = std::chrono::high_resolution_clock::now();
      inferenceTimes[logPtr] = std::chrono::duration_cast
                               <std::chrono::microseconds>(end_time - start_time).count();
      logPtr++;
    }

    for(int chn=0; chn<context->audioOutChannels; chn++)
      audioWrite(context, n, chn, out);
  }

}

void cleanup(LDSPcontext *context, void *userData)
{
  std::string timingLogDir = "/data/user/0/com.ldsp.ldsplite/files";
  std::string timingLogFileName = "inferenceTiming_"+modelName+".txt";
  std::string timingLogFilePath = timingLogDir+"/"+timingLogFileName;

  std::ofstream logFile (timingLogFilePath);
  if (logFile.is_open())
  {
    for (int i=0;i<NUM_SAMPLE_POINTS; i++) {
      logFile << std::to_string(inferenceTimes[i]) << "\n";
    }
  }
  logFile.close();
}