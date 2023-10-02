#pragma once

#include <oboe/Oboe.h>
#include "LDSP.h"
#include "fullduplex/FullDuplexStream.h"

namespace ldsplite {

class OboeAudioEngine : public FullDuplexStream {
 public:
  static constexpr auto channelCount = oboe::ChannelCount::Mono;

  OboeAudioEngine();
  ~OboeAudioEngine();

  oboe::Result start() override;
  oboe::Result stop() override;

  oboe::DataCallbackResult onAudioReady(oboe::AudioStream* outputStream,
                                        void* audioData,
                                        int32_t framesCount) override;

  /**
   * Called when data is available on both streams.
   * Caller should override this method.
//   */
  oboe::DataCallbackResult onBothStreamsReady(
      float *inputData,
      int   numInputFrames,
      float *outputData,
      int   numOutputFrames
  ) override;

 private:
  std::shared_ptr<oboe::AudioStream> _outStream = nullptr;
  std::shared_ptr<oboe::AudioStream> _inStream = nullptr;
  int _samplingRate = 0;
  struct LDSPinternalContext {
    float *audioIn;
    float *audioOut;
    uint32_t audioFrames;
    uint32_t audioInChannels;
    uint32_t audioOutChannels;
    float audioSampleRate;
  } intContext;
  LDSPcontext* userContext = nullptr;
  bool _fullDuplex = true;
  float *silentInBuff = nullptr;

  oboe::Result createStream(bool isInput);
};

}  // namespace ldsplite