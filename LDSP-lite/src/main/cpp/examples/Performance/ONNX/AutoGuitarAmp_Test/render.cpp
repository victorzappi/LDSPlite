/*
    Neural model taken from:
    Wright, Alec, Eero-Pekka Damskägg, Lauri Juvela, and Vesa Välimäki. "Real-time guitar amplifier emulation with deep learning." Applied Sciences 10, no. 3 (2020): 766.
    and
    https://github.com/Alec-Wright/Automated-GuitarAmpModelling
*/

#include "LDSP.h"
#include "libraries/OrtModel/OrtModel.h"

OrtModel ortModel;
std::string modelType = "onnx";
std::string modelName = "AutoGuitarAmp";

float input[1] = {0};
float output[1];


bool setup(LDSPcontext *context, void *userData) {

  std::string modelPath = modelName+"."+modelType;
  if (!ortModel.setup("session1", modelPath))
    LDSP_log("unable to setup ortModel");

  return true;
}

void render(LDSPcontext *context, void *userData)
{
  for(int n=0; n<context->audioFrames; n++) {

    input[0] = audioRead(context, n, 0);

    // Run the model
    ortModel.run(input, output);

    // passthrough test, because the model may not be trained
    audioWrite(context, n, 0, input[0]);
    audioWrite(context, n, 1, input[0]);
  }
}

void cleanup(LDSPcontext *context, void *userData)
{
}
