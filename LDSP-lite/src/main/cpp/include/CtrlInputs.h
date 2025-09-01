//
// Created by vic on 9/1/25.
//

#ifndef LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_CTRLINPUTS_H_
#define LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_CTRLINPUTS_H_


#include "TouchHandler.h"
#include "OboeAudioEngine.h" // for LDSPinternalContext
#include "LDSP.h"
#include <memory>
#include <cstring>

namespace ldsplite {

class CtrlInputs {
 public:
  CtrlInputs() {
    _totalSize = chn_btn_count + 1 + (chn_mt_count - 1) * TouchHandler::MAX_SLOTS;
    _ctrlInputsBuffer = std::make_unique<int[]>(_totalSize);
    std::memset(_ctrlInputsBuffer.get(), 0, _totalSize * sizeof(int));

    _mtInfo.touchSlots = TouchHandler::MAX_SLOTS;
    _mtInfo.touchAxisMax = 1000;
    _mtInfo.touchWidthMax = 100;
    _mtInfo.anyTouchSupported = true;
    _mtInfo.screenResolution[0] = 1920.0f;
    _mtInfo.screenResolution[1] = 1080.0f;
  }

  void setupContext(LDSPinternalContext* context) {
    context->ctrlInputs = _ctrlInputsBuffer.get();
    context->mtInfo = &_mtInfo;
  }


  // Touch delegating methods
  void updateTouch(int slot, int id, float x, float y, float pressure,
                   float majAxis, float minAxis, float orientation,
                   float majWidth, float minWidth) {
    _touchHandler.updateTouch(slot, id, x, y, pressure,
                              majAxis, minAxis, orientation,
                              majWidth, minWidth);
  }

  void updateHover(int slot, float hoverX, float hoverY) {
    _touchHandler.updateHover(slot, hoverX, hoverY);
  }

  void clearTouch(int slot) {
    _touchHandler.clearTouch(slot);
  }

  void setScreenResolution(float width, float height) {
    _touchHandler.setScreenResolution(width, height);
    _mtInfo.screenResolution[0] = width;
    _mtInfo.screenResolution[1] = height;
  }

  // Update buffer from handlers
  void updateBuffer() {
    // Update from touch handler
    _touchHandler.populateCtrlInputs(_ctrlInputsBuffer.get(), _mtInfo.touchSlots);

    // TODO: Update from buttons handler
    _ctrlInputsBuffer[chn_btn_power] = 0;
    _ctrlInputsBuffer[chn_btn_volUp] = 0;
    _ctrlInputsBuffer[chn_btn_volDown] = 0;
  }

  int* getBuffer() { return _ctrlInputsBuffer.get(); }
  const multiTouchInfo* getMultiTouchInfo() const { return &_mtInfo; }

 private:
  TouchHandler _touchHandler;  // Owned by CtrlInputs
  // Future: ButtonsHandler _buttonsHandler;
  std::unique_ptr<int[]> _ctrlInputsBuffer;
  multiTouchInfo _mtInfo;
  int _totalSize;
};

} // namespace ldsplite

#endif //LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_CTRLINPUTS_H_
