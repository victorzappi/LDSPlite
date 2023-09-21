#include <jni.h>
#include <memory>
#include "LDSP_log.h"
#include "LDSPlite.h"

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_create(
    JNIEnv* env,
    jobject obj) {
  auto synthesizer =
      std::make_unique<ldsplite::LDSPlite>();

  if (not synthesizer) {
    LDSP_log("Failed to create the synthesizer.");
    synthesizer.reset(nullptr);
  }

  return reinterpret_cast<jlong>(synthesizer.release());
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_delete(
    JNIEnv* env,
    jobject obj,
    jlong synthesizerHandle) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);

  if (not synthesizer) {
    LDSP_log("Attempt to destroy an unitialized synthesizer.");
    return;
  }

  delete synthesizer;
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_play(
    JNIEnv* env,
    jobject obj,
    jlong synthesizerHandle) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);

  if (synthesizer) {
    synthesizer->start();
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_stop(
    JNIEnv* env,
    jobject obj,
    jlong synthesizerHandle) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);

  if (synthesizer) {
    synthesizer->stop();
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

JNIEXPORT jboolean JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_isPlaying(
    JNIEnv* env,
    jobject obj,
    jlong synthesizerHandle) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);

  if (not synthesizer) {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
    return false;
  }

  return synthesizer->isStarted();
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setFrequency(
    JNIEnv* env,
    jobject obj,
    jlong synthesizerHandle,
    jfloat frequencyInHz) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);
  const auto nativeFrequency = static_cast<float>(frequencyInHz);

  if (synthesizer) {
    //synthesizer->setFrequency(nativeFrequency);
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setVolume(
    JNIEnv* env,
    jobject obj,
    jlong synthesizerHandle,
    jfloat volumeInDb) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);
  const auto nativeVolume = static_cast<float>(volumeInDb);

  if (synthesizer) {
    //synthesizer->setVolume(nativeVolume);
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setWavetable(
        JNIEnv* env,
        jobject obj,
        jlong synthesizerHandle,
        jint wavetable) {
    auto* synthesizer =
            reinterpret_cast<ldsplite::LDSPlite*>(
                    synthesizerHandle);
    //const auto nativeWavetable = static_cast<ldsplite::Wavetable>(wavetable);

    if (synthesizer) {
        //synthesizer->setWavetable(nativeWavetable);
    } else {
        LDSP_log(
                "Synthesizer not created. Please, create the synthesizer first by "
                "calling create().");
    }
}
}
