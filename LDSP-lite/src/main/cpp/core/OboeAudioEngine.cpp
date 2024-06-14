#include "OboeAudioEngine.h"

#include <utility>
#include "LDSP_log.h"

// defined in root's CMakeLists.txt
#ifdef PROJECT_NAME
#define PRJ_NAME PROJECT_NAME
#else
#define PRJ_NAME ""
#endif



using namespace oboe;

namespace ldsplite {
#ifndef NDEBUG
static std::atomic<int> instances{0};
#endif

OboeAudioEngine::OboeAudioEngine(LDSPlite *ldspLite) {
  intContext.audioIn = nullptr;
  intContext.audioOut = nullptr;
  intContext.audioFrames = 0;
  intContext.audioInChannels = 0;
  intContext.audioOutChannels = 0;
  intContext.audioSampleRate = 0;
  intContext.ldspLite = ldspLite;

  //VIC incapsulate internal pointer
  userContext = (LDSPcontext*)&intContext;

  setMNumInputBurstsCushion(0);

  LDSP_log("OboeAudioEngine created. Instances count: %d", ++instances);
}

OboeAudioEngine::~OboeAudioEngine() {
  LDSP_log("OboeAudioEngine destroyed. Instances count: %d", --instances);
  OboeAudioEngine::stop();
}

Result OboeAudioEngine::start() {
  LDSP_log("OboeAudioEngine::start()");
  bool isInput;

  isInput = false;
  createStream(isInput);

  if(_fullDuplex) {
    isInput = true;
  }
  createStream(isInput);

  //VIC
  intContext.projectName = PRJ_NAME;
  intContext.audioSampleRate = (float) getOutputStream()->getSampleRate();
  intContext.audioInChannels = channelCount;
  intContext.audioOutChannels = channelCount;

  setup(userContext, nullptr);

  if(_fullDuplex)
    return FullDuplexStream::start();
  else {
    silentInBuff = new float[getOutputStream()->getFramesPerCallback()];
    memset(silentInBuff, 0, sizeof(float)*getOutputStream()->getFramesPerCallback());

    oboe::Result result = getOutputStream()->requestStart();
    if (result != oboe::Result::OK) {
      delete silentInBuff;
      return result;
    }

    return result;
  }
}


Result OboeAudioEngine::stop() {
  LDSP_log("OboeAudioEngine::stop()");

  cleanup(userContext, nullptr);

  Result result_out = Result::OK;
  Result result_in = Result::OK;

  if(_outStream != nullptr) {
    result_out = _outStream->requestStop();
    _outStream->close();
  }

  if(_fullDuplex) {
    if (_inStream != nullptr) {
      result_in = _inStream->requestStop();
      _inStream->close();
    }

//    if (_outStream != nullptr && _inStream != nullptr)
//      cleanup(userContext, nullptr);
  }
//  else if (_outStream != nullptr)
//    cleanup(userContext, nullptr);

  if(silentInBuff != nullptr)
    delete silentInBuff;

  if(result_out != Result::OK)
    return result_out;
  else if(_fullDuplex) {
    if (result_in != Result::OK)
      return result_in;
  }

  return Result::OK;
}


DataCallbackResult OboeAudioEngine::onAudioReady(oboe::AudioStream* outputStream,
                                                 void* audioData,
                                                 int32_t framesCount) {
  if(_fullDuplex) {
    return FullDuplexStream::onAudioReady(outputStream, audioData, framesCount);
  }
  else {
    //LDSP_log("out frames: %d\n", framesCount);

    intContext.audioFrames = framesCount;
    intContext.audioIn = silentInBuff;
    intContext.audioOut = (float *)audioData;

    render(userContext, nullptr);

    return DataCallbackResult::Continue;
  }
}

DataCallbackResult OboeAudioEngine::onBothStreamsReady(float *inputData,
                                                       int   numInputFrames,
                                                       float *outputData,
                                                       int   numOutputFrames) {

  //LDSP_log("in frames: %d, out frames: %d\n", numInputFrames, numOutputFrames);

  intContext.audioFrames = numOutputFrames;
  intContext.audioIn = inputData;
  intContext.audioOut = outputData;

  render(userContext, nullptr);

  return DataCallbackResult::Continue;
};


  //------------------------------------------------------------------------
Result OboeAudioEngine::createStream(bool isInput) {
  LDSP_log("OboeAudioEngine::createStream()");
  AudioStreamBuilder builder;
  builder.setPerformanceMode(PerformanceMode::LowLatency)
      ->setDirection(isInput ? Direction::Input : Direction::Output)
      ->setSharingMode(SharingMode::Exclusive)
      ->setFormat(AudioFormat::Float)
      ->setChannelCount(channelCount)
      ->setSampleRateConversionQuality(SampleRateConversionQuality::Best)
      ->setUsage(Usage::Media)
      ->setContentType(ContentType::Music)
      //->setDeviceId(deviceId)
      ->setSessionId(SessionId::None)
      ->setChannelConversionAllowed(true)
      ->setFormatConversionAllowed(true)
      ;

    if(_samplingRate!=0)
      builder.setSampleRate(_samplingRate); // otherwise, native samplerate is set automatically

  if(!isInput) {
    builder.setBufferCapacityInFrames(2 * _bufferSize);
    builder.setFramesPerCallback(_bufferSize); // app buffer size
    builder.setDataCallback(this);
  }
  else {
    builder.setInputPreset(InputPreset::VoicePerformance); //VIC if not supported, may switch to Unprocessed... but bigger burst size

    // The input and output buffers will run in sync with input empty
    // and output full. So set the input capacity to match the output.
//    LDSP_log(">____output buffer capacity %d", getOutputStream()->getBufferCapacityInFrames());
    builder.setBufferCapacityInFrames(getOutputStream()->getBufferCapacityInFrames());
//    LDSP_log(">____output frames per callback %d", getOutputStream()->getFramesPerCallback());
    builder.setFramesPerCallback(getOutputStream()->getFramesPerCallback()); // app buffer size
  }

  std::shared_ptr<oboe::AudioStream> oboeStream;
  Result result = builder.openStream(oboeStream);

  if(!isInput) {
    oboeStream->setBufferSizeInFrames(_bufferSize);
  }
  else {
//    LDSP_log(">____output buffer size %d", getOutputStream()->getBufferSizeInFrames());
    oboeStream->setBufferSizeInFrames(getOutputStream()->getBufferSizeInFrames());
//    oboeStream->setBufferSizeInFrames(oboeStream->getFramesPerBurst() * 4);
  }

  if(!isInput) {
    LDSP_log("____output buffer capacity %d", oboeStream->getBufferCapacityInFrames());
    LDSP_log("____output frames per callback %d", oboeStream->getFramesPerCallback());
    LDSP_log("____output buffer size %d", oboeStream->getBufferSizeInFrames());
    LDSP_log("____output frames per burst %d", oboeStream->getFramesPerBurst());
  }
  else {
    LDSP_log("____input buffer capacity %d", oboeStream->getBufferCapacityInFrames());
    LDSP_log("____input frames per callback %d", oboeStream->getFramesPerCallback());
    LDSP_log("____input buffer size %d", oboeStream->getBufferSizeInFrames());
    LDSP_log("____input frames per burst %d", oboeStream->getFramesPerBurst());
  }

  //VIC callback frame count works! it can be set via code for both in and out!
  // buffer capacity seems quite random, but no problem
  // buffer size is complicated:
  // _Aaudio -> can set for output, but not for input [which i generally very big]
  // _OpenSLES -> cannot set it, it is automatically set as 2*burstSize for output and a bigger number that does not make much sense for input
  //              but this is only visible via dumpsys media.audio_flinger, cos the values we log here seem to be bogus
  // burst size seems to be easy to read and should represent audio HAL buffer ; on Motorola g7 [Aaudio], input and output burst size is different!!! input is bigger
  //TODO maybe it'd be cool in Aaudio is the passed buffer size is a multiple of output burst size and suggest that if it's not
  // but more tests are needed to check if extending this test to audio input makes sense—so far, for it seems that in buffer size cannot be modified...


  if(result != Result::OK)
    return result;

  // pass raw pointer to inner object AND pass ownership of local shared
  // pointer to member shared pointer
  if(!isInput) {
    setOutputStream(oboeStream.get());
    _outStream = oboeStream;
  }
  else {
    setInputStream(oboeStream.get());
    _inStream = oboeStream;
  }

  return result;
}



}  // namespace ldsplite