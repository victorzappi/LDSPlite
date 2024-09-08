#pragma once

#include <memory>
#include <mutex>

namespace ldsplite {

class OboeAudioEngine;

class LDSPlite {
 public:
  LDSPlite();

  ~LDSPlite();

  void start();

  void stop();

  bool isStarted() const;

  void setSlider0(float value);
  void setSlider1(float value);
  void setSlider2(float value);
  void setSlider3(float value);

 private:
  std::atomic<bool> _isStarted{false};
  std::mutex _mutex;
  std::unique_ptr<OboeAudioEngine> _audioEngine;
};

}  // namespace ldsplite
