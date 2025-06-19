/******************************************************************************
 *  DSP_and_Math - Library with useful DSP and math functions - .h file
 *  - some functions using float point and standard math.h lib
 *  - some functions using fixed notation to (optimized)
 *
 *  author: Haroldo Amaral - agaelema@gmail.com
 *  v0.4.3 - 2017/10/09
 ******************************************************************************
 *  log:
 *    v0.1      . Initial version
 *              + add rms functions and structures
 *              + add dc filter float version and structures
 *              + add dc filter fixed version and structures
 *              + add dc filter fixed extended version and structures
 *    v0.2      . rename dc filter to iir_hipassfilter
 *              . rename associated structures
 *              + add iir low pass filter and structures
 *    v0.3      . change name of rms functions
 *              + add new rms functions
 *              + add sqrt_Int32 and sqrt_Int64 (integer versions)
 *    v0.4      . change volatile variables
 *              . optimize sqrt_Int32 by defines
 *              . change rms_valueadd input parameter (pass the value instead of pointer)
 *              - remove sqrt_Int64 - not efficient
 *              - remove rms int32 functions - not efficient
 *              + add sine wave gen function
 *              + add rmsClearStruct to function sample by sample
 *              + add Goertzel functions (array and sample-by-sample)
 *    v0.4.1    . improve efficiency on "goertzelArrayInt16_Fixed64()"
 *              - remove old remain functions
 *    v0.4.2    . fix "sineWaveGen_GetSample()" function - phase error
 *    v0.4.3    . organized defines
 ******************************************************************************/

#ifndef _DSP_AND_MATH_H_
#define _DSP_AND_MATH_H_

#include    <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
 *                              DEFINES
 ******************************************************************************/

/* RMS VALUE - ARRAY VERSION */
//#define     RMS_ARRAY_STD         // rms using standard lib (math.h) - more accurate
#define     RMS_ARRAY_OPTIMIZED   // using integer square root algorithm - more efficient

/* RMS VALUE - SAMPLE BY SAMPLE VERSION */
//#define     RMS_SAMPLE_STD         // rms using standard lib (math.h) - more accurate
#define     RMS_SAMPLE_OPTIMIZED   // using integer square root algorithm - more efficient


#define     PI                  3.141592653589793f
#define     TWO_PI              6.283185307179586f
#define     SQRT_OF_2           1.414213562373095f
#define     SQRT_OF_3           1.732050807568877f





/******************************************************************************
 *                              ENUM
 ******************************************************************************/

enum iir_clean
{
    IIR_FILTER_NOT_CLEAN = 0,
    IIR_FILTER_DO_CLEAN,
};


enum sineWaveGen
{
    WAVEGEN_NOTCLEAN = 0,
    WAVEGEN_CLEAN
};





/******************************************************************************
 *              GLOBAL VARIABLES AND TYPEDEFs - prototypes
 ******************************************************************************/

/******************************************************************************
 *                  STRUCT - RMS SAMPLE PARAMETERS
 ******************************************************************************/
/* used to store rms parameters - float samples */
struct rms_struct_float_
{
    uint_fast16_t size_counter;
    float acc;
    float rmsValue;
};
/* used to store rms parameters - float samples */
typedef struct rms_struct_float_ rms_float_t;


/* used to store rms parameters - int16_t sample */
struct rms_struct_int16_
{
    uint_fast16_t size_counter;
    uint32_t acc;
    float rmsValue;
};
/* used to store rms parameters - int16_t sample */
typedef struct rms_struct_int16_ rms_int16_t;



/******************************************************************************
 *                  STRUCT - SINE WAVE PARAMETERS
 ******************************************************************************/
/* used to store sine - generate wave sample by sample */
struct sine_wave_parameters_
{
    float freq;
    float phase_rad;
    float amplitude;
    float V_offset;
    uint_fast16_t points;
    float increment;
    float acc;
};
/* used to store sine - generate wave sample by sample */
typedef struct sine_wave_parameters_ sine_wave_parameters;



/******************************************************************************
 *                  STRUCT - HIGH PASS FILTERS PARAMETERS
 ******************************************************************************/
/* used to store high pass filter float parameters */
struct struct_iir_highpass_float_
{
    float cutoff_Freq;
    float prev_x;
    float y;
    float prev_y;
};
/* used to store high pass filter float parameters */
typedef struct struct_iir_highpass_float_ iirHighPassFloat_t;


/* used to store high pass filter fixed parameters */
struct struct_iir_highpass_fixed_
{
    uint_fast8_t shift_size;    // number of shift used in fixed math
    float cutoff_Freq;
    int32_t A_param;            // (1 - pole), but in fixed notation
    int32_t acc;                // accumulator
    int32_t prev_x;             // last input value
    int32_t y;                  // filtered output
    int32_t prev_y;             // last filtered output sample

};
/* used to store high pass filter fixed parameters */
typedef struct struct_iir_highpass_fixed_ iirHighPassFixed_t;


/* used to store high pass filter fixed extended parameters */
struct struct_iir_highpass_fixed_Extended_
{
    uint_fast8_t shift_size;    // number of shift used in fixed math
    double cutoff_Freq;
    int32_t A_param;            // (1 - pole), but in fixed notation
    int64_t acc;                // accumulator
    int64_t prev_x;             // last input value
    int32_t y;                  // filtered output
    int32_t prev_y;             // last filtered output sample
};
/* used to store high pass filter fixed extended parameters */
typedef struct struct_iir_highpass_fixed_Extended_ iirHighPassFixedExtended_t;



/******************************************************************************
 *                  STRUCT - LOW PASS FILTERS PARAMETERS
 ******************************************************************************/
/* used to store low pass filter float parameters */
struct struct_iir_lowpass_float_
{
    float cutoff_Freq;              // cutoff frequency - from 0 to 1 (normalized bandwidth)
    float b0;                       // input coefficient
    float a1;                       // output coefficient
    float prev_y;                   // last filtered sample
    float y;                        // filtered output
};
/* used to store low pass filter float parameters */
typedef struct struct_iir_lowpass_float_ iirLowPassFloat_t;


/* used to store low pass filter fixed parameters */
struct struct_iir_lowpass_fixed_
{
    uint_fast8_t shift_size;    // number of shift used in fixed math
    float cutoff_Freq;              // cutoff frequency - from 0 to 1 (normalized bandwidth)
    int32_t A_param;            //
    int32_t RoundNumber;              //
    int32_t SHIFTED_filtered;
    int32_t SHIFTED_last_filtered;
    int32_t y;                        // filtered output
};
/* used to store low pass filter fixed parameters */
typedef struct struct_iir_lowpass_fixed_ iirLowPassFixed_t;


/* used to store low pass filter fixed extended parameters */
struct struct_iir_lowpass_fixed_extended_
{
    uint_fast8_t shift_size;    // number of shift used in fixed math
    double cutoff_Freq;              // cutoff frequency - from 0 to 1 (normalized bandwidth)
    int64_t A_param;            //
    int64_t RoundNumber;              //
    int64_t SHIFTED_filtered;
    int64_t SHIFTED_last_filtered;
    int32_t y;                        // filtered output
};
/* used to store low pass filter fixed extended parameters */
typedef struct struct_iir_lowpass_fixed_extended_ iirLowPassFixedExtended_t;


/* used to store low pass filter fixed fast parameters */
struct struct_iir_lowpass_fixed_fast_
{
    int_fast8_t attenuation;
    int32_t filter_acc;
    int32_t y;
};
/* used to store low pass filter fixed fast parameters */
typedef struct struct_iir_lowpass_fixed_fast_ iirLowPassFixedFast_t;



/******************************************************************************
 *                  STRUCT - GOERTZEL DFT PARAMETERS
 ******************************************************************************/
/* used to store goertzel parameters - float array version */
struct goertzel_struct_array_float_
{
    uint_fast16_t size_array;
    float cr_float;
    float ci_float;
    float coeff_float;
    float real_float;
    float imag_float;
    float result;
};
/* used to store goertzel parameters - float array version */
typedef struct goertzel_struct_array_float_ goertzel_array_float_t;


/* used to store goertzel parameters - fixed64 array version */
struct goertzel_struct_array_fixed64_
{
    uint_fast16_t size_array;
    uint_fast8_t shift;
    int64_t cr_fix;
    int64_t ci_fix;
    int64_t coeff_fix;
    int64_t real_fix;
    int64_t imag_fix;
    float result;
};
/* used to store goertzel parameters - fixed64 array version */
typedef struct goertzel_struct_array_fixed64_ goertzel_array_fixed64_t;


/* used to store goertzel parameters - float sample version */
struct goertzel_struct_sample_float_
{
    uint_fast16_t size_array;
    uint_fast16_t counter;
    float cr_float;
    float ci_float;
    float coeff_float;
    float s_float;
    float sprev_float;
    float sprev_float2;
    float real_float;
    float imag_float;
    float result;
};
/* used to store goertzel parameters - float sample version */
typedef struct goertzel_struct_sample_float_ goertzel_sample_float_t;


/* used to store goertzel parameters - fixed64 sample version */
struct goertzel_struct_sample_fixed64_
{
    uint_fast16_t size_array;
    uint_fast8_t shift;
    uint_fast16_t counter;
    int64_t cr_fix;
    int64_t ci_fix;
    int64_t coeff_fix;
    int64_t s_fix;    // usar variavel na funcao
    int64_t sprev_fix;
    int64_t sprev_fix2;
    int64_t real_fix;
    int64_t imag_fix;
    float result;
};
/* used to store goertzel parameters - fixed64 sample version */
typedef struct goertzel_struct_sample_fixed64_ goertzel_sample_fixed64_t;





/******************************************************************************
 *                  MATH FUNCTIONS - prototypes
 ******************************************************************************/

/******************************************************************************
 *                  SQRT FUNCTION - INTEGER VERSION
 ******************************************************************************/
/* SQRT using integer math */
int32_t sqrt_Int32(int32_t x);

/******************************************************************************
 *                  RMS VALUE - ARRAY VERSION
 ******************************************************************************/
float rmsValueArray_Float_StdMath(const float * arrayIn, uint_fast16_t size, float dcLevel);
float rmsValueArray_Int16_StdMath(const int16_t * arrayIn, uint_fast16_t size, int16_t dcLevel);

/******************************************************************************
 *                  RMS VALUE - SAMPLE BY SAMPLE VERSION
 ******************************************************************************/
void rmsValueAddSample_Float(rms_float_t * inputStruct, float sample);
void rmsValueAddSample_Int16(rms_int16_t * inputStruct, int16_t sample);

void rmsClearStruct_Float(rms_float_t * inputStruct);
void rmsClearStruct_Int16(rms_int16_t * inputStruct);

void rmsValueCalcRmsStdMath_Float(rms_float_t * inputStruct);
void rmsValueCalcRmsStdMath_Int16(rms_int16_t * inputStruct);


/******************************************************************************
 *                  SINE WAVE GENERATOR FUNCTIONS
 ******************************************************************************/
void sineWaveGen_Array_Float(float * outputArray, float freq, float phase_rad, float amplitude, float V_offset, uint_fast16_t points, uint_fast8_t doClean);

void sineWaveGen_bySample_Init(sine_wave_parameters *inputParameters, float freq, float phase, float amp, float v_off, uint_fast16_t points, uint_fast8_t doClean);
float sineWaveGen_GetSample(sine_wave_parameters *inputParameters);



/******************************************************************************
 *                  DSP FUNCTIONS - prototypes
 ******************************************************************************/

/******************************************************************************
 *                  HIGH PASS FILTER FUNCTIONS
 ******************************************************************************/
void iir_SinglePoleHighPass_Float_Init(iirHighPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean);
//__inline void iir_SinglePoleHighPass_Float_Init(iirHighPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean);
void iir_SinglePoleHighPass_Float(iirHighPassFloat_t * structInput, float xValueFloat);
//__inline void iir_SinglePoleHighPass_Float(iirHighPassFloat_t * structInput, float xValueFloat);

void iir_SinglePoleHighPass_Fixed_Init(iirHighPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
//__inline void iir_SinglePoleHighPass_Fixed_Init(iirHighPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
void iir_SinglePoleHighPass_Fixed(iirHighPassFixed_t * inputStuct, int32_t xValue);
//__inline void iir_SinglePoleHighPass_Fixed(iirHighPassFixed_t * inputStuct, int32_t xValue);

void iir_SinglePoleHighPass_FixedExtended_Init(iirHighPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
//__inline void iir_SinglePoleHighPass_FixedExtended_Init(iirHighPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
void iir_SinglePoleHighPass_FixedExtended(iirHighPassFixedExtended_t * inputStuct, int32_t xValue);
//__inline void iir_SinglePoleHighPass_FixedExtended(iirHighPassFixedExtended_t * inputStuct, int32_t xValue);


/******************************************************************************
 *                  LOW PASS FILTER FUNCTIONS
 ******************************************************************************/
void iir_SinglePoleLowPass_Float_Init(iirLowPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean);
//__inline void iir_SinglePoleLowPass_Float_Init(iirLowPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean);
void iir_SinglePoleLowPass_Float(iirLowPassFloat_t * inputStruct, float xValueFloat);
//__inline void iir_SinglePoleLowPass_Float(iirLowPassFloat_t * inputStruct, float xValueFloat);

void iir_SinglePoleLowPass_Fixed_Init(iirLowPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
//__inline void iir_SinglePoleLowPass_Fixed_Init(iirLowPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
void iir_SinglePoleLowPass_Fixed(iirLowPassFixed_t * inputStruct, int32_t xValue);
//__inline void iir_SinglePoleLowPass_Fixed(iirLowPassFixed_t * inputStruct, int32_t xValue);

void iir_SinglePoleLowPass_FixedExtended_Init(iirLowPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
//__inline void iir_SinglePoleLowPass_FixedExtended_Init(iirLowPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean);
void iir_SinglePoleLowPass_FixedExtended(iirLowPassFixedExtended_t * inputStruct, int32_t xValue);
//__inline void iir_SinglePoleLowPass_FixedExtended(iirLowPassFixedExtended_t * inputStruct, int32_t xValue);

void iir_SinglePoleLowPass_Fixed_Fast_Init(iirLowPassFixedFast_t * structInput, int_fast8_t attenuation, int_fast8_t doClean);
//__inline void iir_SinglePoleLowPass_Fixed_Fast_Init(iirLowPassFixedFast_t * structInput, int_fast8_t attenuation, int_fast8_t doClean);
void iir_SinglePoleLowPass_Fixed_Fast(iirLowPassFixedFast_t * inputStruct, int32_t xValue);
//__inline void iir_SinglePoleLowPass_Fixed_Fast(iirLowPassFixedFast_t * inputStruct, int32_t xValue);


/******************************************************************************
 *                  GOERTZEL DFT FUNCTIONS
 ******************************************************************************/
void goertzelArrayInit_Float(goertzel_array_float_t * inputStruct, float bin, uint_fast16_t size_array);
void goertzelArrayFloat_Float(goertzel_array_float_t * inputStruct, const float * arrayInput);
void goertzelArrayInt16_Float(goertzel_array_float_t * inputStruct, const int16_t * arrayInput);

void goertzelArrayInit_Fixed64(goertzel_array_fixed64_t * inputStruct, float bin, uint_fast16_t size_array, uint_fast8_t shift);
void goertzelArrayInt16_Fixed64(goertzel_array_fixed64_t * inputStruct, const int16_t * arrayInput);

void goertzelSampleInit_Float(goertzel_sample_float_t * inputStruct, float bin, uint_fast16_t size_array);
void goertzelSampleAddFloat_Float(goertzel_sample_float_t * inputStruct, float sample);
//__inline void goertzelSampleAddFloat_Float(goertzel_sample_float_t * inputStruct, float sample);
void goertzelSampleAddInt16_Float(goertzel_sample_float_t * inputStruct, int16_t sample);
//__inline void goertzelSampleAddInt16_Float(goertzel_sample_float_t * inputStruct, int16_t sample);
void goertzelSampleCalc_Float(goertzel_sample_float_t * inputStruct);

void goertzelSampleInit_Fixed64(goertzel_sample_fixed64_t* inputStruct, float bin, uint_fast16_t size_array, uint_fast8_t shift);
void goertzelSampleAddInt16_Fixed64(goertzel_sample_fixed64_t * inputStruct, int16_t sample);
//__inline void goertzelSampleAddInt16_Fixed64(goertzel_sample_fixed64_t * inputStruct, int16_t sample);
void goertzelSampleCalc_Fixed64(goertzel_sample_fixed64_t * inputStruct);


#ifdef __cplusplus
}
#endif

#endif /* DSP_AND_MATH_H_ */
