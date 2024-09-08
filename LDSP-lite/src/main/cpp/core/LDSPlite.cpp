#include "LDSPlite.h"
#include <cmath>
#include "LDSP_log.h"
#include "OboeAudioEngine.h"
#include "sensors.h"


namespace ldsplite {

LDSPlite::LDSPlite()
    : _audioEngine{std::make_unique<OboeAudioEngine>(this)} {}

LDSPlite::~LDSPlite() = default;

bool LDSPlite::isStarted() const {
  LDSP_log("isStarted() called");
  return _isStarted;
}

//VIC start and stop with sensors works only once! check it out@
void LDSPlite::start() {
  LDSP_log("start() called");
  std::lock_guard<std::mutex> lock(_mutex);

  _audioEngine->init();

  //TODO move this stuff to GUI
  // and pass some of this stuff to audioEngine
  LDSPinitSettings settings;
  settings.samplerate = _audioEngine->getSampleRate();
  settings.periodSize = _audioEngine->getFramesPerCallback();
  settings.captureOff = !_audioEngine->getFullDuplex();

  settings.sensorsOff = true;
  settings.pareametersOff = false;

  settings.verbose = false;

  LDSP_initSensors(&settings);

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
  LDSP_cleanupSensors();
  _isStarted = false;
}

void LDSPlite::setSlider0(float value) {
  _audioEngine->setSlider0(value);
}

void LDSPlite::setSlider1(float value) {
  _audioEngine->setSlider1(value);
}

void LDSPlite::setSlider2(float value) {
  _audioEngine->setSlider2(value);
}

void LDSPlite::setSlider3(float value){
  _audioEngine->setSlider3(value);
}


}  // namespace ldsplite