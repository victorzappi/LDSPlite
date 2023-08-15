//
// Created by Griffin Michalak on 8/10/23.
//

#ifndef LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_H_
#define LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_H_

/**
 *
 * @param floatData float pointer to float buffer where samples will be stored
 * @param framesCount number of elements of the buffer.
 * @param channelCount number of channels
 * @param sampleRate sample rate
 */
void render(float* floatData, int framesCount, int channelCount, float sampleRate);

#endif //LDSP_LITE_APP_SRC_MAIN_CPP_INCLUDE_LDSP_H_

//macros ^
