# DSP_and_Math

The idea behind this library is provide some useful DSP and Math functions, focusing on embedded systems (multi platform).

To enable multiple instances (similar to C++) various functions receive and return values via structs containing the related parameters. With this is possible, for eg, to run two IIR High Pass in diferent signals or channel. But this approach creates a little overhead. To overcome this is possible to select the prototype of functions using "__inline" in the "DSP_and_Math.h" and also use compiler optimization.

To be microcontroller friendly, some functions were implemented entirely in fixed point, other functions use the main calculus in fixed math and return the result in float, allowing a trade off between accuracy and efficiency.

#### Plot Examples

* High pass Filter

![alt text](https://github.com/agaelema/DSP_and_Math/blob/master/highpass_plot.jpg?raw=true "Logo Title Text 1")

* Low pass Filter

![alt text](https://github.com/agaelema/DSP_and_Math/blob/master/lowpass_plot.jpg?raw=true "Logo Title Text 1")


## Implemented Math Functions

#### Square Root (SQRT)

* Square Root of a Integer value (int32_t)
``` c
int32_t sqrt_Int32(int32_t x);
```

#### RMS value

* RMS value of an array of N samples
``` c
float rmsValueArray_Float_StdMath(const float * arrayIn, uint_fast16_t size, float dcLevel);
float rmsValueArray_Int16_StdMath(const int16_t * arrayIn, uint_fast16_t size, int16_t dcLevel);
```

* RMS sample by sample

Enable to add samples on demand and calculate the value after N samples. After add all samples calculation is done with a separate function
``` c
void rmsValueAddSample_Float(rms_float_t * inputStruct, float sample);
void rmsValueAddSample_Int16(rms_int16_t * inputStruct, int16_t sample);

void rmsClearStruct_Float(rms_float_t * inputStruct);
void rmsClearStruct_Int16(rms_int16_t * inputStruct);

void rmsValueCalcRmsStdMath_Float(rms_float_t * inputStruct);
void rmsValueCalcRmsStdMath_Int16(rms_int16_t * inputStruct);
```

#### Sine wave generator

Applying the same function N times is possible to generate complex waves with harmonics (see examples)

* Array version

Generate the sine wave in a defined float array
``` c
void sineWaveGen_Array_Float(float * outputArray, float freq, float phase_rad, float amplitude, float V_offset, uint_fast16_t points, uint_fast8_t doClean);
```

* Sample-by-sample

Allow to generate each sample iteratively
``` c
void sineWaveGen_bySample_Init(sine_wave_parameters *inputParameters, float freq, float phase, float amp, float v_off, uint_fast16_t points, uint_fast8_t doClean);
float sineWaveGen_GetSample(sine_wave_parameters *inputParameters);
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

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and shift size (rotation used in fixed math) between 8 and 15. Bigger shift value give more accuracy but will limit the input value (see function coment). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency without reset filter state.

``` c
void iir_SinglePoleHighPass_Fixed_Init(iirHighPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
```
Second function do the filtering based in the struct with parameters and the input sample. The filtered output is returned in the structure (parameter "y").
``` c
void iir_SinglePoleHighPass_Fixed(iirHighPassFixed_t * inputStuct, int32_t xValue);
```

* IIR Single Pole High Pass Filter - Fixed Extended Point Version

Use fixed point to math increasing the efficiency in devices withou a FPU. Extended version use variables with 64 bits allowing more accuracy and bigger input values

First function initialize the struct with parameters based in the cuttof frequency (normalized from 0 to 1) and shift size (rotation used in fixed math) between 8 and 30. Bigger shift value give more accuracy but will limit the input value (see function coment). It's possible to select between clean or not the variables during the inittialization. Can be useful if you need to change the cutoff frequency without reset filter state.

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

#### Goertzel DFT

Allow evaluate individual terms of a DFT. More efficient than a conventional DFT, but less efficient than a FFT algorithm.

* Using an Array

Calculate the value of a bin (harmonic) in an array (float or int16_t). Initialization functions will compute internal parameters.

``` c
void goertzelArrayInit_Float(goertzel_array_float_t * inputStruct, float bin, uint_fast16_t size_array);
void goertzelArrayFloat_Float(goertzel_array_float_t * inputStruct, const float * arrayInput);
void goertzelArrayInt16_Float(goertzel_array_float_t * inputStruct, const int16_t * arrayInput);

void goertzelArrayInit_Fixed64(goertzel_array_fixed64_t * inputStruct, float bin, uint_fast16_t size_array, uint_fast8_t shift);
void goertzelArrayInt16_Fixed64(goertzel_array_fixed64_t * inputStruct, const int16_t * arrayInput);
```

* Sample-by-sample

Calculate the value of a bin (harmonic) without need an array, saving memory. The initialization functions will calculate internal variables. Altough not use an array, the number of samples is defined by the user and should be respected.

``` c
void goertzelSampleInit_Float(goertzel_sample_float_t * inputStruct, float bin, uint_fast16_t size_array);
void goertzelSampleAddFloat_Float(goertzel_sample_float_t * inputStruct, float sample);
void goertzelSampleAddInt16_Float(goertzel_sample_float_t * inputStruct, int16_t sample);
void goertzelSampleCalc_Float(goertzel_sample_float_t * inputStruct);

void goertzelSampleInit_Fixed64(goertzel_sample_fixed64_t* inputStruct, float bin, uint_fast16_t size_array, uint_fast8_t shift);
void goertzelSampleAddInt16_Fixed64(goertzel_sample_fixed64_t * inputStruct, int16_t sample);
void goertzelSampleCalc_Fixed64(goertzel_sample_fixed64_t * inputStruct);
```
