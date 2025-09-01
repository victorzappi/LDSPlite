#pragma once

#include <memory>
#include <mutex>
#include "CtrlInputs.h"

namespace ldsplite {

class OboeAudioEngine;

class LDSPlite {
 public:
  LDSPlite();

  ~LDSPlite();

  void start();

  void stop();

  bool isStarted() const;

  void updateTouch(int slot, int id, float x, float y, float pressure,
                   float majAxis, float minAxis, float orientation,
                   float majWidth, float minWidth);
  void updateHover(int slot, float hoverX, float hoverY);
  void clearTouch(int slot);
  void setScreenResolution(float width, float height);

  void setSlider0(float value);
  void setSlider1(float value);
  void setSlider2(float value);
  void setSlider3(float value);

 private:
  std::atomic<bool> _isStarted{false};
  std::mutex _mutex;
  std::unique_ptr<OboeAudioEngine> _audioEngine;
  CtrlInputs _ctrlInputs;
};

}  // namespace ldsplite
