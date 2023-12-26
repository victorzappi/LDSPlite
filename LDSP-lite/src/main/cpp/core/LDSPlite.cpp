#include "LDSPlite.h"
#include <cmath>
#include "LDSP_log.h"
#include "OboeAudioEngine.h"


namespace ldsplite {

LDSPlite::LDSPlite()
    : _audioEngine{std::make_unique<OboeAudioEngine>()} {}

LDSPlite::~LDSPlite() = default;

bool LDSPlite::isStarted() const {
  LDSP_log("isStarted() called");
  return _isStarted;
}

void LDSPlite::start() {
  LDSP_log("start() called");
  std::lock_guard<std::mutex> lock(_mutex);
  const auto result = _audioEngine->start();
  if (result == oboe::Result::OK) {
    _isStarted = true;
  } else {
    LDSP_log("Could not start playback.");
  }
}

void LDSPlite::stop() {
  LDSP_log("stop() called");
  std::lock_guard<std::mutex> lock(_mutex);
  _audioEngine->stop();
  _isStarted = false;
}
}  // namespace ldsplite