#include "LDSPlite.h"
#include <cmath>
#include "Log.h"
#include "OboeAudioEngine.h"

namespace ldsplite {


LDSPlite::LDSPlite()
    : _audioEngine{std::make_unique<OboeAudioEngine>(samplingRate)} {}

LDSPlite::~LDSPlite() = default;

bool LDSPlite::isStarted() const {
  LOGD("isStarted() called");
  return _isStarted;
}

void LDSPlite::start() {
  LOGD("start() called");
  std::lock_guard<std::mutex> lock(_mutex);
  const auto result = _audioEngine->start();
  if (result == oboe::Result::OK) {
    _isStarted = true;
  } else {
    LOGD("Could not start playback.");
  }
}

void LDSPlite::stop() {
  LOGD("stop() called");
  std::lock_guard<std::mutex> lock(_mutex);
  _audioEngine->stop();
  _isStarted = false;
}
}  // namespace ldsplite