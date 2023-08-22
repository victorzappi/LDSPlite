#pragma once

#include <oboe/Oboe.h>
#include "LDSP-lite.h"

namespace wavetablesynthesizer {

class OboeAudioEngine : public oboe::AudioStreamDataCallback {
 public:
  static constexpr auto channelCount = oboe::ChannelCount::Mono;

  OboeAudioEngine(int samplingRate);
  ~OboeAudioEngine();

  int32_t play();

  void stop();

  oboe::DataCallbackResult onAudioReady(oboe::AudioStream* audioStream,
                                        void* audioData,
                                        int32_t framesCount) override;

 private:
  std::shared_ptr<oboe::AudioStream> _stream;
  int _samplingRate;
  //VIC
  struct LDSPinternalContext {
    float *audioOut;
    uint32_t audioFrames;
    uint32_t audioOutChannels;
    float audioSampleRate;
  } intContext;
  LDSPcontext* userContext = nullptr;
};
}  // namespace wavetablesynthesizer