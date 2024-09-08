package com.ldsp.ldsplite

import android.content.Context
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.LifecycleOwner
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

class NativeLDSPlite(context: Context) : LDSPlite, DefaultLifecycleObserver {

  private var synthesizerHandle: Long = 0
  private val synthesizerMutex = Object()
  private external fun create(): Long
  private external fun delete(synthesizerHandle: Long)
  private external fun start(synthesizerHandle: Long)
  private external fun stop(synthesizerHandle: Long)
  private external fun isPlaying(synthesizerHandle: Long): Boolean
  private external fun setSlider0(synthesizerHandle: Long, value: Float)
  private external fun setSlider1(synthesizerHandle: Long, value: Float)
  private external fun setSlider2(synthesizerHandle: Long, value: Float)
  private external fun setSlider3(synthesizerHandle: Long, value: Float)

  private external fun storeInstanceInNative(instance: NativeLDSPlite)
  private external fun storeContextInNative(context: Context)
  private external fun storeClassLoader(classLoader: ClassLoader)
  external fun readFileFromAssets(context: Context, path: String): ByteArray

  init {
    // Store the instance in the native code when this object is created
    storeInstanceInNative(this)
    storeContextInNative(context)
    storeClassLoader(this.javaClass.classLoader) //VIC this triggers warning: Type mismatch: inferred type is ClassLoader? but ClassLoader was expected
  }

  companion object {
    init {
      System.loadLibrary("ldsplite")
    }
  }

  override fun onResume(owner: LifecycleOwner) {
    super.onResume(owner)

    //Log.d("NativeLDSPlite", "_________onResume() called")

    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
    }
  }

//  override fun onPause(owner: LifecycleOwner) {
//    super.onPause(owner)
//    Log.d("NativeLDSPlite", "_________onPause() called")
//  }

//  override fun onStop(owner: LifecycleOwner) {
//    super.onStop(owner)
//
//    //Log.d("NativeLDSPlite", "_________onStop() called")
//
//  }

  override fun onDestroy(owner: LifecycleOwner) {

    synchronized(synthesizerMutex) {

      if (synthesizerHandle == 0L)
        return

      // Destroy the synthesizer
      delete(synthesizerHandle)
      synthesizerHandle = 0L
    }

    //Log.d("NativeLDSPlite", "_________onDestroy() called")

    super.onDestroy(owner)
  }



  override suspend fun start() = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      start(synthesizerHandle)
    }
  }

  override suspend fun stop() = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      stop(synthesizerHandle)
    }
  }

  override suspend fun isPlaying(): Boolean = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      return@withContext isPlaying(synthesizerHandle)
    }
  }

  override suspend fun setSlider0(value: Float) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setSlider0(synthesizerHandle, value)
    }
  }

  override suspend fun setSlider1(value: Float) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setSlider1(synthesizerHandle, value)
    }
  }

  override suspend fun setSlider2(value: Float) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setSlider2(synthesizerHandle, value)
    }
  }

  override suspend fun setSlider3(value: Float) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setSlider3(synthesizerHandle, value)
    }
  }

  private fun createNativeHandleIfNotExists() {
    if (synthesizerHandle != 0L)
      return

    // create the synthesizer
    synthesizerHandle = create()
  }
}
