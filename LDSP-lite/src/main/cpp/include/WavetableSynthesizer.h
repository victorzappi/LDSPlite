#pragma once

#include <memory>
#include <mutex>

namespace wavetablesynthesizer {

class OboeAudioEngine;

constexpr auto samplingRate = 48000;

class WavetableSynthesizer {
 public:
  WavetableSynthesizer();

  ~WavetableSynthesizer();

  void play();

  void stop();

  bool isPlaying() const;

 private:
  std::atomic<bool> _isPlaying{false};
  std::mutex _mutex;
  std::unique_ptr<OboeAudioEngine> _audioPlayer;
};
}  // namespace ldsplite
