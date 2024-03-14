#include "OrtModel.h"
#include <iostream>
#include <thread>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>
#include "files_utils.h"
#include <iostream>
#include <fstream>
#include "nnapi_provider_factory.h"
//#include <priority_utils.h>

// thread, priority and niceness
#include <sched.h>
#include <sys/resource.h>
#include <unistd.h> // getpid

#include <numeric> // std::accumulate()

Ort::RunOptions options;

std::vector<std::thread> threads;
std::vector<pthread_t> pthreads;

Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
void* custom_thread_creation_options = nullptr;


// On thread pool creation, ORT calls CreateThreadCustomized to create a thread
OrtCustomThreadHandle CreateThreadCustomized(void* custom_thread_creation_options, OrtThreadWorkerFn work_loop, void* param) {


//  set_priority(0, true);
  // set_niceness(-20, true);

  threads.push_back(std::thread(work_loop, param));
  return reinterpret_cast<OrtCustomThreadHandle>(threads.back().native_handle());


  // int sched = SCHED_FIFO;
  // struct sched_param params;
  // params.sched_priority = 45;


  // std::thread t(work_loop, param);
  // configure the thread by custom_thread_creation_options


  // pthread_t pt;
  // // pt = pthread_create();



  // if (pthread_create(&pt, NULL, (void *(*)(void *)) work_loop, param) != 0) {
  //     perror("pthread_create() error");
  //     exit(1);
  // }

  // pthread_getschedparam(pt, &sched, &params);

  // pthreads.push_back(pt);

  // if (pthread_setschedparam(pt, sched, &params))
  //     std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';

  // return reinterpret_cast<OrtCustomThreadHandle>(pt);
}

// On thread pool destruction, ORT calls JoinThreadCustomized for each created thread
void JoinThreadCustomized(OrtCustomThreadHandle handle) {

  for (auto& t : threads) {
    if (reinterpret_cast<OrtCustomThreadHandle>(t.native_handle()) == handle) {
      // recycling resources ...
      t.join();

    }
  }
}

template <typename T>
T vectorProduct(const std::vector<T> &v) {
  return std::accumulate(v.begin(), v.end(), 1, std::multiplies<T>());
}

bool OrtModel::setup(const char * _sessionName, const char * _modelPath) {

  unsigned int max_threads = std::thread::hardware_concurrency();

  LDSP_log("Max thread: %d\n", max_threads);

  this->modelPath = _modelPath;
  this->sessionName = _sessionName;

  Ort::SessionOptions sessionOptions;
  // sessionOptions.SetIntraOpNumThreads(max_threads);
  // sessionOptions.SetInterOpNumThreads(1);
  sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
  sessionOptions.EnableCpuMemArena();

  // Thread Management
  sessionOptions.SetCustomCreateThreadFn(CreateThreadCustomized);
  sessionOptions.SetCustomThreadCreationOptions(&custom_thread_creation_options);
  sessionOptions.SetCustomJoinThreadFn(JoinThreadCustomized);


  sessionOptions.AddConfigEntry("session.load_model_format", "ONNX");
  sessionOptions.AddConfigEntry("session.use_ort_model_bytes_directly", "1");

  auto content = readFile(this->modelPath);
  const void* onnxByteArray = reinterpret_cast<const void*>(content.data());
  size_t onnxByteArraySize = content.size() * sizeof(char);

  this->env = new Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, _sessionName);
  this->session = new Ort::Session(*env, onnxByteArray, onnxByteArraySize, sessionOptions);

  Ort::AllocatorWithDefaultOptions allocator;

  LDSP_log("\nLoaded Model!!\n");
  // Get number of inputs/outputs to the model
  numInputNodes = this->session->GetInputCount();
  numOutputNodes = this->session->GetOutputCount();

  LDSP_log("Model Dimensions:\n");
  LDSP_log("---Input: %zu\n", numInputNodes);
  LDSP_log("---Output: %zu\n", numOutputNodes);


  // allocate space to hold names of model inputs
  inputNodeNames.resize(numInputNodes);
  inputNodeDims.resize(numInputNodes);
  // Gather metadata information about the model outputs
  for (int i = 0; i < numInputNodes; i++) {

    LDSP_log("==================================\n");

    // Get names of each input node
    Ort::AllocatedStringPtr inputName = session->GetInputNameAllocated(i, allocator);
    LDSP_log("Input %d : name=%s\n", i, inputName.get());
//    LDS
    inputNodeNames[i] = inputName.get();
    inputName.release();

    // Get Data type of each input node
    Ort::TypeInfo type_info = session->GetInputTypeInfo(i);
    auto tensorInfo = type_info.GetTensorTypeAndShapeInfo();

    ONNXTensorElementDataType type = tensorInfo.GetElementType();
    LDSP_log("Input %d : type=%d\n", i, type);

    // Get shapes of input tensors
    inputNodeDims[i] = tensorInfo.GetShape();
    LDSP_log("Input %d : num_dims=%zu\n", i, inputNodeDims.size());
    for (int j = 0; j < inputNodeDims[i].size(); j++) {
      LDSP_log("Input %d : dim %d=%lld\n", i, j, inputNodeDims[i][j]);
      bool hasVariableSize = (int) this->inputNodeDims[i][j] < (int) 0;
      if (hasVariableSize) {
        LDSP_log("Input %d, dim %d has a variable size, forcing a size of 1", i,j);
        inputNodeDims[i][j] = inputNodeDims[i][j]*-1;
      }
    }

    inputTensorSizes.push_back(vectorProduct(inputNodeDims[i]));
    inputTensorValues.push_back(std::vector<float>(inputTensorSizes[i]));
  }


  // allocate space to hold names of model outputs
  outputNodeNames.resize(numOutputNodes);
  outputNodeDims.resize(numOutputNodes);
  // Gather metadata information about the model outputs
  for (int i = 0; i < numOutputNodes; i++) {

    LDSP_log("==================================\n");

    // Get names of each output node
    Ort::AllocatedStringPtr outputName = session->GetOutputNameAllocated(i, allocator);
    LDSP_log("Output %d : name=%s\n", i, outputName.get());
    outputNodeNames[i] = outputName.get();
    outputName.release();

    // Get Data type of each output node
    Ort::TypeInfo typeInfo = session->GetOutputTypeInfo(i);
    auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

    ONNXTensorElementDataType type = tensorInfo.GetElementType();
    LDSP_log("Output %d : type=%d\n", i, type);

    // Get shapes of output tensors
    outputNodeDims[i] = tensorInfo.GetShape();
    LDSP_log("Output %d : num_dims=%zu\n", i, outputNodeDims.size());
    for (int j = 0; j < outputNodeDims.size(); j++) {
      LDSP_log("Output %d : dim %d=%lld\n", i, j, outputNodeDims[i][j]);
      bool hasVariableSize = (int) this->outputNodeDims[i][j] < (int) 0;
      if (hasVariableSize) {
        LDSP_log("Output %d, dim %d has a variable size, forcing a size of 1", i,j);
        outputNodeDims[i][j] = outputNodeDims[i][j]*-1;
      }
    }

    outputTensorSizes.push_back(vectorProduct(outputNodeDims[i]));
    outputTensorValues.push_back(std::vector<float>(outputTensorSizes[i]));
  }

  LDSP_log("\n");

  for (int i = 0; i < numInputNodes; i++) {
    inputTensors.push_back(Ort::Value::CreateTensor<float>(
                               memoryInfo,
                               inputTensorValues[i].data(),
                               inputTensorValues[i].size(),
                               inputNodeDims[i].data(),
                            inputNodeDims[i].size()));
  }
  for (int i = 0; i < numOutputNodes; i++) {
    outputTensors.push_back(Ort::Value::CreateTensor<float>(
        memoryInfo,
        outputTensorValues[i].data(),
        outputTensorValues[i].size(),
        outputNodeDims[i].data(),
        outputNodeDims[i].size()));
  }


  return true;
}


//std::vector<float> OrtModel::run(std::vector<std::vector<float>> inputs) {
//
//  // Copy Inputs
//  for (int i = 0; i < numInputNodes; i++) {
//    for (int j = 0; j < inputTensorSizes[i]; j++) {
//      inputTensorValues[i][j] = inputs[i][j];
//    }
//  }
//
//  // Run Inference
//  this->session->Run(
//      options,
//      inputNodeNames.data(),
//      inputTensors.data(),
//      inputTensors.size(),
//      outputNodeNames.data(),
//      outputTensors.data(),
//      1);
//
//  std::vector<float> out = {1.0f};
//  return out;
//}

void OrtModel::run(std::vector<std::vector<float>> inputs, float * output) {

  // Copy Inputs
  for (int i = 0; i < numInputNodes; i++) {
    for (int j = 0; j < inputTensorSizes[i]; j++) {
      inputTensorValues[i][j] = inputs[i][j];
    }
  }

  // Run Inference
  this->session->Run(
      options,
      inputNodeNames.data(),
      inputTensors.data(),
      inputTensors.size(),
      outputNodeNames.data(),
      outputTensors.data(),
      1);

  for (int i = 0; i < outputTensorSizes[0]; i++) {
    output[i] = outputTensorValues[0][i];
  }
}

std::vector<std::vector<float>> OrtModel::randInputs() {
  std::vector<std::vector<float>> inputs;
  int numInputs = this->session->GetInputCount();
  for (int i = 0; i < numInputs; i++) {
    std::vector<float> inputVec;
    int length = this->inputNodeDims[i][1];
    for (int j = 0; j < length; j++) {
      float val = (rand()%100)/100.0f;
      inputVec.push_back(val);
    }
    inputs.push_back(inputVec);
  }
  return inputs;
}
