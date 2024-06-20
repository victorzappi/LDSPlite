#include "RTNeuralLSTM.h"
#include "files_utils.h"
#include "LDSP_log.h"
#include <fstream> // ofstream

/**
 * Adapted from https://github.com/GuitarML/NeuralPi
 */

using Vec2d = std::vector<std::vector<float>>;

Vec2d transpose(const Vec2d& x)
{
    auto outer_size = x.size();
    auto inner_size = x[0].size();
    Vec2d y(inner_size, std::vector<float>(outer_size, 0.0f));

    for (size_t i = 0; i < outer_size; ++i)
    {
        for (size_t j = 0; j < inner_size; ++j)
            y[j][i] = x[i][j];
    }

    return y;
}

template <typename T1>
void RT_LSTM::set_weights(T1 model, const char* filename)
{
    // Initialize the correct model
    auto& lstm = (*model).template get<0>();
    auto& dense = (*model).template get<1>();

    // read a JSON file
    std::ifstream i2(filename);
    nlohmann::json weights_json;
    i2 >> weights_json;

    Vec2d lstm_weights_ih = weights_json["/state_dict/rec.weight_ih_l0"_json_pointer];
    lstm.setWVals(transpose(lstm_weights_ih));

    Vec2d lstm_weights_hh = weights_json["/state_dict/rec.weight_hh_l0"_json_pointer];
    lstm.setUVals(transpose(lstm_weights_hh));

    std::vector<float> lstm_bias_ih = weights_json["/state_dict/rec.bias_ih_l0"_json_pointer];
    std::vector<float> lstm_bias_hh = weights_json["/state_dict/rec.bias_hh_l0"_json_pointer];
    for (int i = 0; i < 80; ++i)
        lstm_bias_hh[i] += lstm_bias_ih[i];
    lstm.setBVals(lstm_bias_hh);

    Vec2d dense_weights = weights_json["/state_dict/lin.weight"_json_pointer];
    dense.setWeights(dense_weights);

    std::vector<float> dense_bias = weights_json["/state_dict/lin.bias"_json_pointer];
    dense.setBias(dense_bias.data());

}

void RT_LSTM::load_json(const char* filename)
{
    // Read in the JSON file

    // Read the file into a vector of chars
    std::vector<char> fileContent = readFile(filename);
    // Convert the trimmed vector of chars to a string
    std::string fileContentStr(fileContent.begin(), fileContent.end());

    //VIC the parser always throws:
    //  JSON Parse Error: [json.exception.parse_error.101] parse error at line 1, column 13: syntax error while parsing value - unexpected ':'; expected end of input
    //  Error occurred at byte position: 13
    // but the JSON file, the string and the hex dump of the string are all correct
    // and the parsed values look correct too!
    // hence, we ignore the exceptions
  try {
    // Parse the string content to create the JSON object
    nlohmann::json weights_json = nlohmann::json::parse(fileContentStr);

    // Output the parsed JSON for debugging purposes
    std::string parsedJsonStr = weights_json.dump(4);
    size_t chunkSize = 1024; // Define chunk size for logging
    for (size_t i = 0; i < parsedJsonStr.size(); i += chunkSize) {
      std::string chunk = parsedJsonStr.substr(i, chunkSize);
      LDSP_log("%s", chunk.c_str());
    }

    // Get the input size of the JSON file
	int input_size_json = weights_json["/model_data/input_size"_json_pointer];
	input_size = input_size_json;

    // Load the appropriate model
    if (input_size == 1) {
		set_weights(&model, filename);
    }
    else if (input_size == 2) {
		set_weights(&model_cond1, filename);
    }
    else if (input_size == 3) {
		set_weights(&model_cond2, filename);
    }
    // Now you can use weights_json as needed
  } catch (nlohmann::json::parse_error& e) {
    LDSP_log("JSON Parse Error: %s\n", e.what());
    LDSP_log("Error occurred at byte position: %zu\n", e.byte);
  }

}


void RT_LSTM::reset()
{
    if (input_size == 1) {
        model.reset();
    } else {
        model_cond1.reset();
    }
}

void RT_LSTM::process(const float* inData, float* outData, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
        outData[i] = model.forward(inData + i) + inData[i];
}

void RT_LSTM::process(const float* inData, float param, float* outData, int numSamples)
{
    for (int i = 0; i < numSamples; ++i) {
        inArray1[0] = inData[i];
        inArray1[1] = param;
        outData[i] = model_cond1.forward(inArray1) + inData[i];
    }
}

void RT_LSTM::process(const float* inData, float param1, float param2, float* outData, int numSamples)
{
    for (int i = 0; i < numSamples; ++i) {
        inArray2[0] = inData[i];
        inArray2[1] = param1;
        inArray2[2] = param2;
        outData[i] = model_cond2.forward(inArray2) + inData[i];
    }
}
