/*
    Neural model taken from:
    Wright, Alec, Eero-Pekka Damskägg, Lauri Juvela, and Vesa Välimäki. "Real-time guitar amplifier emulation with deep learning." Applied Sciences 10, no. 3 (2020): 766.
    and
    https://github.com/Alec-Wright/Automated-GuitarAmpModelling
*/

#include "LDSP.h"
#include "libraries/OrtModel/OrtModel.h"
#include <chrono>
#include <fstream> // ofstream

OrtModel model;
std::string modelType = "onnx";
std::string modelName = "AutoGuitarAmp";

float input[1];
float output[1] = {0};

//-------------------------------

int outputSize = 1;

unsigned long long * inferenceTimes;
int logPtr = 0;
constexpr int testDuration_sec = 10;
int numLogs;


bool setup(LDSPcontext *context, void *userData)
{
  std::string modelPath = modelName+"."+modelType;
  if (!model.setup("session1", modelPath))
    LDSP_log("unable to setup ortModel");

  //--------------------------------
  inferenceTimes = new unsigned long long[context->audioSampleRate*testDuration_sec*1.01];
  numLogs = context->audioSampleRate*testDuration_sec / outputSize; // division to handle case of models outputting a block of samples

  return true;
}

void render(LDSPcontext *context, void *userData)
{
  for(int n=0; n<context->audioFrames; n++)
  {
    input[0] = audioRead(context, n, 0);

    // Start the Clock
    auto start_time = std::chrono::high_resolution_clock::now();

    model.run(input, output);

    // Stop the clock
    auto end_time = std::chrono::high_resolution_clock::now();
    inferenceTimes[logPtr] = std::chrono::duration_cast
        <std::chrono::microseconds>(end_time - start_time).count();
    logPtr++;

    // passthrough test, because the model may not be trained
    audioWrite(context, n, 0, input[0]);
    audioWrite(context, n, 1, input[0]);

    if(logPtr>=numLogs)
      LDSP_requestStop();
  }
}
void cleanup(LDSPcontext *context, void *userData)
{
  std::string timingLogDir = "/sdcard";//"/data/user/0/com.ldsp.ldsplite/files";
  std::string timingLogFileName = "inferenceTiming_"+modelName+"_out"+std::to_string(outputSize)+"_onnx.txt";
  std::string timingLogFilePath = timingLogDir+"/"+timingLogFileName;

  std::ofstream logFile(timingLogFilePath);
  if(logFile.is_open())
  {
    for(int i=0;i<numLogs; i++)
      logFile << std::to_string(inferenceTimes[i]) << "\n";
  }
  logFile.close();

  delete[] inferenceTimes;
}