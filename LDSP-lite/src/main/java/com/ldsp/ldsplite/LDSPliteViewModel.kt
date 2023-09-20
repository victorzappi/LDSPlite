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
  val audioPermissionGranted: LiveData<Boolean> get() = _audioPermissionGranted

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



  var LDSPlite: LDSPlite? = null
    set(value) {
      field = value
      applyParameters()
    }

  private val _frequency = MutableLiveData(300f)
  val frequency: LiveData<Float>
    get() {
      return _frequency
    }
  private val frequencyRange = 40f..3000f

  private val _volume = MutableLiveData(-24f)
  val volume: LiveData<Float>
    get() {
      return _volume
    }
  val volumeRange = (-60f)..0f

  private var wavetable = Wavetable.SINE

  /**
   * @param frequencySliderPosition slider position in [0, 1] range
   */
  fun setFrequencySliderPosition(frequencySliderPosition: Float) {
    val frequencyInHz = frequencyInHzFromSliderPosition(frequencySliderPosition)
    _frequency.value = frequencyInHz
    viewModelScope.launch {
      LDSPlite?.setFrequency(frequencyInHz)
    }
  }

  fun setVolume(volumeInDb: Float) {
    _volume.value = volumeInDb
    viewModelScope.launch {
      LDSPlite?.setVolume(volumeInDb)
    }
  }

  fun setWavetable(newWavetable: Wavetable) {
    wavetable = newWavetable
    viewModelScope.launch {
      LDSPlite?.setWavetable(newWavetable)
    }
  }

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
          LDSPlite?.play()
        }
        // Only when the synthesizer changed its state, update the button label.
        updatePlayButtonLabel()
      }
    }

  }

  private fun frequencyInHzFromSliderPosition(sliderPosition: Float): Float {
    val rangePosition = linearToExponential(sliderPosition)
    return valueFromRangePosition(frequencyRange, rangePosition)
  }

  fun sliderPositionFromFrequencyInHz(frequencyInHz: Float): Float {
    val rangePosition = rangePositionFromValue(frequencyRange, frequencyInHz)
    return exponentialToLinear(rangePosition)
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

  private val _playButtonLabel = MutableLiveData(R.string.play)
  val playButtonLabel: LiveData<Int>
    get() {
      return _playButtonLabel
    }

  fun applyParameters() {
    viewModelScope.launch {
      LDSPlite?.setFrequency(frequency.value!!)
      LDSPlite?.setVolume(volume.value!!)
      LDSPlite?.setWavetable(wavetable)
      updatePlayButtonLabel()
    }
  }

  private fun updatePlayButtonLabel() {
    viewModelScope.launch {
      if (LDSPlite?.isPlaying() == true) {
        _playButtonLabel.value = R.string.stop
      } else {
        _playButtonLabel.value = R.string.play
      }
    }
  }
}

