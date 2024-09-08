#include "files_utils.h"
#include "LDSP_log.h"
#include <fstream>
#include <sstream>
#include <jni.h>

// Global references for JNI
extern JavaVM* g_JavaVM;
extern jobject g_NativeLDSPliteInstance;
extern jobject g_context;
extern jobject g_classLoader;


bool isFirstDirectorySdcard(const std::string& path) {
  std::istringstream iss(path);
  std::string directory;

  // Extract directories from the path
  std::vector<std::string> directories;
  while (std::getline(iss, directory, '/')) {
    directories.push_back(directory);
  }

  // Check if the first directory is "sdcard"
  if (directories.size()>=1 && directories[1] == "sdcard") {
    return true;
  } else {
    return false;
  }
}


std::vector<char> readFile(const std::string& path) {
  std::vector<char> content;

//  LDSP_log("============== path %s\n", path.c_str());

  if (!isFirstDirectorySdcard(path)) {

    // The path is an asset, call the Java method
    JNIEnv* env = nullptr;
    bool shouldDetach = false;

    // Check if the current thread is attached to the JVM
    jint getEnvStat = g_JavaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
      // Thread not attached, attach it
      if (g_JavaVM->AttachCurrentThread(&env, nullptr) != 0) {
//        LDSP_log("============== Failed to attach thread\n");
        return content; // Failed to attach thread
      }
      shouldDetach = true;
    } else if (getEnvStat == JNI_OK) {
      // Thread already attached, no need to detach later
//      LDSP_log("============== Thread already attached, no need to detach later\n");
    } else {
      // Failed to get JNIEnv
//      LDSP_log("============== Failed to get JNIEnv\n");
      return content;
    }

//    if(env == nullptr) {
//      LDSP_log("============== JNIEnv == nullptr\n");
//    }

    // Use the stored class loader to find the class
    jclass classLoaderClass = env->GetObjectClass(g_classLoader);
    jmethodID findClassMethod = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring className = env->NewStringUTF("com.ldsp.ldsplite.NativeLDSPlite");
    jobject result = env->CallObjectMethod(g_classLoader, findClassMethod, className);
    auto nativeLDSPliteClass = reinterpret_cast<jclass>(result);

    env->DeleteLocalRef(className);
    env->DeleteLocalRef(classLoaderClass);

    if (nativeLDSPliteClass == nullptr) {
//      LDSP_log("Failed to find NativeLDSPlite class.");
      if (shouldDetach) {
        g_JavaVM->DetachCurrentThread();
      }
      return content;
    }

    jmethodID readFileMethodID = env->GetMethodID(nativeLDSPliteClass, "readFileFromAssets",
                                                  "(Landroid/content/Context;Ljava/lang/String;)[B");
    if (readFileMethodID == nullptr) {
      env->DeleteLocalRef(nativeLDSPliteClass);
      LDSP_log("============== Method not found\n");
      if (shouldDetach) {
        g_JavaVM->DetachCurrentThread();
      }
      return content; // Method not found
    }

    // Convert path to jstring
    jstring jPath = env->NewStringUTF(path.c_str());

    // Call the Java method
    auto byteArray = (jbyteArray)env->CallObjectMethod(g_NativeLDSPliteInstance, readFileMethodID, g_context, jPath);

    if (byteArray != nullptr) {
      jsize length = env->GetArrayLength(byteArray);
      jbyte* data = env->GetByteArrayElements(byteArray, JNI_FALSE);
      if (data != nullptr) {
        content = std::vector<char>(data, data + length);
        env->ReleaseByteArrayElements(byteArray, data, JNI_ABORT);
      }
//      else
//        LDSP_log("============== data == nullptr\n");
      env->DeleteLocalRef(byteArray);
    }
//    else
//      LDSP_log("============== byteArray == nullptr\n");

    env->DeleteLocalRef(jPath);
    env->DeleteLocalRef(nativeLDSPliteClass);

    // Detach thread if it was attached in this function
    if (shouldDetach) {
      g_JavaVM->DetachCurrentThread();
    }
  } else {
    // Regular file system path handling
    std::ifstream file(path, std::ios::binary);
    if (file) {
      content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
  }

  return content;
}