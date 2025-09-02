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
  settings.parametersOff = false;

  settings.verbose = false;

  LDSP_initSensors(&settings);

  _ctrlInputs.setupContext(&ldsplite::intContext);
  // we use a callback to update the ctrlInput buffer in OboeAudioEngine,
  // so that the Ctrlnputs class is transparent to the OboeAudioEngine class
  _audioEngine->setUpdateCtrlInBufferCallback([this]() {
    _ctrlInputs.updateBuffer();
  });

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

void LDSPlite::updateAnyTouch(int state) {
  _ctrlInputs.updateAnyTouch(state);
}

void LDSPlite::updateTouch(int slot, int id, float x, float y, float pressure,
                 float majAxis, float minAxis, float orientation,
                 float majWidth, float minWidth) {
  _ctrlInputs.updateTouch(slot, id, x, y, pressure,
                          majAxis, minAxis, orientation,
                          majWidth, minWidth);
}

void LDSPlite::updateHover(int slot, float hoverX, float hoverY) {
  _ctrlInputs.updateHover(slot, hoverX, hoverY);
}

void LDSPlite::clearTouch(int slot) {
  _ctrlInputs.clearTouch(slot);
}

void LDSPlite::setScreenResolution(float width, float height) {
  _ctrlInputs.setScreenResolution(width, height);
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