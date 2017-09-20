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
#define     WAVE_DC_LEVEL       512.0f              // DC offset
#define     WAVE_AMPLITUDE      100.0f              // Peak voltage
#define     WAVE_POINTS         64                  // points peer cycles

#define     SHIFT    15           // how many shifts in fixed version

float sample_original = 0;
int32_t sample_original_int = 0;

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
iirHighPassFloat_t voltage_dcFilter_float;
iirHighPassFixed_t voltage_dcFilter_fixed_01;
iirHighPassFixedExtended_t voltage_dcFilter_fixed_extended;

void setup() {
    Serial.begin(9600);

    /**************************************************
     *  Create and initialize wave struct parameters
     **************************************************/
    sineWaveGen_bySample_Init(&sine1th, 1.0f, 0, WAVE_AMPLITUDE, WAVE_DC_LEVEL, WAVE_POINTS, WAVEGEN_CLEAN);
    sineWaveGen_bySample_Init(&sine8th, 8.0f, 0, WAVE_AMPLITUDE/16, 0, WAVE_POINTS, WAVEGEN_CLEAN);

    /*************************************
     * Initialize structs - High pass
     *************************************/
    iir_SinglePoleHighPass_Float_Init(&voltage_dcFilter_float, 0.004f, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleHighPass_Fixed_Init(&voltage_dcFilter_fixed_01, 0.004f, SHIFT, IIR_FILTER_DO_CLEAN);
    iir_SinglePoleHighPass_FixedExtended_Init(&voltage_dcFilter_fixed_extended, 0.004, SHIFT, IIR_FILTER_DO_CLEAN);
}

void loop()
{
    /*************************************
     * Generate a test signal - sine wave
     *************************************/
    /* fundamental sine wave */
    sample_original = sineWaveGen_GetSample(&sine1th);
    /* 8th harmonic sine wave - sum with original wave */
    sample_original += sineWaveGen_GetSample(&sine8th);

    Serial.print(sample_original);          // print original wave

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
}
