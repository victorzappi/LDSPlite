#include "OboeAudioEngine.h"

#include <utility>
#include "Log.h"


using namespace oboe;

namespace wavetablesynthesizer {
#ifndef NDEBUG
static std::atomic<int> instances{0};
#endif

OboeAudioEngine::OboeAudioEngine(int samplingRate) : _samplingRate(samplingRate) {
  //VIC incapsulate internal pointer
  userContext = (LDSPcontext*)&intContext;

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

int32_t OboeAudioEngine::play() {
  LOGD("OboeAudioEngine::play()");
  AudioStreamBuilder builder;
  const auto result =
      builder.setPerformanceMode(PerformanceMode::LowLatency)
          ->setDirection(Direction::Output)
          ->setSampleRate(_samplingRate)
          ->setDataCallback(this)
          ->setSharingMode(SharingMode::Exclusive)
          ->setFormat(AudioFormat::Float)
          ->setChannelCount(channelCount)
          ->setSampleRateConversionQuality(SampleRateConversionQuality::Best)
          ->openStream(_stream);

  if (result != Result::OK) {
    return static_cast<int32_t>(result);
  }

  //VIC
  intContext.audioSampleRate = _samplingRate;
  intContext.audioOutChannels = channelCount;

  setup(userContext, nullptr);

  const auto playResult = _stream->requestStart();

  return static_cast<int32_t>(playResult);
}

void OboeAudioEngine::stop() {
  LOGD("OboeAudioEngine::stop()");

  if (_stream) {
    _stream->stop();
    _stream->close();
    _stream.reset();
  }
  //VIC
  cleanup(userContext, nullptr);
}

DataCallbackResult OboeAudioEngine::onAudioReady(oboe::AudioStream* audioStream,
                                                 void* audioData,
                                                 int32_t framesCount) {

  float* floatData = reinterpret_cast<float*>(audioData);

  intContext.audioFrames = framesCount;
  intContext.audioOut = floatData;

  render(userContext, nullptr);

  return oboe::DataCallbackResult::Continue;
}
}  // namespace wavetablesynthesizer