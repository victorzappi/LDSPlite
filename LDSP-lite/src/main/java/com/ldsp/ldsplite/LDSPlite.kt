package com.ldsp.ldsplite

import androidx.annotation.StringRes

interface LDSPlite {
  suspend fun start()
  suspend fun stop()
  suspend fun isPlaying() : Boolean
  suspend fun setSlider0(value: Float)
  suspend fun setSlider1(value: Float)
  suspend fun setSlider2(value: Float)
  suspend fun setSlider3(value: Float)
}
