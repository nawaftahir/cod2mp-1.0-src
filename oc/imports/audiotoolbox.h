#ifndef IMPORTS_AUDIOTOOLBOX_H
#define IMPORTS_AUDIOTOOLBOX_H

int AudioConverterDispose();
int AudioConverterFillBuffer();
int AudioConverterFillComplexBuffer();
int AudioConverterNew();
int AudioConverterReset();
int AudioDeviceGetProperty();
int AudioDeviceGetPropertyInfo();
int AudioDeviceSetProperty();
int AudioHardwareGetProperty();
int AudioOutputUnitStart();
int AudioOutputUnitStop();
int AudioUnitAddRenderNotify();
int AudioUnitGetProperty();
int AudioUnitInitialize();
int AudioUnitRemoveRenderNotify();
int AudioUnitRender();
int AudioUnitSetParameter(AudioUnit unit, unsigned int paramID, unsigned int scope, unsigned int elem, float value, unsigned int offset);
int AudioUnitSetProperty();
int AudioUnitUninitialize();

#endif
