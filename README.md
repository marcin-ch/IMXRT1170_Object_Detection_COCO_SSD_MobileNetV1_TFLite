![0_i.MXRT1170-EVK_promo_853x465.png](/doc/github_readme_images/0_i.MXRT1170-EVK_promo_853x465.png)

# Overview
This is a basic example of object detection on i.MXRT1170 evaluation kit ([MIMXRT1170-EVK](https://www.nxp.com/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt1170-evaluation-kit:MIMXRT1170-EVK)).

It is heavily based on example named *tensorflow_lite_label_image_cm7* coming from SDK **2.9.0**. Please be aware that it is not state-of-the-art SDK (the current one is 2.10.1, at the day of writing this guide, i.e. 06.10.2021) however I do still have some troubles with running object detection models on newer SDKs, for further reference please see NXP Community topic [here](https://community.nxp.com/t5/i-MX-RT/RT1062-run-tensorflowlite-cifar10-sample-code-error-message/m-p/1342158#M16383).

What is more, with newer SDK (2.10.*) NXP moved from TensorFlow Lite to TensorFlow Lite for Microcontrollers which is better optimized for ARM MCUs. Following one of the NXP contributor:
> The inference engine still supports TF Lite models, it's just the computations and the library that are specifically optimized for ARM MCUs.

This project is based also on example named *emwin_gui_demo_cm7* which shows how to use emWin graphics library.

In conclusion, this project is a combination of two SDK examples: *tensorflow_lite_label_image_cm7* and *emwin_gui_demo_cm7* and uses TensorFlow Lite inference engine that is part of NXP [eIQ](https://www.nxp.com/design/software/development-software/eiq-ml-development-environment:EIQ) software package.

# Hardware
* [MIMXRT1170-EVK](https://www.nxp.com/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt1170-evaluation-kit:MIMXRT1170-EVK) with attached:
    * [RK055HDMIPI4M](https://www.nxp.com/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt1170-evaluation-kit:MIMXRT1170-EVK#buy) 720x1280 LCD display
    * camera module (based on paper list attached to kit, it is 2592x1944 camera module image sensor with Huatian technology)

# Software
* MCUXpresso IDE v11.4.0 [Build 6224] [2021-07-15]
* SDK_2.x_EVK-MIMXRT1170 version 2.9.0

# Workflow
## 🧾 Import required examples from SDK
1. Import SDK to your **MCUXpresso**
2. From **QuickStart Panel** import SDK example *tensorflow_lite_label_image_cm7*<br>
![1_import_SDK_example.png](/doc/github_readme_images/1_import_SDK_example.png)
    * in **Project options** change **SDK Debug Console** to **UART** to use external debug console via UART (should be checked by default)
    * there is an issue in memory configuration and **NCACHE_REGION** overlaps with **BOARD_SDRAM** therefore change **NCACHE_REGION** from `0x80000000` to `0x83000000`
3. From **QuickStart Panel** import SDK example *emwin_gui_demo_cm7*
    *  in **Project options** change **SDK Debug Console** to **UART** to use external debug console via UART (should be checked by default)
    * there is an issue in memory configuration and **NCACHE_REGION** overlaps with **BOARD_SDRAM** therefore change **NCACHE_REGION** from `0x80000000` to `0x83000000`
4. To keep imported examples not changed, in **Project Explorer** copy and paste *evkmimxrt1170_tensorflow_lite_label_image_cm7* project and change its name to for example *evkmimxrt1170_OD_TFLite*
5. You should get as follows:<br>
![2_project_explorer_with_projects.png](/doc/github_readme_images/2_project_explorer_with_projects.png)

## 📚 Add emWin graphics library
1. Select *evkmimxrt1170_OD_TFLite* project, right mouse click, choose **SDK Management** and then **Manage SDK Components**
2. Go to the **Middleware** tab and check **emWin graphics library**
3. Hit **OK** and accept incomming changes
4. Hit **Build** and you will notice issues with functions `IMAGE_GetImage()` and `IMAGE_GetImageName()`, they are declared in *evkmimxrt1170_OD_TFLite/source/image/**image.h***, however there is a conflict with *evkmimxrt1170_OD_TFLite/emwin/emWin_header/**IMAGE.h***, therefore make following changes:
    * change all occurences of `#include "image.h"` to `#include "image_from_eiq.h"`, it refers to following files:
        * *evkmimxrt1170_OD_TFLite/source/image/image_capture.c*
        * *evkmimxrt1170_OD_TFLite/source/demo_info.cpp*
        * *evkmimxrt1170_OD_TFLite/source/main.cpp*
    * change file name of *evkmimxrt1170_OD_TFLite/source/image/**image.h*** to ***image_from_eiq.h***
        * un-check **Update references** as it has been done in previous step<br>
![3_rename_header_file.png](/doc/github_readme_images/3_rename_header_file.png)
    * open *evkmimxrt1170_OD_TFLite/source/image/image_from_eiq.h* and change all occurences of `_IMAGE_H_` to `_IMAGE_FROM_EIQ_H_`
    * Hit **Build** and **Debug**, should be no issues now
5. Move following files from *evkmimxrt1170_OD_TFLite/emwin/template* to *evkmimxrt1170_OD_TFLite/board*:
    * *emwin_support.h*
    * *emwin_support.c*
6. Open *evkmimxrt1170_emwin_gui_demo_cm7* project and copy below files to corresponding directories in *evkmimxrt1170_OD_TFLite* project (you can do this within **Project Explorer**):
    * */board/pin_mux.h*
    * */board/pin_mux.c*
    * */board/display_support.h*
    * */board/display_support.c*
    * */board/emwin_support.h*
    * */board/emwin_support.c*
    * */touchpanel/fsl_ft5406_rt.h* to */drivers*
    * */touchpanel/fsl_ft5406_rt.c* to */drivers*
    * */touchpanel/fsl_gt911.h* to */drivers*
    * */touchpanel/fsl_gt911.c* to */drivers*
7. Exclude from build *evkmimxrt1170_OD_TFLite/source/video* as it duplicates some already existing functions<br>
![4_multiple_definitions.png](/doc/github_readme_images/4_multiple_definitions.png)
    * select this folder, right mouse click, **Resource Configurations** -> **Exclude from Build** (choose both **Debug** and **Release** configurations)
8. Update code referring to `BOARD_ResetDisplayMix()` function:
    * Copy definition from *evkmimxrt1170_OD_TFLite/source/video/eiq_display_conf.c* to *evkmimxrt1170_OD_TFLite/board/display_support.c*
    ```c
    void BOARD_ResetDisplayMix(void)
    {
        /*
        * Reset the displaymix, otherwise during debugging, the
        * debugger may not reset the display, then the behavior
        * is not right.
        */
        SRC_AssertSliceSoftwareReset(SRC, kSRC_DisplaySlice);
        while (kSRC_SliceResetInProcess == SRC_GetSliceResetState(SRC, kSRC_DisplaySlice))
        {
        }
    }
    ```
    * add `#include "fsl_soc_src.h"`
    * copy prototype `void BOARD_ResetDisplayMix(void);` from *evkmimxrt1170_OD_TFLite/source/video/eiq_display_conf.h* to *evkmimxrt1170_OD_TFLite/board/display_support.h*
9. Update `IMAGE_GetImage()` function in *evkmimxrt1170_OD_TFLite/source/image/image_capture.c* to only process static images:
    ```c
    status_t IMAGE_GetImage(uint8_t* dstData, int32_t dstWidth, int32_t dstHeight, int32_t dstChannels)
    {
	    return IMAGE_Decode(image_data, dstData, dstWidth, dstHeight, dstChannels);
    }
    ```
10. Update `void BOARD_Init()` in *evkmimxrt1170_OD_TFLite/board/board_init.c* to be as follows:
    ```c
    void BOARD_Init()
    {
        BOARD_ConfigMPU();
        BOARD_InitLpuartPins();
        BOARD_InitMipiPanelPins();
        BOARD_MIPIPanelTouch_I2C_Init();
        BOARD_BootClockRUN();
        BOARD_ResetDisplayMix();
        BOARD_InitDebugConsole();
    }
    ```
11. Update *evkmimxrt1170_OD_TFLite/source/main.cpp* with following changes:
    * add two more `#include`:
    ```cpp
    #include "GUI.h"
    #include "emwin_support.h"
    ```
    * in `int main()` function, straight after `DEMO_PrintInfo();` add emWin initialization and function displaying the string at a specified position:
    ```cpp
    GUI_Init();
    GUI_DispStringAt("emWin TEST!", 0, 0);
    ```
12. Hit `Debug` and on the display you should see black background and white string **emWin TEST!**
    * displayed string is really small, but should be there for sure (if all works fine 😉)
13. Displayed string is rotated (display is in portrait mode), therefore make following changes:
    * update *evkmimxrt1170_OD_TFLite/board/emwin_support.h* with:
    ```c
    #define DISPLAY_DRIVER GUIDRV_LIN_OSY_16 // you can try also with GUIDRV_LIN_OSX_16
    //#define DISPLAY_DRIVER GUIDRV_LIN_16
    ```
    * update `void LCD_X_Config(void)` in *evkmimxrt1170_OD_TFLite/board/emwin_support.c* to be as follows:
    ```c
    void LCD_X_Config(void)
    {
        GUI_MULTIBUF_Config(GUI_BUFFERS);
        GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
        if(LCD_GetSwapXY()){
            LCD_SetSizeEx(0, LCD_HEIGHT, LCD_WIDTH);
            LCD_SetVSizeEx(0, LCD_HEIGHT, LCD_WIDTH);
        }
        else{
            LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
            LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
        }
    //    LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
    //    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
        LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);
        BOARD_Touch_Init();
    }
    ```

## 🏭 Prepare object detection model and list of classes
The pretrained model used in this example is COCO SSD MobileNetV1. It is trained to detect 90 classes of objects. The list of classes can be found [here](https://github.com/amikelive/coco-labels/blob/master/coco-labels-paper.txt).
1. Download object detection model from [TensorFlow Lite Object Detection example](https://www.tensorflow.org/lite/examples/object_detection/overview), direct link [here](https://tfhub.dev/tensorflow/lite-model/ssd_mobilenet_v1/1/metadata/1?lite-format=tflite)
2. Go to *evkmimxrt1170_OD_TFLite/doc* and place here downloaded *ssd_mobilenet_v1_1_metadata_1.tflite* model 
3. Open **Command Prompt** with `cmd` and convert model to *.h* array with command:
    ```console
    xxd -i ssd_mobilenet_v1_1_metadata_1.tflite > ssd_mobilenet_v1_1_metadata_1.h
    ```
    * **xxd** is part of [**Vim**](https://www.vim.org/) text editor, and provides a method to dump a binary file (in this case *.tflite*) to hex, therefore please install Vim to be able to make necessary conversions
    * if `xxd -version` does not work straight away after Vim installation, please double check Vim installation directory (in my case: *"C:\Program Files (x86)\Vim\vim82\xxd.exe"*) and use the complete path instead of `xxd`
4. Move converted *ssd_mobilenet_v1_1_metadata_1.h* to *evkmimxrt1170_OD_TFLite/source/model*
5. In **MCUXpresso**, open *evkmimxrt1170_OD_TFLite/source/model/ssd_mobilenet_v1_1_metadata_1.h* and make following changes:
    * add:
    ```c
    #include <cmsis_compiler.h>

    #define MODEL_NAME "ssd_mobilenet_v1_1_metadata_1"
    #define MODEL_INPUT_MEAN 127.5f
    #define MODEL_INPUT_STD 127.5f
    ```
    * change array name to be as follows:
    ```c
    const char ssd_mobilenet_v1_1_metadata_1_tflite[] __ALIGNED(16) = {
    ```
    * the changes are based on default model which comes in SDK example, please refer to *evkmimxrt1170_OD_TFLite/source/model/model_data.h*
6. Update *evkmimxrt1170_OD_TFLite/source/labels.h* with list of classes available [here](https://github.com/amikelive/coco-labels/blob/master/coco-labels-paper.txt)

## 👁️ Make object detection model operational
Running object detection model in application which natively was supposed to make image classification, should provide some interesting information how to handle object detecion model.
1. In *evkmimxrt1170_OD_TFLite/source/model/model.cpp* make following changes:
    * comment out model from SDK example (`model_data.h`) and add new object detection model
    ```cpp
    #include "ssd_mobilenet_v1_1_metadata_1.h"
    //#include "model_data.h"
    ```
    * update `Model_Init()` to use new object detection model
    ```cpp
    model = tflite::FlatBufferModel::BuildFromBuffer(ssd_mobilenet_v1_1_metadata_1_tflite, ssd_mobilenet_v1_1_metadata_1_tflite_len);
    ```
2. Hit **Debug** and in terminal you should see **first** error:
    ```console
    ERROR: Didn't find op for builtin opcode 'CONCATENATION' version '1'
    ERROR: Registration failed.
    Failed to construct interpreter.
    Failed initializing model
    ```
    * this is because not every operation used by new object detection model is supported so far in the project
    * to solve this, update *evkmimxrt1170_OD_TFLite/source/model/model_mobilenet_ops.cpp* with adding missing operation:
    ```cpp
    resolver.AddBuiltin(tflite::BuiltinOperator_CONCATENATION,
						tflite::ops::builtin::Register_CONCATENATION(),
						/* min_version */ 1,
						/* max_version */ 2);
    ```
3. Hit **Debug** and in terminal you should see **second** error:
    ```console
    ERROR: Didn't find op for builtin opcode 'LOGISTIC' version '1'
    ERROR: Registration failed.
    Failed to construct interpreter.
    Failed initializing model
    ```
    * to solve this, update *evkmimxrt1170_OD_TFLite/source/model/model_mobilenet_ops.cpp* with adding missing operation:
    ```cpp
    resolver.AddBuiltin(tflite::BuiltinOperator_LOGISTIC,
						tflite::ops::builtin::Register_LOGISTIC(),
						/* min_version */ 1,
						/* max_version */ 2);
    ```
4. Hit **Debug** and in terminal you should see **third** error (bit different than errors above):
    ```console
    ERROR: Encountered unresolved custom op: TFLite_Detection_PostProcess.
    ERROR: Node number 63 (TFLite_Detection_PostProcess) failed to prepare.
    Failed to allocate tensors!
    Failed initializing model
    ```
    * to solve this, update *evkmimxrt1170_OD_TFLite/source/model/model_mobilenet_ops.cpp* with adding missing operation:
    ```cpp
    #include "tensorflow/lite/kernels/custom_ops_register.h"
    ```
    ```cpp
    resolver.AddCustom("TFLite_Detection_PostProcess", tflite::ops::custom::Register_TFLite_Detection_PostProcess());
    ```
    * update *evkmimxrt1170_OD_TFLite/eiq/tensorflow-lite/tensorflow/lite/kernels/custom_ops_register.h* with following:
    ```c
    TfLiteRegistration* Register_DETECTION_POSTPROCESS();
    TfLiteRegistration* Register_TFLite_Detection_PostProcess() {
    return Register_DETECTION_POSTPROCESS();
    }
    ```
    * this is also available from [NXP community post](https://community.nxp.com/t5/eIQ-Machine-Learning-Software/How-to-add-custom-operators-in-tensorflow-Lite/m-p/1350029/highlight/true#M496) 
5. Hit **Debug** again and all should be fine now
    * please notice, model returns silly detection (we expect to detect **stopwatch**), this is because application still interprets model's outputs as it was for image classification. Therefore, it needs to be updated (please refer to next section **Perform object detection**). However, running object detection model does not return any problems now. 
    ```console
    ----------------------------------------
     Inference time: 3574 ms
     Detected: motorcycle (96%)
    ----------------------------------------
    ```

## ⏱️ Prepare example images to work on
1. Example images are provided with this repository, in *evkmimxrt1170_OD_TFLite/doc* 
    * they are *.bmp* files
        * *stopwatch.bmp*
        * *apple_banana_orange.bmp*
2. To use the images in the project, they need to be converted to *.h* files, using **xxd** tool described in previous sections
    * *stopwatch.bmp* comes with SDK example *tensorflow_lite_label_image_cm7* and already has corresponding C array in *evkmimxrt1170_OD_TFLite/source/image/image_data.h*
    * *apple_banana_orange.bmp* is downloaded from [here](https://messalonskeehs.files.wordpress.com/2013/02/screen-shot-2013-02-06-at-10-50-37-pm.png)
        * it comes as *.png* so conversion to *.bmp* is required (you can do that with **Microsoft Paint** -> **File** -> **Save as** -> **BMP picture**)
        * convert *.bmp* to *.h* file
        ```console
        xxd -i apple_banana_orange.bmp > apple_banana_orange.h
        ```
3. Move converted *apple_banana_orange.h* to *evkmimxrt1170_OD_TFLite/source/image*
4. In **MCUXpresso**, open *evkmimxrt1170_OD_TFLite/source/image/apple_banana_orange.h* and update begining of the file to be as follows:
    ```c
    #define STATIC_IMAGE_NAME "apple_banana_orange_bmp"

    const uint8_t apple_banana_orange_bmp[] = {
    ```
    * the changes are based on SDK example *evkmimxrt1170_OD_TFLite/source/image/image_data.h* (converted *stopwatch.bmp*), i.e. header file with new image should look in the same way

## 🧠 Perform object detection
The detailed changes in project to perform object detecion can be tracked as [commits history](https://github.com/marcin-ch/IMXRT1170_Object_Detection_COCO_SSD_MobileNetV1_TFLite/commits/master), especially [this one](https://github.com/marcin-ch/IMXRT1170_Object_Detection_COCO_SSD_MobileNetV1_TFLite/commit/7aa82b71a3581fc364b32f027e5fcbe98636f999).

Basically, you need to:
1. Provide new model to perform object detection
2. Provide new image to work on
3. Provide all operations used in object detection model
4. Update machine learning code to get and interpret outputs from the object detection model
    * please refer to *evkmimxrt1170_OD_TFLite/source/model/model.cpp* where this functionality has been covered
    * these changes are based on this [Github repo](https://github.com/YijinLiu/tf-cpu/blob/master/benchmark/obj_detect_lite.cc)
    > **Additional info**
    > * [TensorFlow Lite Object Detection example](https://www.tensorflow.org/lite/examples/object_detection/overview) contains useful info how to interpret model's outputs.
5. Draw bounding boxes, recognized classes and confidence levels

Once all changes are done, hit **Debug** and, after flashing the board, you should see on the display image of fruits with bounding boxes, recognized classes and confidence levels.

Use terminal such as Tera Term to get bit more detailed output from an application, especially inference time.

# Summary
![5_imxrt1170_object_detection_resized.jpg](/doc/github_readme_images/5_imxrt1170_object_detection_resized.jpg)

Inference time is approximately **3,8 seconds**, so it limits possible use cases. In another [repository](https://github.com/marcin-ch/IMXRT1060_Object_Detection_COCO_SSD_MobileNetV1_TFLite) there is very similar project for i.MXRT1060 evaluation kit. Inference time is approximately **6 seconds**. It means, we got **35% of improvement of inference time** which should be not surprising as as i.MXRT1170 is the strongest representative of i.MXRT family so far. Important difference between i.MXRT1060 and i.MXRT1170 is maximum operating frequency, 600MHz and 1GHz respectively. For further details, please refer to very useful comparison of i.MXRT family [here](https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i-mx-rt-crossover-mcus:IMX-RT-SERIES).

Inference results are very comparable, there is small offset in **banana** detection (**46%** from i.MXRT1170 versus **48%** from i.MXRT1060).

Perhaps, using different object detection models can bring more accurate results.

**Nevertheless, object detection can be realised on resource constrained platforms such as MCU.**

## Remarks:
1. Detection of **dining table** with score **51%** in the image of fruits is bit surprising. This can be fixed with making the value of `DETECTION_TRESHOLD` in *evkmimxrt1170_OD_TFLite/source/demo_config.h* bit higher (now it has been set to `45`) and detection with scores below `DETECTION_TRESHOLD` will be eliminated. However, in this case, detection of **banana** (which is totally fine) with score **46%** will be eliminated as well.
2. There is another image provided in the example, named *stopwatch.bmp*. To use it, please update the code in *evkmimxrt1170_OD_TFLite/source/image/image_capture.c*:
    ```c
    #include "image_data.h" // stopwatch
    //#include "apple_banana_orange.h"
    ```
    ```c
    #define CURRENT_IMAGE     image_data // stopwatch
    //#define CURRENT_IMAGE     apple_banana_orange_bmp
    ```
3. For better presentation on the display, images are enlarged 2 times with function:
    ```cpp
    GUI_BMP_DrawScaled(static_image, 0, 0, IMAGE_SCALE, 1);
    ```
    * `IMAGE_SCALE` is set to `2` in *evkmimxrt1170_OD_TFLite/source/demo_config.h* and can be modified

## How to use this repo (source code)
1. Clone the repo or download as *.zip* to your local disc drive
    * when clonning please use below command:
    ```console
    git clone https://github.com/marcin-ch/IMXRT1170_Object_Detection_COCO_SSD_MobileNetV1_TFLite.git
    ```
2. Open **MCUXpresso**, you will be asked for choosing existing or creating new workspace, I recommend creating new workspace for testing purposes
3. From **QuickStart Panel** choose **Import project(s) from file system** and then select either unpacked repo (in case you clonned the repo) or zipped repo (in case you downloaded the archive)
4. Make sure **Copy projects into workspace** in **Options** is checked
5. Hit **Finish**
6. Select imported project in **Project Explorer** and hit **Debug** in **QuickStart Panel**, the application should be up and running
7. You can now remove clonned or downloaded repo, as it now exists in your workspace

## How to use this repo (binary file)
If you just want to check how the project looks like running on the board, you can flash binary file available in *evkmimxrt1170_OD_TFLite/doc/evkmimxrt1170_OD_TFLite.bin*. As i.MXRT1170 evaluation kit enumerates as MSD (Mass Storage Device) when connected to PC through USB cable, you can simply drag-n-drop binary file to your board. Wait few moments when flashing is in progress, reset the board and you should see application working.