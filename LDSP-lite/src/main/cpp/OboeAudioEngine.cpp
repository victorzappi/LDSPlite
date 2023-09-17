#include "OboeAudioEngine.h"

#include <utility>
#include "Log.h"


using namespace oboe;

namespace ldsplite {
#ifndef NDEBUG
static std::atomic<int> instances{0};
#endif

OboeAudioEngine::OboeAudioEngine(int samplingRate) : _samplingRate(samplingRate) {
  //VIC incapsulate internal pointer
  userContext = (LDSPcontext*)&intContext;

  setMNumInputBurstsCushion(0);

#ifndef NDEBUG
  LOGD("OboeAudioEngine created. Instances count: %d", ++instances);
#endif
}

OboeAudioEngine::~OboeAudioEngine() {
#ifndef NDEBUG
  LOGD("OboeAudioEngine destroyed. Instances count: %d", --instances);
#endif
  OboeAudioEngine::stop();
}

Result OboeAudioEngine::start() {
  LOGD("OboeAudioEngine::start()");
  bool isInput;
  isInput = false;
  createStream(isInput);
  isInput = true;
  createStream(isInput);

  //VIC
  intContext.audioSampleRate = _samplingRate;
  intContext.audioInChannels = channelCount;
  intContext.audioOutChannels = channelCount;

  setup(userContext, nullptr);

  return FullDuplexStream::start();
}


Result OboeAudioEngine::stop() {
  LOGD("OboeAudioEngine::stop()");

  Result result_out = Result::OK;
  Result result_in = Result::OK;

  if(_outStream != nullptr) {
    result_out = _outStream->requestStop();
    _outStream->close();
  }

  if(_inStream != nullptr) {
    result_in = _inStream->requestStop();
    _inStream->close();
  }

  if(_outStream != nullptr && _inStream != nullptr)
    cleanup(userContext, nullptr);

  if(result_out != Result::OK)
    return result_out;
  else if(result_in != Result::OK)
    return result_in;
  else
    return Result::OK;
}

DataCallbackResult OboeAudioEngine::onBothStreamsReady(
    float *inputData,
    int   numInputFrames,
    float *outputData,
    int   numOutputFrames) {



  intContext.audioFrames = numOutputFrames;
  intContext.audioIn = inputData;
  intContext.audioOut = outputData;

  render(userContext, nullptr);

  return DataCallbackResult::Continue;
};


  //------------------------------------------------------------------------


Result OboeAudioEngine::createStream(bool isInput) {
  LOGD("OboeAudioEngine::createStream()");
  AudioStreamBuilder builder;
  builder.setPerformanceMode(PerformanceMode::LowLatency)
      ->setDirection(isInput ? Direction::Input : Direction::Output)
      ->setSampleRate(_samplingRate)
      ->setSharingMode(SharingMode::Exclusive)
      ->setFormat(AudioFormat::Float)
      ->setChannelCount(channelCount)
      ->setSampleRateConversionQuality(SampleRateConversionQuality::Best)
      ->setPerformanceMode(PerformanceMode::LowLatency)
      ->setInputPreset(InputPreset::VoicePerformance)
      ->setUsage(Usage::Media)
      ->setContentType(ContentType::Music)
      //->setBufferCapacityInFrames(2048) //VIC be careful, if i put -1 as in Oboe AudioStreamTester.java, open(), it crashes
      //->setDeviceId(deviceId)
      ->setSessionId(SessionId::None)
      ->setChannelConversionAllowed(true)
      ->setFormatConversionAllowed(true)
      ;

  if(!isInput)
    builder.setDataCallback(this);
  else {
      // The input and output buffers will run in sync with input empty
      // and output full. So set the input capacity to match the output.
      builder.setBufferCapacityInFrames(getOutputStream()->getBufferCapacityInFrames());
    }

  std::shared_ptr<oboe::AudioStream> oboeStream;
  Result result = builder.openStream(oboeStream);

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