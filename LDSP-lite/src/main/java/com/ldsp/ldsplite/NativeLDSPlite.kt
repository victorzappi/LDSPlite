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
  private external fun play(synthesizerHandle: Long)
  private external fun stop(synthesizerHandle: Long)
  private external fun isPlaying(synthesizerHandle: Long): Boolean
  private external fun setFrequency(synthesizerHandle: Long, frequencyInHz: Float)
  private external fun setVolume(synthesizerHandle: Long, amplitudeInDb: Float)
  private external fun setWavetable(synthesizerHandle: Long, wavetable: Int)
  private external fun storeInstanceInNative(instance: NativeLDSPlite)
  private external fun storeContextInNative(context: Context)
  private external fun storeClassLoader(classLoader: ClassLoader)
  external fun readFileFromAssets(context: Context, path: String): ByteArray

  init {
    // Store the instance in the native code when this object is created
    storeInstanceInNative(this)
    storeContextInNative(context)
    storeClassLoader(this.javaClass.classLoader)
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



  override suspend fun play() = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      play(synthesizerHandle)
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

  override suspend fun setFrequency(frequencyInHz: Float) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setFrequency(synthesizerHandle, frequencyInHz)
    }
  }

  override suspend fun setVolume(volumeInDb: Float) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setVolume(synthesizerHandle, volumeInDb)
    }
  }

  override suspend fun setWavetable(wavetable: Wavetable) = withContext(Dispatchers.Default) {
    synchronized(synthesizerMutex) {
      createNativeHandleIfNotExists()
      setWavetable(synthesizerHandle, wavetable.ordinal)
    }
  }

  private fun createNativeHandleIfNotExists() {
    if (synthesizerHandle != 0L)
      return

    // create the synthesizer
    synthesizerHandle = create()
  }
}
