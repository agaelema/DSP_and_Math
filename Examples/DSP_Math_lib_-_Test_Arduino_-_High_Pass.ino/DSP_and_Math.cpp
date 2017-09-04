/******************************************************************************
 *  DSP_and_Math - Library with useful DSP and math functions - .c file
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

#include    "DSP_and_Math.h"
#include    "math.h"

/******************************************************************************
 *                          MATH FUNCTIONS
 ******************************************************************************/

///* Square root optimized */
//- square root
//https://stackoverflow.com/a/1101217
//https://stackoverflow.com/a/1100591
//https://stackoverflow.com/a/10330951
//https://stackoverflow.com/a/21626924
//http://www.realitypixels.com/turk/opensource/index.html#FractSqrt
//https://github.com/sensorium/Mozzi/blob/master/cogl_sqrti.h
//
//http://www.nxp.com/docs/en/application-note/AN4265.pdf
//- pag 9




/******************************************************************************
 *  Calculate the RMS value of N sample of an array
 *
 *  - INPUT:    const float * arrayIn   (pointer to array with the samples)
 *              uint_fast16_t size      (number of samples - limited to 65535)
 *              float dcLevel           (previously calculated dc level)
 *
 *  - RETURN:   calculated RMS value (float)
 ******************************************************************************/
float rmsValueArray_StdMath(const float * arrayIn, uint_fast16_t size, float dcLevel)
{
    volatile uint_fast16_t counter = size;
    volatile float sample_temp;
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
 *  Add sample to accumulator - Allow to calculate RMS value sample by sample
 *  - square input values and accumulate
 *  - Finalizing calculations in a separate function
 *
 *  - INPUT:    rms_float_t * inputStruct       (pointer to struct with RMS parameters)
 *              const float * sampleFloat       (pointer to float sample)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void rmsValueAddSample(rms_float_t * inputStruct, const float * sampleFloat)
{
    /* square value and accumulate */
    inputStruct->acc += ((*sampleFloat) * (*sampleFloat));
    /* increment counter - used in final step */
    inputStruct->size_counter++;
}


/******************************************************************************
 *  Finalize RMS calculation based in previously added samples
 *  - calculate the average and root square
 *
 *  - INPUT:    rms_float_t * inputStruct       (pointer to struct with RMS parameters)
 *
 *  - RETURN:   N/A (result returned inside the struct)
 ******************************************************************************/
void rmsValueCalcRmsStdMath(rms_float_t * inputStruct)
{
    /*
     * Finalize the math
     * - calculate the average and then extract square root - RMS value
     */
    inputStruct->rmsValue = inputStruct->acc / inputStruct->size_counter;
    inputStruct->rmsValue = sqrtf(inputStruct->rmsValue);
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
    volatile uint_fast8_t shift_temp = shift;
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
    volatile uint_fast8_t shift_temp = shift;
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
    volatile uint_fast8_t shift_temp = shift;

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
    volatile uint_fast8_t shift_temp = shift;
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
