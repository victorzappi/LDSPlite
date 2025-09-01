package com.ldsp.ldsplite

import android.content.Context
import androidx.lifecycle.DefaultLifecycleObserver
import androidx.lifecycle.LifecycleOwner
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

class NativeLDSPlite(context: Context) : LDSPlite, DefaultLifecycleObserver {

  private var ldspLiteHandle: Long = 0
  private val ldspLiteMutex = Object()
  private external fun create(): Long
  private external fun delete(ldspLiteHandle: Long)
  private external fun start(ldspLiteHandle: Long)
  private external fun stop(ldspLiteHanlde: Long)
  private external fun isPlaying(ldspLiteHanlde: Long): Boolean
  private external fun setSlider0(ldspLiteHanlde: Long, value: Float)
  private external fun setSlider1(ldspLiteHanlde: Long, value: Float)
  private external fun setSlider2(ldspLiteHanlde: Long, value: Float)
  private external fun setSlider3(ldspLiteHanlde: Long, value: Float)

  private external fun storeInstanceInNative(instance: NativeLDSPlite)
  private external fun storeContextInNative(context: Context)
  private external fun storeClassLoader(classLoader: ClassLoader)
  external fun readFileFromAssets(context: Context, path: String): ByteArray

  external fun updateTouch(
    ldspLiteHanlde: Long,
    slot: Int,
    id: Int,
    x: Float,
    y: Float,
    pressure: Float,
    majAxis: Float,
    minAxis: Float,
    orientation: Float,
    majWidth: Float,
    minWidth: Float
  )

  external fun updateHover(ldspLiteHanlde: Long, slot: Int, hoverX: Float, hoverY: Float)
  external fun clearTouch(ldspLiteHanlde: Long, slot: Int)
  external fun setScreenResolution(ldspLiteHanlde: Long, width: Float, height: Float)
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

    synchronized(ldspLiteMutex) {
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

    synchronized(ldspLiteMutex) {

      if (ldspLiteHandle == 0L)
        return

      // Destroy the LDSPlite
      delete(ldspLiteHandle)
      ldspLiteHandle = 0L
    }

    //Log.d("NativeLDSPlite", "_________onDestroy() called")

    super.onDestroy(owner)
  }



  override suspend fun start() = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      start(ldspLiteHandle)
    }
  }

  override suspend fun stop() = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      stop(ldspLiteHandle)
    }
  }

  override suspend fun isPlaying(): Boolean = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      return@withContext isPlaying(ldspLiteHandle)
    }
  }

  override suspend fun setSlider0(value: Float) = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      setSlider0(ldspLiteHandle, value)
    }
  }

  override suspend fun setSlider1(value: Float) = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      setSlider1(ldspLiteHandle, value)
    }
  }

  override suspend fun setSlider2(value: Float) = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      setSlider2(ldspLiteHandle, value)
    }
  }

  override suspend fun setSlider3(value: Float) = withContext(Dispatchers.Default) {
    synchronized(ldspLiteMutex) {
      createNativeHandleIfNotExists()
      setSlider3(ldspLiteHandle, value)
    }
  }

  private fun createNativeHandleIfNotExists() {
    if (ldspLiteHandle != 0L)
      return

    // create the LDSPlite
    ldspLiteHandle = create()
  }

  fun updateTouch(
    slot: Int,
    id: Int,
    x: Float,
    y: Float,
    pressure: Float,
    majAxis: Float,
    minAxis: Float,
    orientation: Float,
    majWidth: Float,
    minWidth: Float
  ) {
    synchronized(ldspLiteMutex) {
      if (ldspLiteHandle != 0L) {
        updateTouch(ldspLiteHandle, slot, id, x, y, pressure,
          majAxis, minAxis, orientation, majWidth, minWidth)
      }
    }
  }

  fun updateHover(slot: Int, hoverX: Float, hoverY: Float) {
    synchronized(ldspLiteMutex) {
      if (ldspLiteHandle != 0L) {
        updateHover(ldspLiteHandle, slot, hoverX, hoverY)
      }
    }
  }

  fun clearTouch(slot: Int) {
    synchronized(ldspLiteMutex) {
      if (ldspLiteHandle != 0L) {
        clearTouch(ldspLiteHandle, slot)
      }
    }
  }

  fun setScreenResolution(width: Float, height: Float) {
    synchronized(ldspLiteMutex) {
      if (ldspLiteHandle != 0L) {
        setScreenResolution(ldspLiteHandle, width, height)
      }
    }
  }
}
