/******************************************************************************
 *  MSP430FR6989 - Low Pass Filter
 *  - Generate a sine wave with DC level and apply Low Pass Filter
 *  - test of versions
 *      Float, Fixed, Fixed Extended (with more fractional bits), Fixed Fast
 *
 *  Use "Embedded_Printf" library to plot waves via UART
 *  - https://github.com/agaelema/Embedded_Printf
 *
 *  Excellent Serial plotter
 *  - https://hackaday.io/project/5334-serialplot-realtime-plotting-software
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
//#define     DRAW_WAVE_BY_SAMPLE
#define     DRAW_WAVE_BY_ARRAY

#define     PI                  3.1415926535897932384676

#define     WAVE_DC_LEVEL       512.0f
#define     WAVE_AMPLITUDE      100.0f
#define     WAVE_POINTS         64


float a = 0;
float sample_original = 0;
int32_t sample_original_int = 0;

float array_sample_original[WAVE_POINTS];
float array_sample_no_dc[WAVE_POINTS];

iirLowPassFloat_t voltage_LowPass_float_01;
iirLowPassFixed_t voltage_LowPass_fixed_01;
iirLowPassFixedExtended_t voltage_LowPass_fixedExtended_01;
iirLowPassFixedFast_t voltage_LowPass_fixedFast_01;
iirLowPassFixedFast_t voltage_LowPass_fixedFast_02;

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
     * Initialize structs - Low pass
     *************************************/
    iir_SinglePoleLowPass_Float_Init(&voltage_LowPass_float_01, 0.0051f, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point

    iir_SinglePoleLowPass_Fixed_Init(&voltage_LowPass_fixed_01, 0.0051f, 9, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point

    iir_SinglePoleLowPass_FixedExtended_Init(&voltage_LowPass_fixedExtended_01, 0.0051f, 20, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point

    iir_SinglePoleLowPass_Fixed_Fast_Init(&voltage_LowPass_fixedFast_01, 7, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point

    iir_SinglePoleLowPass_Fixed_Fast_Init(&voltage_LowPass_fixedFast_02, 7, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point


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
            print_float(sample_original, 2);
#endif

#ifdef  DRAW_WAVE_BY_SAMPLE
            /*************************************
             * Generate a test signal - sine wave
             *************************************/
            sample_original = WAVE_DC_LEVEL + sinf(a) * WAVE_AMPLITUDE;
            print_float(sample_original, 2);
            a += increment;
#endif



            /************************************************
             * do the filtering - Low Pass Filters
             ************************************************/

            iir_SinglePoleLowPass_Float(&voltage_LowPass_float_01, (float)sample_original);
            putChar(SEPARATOR);     print_float(voltage_LowPass_float_01.y, 2);

            iir_SinglePoleLowPass_Fixed(&voltage_LowPass_fixed_01, sample_original_int);
            putChar(SEPARATOR);     print_long((long)voltage_LowPass_fixed_01.y);

            iir_SinglePoleLowPass_FixedExtended(&voltage_LowPass_fixedExtended_01, sample_original_int);
            putChar(SEPARATOR);     print_long((long)voltage_LowPass_fixedExtended_01.y);

            /* first order */
            iir_SinglePoleLowPass_Fixed_Fast(&voltage_LowPass_fixedFast_01, sample_original_int);
            putChar(SEPARATOR);     print_long((long)voltage_LowPass_fixedFast_01.y);

            /* Second order - cascading */
            iir_SinglePoleLowPass_Fixed_Fast(&voltage_LowPass_fixedFast_02, voltage_LowPass_fixedFast_01.y);
            putChar(SEPARATOR);     print_long((long)voltage_LowPass_fixedFast_02.y);

            /* end of line */
            putChar('\r');        putChar('\n');

            counter++;
        }
        else        /* after the last point, reset variables */
        {
            counter = 0;                            // reset counter
        }
    }
}
