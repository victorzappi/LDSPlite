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
  auto ldspLite =
      std::make_unique<ldsplite::LDSPlite>();

  if (not ldspLite) {
    LDSP_log("Failed to create LDSPlite.");
    ldspLite.reset(nullptr);
  }

  return reinterpret_cast<jlong>(ldspLite.release());
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
    jlong ldspLiteHandle) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (not ldspLite) {
//    LDSP_log("Attempt to destroy an unitialized LDSPlite.");
    return;
  }

  delete ldspLite;
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_start(
    JNIEnv* env,
    jobject obj,
    jlong ldspLiteHandle) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (ldspLite) {
    ldspLite->start();
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_stop(
    JNIEnv* env,
    jobject obj,
    jlong ldspLiteHandle) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (ldspLite) {
    ldspLite->stop();
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

JNIEXPORT jboolean JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_isPlaying(
    JNIEnv* env,
    jobject obj,
    jlong ldspLiteHandle) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (not ldspLite) {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
    return false;
  }

  return ldspLite->isStarted();
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
                                                jlong ldspLiteHandle,
                                                jfloat value) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);
  const auto nativeValue = static_cast<float>(value);

  if (ldspLite) {
    ldspLite->setSlider0(nativeValue);
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider1(JNIEnv *env,
                                                jobject thiz,
                                                jlong ldspLiteHandle,
                                                jfloat value) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);
  const auto nativeValue = static_cast<float>(value);

  if (ldspLite) {
    ldspLite->setSlider1(nativeValue);
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider2(JNIEnv *env,
                                                jobject thiz,
                                                jlong ldspLiteHandle,
                                                jfloat value) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);
  const auto nativeValue = static_cast<float>(value);

  if (ldspLite) {
    ldspLite->setSlider2(nativeValue);
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setSlider3(JNIEnv *env,
                                                jobject thiz,
                                                jlong ldspLiteHandle,
                                                jfloat value) {
  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);
  const auto nativeValue = static_cast<float>(value);

  if (ldspLite) {
    ldspLite->setSlider3(nativeValue);
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}


// multitouch
extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_updateTouch(
    JNIEnv* env,
    jobject thiz,
    jlong ldspLiteHandle,
    jint slot,
    jint id,
    jfloat x,
    jfloat y,
    jfloat pressure,
    jfloat majAxis,
    jfloat minAxis,
    jfloat orientation,
    jfloat majWidth,
    jfloat minWidth) {

  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (ldspLite) {
    ldspLite->updateTouch(
        static_cast<int>(slot),
        static_cast<int>(id),
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(pressure),
        static_cast<float>(majAxis),
        static_cast<float>(minAxis),
        static_cast<float>(orientation),
        static_cast<float>(majWidth),
        static_cast<float>(minWidth));
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_updateHover(
    JNIEnv* env,
    jobject thiz,
    jlong ldspLiteHandle,
    jint slot,
    jfloat hoverX,
    jfloat hoverY) {

  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (ldspLite) {
    ldspLite->updateHover(
        static_cast<int>(slot),
        static_cast<float>(hoverX),
        static_cast<float>(hoverY));
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_clearTouch(
    JNIEnv* env,
    jobject thiz,
    jlong ldspLiteHandle,
    jint slot) {

  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (ldspLite) {
    ldspLite->clearTouch(static_cast<int>(slot));
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ldsp_ldsplite_NativeLDSPlite_setScreenResolution(
    JNIEnv* env,
    jobject thiz,
    jlong ldspLiteHandle,
    jfloat width,
    jfloat height) {

  auto* ldspLite =
      reinterpret_cast<ldsplite::LDSPlite*>(
          ldspLiteHandle);

  if (ldspLite) {
    ldspLite->setScreenResolution(
        static_cast<float>(width),
        static_cast<float>(height));
  } else {
    LDSP_log(
        "LDSPlite not created. Please, create it first by "
        "calling create().");
  }
}

