/******************************************************************************
 *  Arduino (UNO) - Low Pass Filter
 *  - Generate a sine wave with DC level and apply High Pass Filter
 *  - test of versions
 *      Float, Fixed, Fixed Extended (with more fractional bits), Fixed Fast
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
//#define   SEPARATOR     (',')       // used to separate plotted values - Other Serial plotter

#define     PI                  3.1415926535897932384676

/******************************************************************************
 * Define parameters of simulated wave
 ******************************************************************************/
#define     WAVE_DC_LEVEL       512.0f              // DC offset
#define     WAVE_AMPLITUDE      100.0f              // Peak voltage
#define     WAVE_POINTS         64                  // points peer cycles

float a = 0;
float sample_original = 0;
int32_t sample_original_int = 0;

volatile float sample_no_DC = 0;

float increment = (2*PI)/WAVE_POINTS;
uint16_t counter = 0;

/*
 *  Initialize structures to save filter parameters
 */
iirLowPassFloat_t voltage_LowPass_float_01;
iirLowPassFixed_t voltage_LowPass_fixed_01;
iirLowPassFixedExtended_t voltage_LowPass_fixedExtended_01;
iirLowPassFixedFast_t voltage_LowPass_fixedFast_01;
iirLowPassFixedFast_t voltage_LowPass_fixedFast_02;

void setup() {
  Serial.begin(9600);
  
  /*************************************
     * Initialize structs - High pass
     *************************************/
    iir_SinglePoleLowPass_Float_Init(&voltage_LowPass_float_01, 0.0051f, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleLowPass_Fixed_Init(&voltage_LowPass_fixed_01, 0.0051f, 9, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleLowPass_FixedExtended_Init(&voltage_LowPass_fixedExtended_01, 0.0051f, 20, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleLowPass_Fixed_Fast_Init(&voltage_LowPass_fixedFast_01, 7, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleLowPass_Fixed_Fast_Init(&voltage_LowPass_fixedFast_02, 7, IIR_FILTER_DO_CLEAN);
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



            /************************************************
             * do the filtering - Low Pass Filters
             ************************************************/
            iir_SinglePoleLowPass_Float(&voltage_LowPass_float_01, (float)sample_original);
            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_float_01.y);

            iir_SinglePoleLowPass_Fixed(&voltage_LowPass_fixed_01, sample_original_int);
            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixed_01.y);

            iir_SinglePoleLowPass_FixedExtended(&voltage_LowPass_fixedExtended_01, sample_original_int);
            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixedExtended_01.y);

            /* first order */
            iir_SinglePoleLowPass_Fixed_Fast(&voltage_LowPass_fixedFast_01, sample_original_int);
            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixedFast_01.y);

             /* Second order - cascading */
            iir_SinglePoleLowPass_Fixed_Fast(&voltage_LowPass_fixedFast_02, voltage_LowPass_fixedFast_01.y);
            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixedFast_02.y);

            /* end of line */
            Serial.print('\r');   Serial.println();

            counter++;
        }
        else        /* after the last point reset variables */
        {
            counter = 0;                            // reset counter
        }
}
