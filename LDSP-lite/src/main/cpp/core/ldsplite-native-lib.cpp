#include <jni.h>
#include <memory>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "LDSP_log.h"
#include "LDSPlite.h"

JavaVM* g_JavaVM = nullptr;
jobject g_NativeLDSPliteInstance = nullptr; // Global reference to the NativeLDSPlite instance
jobject g_context = nullptr; // Global reference to the NativeLDSPlite context
jobject g_classLoader = nullptr;

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  g_JavaVM = vm;
  return JNI_VERSION_1_6; // Return the JNI version
}

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
Java_com_ldsp_ldsplite_NativeLDSPlite_storeInstanceInNative(
    JNIEnv *env, jobject,
    jobject instance) {
  if (g_NativeLDSPliteInstance) {
    env->DeleteGlobalRef(g_NativeLDSPliteInstance);
  }
  g_NativeLDSPliteInstance = env->NewGlobalRef(instance);
}

JNIEXPORT void JNICALL Java_com_ldsp_ldsplite_NativeLDSPlite_storeContextInNative(
    JNIEnv *env,
    jobject obj,
    jobject context) {
  if (g_context) {
    env->DeleteGlobalRef(g_context);
  }
  // Create a global reference to the context
  g_context = env->NewGlobalRef(context);
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_storeClassLoader(
    JNIEnv *env,
    jobject,
    jobject classLoader) {
  if (g_classLoader) {
    env->DeleteGlobalRef(g_classLoader);
  }
  g_classLoader = env->NewGlobalRef(classLoader);
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
//    LDSP_log("Attempt to destroy an unitialized synthesizer.");
    return;
  }

  delete synthesizer;
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_start(
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

JNIEXPORT jbyteArray JNICALL Java_com_ldsp_ldsplite_NativeLDSPlite_readFileFromAssets(
    JNIEnv* env,
    jobject obj,
    jobject context,
    jstring path) {

  // Convert jstring to C++ string
  const char* cPath = env->GetStringUTFChars(path, nullptr);
  if (!cPath) {
    // GetStringUTFChars returned null; out of memory or string is null
    return nullptr;
  }
  std::string cppPath(cPath);
  env->ReleaseStringUTFChars(path, cPath);

  // Get the AssetManager from the Context object
  jclass contextClass = env->GetObjectClass(context);
  if (!contextClass) {
    // Failed to find the class of the context object
    return nullptr;
  }

  jmethodID midGetAssets = env->GetMethodID(contextClass, "getAssets", "()Landroid/content/res/AssetManager;");
  if (!midGetAssets) {
    // Failed to find the getAssets method ID
    return nullptr;
  }

  jobject assetManager = env->CallObjectMethod(context, midGetAssets);
  if (!assetManager) {
    // Failed to get the AssetManager object
    return nullptr;
  }

  AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
  if (!mgr) {
    // Failed to get the AAssetManager
    return nullptr;
  }

  AAsset* asset = AAssetManager_open(mgr, cppPath.c_str(), AASSET_MODE_UNKNOWN);
  if (!asset) {
    // Asset not found or could not be opened
    return nullptr;
  }

  // Read asset data into a byte array
  off_t length = AAsset_getLength(asset);
  jbyte* buffer = new jbyte[length];
  int numBytesRead = AAsset_read(asset, buffer, length);
  AAsset_close(asset);

  if (numBytesRead != length) {
    // Failed to read the entire asset
    delete[] buffer;
    return nullptr;
  }

  // Create a jbyteArray to return
  jbyteArray result = env->NewByteArray(length);
  if (!result) {
    // Failed to allocate byte array
    delete[] buffer;
    return nullptr;
  }

  env->SetByteArrayRegion(result, 0, length, buffer);

  // Clean up
  delete[] buffer;

  return result;
}
}



extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider0(JNIEnv *env,
                                                jobject thiz,
                                                jlong synthesizerHandle,
                                                jfloat value) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);
  const auto nativeValue = static_cast<float>(value);

  if (synthesizer) {
    synthesizer->setSlider0(nativeValue);
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider1(JNIEnv *env,
                                                jobject thiz,
                                                jlong synthesizerHandle,
                                                jfloat value) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);
  const auto nativeValue = static_cast<float>(value);

  if (synthesizer) {
    synthesizer->setSlider1(nativeValue);
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider2(JNIEnv *env,
                                                jobject thiz,
                                                jlong synthesizerHandle,
                                                jfloat value) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);
  const auto nativeValue = static_cast<float>(value);

  if (synthesizer) {
    synthesizer->setSlider2(nativeValue);
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider3(JNIEnv *env,
                                                jobject thiz,
                                                jlong synthesizerHandle,
                                                jfloat value) {
  auto* synthesizer =
      reinterpret_cast<ldsplite::LDSPlite*>(
          synthesizerHandle);
  const auto nativeValue = static_cast<float>(value);

  if (synthesizer) {
    synthesizer->setSlider3(nativeValue);
  } else {
    LDSP_log(
        "Synthesizer not created. Please, create the synthesizer first by "
        "calling create().");
  }
}

