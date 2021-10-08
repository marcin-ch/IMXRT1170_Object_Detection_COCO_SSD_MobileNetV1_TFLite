/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>

#include "demo_info.h"
#include "demo_config.h"
#include "image_from_eiq.h"
#include "model.h"

void DEMO_PrintInfo()
{
    std::cout << EOL << EOL << "Object detection example using a TensorFlow Lite model." << EOL
        << "Detection threshold: " << DETECTION_TRESHOLD << "%" << EOL
		<< "Expected category: " << "Multiple objects, existing in COCO dataset" << EOL
//		<< "Expected category: " << IMAGE_GetImageName() << EOL
        << "Model: " << MODEL_GetModelName() << EOL;
}
