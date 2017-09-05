/******************************************************************************
 *  Arduino (UNO) - RMS value
 *  - Generate a sine wave with (or withoud) DC level and calculate the RMS value
 *  - test of versions
 *      Array based, sample by sample
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
#define     WAVE_DC_LEVEL       0.0f              // DC offset
#define     WAVE_AMPLITUDE      100.0f              // Peak voltage
#define     WAVE_POINTS         64                  // points peer cycles

float a = 0;
float sample_original = 0;
int32_t sample_original_int = 0;
float array_sample_original[WAVE_POINTS];

volatile float sample_no_DC = 0;

float increment = (2*PI)/WAVE_POINTS;
uint16_t counter = 0;

/*
 *  Initialize structures to save filter parameters
 */
rms_float_t rms_teste = {0};

void setup()
{
  Serial.begin(9600);
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

            array_sample_original[counter] = sample_original;
//            Serial.print(sample_original);          // print original wave
            a += increment;
            
            sample_original_int = (int32_t)sample_original;     // cast value to integer

            /* sample without DC */
            sample_no_DC = sample_original - WAVE_DC_LEVEL;



//            /************************************************
//             * do the filtering - Low Pass Filters
//             ************************************************/
//            iir_SinglePoleLowPass_Float(&voltage_LowPass_float_01, (float)sample_original);
//            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_float_01.y);
//
//            iir_SinglePoleLowPass_Fixed(&voltage_LowPass_fixed_01, sample_original_int);
//            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixed_01.y);
//
//            iir_SinglePoleLowPass_FixedExtended(&voltage_LowPass_fixedExtended_01, sample_original_int);
//            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixedExtended_01.y);
//
//            /* first order */
//            iir_SinglePoleLowPass_Fixed_Fast(&voltage_LowPass_fixedFast_01, sample_original_int);
//            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixedFast_01.y);
//
//             /* Second order - cascading */
//            iir_SinglePoleLowPass_Fixed_Fast(&voltage_LowPass_fixedFast_02, voltage_LowPass_fixedFast_01.y);
//            Serial.print(SEPARATOR);    Serial.print(voltage_LowPass_fixedFast_02.y);

//            /* end of line */
//            Serial.print('\r');   Serial.println();

            /* add new sample to RMS struct */
            rmsValueAddSample(&rms_teste, &sample_original);

            counter++;
        }
        else        /* after the last point reset variables */
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
            Serial.print("RMS sample by sample: ");    Serial.print(rms_test_float, 4);    Serial.print("    ");

            /* calculate rms - version array input */
            rms_test_float = rmsValueArray_StdMath(array_sample_original, WAVE_POINTS, 0);
            Serial.print("RMS of Array: ");    Serial.println(rms_test_float, 4);
        }
}
