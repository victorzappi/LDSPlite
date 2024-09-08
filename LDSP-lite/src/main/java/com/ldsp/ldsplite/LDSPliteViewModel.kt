package com.ldsp.ldsplite

import androidx.lifecycle.*
import kotlinx.coroutines.launch
import kotlin.math.exp
import kotlin.math.ln
import kotlin.coroutines.Continuation
import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine


class LDSPliteViewModel : ViewModel() {

  // LiveData to signal the Activity when to request permission
  private val _requestPermissionEvent = MutableLiveData<Boolean>()
  val requestPermissionEvent: LiveData<Boolean> get() = _requestPermissionEvent

  // LiveData to represent the state of the audio permission
  private val _audioPermissionGranted = MutableLiveData<Boolean>()
  private val _writePermissionGranted = MutableLiveData<Boolean>()
  val audioPermissionGranted: LiveData<Boolean> get() = _audioPermissionGranted
  val writePermissionGranted: LiveData<Boolean> get() = _writePermissionGranted

  // Call this method when you want to request permission
  fun requestPermission() {
    _requestPermissionEvent.value = true
  }

  // Handle the result of the permission request
  fun handlePermissionResult(isGranted: Boolean) {
    _audioPermissionGranted.value = isGranted
    //    if (isGranted) {
    //      // Handle granted permission
    //      // For example, you might start some audio processing here
    //    } else {
    //      // Handle denied permission
    //      // You might show a message to the user or disable certain features
    //    }
  }

  // This method can be used to set the result of the audio permission check
  // This is useful if you want to check the permission state outside of the permission request flow
  fun setAudioPermissionResult(granted: Boolean) {
    _audioPermissionGranted.value = granted
  }

  fun setWritePermissionResult(granted: Boolean) {
    _writePermissionGranted.value = granted
  }




  var LDSPlite: LDSPlite? = null
    set(value) {
      field = value
      applySliders()
    }

  private val _frequency = MutableLiveData(0f)
  val frequency: LiveData<Float>
    get() {
      return _frequency
    }
  private val frequencyRange = 40f..3000f

  private val _slider0 = MutableLiveData(0f)
  val slider0: LiveData<Float>
    get() {
      return slider0
    }
  private val _slider1 = MutableLiveData(0f)
  val slider1: LiveData<Float>
    get() {
      return slider1
    }
  private val _slider2 = MutableLiveData(0f)
  val slider2: LiveData<Float>
    get() {
      return slider2
    }
  private val _slider3 = MutableLiveData(0f)
  val slider3: LiveData<Float>
    get() {
      return slider3
    }


  fun setSlider0(value: Float) {
    _slider0.value = value
    viewModelScope.launch {
      LDSPlite?.setSlider0(value)
    }
  }

  fun setSlider1(value: Float) {
    _slider1.value = value
    viewModelScope.launch {
      LDSPlite?.setSlider1(value)
    }
  }

  fun setSlider2(value: Float) {
    _slider2.value = value
    viewModelScope.launch {
      LDSPlite?.setSlider2(value)
    }
  }

  fun setSlider3(value: Float) {
    _slider3.value = value
    viewModelScope.launch {
      LDSPlite?.setSlider3(value)
    }
  }


  val setSliderFunctions: Array<(Float) -> Unit> = arrayOf(
    ::setSlider0,
    ::setSlider1,
    ::setSlider2,
    ::setSlider3
  )

  private suspend fun ensureAudioPermissionGranted(): Boolean {
    if (audioPermissionGranted.value == true) {
      return true
    }

    // Trigger the permission request
    requestPermission()

    // Wait for permission result
    return suspendCoroutine { continuation: Continuation<Boolean> ->
      audioPermissionGranted.observeForever(object : Observer<Boolean> {
        override fun onChanged(isGranted: Boolean) {
          audioPermissionGranted.removeObserver(this)
          continuation.resume(isGranted)
        }
      })
    }
  }


  fun playClicked() {
    viewModelScope.launch {
      if (!ensureAudioPermissionGranted()) {
        // If permission is not granted, return
        return@launch
      }

      // Continue with your playback logic here
      // start() and stop() are suspended functions => we must launch a coroutine
      viewModelScope.launch {
        if (LDSPlite?.isPlaying() == true) {
          LDSPlite?.stop()
        } else {
          LDSPlite?.start()
        }
        // Only when the synthesizer changed its state, update the button label.
        updateStartButtonLabel()
      }
    }

  }

  companion object LinearToExponentialConverter {

    private const val MINIMUM_VALUE = 0.001f
    fun linearToExponential(value: Float): Float {
      assert(value in 0f..1f)


      if (value < MINIMUM_VALUE) {
        return 0f
      }

      return exp(ln(MINIMUM_VALUE) - ln(MINIMUM_VALUE) * value)
    }

    fun valueFromRangePosition(range: ClosedFloatingPointRange<Float>, rangePosition: Float): Float {
      assert(rangePosition in 0f..1f)

      return range.start + (range.endInclusive - range.start) * rangePosition
    }


    fun rangePositionFromValue(range: ClosedFloatingPointRange<Float>, value: Float): Float {
      assert(value in range)

      return (value - range.start) / (range.endInclusive - range.start)
    }


    fun exponentialToLinear(rangePosition: Float): Float {
      assert(rangePosition in 0f..1f)

      if (rangePosition < MINIMUM_VALUE) {
        return rangePosition
      }

      return (ln(rangePosition) - ln(MINIMUM_VALUE)) / (-ln(MINIMUM_VALUE))
    }
  }

  private val _startButtonLabel = MutableLiveData(R.string.start)
  val startButtonLabel: LiveData<Int>
    get() {
      return _startButtonLabel
    }

  fun applySliders() {
    viewModelScope.launch {
      LDSPlite?.setSlider0(_slider0.value!!)
      LDSPlite?.setSlider1(_slider1.value!!)
      LDSPlite?.setSlider2(_slider2.value!!)
      LDSPlite?.setSlider3(_slider3.value!!)

      updateStartButtonLabel()
    }
  }

  private fun updateStartButtonLabel() {
    viewModelScope.launch {
      if (LDSPlite?.isPlaying() == true) {
        _startButtonLabel.value = R.string.stop
      } else {
        _startButtonLabel.value = R.string.start
      }
    }
  }
}

