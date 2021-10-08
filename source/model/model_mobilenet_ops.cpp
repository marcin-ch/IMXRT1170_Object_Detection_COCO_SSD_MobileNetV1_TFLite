/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/kernels/builtin_op_kernels.h"
#include "tensorflow/lite/kernels/custom_ops_register.h"

void MODEL_RegisterOps(tflite::MutableOpResolver &resolver)
{
    resolver.AddBuiltin(tflite::BuiltinOperator_AVERAGE_POOL_2D,
                        tflite::ops::builtin::Register_AVERAGE_POOL_2D(),
                        /* min_version */ 1,
                        /* max_version */ 2);
    resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                        tflite::ops::builtin::Register_CONV_2D(),
                        /* min_version */ 1,
                        /* max_version */ 3);
    resolver.AddBuiltin(tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
                        tflite::ops::builtin::Register_DEPTHWISE_CONV_2D(),
                        /* min_version */ 1,
                        /* max_version */ 3);
    resolver.AddBuiltin(tflite::BuiltinOperator_DEQUANTIZE,
                        tflite::ops::builtin::Register_DEQUANTIZE(),
                        /* min_version */ 1,
                        /* max_version */ 4);
    resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                        tflite::ops::builtin::Register_RESHAPE());
    resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                        tflite::ops::builtin::Register_SOFTMAX(),
                        /* min_version */ 1,
                        /* max_version */ 2);
    resolver.AddBuiltin(tflite::BuiltinOperator_CONCATENATION,
						tflite::ops::builtin::Register_CONCATENATION(),
						/* min_version */ 1,
						/* max_version */ 2);
    resolver.AddBuiltin(tflite::BuiltinOperator_LOGISTIC,
						tflite::ops::builtin::Register_LOGISTIC(),
						/* min_version */ 1,
						/* max_version */ 2);
    resolver.AddCustom("TFLite_Detection_PostProcess", tflite::ops::custom::Register_TFLite_Detection_PostProcess());
}
