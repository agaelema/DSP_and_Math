# DSP_and_Math
The idea behind this library is provide some useful DSP and Math functions, focusing on multi platform embedded systems.

To enable multiple instances (similar to C++) various functions receive and return values via structs containing the related parameters. With this is possible, for eg, to run two IIR High Pass in diferent signals or channel. But this approacha creates a little overhead. To overcome this is possible to select the prototype of functions using "__inline" in the "DSP_and_Math.h".

## Implemented Math Functions

#### RMS value

* RMS value of array
``` c
float rmsValueArray_StdMath(const float * arrayIn, uint_fast16_t size, float dcLevel);
```

* RMS sample by sample

Enable to add samples on demand and calculate the value after N samples
``` c
void rmsValueAddSample(rms_float_t * inputStruct, const float * sampleFloat);
void rmsValueCalcRmsStdMath(rms_float_t * inputStruct);
```

## Implemented DSP Functions

#### IIR Single Pole High Pass

* IIR Single Pole High Pass Filter - Float Point Version

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleHighPass_Float_Init(iirHighPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleHighPass_Float(iirHighPassFloat_t * structInput, float xValueFloat);
```

* IIR Single Pole High Pass Filter - Fixed Point Version

Use fixed point to math increasing the efficiency in devices withou a FPU.

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and shift size (rotation used in fixed math) between 8 and 15. A bigger shift value give more accuracy but will limit the input value (see function coment). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleHighPass_Fixed_Init(iirHighPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleHighPass_Fixed(iirHighPassFixed_t * inputStuct, int32_t xValue);
```

* IIR Single Pole High Pass Filter - Fixed Extended Point Version

Use fixed point to math increasing the efficiency in devices withou a FPU. Extended version use variables with 64 bits allowing more accuracy and bigger input values

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and shift size (rotation used in fixed math) between 8 and 15. A bigger shift value give more accuracy but will limit the input value (see function coment). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleHighPass_FixedExtended_Init(iirHighPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleHighPass_FixedExtended(iirHighPassFixedExtended_t * inputStuct, int32_t xValue);
```

#### IIR Single Pole Low Pass

* IIR Single Pole Low Pass Filter - Float Point Version

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleLowPass_Float_Init(iirLowPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleLowPass_Float(iirLowPassFloat_t * inputStruct, float xValueFloat);
```

* IIR Single Pole High Low Filter - Fixed Point Version

Use fixed point to math increasing the efficiency in devices withou a FPU.

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and shift size (rotation used in fixed math) between 8 and 15. A bigger shift value give more accuracy but will limit the input value (see function coment). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleLowPass_Fixed_Init(iirLowPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleLowPass_Fixed(iirLowPassFixed_t * inputStruct, int32_t xValue);
```

* IIR Single Pole Low Pass Filter - Fixed Extended Point Version

Use fixed point to math increasing the efficiency in devices withou a FPU. Extended version use variables with 64 bits allowing more accuracy and bigger input values

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and shift size (rotation used in fixed math) between 8 and 15. A bigger shift value give more accuracy but will limit the input value (see function coment). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleLowPass_FixedExtended_Init(iirLowPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleLowPass_FixedExtended(iirLowPassFixedExtended_t * inputStruct, int32_t xValue);
```

* IIR Single Pole Low Pass Filter - Fixed Point - Fast Version

Use fixed point to math increasing the efficiency in devices withou a FPU. The cutoff frequency is determined by attenuation (number of rotations). Bigger values give lower cutoff. This version is more efficient than normal fixed version.

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and attenuation. It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency withou reset entire filter state.

``` c
void iir_SinglePoleLowPass_Fixed_Fast_Init(iirLowPassFixedFast_t * structInput, int_fast8_t attenuation, int_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleLowPass_Fixed_Fast(iirLowPassFixedFast_t * inputStruct, int32_t xValue);
```