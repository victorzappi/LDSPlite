#include "WavetableSynthesizer.h"
#include <cmath>
#include "Log.h"
#include "OboeAudioEngine.h"

namespace wavetablesynthesizer {


WavetableSynthesizer::WavetableSynthesizer()
    : _audioPlayer{std::make_unique<OboeAudioEngine>(samplingRate)} {}

WavetableSynthesizer::~WavetableSynthesizer() = default;

bool WavetableSynthesizer::isPlaying() const {
  LOGD("isPlaying() called");
  return _isPlaying;
}

void WavetableSynthesizer::play() {
  LOGD("play() called");
  std::lock_guard<std::mutex> lock(_mutex);
  const auto result = _audioPlayer->play();
  if (result == 0) {
    _isPlaying = true;
  } else {
    LOGD("Could not start playback.");
  }
}

void WavetableSynthesizer::stop() {
  LOGD("stop() called");
  std::lock_guard<std::mutex> lock(_mutex);
  _audioPlayer->stop();
  _isPlaying = false;
}
}  // namespace wavetablesynthesizer