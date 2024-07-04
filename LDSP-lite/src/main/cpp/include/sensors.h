//
// Created by vic on 7/4/24.
//

#ifndef LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_SENSORS_H_
#define LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_SENSORS_H_

#include <android/sensor.h>
#include <unordered_map> // unordered_map
#include "LDSP.h"
#include "OboeAudioEngine.h" // for LDSPinternalContext
#include "enums.h"

using std::unordered_map;

// wrapper of android sensor types, to ensure compatibility with older and newer versions of Android
// note that we include only a subset of all the sensors supported by android, because many of them are not very 'useful'
// this is a special wrapper, that permits to print the ENUM ad to cycle via indices
ENUM(LDSP_sensor, short,
     accelerometer = ASENSOR_TYPE_ACCELEROMETER,
     magnetometer  = ASENSOR_TYPE_MAGNETIC_FIELD,
     gyroscope     = ASENSOR_TYPE_GYROSCOPE,
     light         = ASENSOR_TYPE_LIGHT,
     proximity     = ASENSOR_TYPE_PROXIMITY,
     count         = 5 // BE CAREFUL! this entry has to be updated manually and it does not matter if has same value as other entries
// minimum set of sensors, compatible all the way down to Android 4.1 Jelly Bean, API level 16
//TODO extend this according to API LEVEL
)
// number of input channels associated with each sensor
// BE CAREFUL! this has to be updated manually, according to:
// LDSP_sensor ENUMs [same number of entries, sam order]
// AND
// sensorChannel enum in LDSP.h [each LDSP_sensor entry has to correspond to 1 or mulitple sensorChannel enum entry, according to number of channels set in here]
static const string sensors_channelsInfo[LDSP_sensor::count][2] = {
    {"3", "acceleration on x [m/s^2], acceleration on y [m/s^2], acceleration on z [m/s^2]"}, // accelerometer
    {"3", "magentic field on x [uT], magentic field on y [uT], magentic field on z [uT]"}, // magnetometer
    {"3", "rate of rotation around x [rad/s], rate of rotation around y [rad/s], rate of rotation around z [rad/s]"}, // gyroscope
    {"1", "illuminance [lx]"}, // light
    {"1", "distance [cm]"}  // proximity
};
//VIC any ways to retrieve number of channels per sensor from freaking android API?!?!?

// if max values are reported in here, the sensor input is normalized
// BE CAREFUL! this has to updated manually, according to LDSP_sensor ENUMs
// static const float sensors_max[LDSP_sensor::count] = {
//     2*9.8,  // 2g [m/s^2]
//     1000,   // 1000 uT
//     -1,     // no max, no normalization
//     -1,
//     10
// };


struct sensor_struct {
  const ASensor *asensor;
  bool present;
  unsigned int type;
  unsigned int numOfChannels;
  sensorChannel *channels;
};

struct LDSPsensorsContext {
  unsigned int sensorsCount = 0;
  sensor_struct sensors[LDSP_sensor::count];
  unordered_map<int, int> sensorsType_index; // automatically populated
  float *sensorBuffer;
  bool *sensorSupported;
  string *sensorsDetails;
};

void readSensors();

#endif //LDSP_LITE_LDSP_LITE_SRC_MAIN_CPP_INCLUDE_SENSORS_H_
