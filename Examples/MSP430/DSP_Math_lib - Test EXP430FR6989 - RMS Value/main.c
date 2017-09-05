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
 *  2017/09/04
 ******************************************************************************/
#include    <msp430.h>
#include    "driverlib.h"
#include    <math.h>

#include    "embedded_printf.h"
#include    "DSP_and_Math.h"


#define   SEPARATOR     (',')       // used to separate plotted values

/* select if the wave will be draw sample by sample or based in a array */
#define     DRAW_WAVE_BY_SAMPLE
//#define     DRAW_WAVE_BY_ARRAY

#define     PI                  3.1415926535897932384676

/******************************************************************************
 * Define parameters of simulated wave
 ******************************************************************************/
#define     WAVE_DC_LEVEL       0.0f
#define     WAVE_AMPLITUDE      100.0f
#define     WAVE_POINTS         64


float a = 0;
float sample_original = 0;
int32_t sample_original_int = 0;

float array_sample_original[WAVE_POINTS];
float array_sample_no_dc[WAVE_POINTS];

/******************************************************************************
 *  Initialize structures to save filter parameters
 ******************************************************************************/
rms_float_t rms_teste = {0};



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


    /* variables to calculate wave points and control the loop */
    volatile float increment = (2*PI)/WAVE_POINTS;
    volatile uint16_t counter = 0;

#ifdef      DRAW_WAVE_BY_ARRAY
    for (counter = 0; counter < WAVE_POINTS; counter++)
    {
        sample_original = WAVE_DC_LEVEL + sinf(a) * WAVE_AMPLITUDE;
//        sample_original = WAVE_DC_LEVEL + (sinf(a) * WAVE_AMPLITUDE) + (sinf(8*a) * (WAVE_AMPLITUDE/8));
        array_sample_original[counter] = sample_original;
        a += increment;
    }
   counter = 0;
#endif

   volatile float sample_no_DC = 0;

    while(1)
    {
        if(counter < WAVE_POINTS)
        {

#ifdef  DRAW_WAVE_BY_ARRAY
            sample_original = array_sample_original[counter];
            sample_original_int = (int32_t)sample_original;
//            print_float(sample_original, 2);
#endif

#ifdef  DRAW_WAVE_BY_SAMPLE
            /*************************************
             * Generate a test signal - sine wave
             *************************************/
            sample_original = WAVE_DC_LEVEL + sinf(a) * WAVE_AMPLITUDE;
//            sample_original = WAVE_DC_LEVEL + (sinf(a) * WAVE_AMPLITUDE) + (sinf(8*a) * (WAVE_AMPLITUDE/8));
            array_sample_original[counter] = sample_original;
            a += increment;
#endif

            /* add new sample to RMS struct */
            rmsValueAddSample(&rms_teste, &sample_original);

            counter++;
        }
        else        /* after the last point, do some math and reset variables */
        {
            counter = 0;                            // reset counter

            volatile float rms_test_float = 0;
            /************************************************
             * test both methods to calculate RMS value
             * - by an entire array
             * - sample by sample (separate finalization
             ************************************************/

            /* finalize the math by sample */
            rmsValueCalcRmsStdMath(&rms_teste);
            rms_test_float = rms_teste.rmsValue;
            __no_operation();                               // debug point
            print_string("RMS sample by sample: "); print_float(rms_test_float, 4);  print_string("    ");

            /* calculate rms - version array input */
            rms_test_float = rmsValueArray_StdMath(array_sample_original, WAVE_POINTS, 0);
            print_string("RMS of array: "); print_float(rms_test_float, 4);  print_string("\r\n");
            __no_operation();                               // debug point
        }
    }
}
