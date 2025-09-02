package com.ldsp.ldsplite

import android.util.Log
import android.Manifest
import android.content.pm.ActivityInfo
import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.ui.viewinterop.AndroidView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.*
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.tooling.preview.Devices
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.content.ContextCompat
import androidx.lifecycle.viewmodel.compose.viewModel
import com.ldsp.ldsplite.ui.theme.LDSPliteTheme


class MainActivity : ComponentActivity() {

  private lateinit var nativeLDSP: NativeLDSPlite
  private val ldspViewModel: LDSPliteViewModel by viewModels()

  private val requestPermissionLauncher =
    registerForActivityResult(ActivityResultContracts.RequestPermission()) { isGranted: Boolean ->
      ldspViewModel.handlePermissionResult(isGranted)
    }


  override fun onCreate(savedInstanceState: Bundle?) {
    //Log.d("MainActivity", ">>>>>>onCreate() called")

    super.onCreate(savedInstanceState)

    // Initialize nativeLDSP with the context
    nativeLDSP = NativeLDSPlite(this)

    requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
    lifecycle.addObserver(nativeLDSP)
    // pass the nativeLDSP to the ViewModel
    ldspViewModel.LDSPlite = nativeLDSP
    setContent {
      LDSPliteTheme {
        Surface(modifier = Modifier.fillMaxSize(), color = MaterialTheme.colors.background) {
          // pass the ViewModel down the composables' hierarchy
          LDSPliteApp(Modifier, ldspViewModel, nativeLDSP)
        }
      }
    }

    // Initial permissions check
    checkAudioPermission()
    checkWritePermission()

    // Observe the LiveData from ViewModel to know when to request permission
    ldspViewModel.requestPermissionEvent.observe(this) { shouldRequest ->
      if (shouldRequest) {
        requestAudioPermission()
        requestWritePermission()
      }
    }
  }

  private fun checkAudioPermission() {
    val isGranted = ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED
    ldspViewModel.setAudioPermissionResult(isGranted)
  }

  private fun checkWritePermission() {
    val isGranted = ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
    ldspViewModel.setWritePermissionResult(isGranted)
  }


  private fun requestAudioPermission() {
    // Check the permission and update the ViewModel
    checkAudioPermission()

    // If permission is not granted, request it
    if (ldspViewModel.audioPermissionGranted.value != true) {
      requestPermissionLauncher.launch(Manifest.permission.RECORD_AUDIO)
    }
  }

  private fun requestWritePermission() {
    // Check the permission and update the ViewModel
    checkWritePermission()

    // If permission is not granted, request it
    if (ldspViewModel.writePermissionGranted.value != true) {
      requestPermissionLauncher.launch(Manifest.permission.WRITE_EXTERNAL_STORAGE)
    }
  }



  override fun onDestroy() {
    //Log.d("MainActivity", ">>>>>>onDestroy() called")
    super.onDestroy()
    lifecycle.removeObserver(nativeLDSP)
  }

  override fun onResume() {
    //Log.d("MainActivity", ">>>>>>onResume() called")
    super.onResume()
    ldspViewModel.applySliders()
  }
}

@Composable
fun LDSPliteApp(
  modifier: Modifier,
  ldspViewModel: LDSPliteViewModel = viewModel(),
  nativeLDSP: NativeLDSPlite
) {
  var currentView by remember { mutableStateOf("controls") }

  Column(modifier = modifier.fillMaxSize()) {
    // Tab buttons at the top
    Row(
      modifier = Modifier
        .fillMaxWidth()
        .padding(8.dp),
      horizontalArrangement = Arrangement.SpaceEvenly
    ) {
      Button(
        onClick = { currentView = "controls" },
        colors = ButtonDefaults.buttonColors(
          backgroundColor = if (currentView == "controls") MaterialTheme.colors.primary
          else MaterialTheme.colors.surface
        )
      ) {
        Text("Start/Stop")
      }
      Button(
        onClick = { currentView = "sliders" },
        colors = ButtonDefaults.buttonColors(
          backgroundColor = if (currentView == "sliders") MaterialTheme.colors.primary
          else MaterialTheme.colors.surface
        )
      ) {
        Text("Sliders")
      }
      Button(
        onClick = { currentView = "touch" },
        colors = ButtonDefaults.buttonColors(
          backgroundColor = if (currentView == "touch") MaterialTheme.colors.primary
          else MaterialTheme.colors.surface
        )
      ) {
        Text("Touch")
      }
    }

    // Show the selected view
    Box(modifier = Modifier.fillMaxSize()) {
      when (currentView) {
        "controls" -> {
          Column(
            modifier = Modifier.fillMaxSize(),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
          ) {
            StartPanel(Modifier, ldspViewModel)
          }
        }
        "sliders" -> {
          Column(
            modifier = Modifier.fillMaxSize(),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
          ) {
            ControlPanel(Modifier, ldspViewModel)
          }
        }
        "touch" -> {
          AndroidView(
            factory = { context ->
              TouchSurfaceView(context).apply {
                setNativeLDSP(nativeLDSP)
//                setBackgroundColor(Color.TRANSPARENT)
                setBackgroundColor(Color.parseColor("#10FF0000"))  // Slight red tint to see it
              }
            },
            modifier = Modifier.fillMaxSize()
          )
        }
      }
    }
  }
}
@Composable
private fun ControlPanel(
  modifier: Modifier,
  ldspViewModel: LDSPliteViewModel
) {
  Row(
    modifier = modifier
      .fillMaxWidth()
      .fillMaxHeight(0.7f),
    horizontalArrangement = Arrangement.SpaceEvenly,
    verticalAlignment = Alignment.CenterVertically
  ) {
    // First column with two sliders
    Column(
      modifier = modifier.weight(1f), // Equally distribute space between the two columns
      verticalArrangement = Arrangement.SpaceEvenly,
      horizontalAlignment = Alignment.CenterHorizontally
    ) {
      LDSPslider(modifier, ldspViewModel, 0)
      LDSPslider(modifier, ldspViewModel, 1)
    }

    // Second column with two sliders
    Column(
      modifier = modifier.weight(1f), // Equally distribute space between the two columns
      verticalArrangement = Arrangement.SpaceEvenly,
      horizontalAlignment = Alignment.CenterHorizontally
    ) {
      LDSPslider(modifier, ldspViewModel, 2)
      LDSPslider(modifier, ldspViewModel, 3)
    }
  }
}


@Composable
private fun StartPanel(
  modifier: Modifier,
  ldspViewModel: LDSPliteViewModel
) {
  Row(
    modifier = modifier
      .fillMaxWidth()
      .fillMaxHeight(),
    horizontalArrangement = Arrangement.Center,
    verticalAlignment = Alignment.CenterVertically
  ) {
    Column(
      modifier = modifier.fillMaxWidth(0.7f),
      horizontalAlignment = Alignment.CenterHorizontally
    ) {
      StartBUtton(modifier, ldspViewModel)
    }
  }
}

//TODO re-instate updated of the slider coming from the view model
// then
// update the sliders with the potential initial values set in render.cpp setup() function
// via new method LDSPlite::propagateSliderX()
@Composable
private fun LDSPslider(
  modifier: Modifier,
  ldspViewModel: LDSPliteViewModel,
  index: Int
) {
  // Get the function references from the ViewModel
  val selectedSetParamFunc = ldspViewModel.setSliderFunctions[index]

  val initialSliderPosition = 0f

  // Use a rememberSaveable state to manage the slider position locally
  val sliderPosition = rememberSaveable {
    mutableStateOf(initialSliderPosition)
  }

  // When the slider position changes, update both the local state and the ViewModel
  LDSPsliderContent(
    modifier = modifier,
    value = sliderPosition.value,
    onValueChange = {
      sliderPosition.value = it
      selectedSetParamFunc(it) // Update the ViewModel
    },
    index = index
  )
}

@Composable
private fun LDSPsliderContent(
  modifier: Modifier,
  value: Float,
  onValueChange: (Float) -> Unit,
  index: Int
) {
  // Update the slider label with the current index and value
  val sliderLabel = stringResource(R.string.param, index, value)
  Text(sliderLabel, modifier = modifier)
  Slider(
    modifier = modifier,
    value = value,
    onValueChange = onValueChange,
    valueRange = 0F..1F
  )
}


@Composable
private fun StartBUtton(modifier: Modifier, ldspViewModel: LDSPliteViewModel) {
  // The label of the start button is now an observable state, an instance of State<Int?>.
  // State<Int?> is used because the label is the id value of the resource string.
  // Thanks to the fact that the composable observes the label,
  // the composable will be recomposed (redrawn) when the observed state changes.
  val startButtonLabel = ldspViewModel.startButtonLabel.observeAsState()

  StartButtonContent(modifier = modifier,
    // onClick handler now simply notifies the ViewModel that it has been clicked
    onClick = {
      ldspViewModel.playClicked()
    },
    // startButtonLabel will never be null; if it is, then we have a serious implementation issue
    buttonLabel = stringResource(startButtonLabel.value!!))
}

@Composable
private fun StartButtonContent(modifier: Modifier, onClick: () -> Unit, buttonLabel: String) {
  Button(modifier = modifier,
    onClick = onClick) {
    Text(buttonLabel)
  }
}


//@Preview(showBackground = true, device = Devices.AUTOMOTIVE_1024p, widthDp = 1024, heightDp = 720)
//@Composable
//fun LDSPlitePreview() {
//  LDSPliteTheme {
//    LDSPliteApp(Modifier, LDSPliteViewModel())
//  }
//}


@Preview(showBackground = true, device = Devices.AUTOMOTIVE_1024p, widthDp = 1024, heightDp = 720)
@Composable
fun LDSPlitePreview() {
  // Mocking the ViewModel for preview purposes
  val mockViewModel = LDSPliteViewModel().apply {
    // You can initialize any required state here if needed for the preview
  }

  // Create a mock NativeLDSPlite for preview (won't actually work but allows preview)
  val context = LocalContext.current
  val mockNative = remember { NativeLDSPlite(context) }

  LDSPliteTheme {
    LDSPliteApp(Modifier, mockViewModel, mockNative)
  }
}
