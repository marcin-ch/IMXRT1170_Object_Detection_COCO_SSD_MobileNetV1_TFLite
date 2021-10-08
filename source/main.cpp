/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>

#include "board_init.h"
#include "demo_config.h"
#include "demo_info.h"
#include "image_from_eiq.h"
#include "image_utils.h"
#include "model.h"
#include "output_postproc.h"
#include "timer.h"

#include "GUI.h"
#include "emwin_support.h"

int main()
{
    BOARD_Init();
    TIMER_Init();

    DEMO_PrintInfo();

    GUI_Init();
    GUI_DispStringAt("emWin TEST!", 0, 0);

    const uint8_t* static_image = IMAGE_ReadStaticImage();

    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    GUI_SetBkColor(GUI_GREEN);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_LARGE_FONT);
    GUI_BMP_DrawScaled(static_image, 0, 0, IMAGE_SCALE, 1); //image enlarged xIMAGE_SCALE
    GUI_DispStringAt("Image to process" , 0, 0);

    if (MODEL_Init() != kStatus_Success)
    {
        std::cerr << "Failed initializing model" << EOL;
        for (;;) {}
    }

    tensor_dims_t inputDims;
    tensor_type_t inputType;
    uint8_t* inputData = MODEL_GetInputTensorData(&inputDims, &inputType);

//    tensor_dims_t outputDims;
//    tensor_type_t outputType;
//    uint8_t* outputData = MODEL_GetOutputTensorData(&outputDims, &outputType);

//    while (1)
//    {
        /* Expected tensor dimensions: [batches, height, width, channels] */
        if (IMAGE_GetImage(inputData, inputDims.data[2], inputDims.data[1], inputDims.data[3]) != kStatus_Success)
        {
            std::cerr << "Failed retrieving input image" << EOL;
            for (;;) {}
        }

        MODEL_ConvertInput(inputData, &inputDims, inputType);

        auto startTime = TIMER_GetTimeInUS();
        MODEL_RunInference();
        auto endTime = TIMER_GetTimeInUS();

        MODEL_OD_Outputs_PostProc(endTime - startTime);

//        MODEL_ProcessOutput(outputData, &outputDims, outputType, endTime - startTime);

        std::cout << "@@@ ! END ! @@@.\r\n";
        for (;;) {}
//    }
}
