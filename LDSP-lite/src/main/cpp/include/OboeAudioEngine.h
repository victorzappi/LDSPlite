#pragma once

#include <oboe/Oboe.h>
#include "LDSP.h"
#include "fullduplex/FullDuplexStream.h"
#include <atomic>

namespace ldsplite {

//VIC not a great solution to make internal context visible to sensors.cpp like in LDSP
struct LDSPinternalContext {
  float *audioIn;
  float *audioOut;
  uint32_t audioFrames;
  uint32_t audioInChannels;
  uint32_t audioOutChannels;
  float audioSampleRate;
  float *sensors;
  int *ctrlInputs;
  uint32_t sensorChannels;
  bool *sensorsSupported;
  string *sensorsDetails;
  float controlSampleRate; // sensors and output devices
  multiTouchInfo *mtInfo;
  string projectName;
  float *sliders;
  LDSPlite *ldspLite;
};
//VIC and this is a terrible solution to share internal context with sensors.cpp as extern like in LDSP
extern LDSPinternalContext intContext; // Declaration of the variable

class OboeAudioEngine : public FullDuplexStream {
 public:
  static constexpr auto channelCount = oboe::ChannelCount::Mono;

  OboeAudioEngine(LDSPlite *ldspLite);
  ~OboeAudioEngine();

  void init();
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

  void callRender(int audioFrames, float* audioIn, float* audioOut);

  void setUpdateCtrlInBufferCallback(std::function<void()> callback) {
    _updateCtrlInBufferCallback = callback;
  }
  
  float getSampleRate();
  int getFramesPerCallback();
  int getFullDuplex();
  void setSlider0(float param);
  void setSlider1(float param);
  void setSlider2(float param);
  void setSlider3(float param);

 private:
  std::shared_ptr<oboe::AudioStream> _outStream = nullptr;
  std::shared_ptr<oboe::AudioStream> _inStream = nullptr;
  int _sampleRate = 0;
  int _bufferSize = 384;
  LDSPcontext* userContext = nullptr;
  bool _fullDuplex;
  float *silentInBuff = nullptr;
  std::atomic<float> _slider0;
  std::atomic<float> _slider1;
  std::atomic<float> _slider2;
  std::atomic<float> _slider3;
  float _sliders[4] = {0};
  bool _slidersOff;

  std::function<void()> _updateCtrlInBufferCallback;

  oboe::Result createStream(bool isInput);
};


//----------------------------------

inline void OboeAudioEngine::callRender(int audioFrames, float* audioIn, float* audioOut) {
  intContext.audioFrames = audioFrames;
  intContext.audioIn = audioIn;
  intContext.audioOut = audioOut;

  if(!_slidersOff)
  {
    intContext.sliders[0] = _slider0.load();
    intContext.sliders[1] = _slider2.load();
    intContext.sliders[2] = _slider2.load();
    intContext.sliders[3] = _slider3.load();
  }

  if (_updateCtrlInBufferCallback) {
    _updateCtrlInBufferCallback();
  }
  
  render(userContext, nullptr);
}

inline float OboeAudioEngine::getSampleRate() {
  return (float) getOutputStream()->getSampleRate();
}

// this is equivalent to LDSP periodSize or audioFrames
inline int OboeAudioEngine::getFramesPerCallback() {
  return getOutputStream()->getFramesPerCallback();
}

inline int OboeAudioEngine::getFullDuplex() {
  return _fullDuplex;
}

inline void OboeAudioEngine::setSlider0(float param) {
  _slider0.store(param);
}

inline void OboeAudioEngine::setSlider1(float param) {
  _slider1.store(param);
}

inline void OboeAudioEngine::setSlider2(float param) {
  _slider2.store(param);
}

inline void OboeAudioEngine::setSlider3(float param){
  _slider3.store(param);
}

}  // namespace ldsplite