/******************************************************************************
 *  DSP_and_Math - Library with useful DSP and math functions - .c file
 *  - some functions using float point and standard math.h lib
 *  - some functions using fixed notation to (optimized)
 *
 *  author: Haroldo Amaral - agaelema@globo.com
 *  v0.4.2 - 2017/10/09
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
 ******************************************************************************/

#include    "DSP_and_Math.h"
#include    "math.h"

/******************************************************************************
 *                          MATH FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 *  Calculate the Square root of a 32 bit signed number
 *  - will return "-1" if the number is negative
 *
 *  - INPUT:    int32_t x           (32 bit signed input number)
 *
 *  - RETURN:   root                (integer square root of x)
 *
 * Reference: http://www.codecodex.com/wiki/Calculate_an_integer_square_root
 ******************************************************************************/
int32_t sqrt_Int32(int32_t x)
{
    register uint32_t root, remainder, place;

    /* verify if number is negative */
    if (x < 0)
    {
        return (-1);        // return "-1" - error
    }
    /* verify if number is zero */
    if (x == 0)
    {
        return (0);
    }

    root = 0;
    remainder = x;
    place = 0x40000000;         // to 32 bit input

    while (place > remainder)
        place = place >> 2;
    while (place)
    {
        if (remainder >= root + place)
        {
            remainder = remainder - root - place;
            root = root + (place << 1);
        }
        root = root >> 1;
        place = place >> 2;
    }
    return root;
}




/******************************************************************************
 *  Calculate the RMS value of N sample of a float array
 *  - use float variables to accumulate and return the result in float
 *
 *  - INPUT:    const float * arrayIn   (pointer to array with the samples)
 *              uint_fast16_t size      (number of samples - limited to 65535)
 *              float dcLevel           (previously calculated dc level)
 *
 *  - RETURN:   calculated RMS value (float)
 ******************************************************************************/
float rmsValueArray_Float_StdMath(const float * arrayIn, uint_fast16_t size, float dcLevel)
{
    uint_fast16_t counter;
    float sample_temp;
    float acc = 0;

    /* check if there is a dc leve (different of zero) */
    if (dcLevel)
    {
        for (counter = 0; counter < size; counter++)
            {
                sample_temp = arrayIn[counter] - dcLevel;       // subtraction of dc level before square
                acc += (sample_temp * sample_temp);             // square and accumulate
            }
    }
    /* if dc level is zero, the subtraction can be removed */
    else
    {
        for (counter = 0; counter < size; counter++)
            {
                sample_temp = arrayIn[counter];                 // save the sample
                acc += (sample_temp * sample_temp);             // square and accumulate
            }
    }

    /*
     * calculate the average and then extract square root - RMS value
     */
    acc /= (float)size;
    return (float)sqrtf(acc);
}


/******************************************************************************
 *  Calculate the RMS value of N sample of a int16_t array
 *  - use integer (32 bits) variables to accumulate and return the result in float
 *  - limitations about the input limit during math stage
 *
 *  - INPUT:    const float * arrayIn   (pointer to array with the samples)
 *              uint_fast16_t size      (number of samples - limited to 65535)
 *              float dcLevel           (previously calculated dc level)
 *
 *  - RETURN:   calculated RMS value (float)
 ******************************************************************************/
float rmsValueArray_Int16_StdMath(const int16_t * arrayIn, uint_fast16_t size, int16_t dcLevel)
{
    uint_fast16_t counter;
    int32_t sample_temp;
    uint32_t acc = 0;

    /* check if there is a dc leve (different of zero) */
    if (dcLevel)
    {
        for (counter = 0; counter < size; counter++)
            {
            sample_temp = (int32_t)(arrayIn[counter] - dcLevel);        // subtraction of dc level before square
            acc = acc + (uint32_t)(sample_temp * sample_temp);          // square and accumulate
            }
    }
    /* if dc level is zero, the subtraction can be removed */
    else
    {
        for (counter = 0; counter < size; counter++)
            {
                sample_temp = (int32_t)arrayIn[counter];                // save the sample
                acc = acc + (uint32_t)(sample_temp * sample_temp);      // square and accumulate
            }
    }

    /*
     * calculate the average and then extract square root - RMS value
     */
#if defined (RMS_ARRAY_STD)
    /************************************************************
     * version using std math.h square root function
     ************************************************************/
    float result;
    result = (float)acc/size;
    return sqrtf(result);

#elif   defined(RMS_ARRAY_OPTIMIZED)
    /************************************************************
     * version using integer square root function
     ************************************************************/
    uint32_t result;
    result = acc/size;

    /************************************************************
     * To reach better accuracy, shift the result when possible
     * - transform in a fixed point math
     ************************************************************/
    if (result & (3UL << 30))   // verify if bit 30 and 31 is true
    {
        result = sqrt_Int32(result);    // if yes, do the sqrt
        return (float)(result);
    }
    else if (result & (3UL << 28))  // verify bits 28 and 29
    {
        result = sqrt_Int32((result) << 1); // rotate 1x to left == multiply by 2
        return (float)(result/1.414213f);   // divide the result by sqrt of 2
    }
    else if (result & (15UL << 24)) // verify bits between 24 and 27
    {
        result = sqrt_Int32((result) << 3); // rotate 3x to left == multiply by 2
        return (float)(result/2.828427f);   // divide the result by sqrt of 8
    }
    else if (result & (15UL << 20))
    {
        result = sqrt_Int32((result) << 7);
        return (float)(result/11.313708f);
    }
    else if (result & (15UL << 16))
    {
        result = sqrt_Int32((result) << 11);
        return (float)(result/45.254834f);
    }
    else if (result & (15UL << 12))
    {
        result = sqrt_Int32((result) << 15);
        return (float)(result/181.019336f);
    }
    else if (result & (15UL << 8))
    {
        result = sqrt_Int32((result) << 19);
        return (float)(result/724.077343f);
    }
    else
    {
        result = sqrt_Int32((result) << 23);
        return (float)(result/2896.309376f);
    }

#else
#error      "RMS Array Int16 - invalid option, select one define!"
#endif
}




/******************************************************************************
 *  Add sample to accumulator - Allow to calculate RMS value sample by sample
 *  - square input values and accumulate
 *  - Finalizing calculations in a separate function
 *
 *  - INPUT:    rms_float_t * inputStruct       (pointer to struct with RMS parameters)
 *              const float * sampleFloat       (pointer to float sample)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
//void rmsValueAddSample_Float(rms_float_t * inputStruct, const float * sample)
void rmsValueAddSample_Float(rms_float_t * inputStruct, float sample)
{
    /* square value and accumulate */
    inputStruct->acc += (sample * sample);
    /* increment counter - used in final step */
    inputStruct->size_counter++;
}


/******************************************************************************
 *  Add sample to accumulator - Allow to calculate RMS value sample by sample
 *  - square input values and accumulate
 *  - Finalizing calculations in a separate function
 *
 *  - INPUT:    rms_int16_t * inputStruct       (pointer to struct with RMS parameters)
 *              const float * sampleFloat       (pointer to float sample)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
//void rmsValueAddSample_Int16(rms_int16_t * inputStruct, const int16_t * sample)
void rmsValueAddSample_Int16(rms_int16_t * inputStruct, int16_t sample)
{
    int32_t sample_temp;
    sample_temp = (int32_t)sample;                 // save the sample

    /* square value and accumulate */
    inputStruct->acc = inputStruct->acc + (uint32_t)(sample_temp * sample_temp);             // square and accumulate
    /* increment counter - used in final step */
    inputStruct->size_counter++;
}


/******************************************************************************
 *  Clear RMS Float Struct - reset all parameters (variables)
 *  - enable to clear some rms calculation without do the math
 *
 *  - INPUT:    rms_float_t * inputStruct       (pointer to struct with RMS parameters)
 *
 *  - RETURN:   N/A
 ******************************************************************************/
void rmsClearStruct_Float(rms_float_t * inputStruct)
{
    inputStruct->acc = 0;
    inputStruct->rmsValue = 0;
    inputStruct->size_counter = 0;
}


/******************************************************************************
 *  Clear RMS Int16 Struct - reset all parameters (variables)
 *  - enable to clear some rms calculation without do the math
 *
 *  - INPUT:    rms_int16_t * inputStruct       (pointer to struct with RMS parameters)
 *
 *  - RETURN:   N/A
 ******************************************************************************/
void rmsClearStruct_Int16(rms_int16_t * inputStruct)
{
    inputStruct->acc = 0;
    inputStruct->rmsValue = 0;
    inputStruct->size_counter = 0;
}


/******************************************************************************
 *  Finalize RMS calculation based in previously added samples
 *  - calculate the average and root square
 *
 *  - INPUT:    rms_float_t * inputStruct       (pointer to struct with RMS parameters)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void rmsValueCalcRmsStdMath_Float(rms_float_t * inputStruct)
{
    /*
     * Finalize the math
     * - calculate the average and then extract square root - RMS value
     */
    inputStruct->rmsValue = inputStruct->acc / inputStruct->size_counter;
    inputStruct->rmsValue = sqrtf(inputStruct->rmsValue);
    inputStruct->acc = 0;                   // clear accumulator
    inputStruct->size_counter = 0;          // clear counter
}


/******************************************************************************
 *  Finalize RMS calculation based in previously added samples
 *  - calculate the average and root square
 *
 *  - INPUT:    rms_float_t * inputStruct       (pointer to struct with RMS parameters)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void rmsValueCalcRmsStdMath_Int16(rms_int16_t * inputStruct)
{

    /*
     * Finalize the math
     * - calculate the average and then extract square root - RMS value
     */
#if defined (RMS_SAMPLE_STD)
    volatile float result;
    result = (float)inputStruct->acc / inputStruct->size_counter;
    inputStruct->rmsValue = sqrtf(result);
    inputStruct->acc = 0;                   // clear accumulator
    inputStruct->size_counter = 0;          // clear counter

#elif   defined(RMS_SAMPLE_OPTIMIZED)
    uint32_t result;
    result = inputStruct->acc/inputStruct->size_counter;

    if (result & (3UL << 30))
    {
        result = sqrt_Int32(result);
        inputStruct->rmsValue = result;
    }
    else if (result & (3UL << 28))
    {
        result = sqrt_Int32((result) << 1);
        inputStruct->rmsValue = (float)(result/1.414213f);
    }
    else if (result & (15UL << 24))
    {
        result = sqrt_Int32((result) << 3);
        inputStruct->rmsValue = (float)(result/2.828427f);
    }
    else if (result & (15UL << 20))
    {
        result = sqrt_Int32((result) << 7);
        inputStruct->rmsValue = (float)(result/11.313708f);
    }
    else if (result & (15UL << 16))
    {
        result = sqrt_Int32((result) << 11);
        inputStruct->rmsValue = (float)(result/45.254834f);
    }
    else if (result & (15UL << 12))
    {
        result = sqrt_Int32((result) << 15);
        inputStruct->rmsValue = (float)(result/181.019336f);
    }
    else if (result & (15UL << 8))
    {
        result = sqrt_Int32((result) << 19);
        inputStruct->rmsValue = (float)(result/724.077343f);
    }
    else
    {
        result = sqrt_Int32((result) << 23);
        inputStruct->rmsValue = (float)(result/2896.309376f);
    }

#else
#error      "RMS Sample by Sample Int16 - invalid option, select one define!"
#endif

}




/******************************************************************************
 *  Sine wave generator - array version
 *  - using an Array
 *  - doClean enable to generate waves with harmonics by reusing the array
 *
 *  - INPUT:    float * outputArray         (array to store samples)
 *              float freq                  (frequency of signal)
 *              float phase_rad             (phase displacement in rad)
 *              float amplitude             (amplitude of wave - peak value)
 *              float V_offset              (offset value)
 *              uint_fast16_t points        (points peer cycle)
 *              uint_fast8_t doClean        (clean the array before gen)
 *
 *  - RETURN:   N/A
 ******************************************************************************/
void sineWaveGen_Array_Float(float * outputArray, float freq, float phase_rad,
                             float amplitude, float V_offset, uint_fast16_t points, uint_fast8_t doClean)
{
    uint_fast16_t counter;

    /* clean array before calculate new samples */
    if (doClean)
    {
        for (counter = 0; counter < points; counter++)
        {
            outputArray[counter] = 0;                   // reset all array
        }
    }

    float increment = (TWO_PI * freq)/points;           // samples interval in rad
    float x = 0;

    for (counter = 0; counter < points; counter++)
    {
        outputArray[counter] += (amplitude * sinf(x + phase_rad)) + V_offset;
        x += increment;
    }
}


/******************************************************************************
 *  Sine wave generator - Initialize struct parameters
 *  - sample-by-sample version - generate wave on the fly
 *
 *  - INPUT:    sine_wave_parameters *inputParameters   (struct with parameters)
 *              float freq                          (frequency of signal)
 *              float phase_rad                     (phase displacement in rad)
 *              float amplitude                     (amplitude of wave - peak value)
 *              float V_offset                      (offset value)
 *              uint_fast16_t points                (points peer cycle)
 *              uint_fast8_t doClean                (clean the accumulator - reset wave)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void sineWaveGen_bySample_Init(sine_wave_parameters *inputParameters, float freq, float phase, float amp, float v_off, uint_fast16_t points, uint_fast8_t doClean)
{
    inputParameters->freq = freq;
    inputParameters->phase_rad = phase;
    inputParameters->amplitude = amp;
    inputParameters->V_offset = v_off;
    inputParameters->points = points;

    inputParameters->increment = (TWO_PI * freq)/points;         // samples interval in rad

    if (doClean)
    {
        inputParameters->acc = 0;
    }
}


/******************************************************************************
 *  Sine wave generator - Calculate the current sample
 *
 *  - INPUT:    sine_wave_parameters *inputParameters   (struct with parameters)
 *
 *  - RETURN:   (float)WaveSample                   (current sample)
 ******************************************************************************/
float sineWaveGen_GetSample(sine_wave_parameters *inputParameters)
{
    float WaveSample = 0;
//    float sine_param = inputParameters->acc + inputParameters->phase_rad;
    float sine_param = inputParameters->acc + inputParameters->phase_rad;

    /* calculate the sample */
    WaveSample = inputParameters->amplitude * sinf(sine_param) + inputParameters->V_offset;
    inputParameters->acc += inputParameters->increment;     // increment the accumulator
    return WaveSample;
}




/******************************************************************************
 *                          DSP FUNCTIONS
 ******************************************************************************/

/******************************************************************************
 *  IIR Single Pole High Pass - Float Version - Initialization
 *
 * - INPUT:     iirHighPassFloat_t * structInput    (pointer to struct with filter parameters)
 *              float cutoffFreq                    (pole value)
 *              uint_fast8_t doClean                (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleHighPass_Float_Init(iirHighPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean)
{
    structInput->cutoff_Freq = (1.0f - cutoffFreq);

    if (doClean)
    {
        structInput->prev_x = 0;
        structInput->prev_y = 0;
        structInput->y = 0;
    }
}


/******************************************************************************
 *  IIR Single Pole High Pass - Float Version
 *  - use float math
 *  - more efficient with FPU
 *
 * - INPUT:     iirHighPassFloat_t * structInput    (pointer to struct with filter parameters)
 *              float xValueFloat                   (input/sample value)
 *
 * - RETURN:    N/A (result returned inside the struct)
 *
 * Reference: https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
 ******************************************************************************/
void iir_SinglePoleHighPass_Float(iirHighPassFloat_t * structInput, float xValueFloat)
{
    /********************************
     * y = x - xm1 + (0.995 * ym1);
     * xm1 = x;
     * ym1 = y;
     ********************************/
    structInput->y = xValueFloat - structInput->prev_x + (structInput->cutoff_Freq * structInput->prev_y);
    structInput->prev_x = xValueFloat;
    structInput->prev_y = structInput->y;
}




/******************************************************************************
 *  IIR Single Pole High Pass - Fixed Version Initialization
 *
 *  - INPUT:    iirHighPassFixed_t * structInput    (pointer to struct with filter parameters)
 *              float cutoffFreq                    (pole value)
 *              uint_fast8_t shift                  (shift of fixed math - from 8 to 15)
 *              uint_fast8_t doClean                (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleHighPass_Fixed_Init(iirHighPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean)
{
    uint_fast8_t shift_temp = shift;

    /*
     * prevent shift bigger than 15 (filter stop to work)
     */
    if (shift_temp > 15)
    {
        structInput->shift_size = 15;
    }
    else if(shift_temp < 8)
    {
        structInput->shift_size = 8;
    }
    else
    {
        structInput->shift_size = shift_temp;
    }

    structInput->cutoff_Freq = cutoffFreq;
    structInput->A_param = (int32_t)((1u << structInput->shift_size) * (structInput->cutoff_Freq));

    if (doClean)
    {
        structInput->acc = 0;
        structInput->prev_x = 0;
        structInput->y = 0;
        structInput->prev_y = 0;
    }
}


/******************************************************************************
 *  IIR Single Pole High Pass - Fixed Version
 *  - use fixed integer math with internal variables of 32 bit
 *  - faster than float version without FPU
 *
 *  - INPUT:    iirHighPassFixed_t * inputStuct (pointer to struct with filter parameters)
 *              int32_t xValue                  (input/sample value)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 *
 ******************************************************************************
 * - LIMITS:    SHIFTS      INPUT VALUE (Approximate value, influenced by cutoff value)
 *                15          +/- 61.7k
 *                14          +/-123.4k
 *                13          +/-246.9k
 *                12          +/-493.8k
 *                11          +/-987.5k
 *                10          +/-1.975M
 *                 9          +/-3.950M
 *                 8          +/-7.900M
 *
 * Reference: https://dspguru.com/dsp/tricks/fixed-point-dc-blocking-filter-with-noise-shaping/
 ******************************************************************************/
void iir_SinglePoleHighPass_Fixed(iirHighPassFixed_t * inputStuct, int32_t xValue)
{
    inputStuct->acc -= inputStuct->prev_x;
    inputStuct->prev_x = (xValue << inputStuct->shift_size);
    inputStuct->acc += inputStuct->prev_x;
    inputStuct->acc -= (inputStuct->A_param * inputStuct->prev_y);
    inputStuct->prev_y = inputStuct->acc >> inputStuct->shift_size;
    inputStuct->y = inputStuct->prev_y;
}




/******************************************************************************
 *  IIR Single Pole High Pass - Fixed Extended Version Initialization
 *
 *  - INPUT:    iirHighPassFixedExtended_t * structInput    (pointer to struct with filter parameters)
 *              double cutoffFreq                           (pole value)
 *              uint_fast8_t shift                          (shift of fixed math - from 8 to 15)
 *              uint_fast8_t doClean                        (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleHighPass_FixedExtended_Init(iirHighPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean)
{
    uint_fast8_t shift_temp = shift;

    /*
     * prevent shift bigger than 30 (filter stop to work)
     */
    if (shift_temp > 30)
    {
        structInput->shift_size = 30;
    }
    else if(shift_temp < 8)
    {
        structInput->shift_size = 8;
    }
    else
    {
        structInput->shift_size = shift_temp;
    }

    structInput->cutoff_Freq = cutoffFreq;
    structInput->A_param = (int32_t)( (1L << structInput->shift_size) * (structInput->cutoff_Freq) );
    if (doClean)
    {
        structInput->acc = 0;
        structInput->prev_x = 0;
        structInput->y = 0;
        structInput->prev_y = 0;
    }
}


/******************************************************************************
 *  IIR Single Pole High Pass - Fixed Extended Version
 *  - use fixed integer math with internal variables of 64 bit
 *  - more headroom
 *
 *  - INPUT:    iirHighPassFixedExtended_t * inputStuct (pointer to struct with filter parameters)
 *              int32_t xValue                          (input/sample value)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 *
 ******************************************************************************
 * - LIMITS:    SHIFTS      INPUT VALUE (Approximate value, influenced by cutoff)
 *                30          +/- 61.7k
 *                29          +/-123.4k
 *                28          +/-246.9k
 *                27          +/-493.8k
 *                26          +/-987.5k
 *                25          +/-1.975M
 *                24          +/-3.950M
 *                23          +/-7.900M
 *
 * Reference: https://dspguru.com/dsp/tricks/fixed-point-dc-blocking-filter-with-noise-shaping/
 ******************************************************************************/
void iir_SinglePoleHighPass_FixedExtended(iirHighPassFixedExtended_t * inputStuct, int32_t xValue)
{
    inputStuct->acc -= inputStuct->prev_x;
    inputStuct->prev_x = ((int64_t)xValue << inputStuct->shift_size);
    inputStuct->acc += inputStuct->prev_x;
    inputStuct->acc -= (inputStuct->A_param * (int64_t)inputStuct->prev_y);
    inputStuct->prev_y = (int32_t)(inputStuct->acc >> inputStuct->shift_size);
    inputStuct->y = inputStuct->prev_y;
}




/******************************************************************************
 *  IIR Single Pole Low Pass - Float Version - Initialization
 *
 *  - INPUT:    iirLowPassFloat_t * structInput     (pointer to struct with filter parameters)
 *              float cutoffFreq                    (pole value)
 *              uint_fast8_t doClean                (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleLowPass_Float_Init(iirLowPassFloat_t * structInput, float cutoffFreq, uint_fast8_t doClean)
{
    structInput->cutoff_Freq = cutoffFreq;

    /***************************************
     * b0 = (1.0f - x)      input coeff
     * a1 = x               output coeff
     ***************************************/
    structInput->b0 = (structInput->cutoff_Freq);
    structInput->a1 = (1.0f - structInput->cutoff_Freq);

    if (doClean)
    {
        structInput->prev_y = 0;
        structInput->y = 0;
    }
}



/******************************************************************************
 *  IIR Single Pole Low Pass - Float Version
 *  - use float math (more efficient with FPU)
 *
 *  - INPUT:    iirLowPassFloat_t * structInput     (pointer to struct with filter parameters)
 *              float xValueFloat                   (input/sample value)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 *
 *  Reference: http://www.dspguide.com/ch19.htm
 ******************************************************************************/
void iir_SinglePoleLowPass_Float(iirLowPassFloat_t * inputStruct, float xValueFloat)
{
    /***************************************
     * b = input coefficients
     * a = output coefficients
     *
     * y = b0*input + a1*y1
     * y1 = y
     ***************************************/
    inputStruct->y = (inputStruct->b0 * xValueFloat) + (inputStruct->a1 * inputStruct->prev_y);
    inputStruct->prev_y = inputStruct->y;
}




/******************************************************************************
 *  IIR Single Pole Low Pass - Fixed Version - Initialization
 *
 *  - INPUT:    iirLowPassFixed_t * structInput     (pointer to struct with filter parameters)
 *              float cutoffFreq                    (pole value)
 *              uint_fast8_t shift                  (shift of fixed math - from 8 to 15)
 *              uint_fast8_t doClean                (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleLowPass_Fixed_Init(iirLowPassFixed_t * structInput, float cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean)
{
    uint_fast8_t shift_temp = shift;

    /**********************************************************
     * prevent shift bigger than 12 (filter stop to work)
     **********************************************************/
    if (shift_temp > 12)
    {
        structInput->shift_size = 12;
    }
    else if(shift_temp < 8)
    {
        structInput->shift_size = 8;
    }
    else
    {
        structInput->shift_size = shift_temp;
    }

    structInput->RoundNumber = (1l << structInput->shift_size);

    structInput->cutoff_Freq = cutoffFreq;
    structInput->A_param = (int32_t)(structInput->cutoff_Freq * (1l << structInput->shift_size));

    if (doClean)
        {
            structInput->SHIFTED_filtered = 0;
            structInput->SHIFTED_last_filtered = 0;
            structInput->y = 0;
        }
}


/******************************************************************************
 *  IIR Single Pole Low Pass - Fixed Version
 *
 *  - INPUT:    iirLowPassFixed_t * structInput     (pointer to struct with filter parameters)
 *              int32_t xValue                      (input/sample value)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 *
 ******************************************************************************
 * - LIMITS:    SHIFTS      INPUT VALUE (Approximate value, influenced by cuttof)
 *                12          +/- 30k
 *                11          +/- 120k
 *                10          +/- 480k
 *                09          +/- 1.920M
 *                08          +/- 7.680M
 *
 *  Reference: https://learn.openenergymonitor.org/electricity-monitoring/ctac/digital-filters-for-offset-removal
 ******************************************************************************/
void iir_SinglePoleLowPass_Fixed(iirLowPassFixed_t * inputStruct, int32_t xValue)
{
    /***************************************
     * y = y1 + cutoffFreq * (input - y1)
     * y1 = y
     ***************************************/

    /* RoundNumber math adding 0.5 before return the number */
    inputStruct->SHIFTED_filtered = inputStruct->SHIFTED_last_filtered + (inputStruct->A_param * ((xValue << inputStruct->shift_size) - inputStruct->SHIFTED_filtered + inputStruct->RoundNumber) >> inputStruct->shift_size);
    inputStruct->SHIFTED_last_filtered = inputStruct->SHIFTED_filtered;
    inputStruct->y = inputStruct->SHIFTED_filtered >> inputStruct->shift_size;
}



/******************************************************************************
 *  IIR Single Pole Low Pass - Fixed Extended Version - Initialization
 *
 *  - INPUT:    iirLowPassFixedExtended_t * structInput (pointer to struct with filter parameters)
 *              double cutoffFreq                       (pole value)
 *              uint_fast8_t shift                      (shift of fixed math - from 8 to 15)
 *              uint_fast8_t doClean                    (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleLowPass_FixedExtended_Init(iirLowPassFixedExtended_t * structInput, double cutoffFreq, uint_fast8_t shift, uint_fast8_t doClean)
{
    uint_fast8_t shift_temp = shift;

    /*
     * prevent shift bigger than 28 (filter stop to work)
     */
    if (shift_temp > 28)
    {
        structInput->shift_size = 28;
    }
    else if(shift_temp < 8)
    {
        structInput->shift_size = 8;
    }
    else
    {
        structInput->shift_size = shift_temp;
    }

    structInput->RoundNumber = (int64_t)(1LL << structInput->shift_size);

    structInput->cutoff_Freq = cutoffFreq;
    structInput->A_param = (int64_t)(structInput->cutoff_Freq * (1LL << structInput->shift_size));

    if (doClean)
        {
            structInput->SHIFTED_filtered = 0;
            structInput->SHIFTED_last_filtered = 0;
            structInput->y = 0;
        }
}



/******************************************************************************
 *  IIR Single Pole Low Pass - Fixed Extended Version
 *
 *  - INPUT:    iirLowPassFixed_t * structInput     (pointer to struct with filter parameters)
 *              int32_t xValue                      (input/sample value)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 *
 *  Reference: https://learn.openenergymonitor.org/electricity-monitoring/ctac/digital-filters-for-offset-removal
 ******************************************************************************/
void iir_SinglePoleLowPass_FixedExtended(iirLowPassFixedExtended_t * inputStruct, int32_t xValue)
{
    /***************************************
     * y = y1 + cutoffFreq * (input - y1)
     * y1 = y
     ***************************************/

    /* RoundNumber math adding 0.5 before return the number */
    inputStruct->SHIFTED_filtered = inputStruct->SHIFTED_last_filtered + (inputStruct->A_param * (((int64_t)xValue << inputStruct->shift_size) - inputStruct->SHIFTED_filtered + inputStruct->RoundNumber) >> inputStruct->shift_size);
    inputStruct->SHIFTED_last_filtered = inputStruct->SHIFTED_filtered;
    inputStruct->y = (int32_t)(inputStruct->SHIFTED_filtered >> inputStruct->shift_size);
}




/******************************************************************************
 *  IIR Single Pole Low Pass - Fixed FAST Version - Initialization
 *
 *  - INPUT:    iirLowPassFixedExtended_t * structInput (pointer to struct with filter parameters)
 *              int_fast8_t attenuation                 (attenuation factor - see reference)
 *              uint_fast8_t doClean                    (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void iir_SinglePoleLowPass_Fixed_Fast_Init(iirLowPassFixedFast_t * structInput, int_fast8_t attenuation, int_fast8_t doClean)
{
    structInput->attenuation = attenuation;

    if (doClean)
    {
        structInput->filter_acc = 0;
        structInput->y = 0;
    }
}


/******************************************************************************
 *  IIR Single Pole Low Pass - Fixed FAST Version - Initialization
 *  - leaky integrator - very efficient
 *
 *  - INPUT:    iirLowPassFixedExtended_t * structInput (pointer to struct with filter parameters)
 *              int_fast8_t attenuation                 (attenuation factor - see reference)
 *              uint_fast8_t doClean                    (clean variables after change the pole)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 *
 *  Reference: http://www.edn.com/design/systems-design/4320010/A-simple-software-lowpass-filter-suits-embedded-system-applications
 ******************************************************************************/
void iir_SinglePoleLowPass_Fixed_Fast(iirLowPassFixedFast_t * inputStruct, int32_t xValue)
{
    /*******************************************************
     * filt = filt - (filt >> attenuationFactor) + input
     * y = filt >> attenuationFactor
     *******************************************************/
    inputStruct->filter_acc = inputStruct->filter_acc - (inputStruct->filter_acc >> inputStruct->attenuation) + xValue;
    inputStruct->y = (inputStruct->filter_acc >> inputStruct->attenuation);
}


/******************************************************************************
 *  Goertzel DFT - Float Array Version - Initialize Structure Parameters (FLOAT)
 *
 *  - INPUT:    goertzel_array_float_t * inputStruct    (pointer to struct with parameters)
 *              float bin                               (desired bin - what harmonic)
 *              uint_fast16_t size_array                (array size - number of samples)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelArrayInit_Float(goertzel_array_float_t * inputStruct, float bin, uint_fast16_t size_array)
{
    float w = (2 * PI * bin)/size_array;
    inputStruct->cr_float = cosf(w);
    inputStruct->ci_float = sinf(w);

    inputStruct->coeff_float = 2 * inputStruct->cr_float;
    inputStruct->size_array = size_array;
}


/******************************************************************************
 *  Goertzel DFT - Float Math Array Version - Do the Math (FLOAT INPUT)
 *  - Calculate the amplitude of a desired bin (frequency) of a wave
 *
 *  - INPUT:    goertzel_array_float_t * inputStruct    (pointer to struct with parameters)
 *              const float * arrayInput                (pointer to array with input samples)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelArrayFloat_Float(goertzel_array_float_t * inputStruct, const float * arrayInput)
{
    float s_float = 0;
    float sprev_float = 0;
    float sprev_float2 = 0;

    uint_fast16_t size_array = inputStruct->size_array;

    uint_fast16_t i;
    for (i = 0; i < size_array ; i++)
    {
        s_float = arrayInput[i] + (inputStruct->coeff_float * sprev_float) - sprev_float2;
        sprev_float2 = sprev_float;
        sprev_float = s_float;
    }

    float real_float = (sprev_float - sprev_float2 * inputStruct->cr_float);
    float imag_float = (sprev_float2 * inputStruct->ci_float);
    inputStruct->real_float = real_float;
    inputStruct->imag_float = imag_float;

    float result = (sqrtf((real_float*real_float)+(imag_float*imag_float)))/(size_array/2);
    inputStruct->result = result;
}


/******************************************************************************
 *  Goertzel DFT - Float Math Array Version - Do the Math (INT16 INPUT)
 *  - Calculate the amplitude of a desired bin (frequency) of a wave
 *
 *  - INPUT:    goertzel_array_float_t * inputStruct    (pointer to struct with parameters)
 *              const int16_t * arrayInput              (pointer to array with input samples)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelArrayInt16_Float(goertzel_array_float_t * inputStruct, const int16_t * arrayInput)
{
    float s_float = 0;
    float sprev_float = 0;
    float sprev_float2 = 0;

    uint_fast16_t size_array = inputStruct->size_array;

    uint_fast16_t i;
    for (i = 0; i < size_array ; i++)
    {
        s_float = (float)arrayInput[i] + inputStruct->coeff_float * sprev_float - sprev_float2;
        sprev_float2 = sprev_float;
        sprev_float = s_float;
    }

    float real_float = (sprev_float - sprev_float2 * inputStruct->cr_float);
    float imag_float = (sprev_float2 * inputStruct->ci_float);
    inputStruct->real_float = real_float;
    inputStruct->imag_float = imag_float;

//    float result = (sqrtf((real_float*real_float)+(imag_float*imag_float))) / ((float)size_array/2.0f);   // waste more time
//    float result = (sqrtf((real_float*real_float)+(imag_float*imag_float))) / (size_array/2);
    float result = (sqrtf((real_float*real_float)+(imag_float*imag_float)));        // extract square root
    result = result / size_array;                   // divide by the total of samples
    result = result * 2.0f;                         // multiply by 2
    inputStruct->result = result;                   // store in the struct
}


/******************************************************************************
 *  Goertzel DFT - Fixed 64 Math Array Version - Initialize Structure Parameters (FIXED64)
 *
 *  - INPUT:    goertzel_array_fixed64_t * inputStruct  (pointer to struct with parameters)
 *              float bin                               (desired bin - what harmonic)
 *              uint_fast16_t size_array                (array size - number of samples)
 *              uint_fast8_t shift                      (shift size used in fixed math)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelArrayInit_Fixed64(goertzel_array_fixed64_t * inputStruct, float bin, uint_fast16_t size_array, uint_fast8_t shift)
{
    float w = (2 * PI * bin)/size_array;
    float cr_float = cosf(w);
    float ci_float = sinf(w);

    inputStruct->cr_fix = (int64_t)(cr_float * (1LL << shift));     // try rounding
    inputStruct->ci_fix = (int64_t)(ci_float * (1LL << shift));     // try rounding

    float coeff_float = 2 * cr_float;
    inputStruct->coeff_fix = (int64_t)(coeff_float * (1LL << shift));

    inputStruct->size_array = size_array;
    inputStruct->shift = shift;
}


/******************************************************************************
 *  Goertzel DFT - Fixed 64 Math Array Version - Do the Math (INT16 INPUT)
 *  - Calculate the amplitude of a desired bin (frequency) of a wave
 *
 *  - INPUT:    goertzel_array_float_t * inputStruct    (pointer to struct with parameters)
 *              const int16_t * arrayInput              (pointer to array with input samples)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelArrayInt16_Fixed64(goertzel_array_fixed64_t * inputStruct, const int16_t * arrayInput)
{
    int64_t s_fix = 0;
    int64_t sprev_fix = 0;
    int64_t sprev_fix2 = 0;

    uint_fast16_t size_array = inputStruct->size_array;
    uint_fast8_t shift = inputStruct->shift;

    uint_fast16_t i;
    for (i = (0); i < size_array ; i++)
    {
        s_fix = ((int64_t)arrayInput[i] << shift) + ((int64_t)(inputStruct->coeff_fix * sprev_fix) >> shift) - sprev_fix2;
        sprev_fix2 = sprev_fix;
        sprev_fix = s_fix;
    }

    int64_t real_fix = (sprev_fix - ((sprev_fix2 * inputStruct->cr_fix) >> shift));
    int64_t imag_fix = (sprev_fix2 * inputStruct->ci_fix) >> shift;

    inputStruct->real_fix = real_fix;
    inputStruct->imag_fix = imag_fix;

    //    float result = sqrtf((float)((real_fix*real_fix)>>shift) + ((imag_fix*imag_fix)>>shift) );   // two shift
    float result = sqrtf((float)((((real_fix*real_fix) + (imag_fix*imag_fix)) >> shift)));      // one shift
    result = result / inputStruct->size_array;
    result = result * 2;


    float result_float = (float)result / (1LL << (shift >> 1)); // divide pela raiz quadarda de Shift. ex.: shift = 10 (2^10), divide por 2^5
    inputStruct->result = result_float;
}


/******************************************************************************
 *  Goertzel DFT - Float Math Sample-by-sample Version - Initialize Structure Parameters
 *
 *  - INPUT:    goertzel_sample_float_t * inputStruct   (pointer to struct with parameters)
 *              float bin                               (desired bin - what harmonic)
 *              uint_fast16_t size_array                (array size - number of samples)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleInit_Float(goertzel_sample_float_t * inputStruct, float bin, uint_fast16_t size_array)
{
    float w = (2 * PI * bin)/size_array;
    inputStruct->cr_float = cosf(w);
    inputStruct->ci_float = sinf(w);

    inputStruct->coeff_float = 2 * inputStruct->cr_float;
    inputStruct->size_array = size_array;
}


/******************************************************************************
 *  Goertzel DFT - Float Math Sample-by-sample Version - Add sample (FLOAT)
 *  - Pre calculate each sample
 *
 *  - INPUT:    goertzel_sample_float_t * inputStruct   (pointer to struct with parameters)
 *              float sample                            (input sample)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleAddFloat_Float(goertzel_sample_float_t * inputStruct, float sample)
{
    if (inputStruct->counter < inputStruct->size_array)
    {
        float s_float = sample + (inputStruct->coeff_float * inputStruct->sprev_float) - inputStruct->sprev_float2;
        inputStruct->sprev_float2 = inputStruct->sprev_float;
        inputStruct->sprev_float = s_float;

        inputStruct->counter++;
        inputStruct->s_float = s_float;
    }
}


/******************************************************************************
 *  Goertzel DFT - Float Math Sample-by-sample Version - Add sample (INT16)
 *  - Pre calculate each sample
 *
 *  - INPUT:    goertzel_sample_float_t * inputStruct   (pointer to struct with parameters)
 *              int16_t sample                          (input sample)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleAddInt16_Float(goertzel_sample_float_t * inputStruct, int16_t sample)
{
    if (inputStruct->counter < inputStruct->size_array)
    {
        float s_float = (float)sample + (inputStruct->coeff_float * inputStruct->sprev_float) - inputStruct->sprev_float2;
        inputStruct->sprev_float2 = inputStruct->sprev_float;
        inputStruct->sprev_float = s_float;

        inputStruct->counter++;
        inputStruct->s_float = s_float;
    }
}


/******************************************************************************
 *  Goertzel DFT - Float Math Sample-by-sample Version - Finalize math
 *  - calculate the Real, Imag and Magnitude
 *
 *  - INPUT:    goertzel_sample_float_t * inputStruct   (pointer to struct with parameters)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleCalc_Float(goertzel_sample_float_t * inputStruct)
{
    float real_float = (inputStruct->sprev_float - inputStruct->sprev_float2 * inputStruct->cr_float);
    float imag_float = (inputStruct->sprev_float2 * inputStruct->ci_float);
    inputStruct->real_float = real_float;
    inputStruct->imag_float = imag_float;

    float result = (sqrtf((real_float*real_float)+(imag_float*imag_float)))/(inputStruct->size_array/2);
    inputStruct->result = result;

    inputStruct->s_float = 0;
    inputStruct->sprev_float = 0;
    inputStruct->sprev_float2 = 0;
    inputStruct->counter = 0;
}


/******************************************************************************
 *  Goertzel DFT - Fixed 64 Math Sample-by-sample Version - Initialize Structure Parameters
 *
 *  - INPUT:    goertzel_sample_fixed64_t* inputStruct  (pointer to struct with parameters)
 *              float bin                               (desired bin - what harmonic)
 *              uint_fast16_t size_array                (array size - number of samples)
 *              uint_fast8_t shift                      (shift size used in fixed math)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleInit_Fixed64(goertzel_sample_fixed64_t* inputStruct, float bin, uint_fast16_t size_array, uint_fast8_t shift)
{
    float w = (2 * PI * bin)/size_array;
    float cr_float = cosf(w);
    float ci_float = sinf(w);

    inputStruct->cr_fix = (int64_t)(cr_float * (1LL << shift));     // try rounding
    inputStruct->ci_fix = (int64_t)(ci_float * (1LL << shift));     // try rounding

    float coeff_float = 2 * cr_float;
    inputStruct->coeff_fix = (int64_t)(coeff_float * (1LL << shift));

    inputStruct->size_array = size_array;
    inputStruct->shift = shift;
}


/******************************************************************************
 *  Goertzel DFT - Fixed 64 Math Sample-by-sample Version - Add sample (INT16)
 *  - Pre calculate each sample using fixed math
 *
 *  - INPUT:    goertzel_sample_fixed64_t * inputStruct (pointer to struct with parameters)
 *              int16_t sample                          (input sample)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleAddInt16_Fixed64(goertzel_sample_fixed64_t * inputStruct, int16_t sample)
{
    if (inputStruct->counter < inputStruct->size_array)
    {
        int_fast16_t shift = inputStruct->shift;

        int64_t s_fix = ((int64_t)sample << shift) + ((inputStruct->coeff_fix * inputStruct->sprev_fix) >> shift) - inputStruct->sprev_fix2;
        inputStruct->sprev_fix2 = inputStruct->sprev_fix;
        inputStruct->sprev_fix = s_fix;
        inputStruct->s_fix = s_fix;
    }
}


/******************************************************************************
 *  Goertzel DFT - Fixed 64 Math Sample-by-sample Version - Finalize math
 *  - calculate the Real, Imag and Magnitude - use float math in final step
 *
 *  - INPUT:    goertzel_sample_fixed64_t * inputStruct  (pointer to struct with parameters)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void goertzelSampleCalc_Fixed64(goertzel_sample_fixed64_t * inputStruct)
{
    int_fast16_t shift = inputStruct->shift;

    int64_t real_fix = (inputStruct->sprev_fix - ((inputStruct->sprev_fix2 * inputStruct->cr_fix) >> shift));
    int64_t imag_fix = (inputStruct->sprev_fix2 * inputStruct->ci_fix) >> shift;
    inputStruct->real_fix = real_fix;
    inputStruct->imag_fix = imag_fix;

//    float result = sqrtf((float)((real_fix*real_fix)>>shift) + ((imag_fix*imag_fix)>>shift) );   // two shift
    float result = sqrtf((float)((((real_fix*real_fix) + (imag_fix*imag_fix)) >> shift)));      // one shift
    result = result / inputStruct->size_array;
    result = result * 2;

    float result_float = (float)result / (1LL << (shift >> 1)); // sivide by sqrt(Shift). ex.: shift = 10 (2^10), divide by 2^5
    inputStruct->result = result_float;

    inputStruct->s_fix = 0;
    inputStruct->sprev_fix = 0;
    inputStruct->sprev_fix2 = 0;
    inputStruct->counter = 0;
}
