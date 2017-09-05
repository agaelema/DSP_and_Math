/******************************************************************************
 *  DSP_and_Math - Library with useful DSP and math functions - .h file
 *  - some functions using float point and standard math.h lib
 *  - some functions using fixed notation to (optimized)
 *
 *  author: Haroldo Amaral - agaelema@globo.com
 *  v0.2 - 2017/08/31
 ******************************************************************************
 *  log:
 *    v0.1  . Initial version
 *          + add rms functions and structures
 *          + add dc filter float version and structures
 *          + add dc filter fixed version and structures
 *          + add dc filter fixed extended version and structures
 *    v0.2  . rename dc filter to iir_hipassfilter
 *          . rename associated structures
 *          + add iir low pass filter and structures
 ******************************************************************************/

#ifndef _DSP_AND_MATH_H_
#define _DSP_AND_MATH_H_

#include    <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 *                              DEFINE / ENUM
 ******************************************************************************/

enum iir_clean
{
    IIR_FILTER_NOT_CLEAN = 0,
    IIR_FILTER_DO_CLEAN,
};




/******************************************************************************
 *              GLOBAL VARIABLES AND TYPEDEFs - prototypes
 ******************************************************************************/

/* used to store rms parameters */
struct rms_struct_float_
{
    uint_fast16_t size_counter;
    float acc;
    float rmsValue;
};
/* used to store rms parameters */
typedef struct rms_struct_float_ rms_float_t;


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
 *                  MATH FUNCTIONS - prototypes
 ******************************************************************************/

float rmsValueArray_StdMath(const float * arrayIn, uint_fast16_t size, float dcLevel);

void rmsValueAddSample(rms_float_t * inputStruct, const float * sampleFloat);
void rmsValueCalcRmsStdMath(rms_float_t * inputStruct);



/******************************************************************************
 *                  DSP FUNCTIONS - prototypes
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


#ifdef __cplusplus
}
#endif

#endif /* DSP_AND_MATH_H_ */
