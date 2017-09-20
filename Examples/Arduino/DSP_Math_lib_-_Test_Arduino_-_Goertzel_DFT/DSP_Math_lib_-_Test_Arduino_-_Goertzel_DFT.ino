/******************************************************************************
 *  Arduino (UNO) - Goertzel DFT
 *  - Generate a sine wave and use Goertzel algorithm to calculate harmonics
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
float array_sample_original[WAVE_POINTS];

uint16_t counter = 0;

/******************************************************************************
 *  Initialize structures to waveform
 ******************************************************************************/
sine_wave_parameters sine1th = {0};
sine_wave_parameters sine8th = {0};

/******************************************************************************
 *  Initialize structures to save goertzel parameters
 ******************************************************************************/
goertzel_array_float_t goertzel_01_01th;
goertzel_array_float_t goertzel_01_08th;


void setup()
{
    Serial.begin(9600);

    /*
     *  Create and initialize wave struct parameters
     */
    sineWaveGen_bySample_Init(&sine1th, 1.0f, 0, WAVE_AMPLITUDE, WAVE_DC_LEVEL, WAVE_POINTS, WAVEGEN_CLEAN);
    sineWaveGen_bySample_Init(&sine8th, 8.0f, 0, WAVE_AMPLITUDE/16, 0, WAVE_POINTS, WAVEGEN_CLEAN);

    /*************************************
     * Initialize Goertzel structures
     *************************************/
    goertzelArrayInit_Float(&goertzel_01_01th, 1, 64);
    goertzelArrayInit_Float(&goertzel_01_08th, 8, 64);
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
        /* 8th harmonic sine wave - sum with original wave */
        sample_original += sineWaveGen_GetSample(&sine8th);


        array_sample_original[counter] = (float)sample_original;

        counter++;
    }
    else        /* after the last point reset variables */
    {
        counter = 0;                            // reset counter

        /* Goertzel DFT of an array */
        goertzelArrayFloat_Float(&goertzel_01_01th, array_sample_original);
        goertzelArrayFloat_Float(&goertzel_01_08th, array_sample_original);

        Serial.print(" Goertzel Array -");
        Serial.print(" 1th: ");              Serial.print(goertzel_01_01th.result, 2);
        Serial.print(" 8th: ");              Serial.print(goertzel_01_08th.result, 2);

        Serial.print("\r\n");
    }
}
