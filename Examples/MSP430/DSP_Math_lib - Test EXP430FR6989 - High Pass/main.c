/******************************************************************************
 *  MSP430FR6989 - High Pass Filter
 *  - Generate a sine wave with DC level and apply High Pass Filter
 *  - test of versions
 *      Float, Fixed, Fixed Extended (with more fractional bits)
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

/******************************************************************************
 * Define parameters of simulated wave
 ******************************************************************************/
#define     WAVE_DC_LEVEL       512.0f
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
iirHighPassFloat_t voltage_dcFilter_float;
iirHighPassFixed_t voltage_dcFilter_fixed_01;
iirHighPassFixedExtended_t voltage_dcFilter_fixed_extended;



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
     * Initialize structs - High pass
     *************************************/
    iir_SinglePoleHighPass_Float_Init(&voltage_dcFilter_float, 0.004f, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point

    iir_SinglePoleHighPass_Fixed_Init(&voltage_dcFilter_fixed_01, 0.004f, 15, IIR_FILTER_DO_CLEAN);
    __no_operation();                               // debug point

    iir_SinglePoleHighPass_FixedExtended_Init(&voltage_dcFilter_fixed_extended, 0.004, 30, IIR_FILTER_DO_CLEAN);
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

            /* sample without DC */
            sample_no_DC = sample_original - WAVE_DC_LEVEL;
            array_sample_no_dc[counter] = sample_no_DC;

            /* draw original wave without DC level */
            putChar(SEPARATOR);     print_float(sample_no_DC, 2);



            /************************************************
             * do the filtering - High Pass Filters
             ************************************************/
            __no_operation();                               // debug point
            iir_SinglePoleHighPass_Float(&voltage_dcFilter_float, sample_original);
            putChar(SEPARATOR);     print_float(voltage_dcFilter_float.y, 2);

            __no_operation();                               // debug point
            iir_SinglePoleHighPass_Fixed(&voltage_dcFilter_fixed_01, sample_original_int);
            putChar(SEPARATOR);     print_long((long)voltage_dcFilter_fixed_01.y);

            __no_operation();                               // debug point
            iir_SinglePoleHighPass_FixedExtended(&voltage_dcFilter_fixed_extended, sample_original_int);
            putChar(SEPARATOR);     print_long((long)voltage_dcFilter_fixed_extended.y);

            /* end of line */
            putChar('\r');        putChar('\n');

            counter++;
        }
        else        /* after the last point reset variables */
        {
            counter = 0;                            // reset counter
        }
    }
}
