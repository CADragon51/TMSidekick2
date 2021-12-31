/*
 * AudioFilterEqualizer_I16
 *
 * Created: Bob Larkin   W7PUA   14 May 2020
 *
 * This is a direct translation of the receiver audio equalizer written
 * by this author for the open-source DSP-10 radio in 1999.  See
 * http://www.janbob.com/electron/dsp10/dsp10.htm and
 * http://www.janbob.com/electron/dsp10/uhf3_35a.zip
 * This version processes blocks of 16-bit integer audio (as opposed to
 * the Chip Audette style F32 floating point baudio.)
 *
 * Credit and thanks to PJRC, Paul Stoffregen and Teensy products for the audio
 * system and library that this is built upon as well as the float32
 * work of Chip Audette embodied in the OpenAudio_ArduinoLibrary. Many thanks
 * for the library structures and wonderful Teensy products.
 *
 * This equalizer is specified by an array of 'nBands' frequency bands
 * each of of arbitrary frequency span.  The first band always starts at
 * 0.0 Hz, and that value is not entered.  Each band is specified by the upper
 * frequency limit to the band.
 * The last band always ends at half of the sample frequency, which for 44117 Hz
 * sample frequency would be 22058.5.  Each band is specified by its upper
 * frequency in an .INO supplied array feq[].  The dB level of that band is
 * specified by a value, in dB, arranged in an .INO supplied array
 * aeq[].  Thus a trivial bass/treble control might look like:
 *   nBands = 3;
 *   feq[] = {300.0, 1500.0, 22058.5};
 *   float32_t bass = -2.5;  // in dB, relative to anything
 *   float32_t treble = 6.0;
 *   aeq[] = {bass, 0.0, treble};
 *
 * It may be obvious that this equalizer is a more general case of the common
 * functions such as low-pass, band-pass, notch, etc.   For instance, a pair
 * of band pass filters would look like:
 *   nBands = 5;
 *   feq[] = {500.0, 700.0, 2000.0, 2200.0, 22058.5};
 *   aeq[] = {-100.0, 0.0, -100.0, 2.0, -100.0};
 * where we added 2 dB of gain to the 2200 to 2400 Hz filter, relative to the 500
 * to 700 Hz band.
 *
 * An octave band equalizer is made by starting at some low frequency, say 40 Hz for the
 * first band.  The lowest frequency band will be from 0.0 Hz up to that first frequency.
 * Next multiply the first frequency by 2, creating in our example, a band from 40.0
 * to 80 Hz.  This is continued until the last frequency is about 22058 Hz.
 * This works out to require 10 bands, as follows:
 *   nBands = 10;
 *   feq[] = {    40.0, 80.0, 160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0, 10240.0, 22058.5};
 *   aeq[] = {  5.0,  4.0,  2.0,   -3.0, -4.0,  -1.0,    3.0, 6.0,    3.0,     0.5      };
 *
 * For a "half octave" equalizer, multiply each upper band limit by the square root of 2 = 1.414
 * to get the next band limit.  For that case, feq[] would start with a sequence
 * like 40, 56.56, 80.00, 113.1, 160.0, ... for a total of about 20 bands.
 *
 * How well all of this is achieved depends on the number of FIR coefficients
 * being used.  In the Teensy 3.6 / 4.0 the resourses allow a hefty number,
 * say 201, of coefficients to be used without stealing all the processor time
 * (see Timing, below).  The coefficient and FIR memory is sized for a maximum of
 * 250 coefficients, but can be recompiled for bigger with the define FIR_MAX_COEFFS.
 * To simplify calculations, the number of FIR coefficients should be odd.  If not
 * odd, the number will be reduced by one, quietly.
 *
 * If you try to make the bands too narrow for the number of FIR coeffficients,
 * the approximation to the desired curve becomes poor.  This can all be evaluated
 * by the function getResponse(nPoints, pResponse) which fills an .INO-supplied array
 * pResponse[nPoints] with the frequency response of the equalizer in dB.  The nPoints
 * are spread evenly between 0.0 and half of the sample frequency.
 *
 * Initialization is a 2-step process.  This makes it practical to change equalizer
 * levels on-the-fly.  The constructor starts up with a 4-tap FIR setup for direct
 * pass through.  Then the setup() in the .INO can specify the equalizer.
 * The newEqualizer() function has several parameters, the number of equalizer bands,
 * the frequencies of the bands, and the sidelobe level. All of these can be changed
 * dynamically.  This function can be changed dynamically, but it may be desireable to
 * mute the audio during the change to prevent clicks.
 * 
 * This 16-bit integer version adjusts the maximum coefficient size to scale16 in the calls
 * to both equalizerNew() and getResponse().  Broadband equalizers can work with full-scale
 * 32767.0f sorts of levels, where narrow band filtering may need smaller values to
 * prevent overload. Experiment and check carefully.  Use lower values if there are doubts.
 * 
 * For a pass-through function, something like this (which can be intermixed with fancy equalizers):
 * float32_t fBand[] = {10000.0f, 22058.5f}; 
 * float32_t dbBand[] = {0.0f, 0.0f};
 * equalize1.equalizerNew(2, &fBand[0], &dbBand[0], 4, &equalizeCoeffs[0], 30.0f, 32767.0f);
 *
 * Measured Q15 timing of update() for a 128 sample block on a T3.6:
 *     Fixed time 11.6 microseconds
 *     Per FIR Coefficient time 5.6 microseconds
 *     Total for 200 FIR Coefficients = 1140 microseconds (39.3% of fs=44117 Hz available time)
 *
 * Copyright (c) 2020 Bob Larkin
 * Any snippets of code from PJRC used here brings with it
 * the associated license.
 *
 * In addition, work here is covered by MIT LIcense:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _filter_equalizer_h
#define _filter_equalizer_h

#include "Arduino.h"
#include "arm_math.h"
#include "Audio.h"
#include "AudioStream.h"

#ifndef MF_PI
#define MF_PI 3.1415926f
#endif

// Temporary timing test
#define TEST_TIME_EQ 0

#define EQUALIZER_MAX_COEFFS 250

#define ERR_EQ_BANDS 1
#define ERR_EQ_SIDELOBES 2
#define ERR_EQ_NFIR 3

class AudioFilterEqualizer : public AudioStream
{
  public:
    AudioFilterEqualizer(void): AudioStream(1,inputQueueArray) {
      // Initialize FIR instance (ARM DSP Math Library) with default simple passthrough FIR
      if (arm_fir_init_q15(&fir_inst, nFIRused, (q15_t *)cf16, &StateQ15[0], AUDIO_BLOCK_SAMPLES)
           != ARM_MATH_SUCCESS) {
        cf16 = NULL;
      }
    }

    uint16_t equalizerNew(uint16_t _nBands, float32_t *feq, float32_t *adb,
                  uint16_t _nFIR, int16_t *_cf, float32_t kdb, float32_t scale16);

    void getResponse(uint16_t nFreq, float32_t *rdb, float32_t scale16);

    void update(void);

  private:
    audio_block_t *inputQueueArray[1];
    uint16_t block_size = AUDIO_BLOCK_SAMPLES;
    int16_t firStart[4] = {0, 32767, 0, 0};  // Initialize to passthrough
    int16_t* cf16 = firStart;        // pointer to current coefficients
    uint16_t nFIR  = 4;              // Number of coefficients
    uint16_t nFIRdesign = 3;         // used in designing filter
    uint16_t nFIRused = 4;           // Adjusted nFIR, nFIR-1 for nFIR odd.
    uint16_t nBands = 2;
    float32_t sample_rate_Hz = AUDIO_SAMPLE_RATE;

    //  *Temporary* - TEST_TIME allows measuring time in microseconds for each part of the update()
#if TEST_TIME_EQ
    elapsedMicros tElapse;
    int32_t iitt = 999000;     // count up to a million during startup
#endif

    // ARM DSP Math library filter instance
    arm_fir_instance_q15 fir_inst;
    int16_t StateQ15[AUDIO_BLOCK_SAMPLES + EQUALIZER_MAX_COEFFS];  // max, max
    
    /* float i0f(float x)  Returns the modified Bessel function Io(x).
     * Algorithm is based on Abromowitz and Stegun, Handbook of Mathematical
     * Functions, and Press, et. al., Numerical Recepies in C.
     * All in 32-bit floating point
     */
    float i0f(float x) {
      float af, bf, cf;
      if( (af=fabsf(x)) < 3.75f ) {
        cf = x/3.75f;
        cf = cf*cf;
        bf=1.0f+cf*(3.515623f+cf*(3.089943f+cf*(1.20675f+cf*(0.265973f+
             cf*(0.0360768f+cf*0.0045813f)))));
      }
      else {
        cf = 3.75f/af;
        bf=(expf(af)/sqrtf(af))*(0.3989423f+cf*(0.0132859f+cf*(0.0022532f+
             cf*(-0.0015756f+cf*(0.0091628f+cf*(-0.0205771f+cf*(0.0263554f+
             cf*(-0.0164763f+cf*0.0039238f))))))));
      }
      return bf;
    }
};
#endif