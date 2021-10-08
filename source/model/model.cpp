/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>

#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#include "tensorflow/lite/string_util.h"

#include "model.h"
#include "ssd_mobilenet_v1_1_metadata_1.h"
//#include "model_data.h"
#include "labels.h"
#include "GUI.h"
#include "demo_config.h"
#include "image_from_eiq.h"

std::unique_ptr<tflite::FlatBufferModel> model;
std::unique_ptr<tflite::Interpreter> s_interpreter;
TfLiteTensor* inputTensor;

extern void MODEL_RegisterOps(tflite::MutableOpResolver &resolver);

status_t MODEL_Init()
{
	model = tflite::FlatBufferModel::BuildFromBuffer(ssd_mobilenet_v1_1_metadata_1_tflite, ssd_mobilenet_v1_1_metadata_1_tflite_len);
//	model = tflite::FlatBufferModel::BuildFromBuffer(model_data, model_data_len);
    if (!model)
    {
        std::cerr << "Failed to load model.\r\n";
        return kStatus_Fail;
    }

    tflite::MutableOpResolver resolver;
    MODEL_RegisterOps(resolver);

    tflite::InterpreterBuilder(*model, resolver)(&s_interpreter);
    if (!s_interpreter)
    {
        std::cerr << "Failed to construct interpreter.\r\n";
        return kStatus_Fail;
    }

    int input = s_interpreter->inputs()[0];

    const std::vector<int> inputs = s_interpreter->inputs();
    const std::vector<int> outputs = s_interpreter->outputs();

    if (s_interpreter->AllocateTensors() != kTfLiteOk)
    {
        std::cerr << "Failed to allocate tensors!\r\n";
        return kStatus_Fail;
    }

    /* Get input dimension from the input tensor metadata
       assuming one input only */
    inputTensor = s_interpreter->tensor(input);

#ifdef DEMO_VERBOSE
    const std::vector<int> inputs = s_interpreter->inputs();
    const std::vector<int> outputs = s_interpreter->outputs();

    std::cout << "input: " << inputs[0] << "\r\n";
    std::cout << "number of inputs: " << inputs.size() << "\r\n";
    std::cout << "number of outputs: " << outputs.size() << "\r\n";

    std::cout << "tensors size: " << s_interpreter->tensors_size() << "\r\n";
    std::cout << "nodes size: " << s_interpreter->nodes_size() << "\r\n";
    std::cout << "inputs: " << s_interpreter->inputs().size() << "\r\n";
    std::cout << "input(0) name: " << s_interpreter->GetInputName(0) << "\r\n";

    int t_size = s_interpreter->tensors_size();
    TfLiteTensor *tensorPtr = 0;
    for (int i = 0; i < t_size; i++)
    {
        if (s_interpreter->tensor(i)->name)
        {
            tensorPtr = s_interpreter->tensor(i);
            std::cout << i << ": " << tensorPtr->name << ", "
                      << tensorPtr->bytes << ", "
                      << tensorPtr->type << ", "
                      << tensorPtr->params.scale << ", "
                      << tensorPtr->params.zero_point << "\r\n";
        }
    }

    std::cout << "\r\n";
#endif

    return kStatus_Success;
}

status_t MODEL_RunInference()
{
    if (s_interpreter->Invoke() != kTfLiteOk)
    {
        std::cerr << "Failed to invoke tflite!\r\n" << std::endl;
        return kStatus_Fail;
    }

    return kStatus_Success;
}

uint8_t* GetTensorData(TfLiteTensor* tensor, tensor_dims_t* dims, tensor_type_t* type)
{
    switch (tensor->type)
    {
        case kTfLiteFloat32:
            *type = kTensorType_FLOAT32;
            break;
        case kTfLiteUInt8:
            *type = kTensorType_UINT8;
            break;
        case kTfLiteInt8:
            *type = kTensorType_INT8;
            break;
        default:
            assert("Unknown input tensor data type");
    };

    dims->size = tensor->dims->size;
    assert(dims->size <= MAX_TENSOR_DIMS);
    for (int i = 0; i < tensor->dims->size; i++)
    {
        dims->data[i] = tensor->dims->data[i];
    }

    return tensor->data.uint8;
}

uint8_t* MODEL_GetInputTensorData(tensor_dims_t* dims, tensor_type_t* type)
{
    return GetTensorData(inputTensor, dims, type);
}

uint8_t* MODEL_GetOutputTensorData(tensor_dims_t* dims, tensor_type_t* type)
{
    int output = s_interpreter->outputs()[0];
    TfLiteTensor* outputTensor = s_interpreter->tensor(output);

    return GetTensorData(outputTensor, dims, type);
}

template<typename T>
T* TensorData(TfLiteTensor* tensor, int batch_index);

template<>
float* TensorData(TfLiteTensor* tensor, int batch_index) {
	int nelems = 1;
	for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
	switch (tensor->type) {
	case kTfLiteFloat32:
		return tensor->data.f + nelems * batch_index;
	default:
		std::cout << "Should not reach here!";
	}
	return nullptr;
}

template<>
uint8_t* TensorData(TfLiteTensor* tensor, int batch_index) {
	int nelems = 1;
	for (int i = 1; i < tensor->dims->size; i++) nelems *= tensor->dims->data[i];
	switch (tensor->type) {
	case kTfLiteUInt8:
		return tensor->data.uint8 + nelems * batch_index;
	default:
		std::cout << "Should not reach here!";
	}
	return nullptr;
}

void MODEL_OD_Outputs_PostProc(int inferenceTime){
	TfLiteTensor* output_locations_ = s_interpreter->tensor(s_interpreter->outputs()[0]);
	TfLiteTensor* output_classes_ = s_interpreter->tensor(s_interpreter->outputs()[1]);
	TfLiteTensor* output_scores_ = s_interpreter->tensor(s_interpreter->outputs()[2]);
	TfLiteTensor* num_detections_ = s_interpreter->tensor(s_interpreter->outputs()[3]);

	const float* detection_locations = TensorData<float>(output_locations_, 0);
	const float* detection_classes = TensorData<float>(output_classes_, 0);
	const float* detection_scores = TensorData<float>(output_scores_, 0);
	const float* num_detections = TensorData<float>(num_detections_, 0);
	const int num_detections_value = *num_detections;

	std::cout << "----------------------------------------\r\n"
              << "     Inference time: " << inferenceTime / 1000 << " ms\r\n"
              << "----------------------------------------\r\n";

	const uint8_t* static_image = IMAGE_ReadStaticImage();
	int image_height = IMAGE_GetImageHeight();
	int image_width = IMAGE_GetImageWidth();

	GUI_SetBkColor(GUI_WHITE);
	GUI_BMP_DrawScaled(static_image, 0, 0, IMAGE_SCALE, 1); //image enlarged xIMAGE_SCALE
	GUI_SetBkColor(GUI_GRAY_D0); // to make light grey background for printing classes/class indexes
	GUI_SetPenSize(6);

	for (int i = 0; i < num_detections_value; i++){
		const float class_idx = detection_classes[i];
		const std::string cls = labels[int(class_idx)];
//		const std::string cls = labels[detection_classes[i]];
		const float score = detection_scores[i];

		const int ymin = detection_locations[4 * i] * image_height * IMAGE_SCALE;
		const int xmin = detection_locations[4 * i + 1] * image_width * IMAGE_SCALE;
		const int ymax = detection_locations[4 * i + 2] * image_height * IMAGE_SCALE;
		const int xmax = detection_locations[4 * i + 3] * image_width * IMAGE_SCALE;

		int n = cls.length();
		char class_name[n+1];
		strcpy(class_name, cls.c_str()); // convert string to char to display it on LCD

		char print_buf_score[10];

		if(score * 100 > DETECTION_TRESHOLD) {
			std::cout << "Detected " << cls << " with score " << (int)(score*100) << " [" << xmin << "," << ymin << ":" << xmax << "," << ymax << "]\r\n";

			if(i==0){
				GUI_SetColor(GUI_BLUE);
			}
			else{
				GUI_SetColor(GUI_MAKE_COLOR(0x00FF0000/(i*256))); // print different colors for different classes
			}

			sprintf(print_buf_score, "%2d%%", (int)(score*100));
			GUI_DrawRect(xmin, ymin, xmax, ymax); // print bounding boxes
			GUI_DispStringAt(class_name, xmin+(5*IMAGE_SCALE), ymin+(5*IMAGE_SCALE)); // print recognized classes
			GUI_DispStringAt(print_buf_score, xmin+(5*IMAGE_SCALE), ymin+(20*IMAGE_SCALE)); // print scores (confidence levels)
		}
	}
}

// Convert unsigned 8-bit image data to model input format in-place.
void MODEL_ConvertInput(uint8_t* data, tensor_dims_t* dims, tensor_type_t type)
{
    int size = dims->data[2] * dims->data[1] * dims->data[3];
    switch (type)
    {
        case kTensorType_UINT8:
            break;
        case kTensorType_INT8:
            for (int i = size - 1; i >= 0; i--)
            {
                reinterpret_cast<int8_t*>(data)[i] =
                    static_cast<int>(data[i]) - 127;
            }
            break;
        case kTensorType_FLOAT32:
            for (int i = size - 1; i >= 0; i--)
            {
                reinterpret_cast<float*>(data)[i] =
                    (static_cast<int>(data[i]) - MODEL_INPUT_MEAN) / MODEL_INPUT_STD;
            }
            break;
        default:
            assert("Unknown input tensor data type");
    }
}

const char* MODEL_GetModelName()
{
    return MODEL_NAME;
}
