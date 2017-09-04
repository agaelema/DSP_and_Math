/******************************************************************************
 *  Arduino (UNO) - High Pass Filter
 *  - Generate a sine wave with DC level and apply High Pass Filter
 *  - test of versions
 *      Float, Fixed, Fixed Extended (with more fractional bits)
 *
 *  Excellent Serial plotter
 *  - https://hackaday.io/project/5334-serialplot-realtime-plotting-software
 *
 *  Author: Haroldo Amaral - agaelema@gmail.com
 *  2017/09/04
 ******************************************************************************/
#include "DSP_and_Math.h"

/******************************************************************************
 * Serial plotter separator
 ******************************************************************************/
#define   SEPARATOR     (' ')       // used to separate plotted values - Arduino Serial plotter
//#define   SEPARATOR     (',')       // used to separate plotted values - 

#define     PI                  3.1415926535897932384676

#define     WAVE_DC_LEVEL       512.0f
#define     WAVE_AMPLITUDE      100.0f
#define     WAVE_POINTS         64

#define     SHIFT    15           // how many shifts in fixed version

float a = 0;
float sample_original = 0;
int32_t sample_original_int = 0;

volatile float sample_no_DC = 0;

float increment = (2*PI)/WAVE_POINTS;
uint16_t counter = 0;

/*
 *  Initialize structures to save filter parameters
 */
iirHighPassFloat_t voltage_dcFilter_float;
iirHighPassFixed_t voltage_dcFilter_fixed_01;
iirHighPassFixedExtended_t voltage_dcFilter_fixed_extended;

void setup() {
  Serial.begin(9600);
  
  /*************************************
     * Initialize structs - High pass
     *************************************/
    iir_SinglePoleHighPass_Float_Init(&voltage_dcFilter_float, 0.004f, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleHighPass_Fixed_Init(&voltage_dcFilter_fixed_01, 0.004f, SHIFT, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleHighPass_FixedExtended_Init(&voltage_dcFilter_fixed_extended, 0.004, SHIFT, IIR_FILTER_DO_CLEAN);
}

void loop()
{
  if(counter < WAVE_POINTS)
        {
            /*************************************
             * Generate a test signal - sine wave
             *************************************/
            sample_original = WAVE_DC_LEVEL + sinf(a) * WAVE_AMPLITUDE;
//        sample_original = WAVE_DC_LEVEL + (sinf(a) * WAVE_AMPLITUDE) + (sinf(8*a) * (WAVE_AMPLITUDE/8));
            Serial.print(sample_original);          // print original wave
            a += increment;
            
            sample_original_int = (int32_t)sample_original;     // cast value to integer

            /* sample without DC */
            sample_no_DC = sample_original - WAVE_DC_LEVEL;

            /* draw original wave without DC level */
            Serial.print(SEPARATOR);    Serial.print(sample_no_DC);



            /************************************************
             * do the filtering - High Pass Filters
             ************************************************/
            iir_SinglePoleHighPass_Float(&voltage_dcFilter_float, sample_original);
            Serial.print(SEPARATOR);    Serial.print(voltage_dcFilter_float.y);

            iir_SinglePoleHighPass_Fixed(&voltage_dcFilter_fixed_01, sample_original_int);
            Serial.print(SEPARATOR);    Serial.print(voltage_dcFilter_fixed_01.y);

            iir_SinglePoleHighPass_FixedExtended(&voltage_dcFilter_fixed_extended, sample_original_int);
            Serial.print(SEPARATOR);    Serial.print(voltage_dcFilter_fixed_extended.y);

            /* end of line */
            Serial.print('\r');   Serial.println();

            counter++;
        }
        else        /* after the last point reset variables */
        {
            counter = 0;                            // reset counter
        }
}
