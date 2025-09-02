package com.ldsp.ldsplite

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.InputDevice
import android.view.MotionEvent
import android.view.View

class TouchSurfaceView @JvmOverloads constructor(
  context: Context,
  attrs: AttributeSet? = null,
  defStyleAttr: Int = 0
) : View(context, attrs, defStyleAttr) {

  private var nativeLDSP: NativeLDSPlite? = null

  fun setNativeLDSP(native: NativeLDSPlite) {
    nativeLDSP = native
  }

  override fun onTouchEvent(event: MotionEvent): Boolean {
    if (nativeLDSP == null) return false

    val pointerCount = event.pointerCount
    val action = event.actionMasked
    val actionIndex = event.actionIndex

    when (action) {
      MotionEvent.ACTION_DOWN -> {
        // First finger down - anyTouch becomes true
        nativeLDSP?.updateAnyTouch(1)

        val pointerId = event.getPointerId(actionIndex)
        val slot = findSlotForId(pointerId)
        if (slot >= 0) {
          updateTouchData(event, actionIndex, slot, pointerId)
        }
      }

      MotionEvent.ACTION_POINTER_DOWN -> {
        // Additional finger down - anyTouch already true, no need to update
        val pointerId = event.getPointerId(actionIndex)
        val slot = findSlotForId(pointerId)
        if (slot >= 0) {
          updateTouchData(event, actionIndex, slot, pointerId)
        }
      }

      MotionEvent.ACTION_MOVE -> {
        // Update all active pointers
        for (i in 0 until pointerCount) {
          val pointerId = event.getPointerId(i)
          val slot = findSlotForId(pointerId)
          if (slot >= 0) {
            updateTouchData(event, i, slot, pointerId)
          }
        }
      }

      MotionEvent.ACTION_POINTER_UP -> {
        // A finger up but others remain - anyTouch stays true
        val pointerId = event.getPointerId(actionIndex)
        val slot = findSlotForId(pointerId)
        if (slot >= 0) {
          nativeLDSP?.clearTouch(slot)
          releaseSlot(slot)
        }
      }

      MotionEvent.ACTION_UP -> {
        // Last finger up - anyTouch becomes false
        nativeLDSP?.updateAnyTouch(0)

        val pointerId = event.getPointerId(actionIndex)
        val slot = findSlotForId(pointerId)
        if (slot >= 0) {
          nativeLDSP?.clearTouch(slot)
          releaseSlot(slot)
        }
      }

      MotionEvent.ACTION_HOVER_MOVE -> {
        // Handle hover if device supports it (stylus, etc.)
        val slot = 0 // Use slot 0 for hover
        val hoverX = event.x
        val hoverY = event.y
        nativeLDSP?.updateHover(slot, hoverX, hoverY)
      }

      MotionEvent.ACTION_CANCEL -> {
        // System cancelled all touches - anyTouch becomes false
        nativeLDSP?.updateAnyTouch(0)
        
        // Clear all slots that we think are active, not just what's in the event
        for ((pointerId, slot) in slotMap.entries) {
          nativeLDSP?.clearTouch(slot)
        }
        // Now reset our tracking
        usedSlots.fill(false)
        slotMap.clear()
      }
    }

    return true
  }


  private fun updateTouchData(event: MotionEvent, pointerIndex: Int, slot: Int, pointerId: Int) {
    val x = event.getX(pointerIndex)
    val y = event.getY(pointerIndex)
    val pressure = event.getPressure(pointerIndex)

    // Get axis values if available
    val majAxis = if (event.device.motionRanges.any { it.axis == MotionEvent.AXIS_TOUCH_MAJOR }) {
      event.getTouchMajor(pointerIndex)
    } else 0f

    val minAxis = if (event.device.motionRanges.any { it.axis == MotionEvent.AXIS_TOUCH_MINOR }) {
      event.getTouchMinor(pointerIndex)
    } else 0f

    val orientation = if (event.device.motionRanges.any { it.axis == MotionEvent.AXIS_ORIENTATION }) {
      event.getOrientation(pointerIndex)
    } else 0f

    val majWidth = if (event.device.motionRanges.any { it.axis == MotionEvent.AXIS_TOOL_MAJOR }) {
      event.getToolMajor(pointerIndex)
    } else 0f

    val minWidth = if (event.device.motionRanges.any { it.axis == MotionEvent.AXIS_TOOL_MINOR }) {
      event.getToolMinor(pointerIndex)
    } else 0f

    // Send to native
    nativeLDSP?.updateTouch(
      slot, pointerId, x, y, pressure,
      majAxis, minAxis, orientation,
      majWidth, minWidth
    )
  }

  // Simple slot management - maps pointer IDs to slots
  private val slotMap = mutableMapOf<Int, Int>()
  private val usedSlots = BooleanArray(10) // Support up to 10 touches

  private fun findSlotForId(pointerId: Int): Int {
    // Check if we already have a slot for this ID
    slotMap[pointerId]?.let { return it }

    // Find first available slot
    for (i in usedSlots.indices) {
      if (!usedSlots[i]) {
        usedSlots[i] = true
        slotMap[pointerId] = i
        return i
      }
    }

    return -1 // No slots available
  }

  private fun releaseSlot(slot: Int) {
    if (slot >= 0 && slot < usedSlots.size) {
      usedSlots[slot] = false
      // API 22 compatible way to remove entries
      val iterator = slotMap.entries.iterator()
      while (iterator.hasNext()) {
        val entry = iterator.next()
        if (entry.value == slot) {
          iterator.remove()
        }
      }
    }
  }

  override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
    super.onSizeChanged(w, h, oldw, oldh)
    // Update screen resolution in native code
    nativeLDSP?.setScreenResolution(w.toFloat(), h.toFloat())
  }
}