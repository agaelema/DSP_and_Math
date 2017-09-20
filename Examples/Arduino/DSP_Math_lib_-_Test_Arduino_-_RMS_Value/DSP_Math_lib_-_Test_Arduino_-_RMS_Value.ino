/******************************************************************************
 *  Arduino (UNO) - RMS value
 *  - Generate a sine wave with (or withoud) DC level and calculate the RMS value
 *  - test of versions
 *      Array based, sample by sample
 *
 *  Author: Haroldo Amaral - agaelema@gmail.com
 *  2017/09/19
 ******************************************************************************/
#include "DSP_and_Math.h"

/******************************************************************************
 * Serial plotter separator
 ******************************************************************************/
#define   SEPARATOR     (' ')       // used to separate plotted values - Arduino Serial plotter
//#define   SEPARATOR     (',')       // used to separate plotted values - Other Serial plotter

#define     PI                  3.141592653589793f

/******************************************************************************
 * Define parameters of simulated wave
 ******************************************************************************/
#define     WAVE_DC_LEVEL       0.0f              // DC offset
#define     WAVE_AMPLITUDE      100.0f              // Peak voltage
#define     WAVE_POINTS         64                  // points peer cycles

float sample_original = 0;
int16_t sample_original_int16 = 0;
float array_sample_original[WAVE_POINTS];

volatile float sample_no_DC = 0;

uint16_t counter = 0;

/******************************************************************************
 *  Initialize structures to waveform
 ******************************************************************************/
sine_wave_parameters sine1th = {0};
sine_wave_parameters sine8th = {0};

/******************************************************************************
 *  Initialize structures to save filter parameters
 ******************************************************************************/
rms_float_t rms_teste_float = {0};
rms_int16_t rms_teste_int16 = {0};

void setup()
{
    Serial.begin(9600);

    /*
     *  Create and initialize wave struct parameters
     */
    sineWaveGen_bySample_Init(&sine1th, 1.0f, 0, WAVE_AMPLITUDE, WAVE_DC_LEVEL, WAVE_POINTS, WAVEGEN_CLEAN);
    sineWaveGen_bySample_Init(&sine8th, 8.0f, 0, WAVE_AMPLITUDE/16, 0, WAVE_POINTS, WAVEGEN_CLEAN);
}

void loop()
{
    if(counter < WAVE_POINTS)
    {
        /*************************************
         * Generate a test signal - sine wave
         *************************************/
        /* fundamental sine wave */
        sample_original = sineWaveGen_GetSample(&sine1th);
//        /* 8th harmonic sine wave - sum with original wave */
//        sample_original += sineWaveGen_GetSample(&sine8th);


        array_sample_original[counter] = (float)sample_original;
        sample_original_int16 = (int16_t)sample_original;

        /* add new sample to RMS struct */
        rmsValueAddSample_Float(&rms_teste_float, sample_original);

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
        rmsValueCalcRmsStdMath_Float(&rms_teste_float);
        rms_test_float = rms_teste_float.rmsValue;
        Serial.print("RMS sample by sample: ");    Serial.print(rms_test_float, 4);    Serial.print("    ");

        /* calculate rms - version array input */
        rms_test_float = rmsValueArray_Float_StdMath(array_sample_original, WAVE_POINTS, 0);
        Serial.print("RMS of Array: ");    Serial.println(rms_test_float, 4);
    }
}
