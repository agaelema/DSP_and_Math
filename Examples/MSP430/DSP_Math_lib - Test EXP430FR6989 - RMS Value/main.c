/******************************************************************************
 *  MSP430FR6989 - RMS Value
 *  - Generate a sine wave with DC level and calculate the RMS value
 *  - test of versions
 *      array, sample by sample
 *
 *  Use "Embedded_Printf" library to print values
 *  - https://github.com/agaelema/Embedded_Printf
 *
 *  Author: Haroldo Amaral - agaelema@gmail.com
 *  2017/09/19
 ******************************************************************************/
#include    <msp430.h>
#include    "driverlib.h"
#include    <math.h>

#include    "embedded_printf.h"
#include    "DSP_and_Math.h"


#define   SEPARATOR     (',')       // used to separate plotted values

/* select if the wave will be draw sample by sample or based in a array */
//#define     DRAW_WAVE_BY_SAMPLE
#define     DRAW_WAVE_BY_ARRAY

#define     PI                  3.141592653589793f

/******************************************************************************
 * Define parameters of simulated wave
 ******************************************************************************/
#define     WAVE_DC_LEVEL       0.0f
#define     WAVE_AMPLITUDE      100.0f
#define     WAVE_POINTS         64


float a = 0;
float sample_original = 0;
int16_t sample_original_int16 = 0;

float array_sample_original[WAVE_POINTS];
float array_sample_no_dc[WAVE_POINTS];
int16_t array_sample_int16[WAVE_POINTS];


/******************************************************************************
 *  Initialize structures to save filter parameters
 ******************************************************************************/
rms_float_t rms_teste_float = {0};
rms_int16_t rms_teste_int16 = {0};



/******************************************************************************
 *          MAIN LOOP
 ******************************************************************************/
void main(void)
{
    /* Stop WDT */
    WDT_A_hold(WDT_A_BASE);

    /******************************************************************************
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     ******************************************************************************/
    PMM_unlockLPM5();

    /* embedded printf serial configuration */
    serial_configure();

    PJSEL0 |= BIT4 | BIT5;                      // configure pin for XT1

    /*
     * MCLK and SMCLK = DCOCLK 8MHz
     * ACLK = LFXT
     */
    CSCTL0_H = CSKEY >> 8;                      // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;               // Set DCO to 8MHz
    CSCTL2 = SELA__LFXTCLK |                    // ACLK source: LFXT
            SELS__DCOCLK |                      // SMCLK source : DCOCLK
            SELM__DCOCLK;                       // MCLK source: DCOCLK
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;       // Set all dividers
    CSCTL4 &= ~LFXTOFF;                         // enable LFXT
    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1&OFIFG);                 // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers


    volatile uint16_t counter = 0;

#ifdef      DRAW_WAVE_BY_ARRAY
    for (counter = 0; counter < WAVE_POINTS; counter++)
    {
        /*************************************
         * Generate a test signal - sine wave
         *************************************/
        /* fundamental sine wave */
        sineWaveGen_Array_Float(array_sample_original, 1.0f, 0.0f, WAVE_AMPLITUDE, WAVE_DC_LEVEL, WAVE_POINTS, WAVEGEN_CLEAN);
        /* 8th harmonic sine wave - sum with original wave */
        sineWaveGen_Array_Float(array_sample_original, 8.0f, 0.0f, WAVE_AMPLITUDE/16, 0, WAVE_POINTS, WAVEGEN_NOTCLEAN);
    }
    counter = 0;
#endif
#ifdef      DRAW_WAVE_BY_SAMPLE
    sine_wave_parameters sine1th = {0};
    sineWaveGen_bySample_Init(&sine1th, 1.0f, 0, WAVE_AMPLITUDE, WAVE_DC_LEVEL, WAVE_POINTS, WAVEGEN_CLEAN);

    sine_wave_parameters sine8th = {0};
    sineWaveGen_bySample_Init(&sine8th, 8.0f, 0, WAVE_AMPLITUDE/16, 0, WAVE_POINTS, WAVEGEN_CLEAN);
#endif


    volatile float sample_no_DC = 0;

    while(1)
    {
        if(counter < WAVE_POINTS)
        {

#ifdef  DRAW_WAVE_BY_ARRAY
            sample_original = array_sample_original[counter];
#endif

#ifdef  DRAW_WAVE_BY_SAMPLE
            /*************************************
             * Generate a test signal - sine wave
             *************************************/
            /* fundamental sine wave */
            sample_original = sineWaveGen_GetSample(&sine1th);
            /* 8th harmonic sine wave - sum with original wave */
            sample_original += sineWaveGen_GetSample(&sine8th);
#endif

            array_sample_original[counter] = (float)sample_original;
            array_sample_int16[counter] = (int16_t)sample_original;
            sample_original_int16 = (int16_t)sample_original;

            /* add new sample to RMS struct */
            rmsValueAddSample_Float(&rms_teste_float, sample_original);
            rmsValueAddSample_Int16(&rms_teste_int16, sample_original_int16);

            __no_operation();

            counter++;
        }
        else        /* after the last point, do some math and reset variables */
        {
            counter = 0;                            // reset counter

            volatile float rms_test_float0 = 0;
            volatile float rms_test_float1 = 0;
            volatile float rms_test_float2 = 0;
            volatile float rms_test_float3 = 0;
            volatile float rms_test_float4 = 0;
            volatile float rms_test_float5 = 0;
            /************************************************
             * test both methods to calculate RMS value
             * - by an entire array
             * - sample by sample (separate finalization
             ************************************************/

            /* finalize the math by sample */
            rmsValueCalcRmsStdMath_Float(&rms_teste_float);
            rms_test_float0 = rms_teste_float.rmsValue;

            rmsValueCalcRmsStdMath_Int16(&rms_teste_int16);
            rms_test_float1 = rms_teste_int16.rmsValue;


            print_string("RMS sample by sample: ");
            print_float(rms_test_float0, 4);    print_string(" ");
            print_float(rms_test_float1, 4);    print_string(" ");
            __no_operation();                               // debug point

            /* calculate rms - version array input */
            rms_test_float3 = rmsValueArray_Float_StdMath(array_sample_original, WAVE_POINTS, 0);
            rms_test_float4 = rmsValueArray_Int16_StdMath(array_sample_int16, WAVE_POINTS, 0);
            __no_operation();                               // debug point

            print_string("RMS of array: ");
            print_float(rms_test_float3, 4);     print_string(" ");
            print_float(rms_test_float4, 4);     print_string(" ");
            print_string("\r\n");
            __no_operation();                               // debug point
        }
    }
}
