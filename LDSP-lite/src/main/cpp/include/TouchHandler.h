//
// Created by vic on 9/1/25.
//

#ifndef LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_TOUCHHANDLER_H_
#define LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_TOUCHHANDLER_H_

#include "LDSP.h"
#include <atomic>
#include <array>

namespace ldsplite {

struct TouchData {
  std::atomic<int> id{-1};
  std::atomic<float> x{0};
  std::atomic<float> y{0};
  std::atomic<float> pressure{0};
  std::atomic<float> majAxis{0};
  std::atomic<float> minAxis{0};
  std::atomic<float> orientation{0};
  std::atomic<float> hoverX{0};
  std::atomic<float> hoverY{0};
  std::atomic<float> majWidth{0};
  std::atomic<float> minWidth{0};
};

class TouchHandler {
 public:
  static constexpr int MAX_SLOTS = 10;

  TouchHandler() {
    anyTouch.store(0);
    for (int i = 0; i < MAX_SLOTS; ++i) {
      _touches[i].id.store(-1, std::memory_order_relaxed);
    }
  }

  void updateAnyTouch(int state) {
    anyTouch.store(state);
  }

  void updateTouch(int slot, int id, float x, float y, float pressure,
                   float majAxis, float minAxis, float orientation,
                   float majWidth, float minWidth) {
    if (slot < 0 || slot >= MAX_SLOTS) return;

    _touches[slot].id.store(id, std::memory_order_relaxed);
    _touches[slot].x.store(x, std::memory_order_relaxed);
    _touches[slot].y.store(y, std::memory_order_relaxed);
    _touches[slot].pressure.store(pressure, std::memory_order_relaxed);
    _touches[slot].majAxis.store(majAxis, std::memory_order_relaxed);
    _touches[slot].minAxis.store(minAxis, std::memory_order_relaxed);
    _touches[slot].orientation.store(orientation, std::memory_order_relaxed);
    _touches[slot].majWidth.store(majWidth, std::memory_order_relaxed);
    _touches[slot].minWidth.store(minWidth, std::memory_order_relaxed);
  }

  void updateHover(int slot, float hoverX, float hoverY) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    _touches[slot].hoverX.store(hoverX, std::memory_order_relaxed);
    _touches[slot].hoverY.store(hoverY, std::memory_order_relaxed);
  }

  void clearTouch(int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    _touches[slot].id.store(-1, std::memory_order_relaxed);
  }

  void setScreenResolution(float width, float height) {
    _screenWidth = width;
    _screenHeight = height;
  }

  // Populate ctrlInputs array with touch data
  void populateCtrlInputs(int* ctrlInputs, int touchSlots) {
    // Skip button section (chn_btn_count elements)
    int offset = chn_btn_count;

    // Set anyTouch flag - check if any touch is active
//    int anyTouch = 0;
//    for (const auto& touch : _touches) {
//      if (touch.id.load(std::memory_order_relaxed) != -1) {
//        anyTouch = 1;
//        break;
//      }
//    }
    ctrlInputs[offset] = anyTouch.load();//anyTouch;
    offset++;

    // Fill touch data for each channel
    // Layout: (channel-1) * touchSlots + slot
    int slots = (touchSlots < MAX_SLOTS) ? touchSlots : MAX_SLOTS;

    for (int slot = 0; slot < slots; ++slot) {
      // chn_mt_x (channel index 0 after anyTouch)
      ctrlInputs[offset + 0 * slots + slot] =
          static_cast<int>(_touches[slot].x.load(std::memory_order_relaxed));

      // chn_mt_y (channel index 1)
      ctrlInputs[offset + 1 * slots + slot] =
          static_cast<int>(_touches[slot].y.load(std::memory_order_relaxed));

      // chn_mt_majAxis (channel index 2)
      ctrlInputs[offset + 2 * slots + slot] =
          static_cast<int>(_touches[slot].majAxis.load(std::memory_order_relaxed));

      // chn_mt_minAxis (channel index 3)
      ctrlInputs[offset + 3 * slots + slot] =
          static_cast<int>(_touches[slot].minAxis.load(std::memory_order_relaxed));

      // chn_mt_orientation (channel index 4)
      ctrlInputs[offset + 4 * slots + slot] =
          static_cast<int>(_touches[slot].orientation.load(std::memory_order_relaxed));

      // chn_mt_hoverX (channel index 5)
      ctrlInputs[offset + 5 * slots + slot] =
          static_cast<int>(_touches[slot].hoverX.load(std::memory_order_relaxed));

      // chn_mt_hoverY (channel index 6)
      ctrlInputs[offset + 6 * slots + slot] =
          static_cast<int>(_touches[slot].hoverY.load(std::memory_order_relaxed));

      // chn_mt_majWidth (channel index 7)
      ctrlInputs[offset + 7 * slots + slot] =
          static_cast<int>(_touches[slot].majWidth.load(std::memory_order_relaxed));

      // chn_mt_minWidth (channel index 8)
      ctrlInputs[offset + 8 * slots + slot] =
          static_cast<int>(_touches[slot].minWidth.load(std::memory_order_relaxed));

      // chn_mt_pressure (channel index 9)
      ctrlInputs[offset + 9 * slots + slot] =
          static_cast<int>(_touches[slot].pressure.load(std::memory_order_relaxed) * 1000.0f);

      // chn_mt_id (channel index 10)
      ctrlInputs[offset + 10 * slots + slot] =
          _touches[slot].id.load(std::memory_order_relaxed);
    }
  }

  float getScreenWidth() const { return _screenWidth; }
  float getScreenHeight() const { return _screenHeight; }

 private:
  std::atomic<int> anyTouch;
  std::array<TouchData, MAX_SLOTS> _touches;
  float _screenWidth = 1920.0f;
  float _screenHeight = 1080.0f;
};

} // namespace ldsplite

#endif //LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_TOUCHHANDLER_H_
