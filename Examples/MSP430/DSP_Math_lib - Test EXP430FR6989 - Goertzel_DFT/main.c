/******************************************************************************
 *  MSP430FR6989 - Goertzel DFT
 *  - Generate a sine wave and use Goertzel algorithm to calculate harmonics
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


float sample_original = 0;
int16_t sample_original_int16 = 0;

float array_sample_original[WAVE_POINTS];
float array_sample_no_dc[WAVE_POINTS];
int16_t array_sample_int16[WAVE_POINTS];


/******************************************************************************
 *  Initialize structures to save goertzel parameters
 ******************************************************************************/
goertzel_array_float_t goertzel_01_01th;
goertzel_array_float_t goertzel_01_08th;
goertzel_sample_float_t goertzel_02_01th;
goertzel_sample_float_t goertzel_02_08th;
goertzel_array_fixed64_t goertzel_03_01th;
goertzel_array_fixed64_t goertzel_03_08th;

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


    /*************************************
     * Initialize Goertzel structures
     *************************************/
    goertzelArrayInit_Float(&goertzel_01_01th, 1, 64);
    goertzelArrayInit_Float(&goertzel_01_08th, 8, 64);
    goertzelSampleInit_Float(&goertzel_02_01th, 1, 64);
    goertzelSampleInit_Float(&goertzel_02_08th, 8, 64);
    goertzelArrayInit_Fixed64(&goertzel_03_01th, 1, 64, 18);
    goertzelArrayInit_Fixed64(&goertzel_03_08th, 8, 64, 18);


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


            /* add new samples to goertzel struct */
            goertzelSampleAddFloat_Float(&goertzel_02_01th, array_sample_original[counter]);
            goertzelSampleAddFloat_Float(&goertzel_02_08th, array_sample_original[counter]);

            __no_operation();

            counter++;
        }
        else        /* after the last point, do some math and reset variables */
        {
            counter = 0;                            // reset counter

            /* Goertzel DFT of an array */
            goertzelArrayFloat_Float(&goertzel_01_01th, array_sample_original);
            goertzelArrayFloat_Float(&goertzel_01_08th, array_sample_original);
            __no_operation();

            print_string(" Goertzel Array -");
            print_string(" 1th: ");              print_float(goertzel_01_01th.result, 2);
            print_string(" 8th: ");              print_float(goertzel_01_08th.result, 2);
            __no_operation();

            /* Goertzel DFT of sample-by-sample - finalize math */
            goertzelSampleCalc_Float(&goertzel_02_01th);
            goertzelSampleCalc_Float(&goertzel_02_08th);
            __no_operation();

            print_string(" Sample-by-sample -");
            print_string(" 1th: ");              print_float(goertzel_02_01th.result, 2);
            print_string(" 8th: ");              print_float(goertzel_02_08th.result, 2);
            __no_operation();

            goertzelArrayInt16_Fixed64(&goertzel_03_01th, array_sample_int16);
            goertzelArrayInt16_Fixed64(&goertzel_03_08th, array_sample_int16);

            print_string(" array fixed64 -");
            print_string(" 1th: ");              print_float(goertzel_03_01th.result, 2);
            print_string(" 8th: ");              print_float(goertzel_03_08th.result, 2);

            print_string("\r\n");
            __no_operation();
        }
    }
}
