/* AudioFilterEqualizer_I16.cpp
 *
 * Bob Larkin,  W7PUA  14 May 2020
 *
 * See AudioFilterEqualizer_I16.h for much more explanation on usage.
 * 
 * Copyright (c) 2020 Bob Larkin
 * Any snippets of code from PJRC and used here brings with it
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

#include "AudioFilterEqualizer_I16.h"

void AudioFilterEqualizer::update(void)  {
    audio_block_t *block, *block_new;

#if TEST_TIME_EQ
  if (iitt++ >1000000) iitt = -10;
  uint32_t t1, t2;
  t1 = tElapse;
#endif
    block = receiveReadOnly();
    if (!block) return;

    // Check for coefficients
    if (cf16 == NULL) {
      release(block);
      return;
    }

    // get a block for the FIR output
    block_new = allocate();
    if (block_new) {
        //apply the FIR
        arm_fir_q15(&fir_inst, block->data, block_new->data, AUDIO_BLOCK_SAMPLES);
        transmit(block_new); // send the FIR output
        release(block_new);
    }
    release(block);
    
#if TEST_TIME_EQ
  t2 = tElapse;
  if(iitt++ < 0) {Serial.print("At AudioEqualizer end, microseconds = ");  Serial.println (t2 - t1); }
  t1 = tElapse;
#endif
}

/* equalizerNew() calculates the Equalizer FIR filter coefficients. Works from:
 * uint16_t equalizerNew(uint16_t _nBands, float32_t *feq, float32_t *adb,
                      uint16_t _nFIR, int16_t *_cf, float32_t kdb, float32_t scale16)
 *   nBands   Number of equalizer bands
 *   feq      Pointer to array feq[] of nBands breakpoint frequencies, fractions of sample rate, Hz
 *   adb      Pointer to array aeq[] of nBands levels, in dB, for the feq[] defined frequency bands
 *   nFIR     The number of FIR coefficients (taps) used in the equalzer 
 *   cf       Pointer to an array of int16 to hold FIR coefficients
 *   kdb      A parameter that trades off sidelobe levels for sharpness of band transition.
 *            kdb=30 sharp cutoff, higher sidelobes
 *            kdb=60 slow cutoff, low sidelobes
 *   scale16  A float number that sets the maximum int value for coefficients. Max 32768.0f
 *
 * The arrays, feq[], aeq[] and cf[] are supplied by the calling .INO
 *
 * Returns: 0 if successful, or an error code if not.
 * Errors:  1 = ERR_EQ_BANDS =     Too many bands, 50 max
 *          2 = ERR_EQ_SIDELOBES = Sidelobe level out of range, must be > 0
 *          3 = ERR_EQ_NFIR =      nFIR out of range
 *
 * Note - This function runs at setup time, and there is no need to fret about
 * processor speed.  Likewise, local arrays are created on the stack and memory space is
 * available for other use when this function closes.
 */
uint16_t AudioFilterEqualizer::equalizerNew(uint16_t _nBands,  float32_t* feq,  float32_t* adb,
                      uint16_t _nFIR,  int16_t* pcf16,  float32_t kdb,  float32_t scale16)  {
    uint16_t i, j;
    uint16_t nHalfFIR;
    float32_t beta, kbes;
    float32_t q, xj2, scaleXj2, WindowWt;
    float32_t cf[250];
    float32_t fNorm[50];   // Normalized to the sampling frequency
    float32_t aVolts[50];  // Convert from dB to "quasi-Volts"

    cf16 = pcf16;   // Set the private copies
    nFIR = _nFIR;
    nBands = _nBands;

    // Check range of nFIR. q15 FIR requires even number of coefficients,
    // but for historic reasons, we design odd number FIR.  So add a 
    // variable nFIRused that is even, and one more than the design value.
    if (nFIR<4 || nFIR>EQUALIZER_MAX_COEFFS)
        return ERR_EQ_NFIR;

    if (2*(nFIR/2) == nFIR)  {       // nFIR even
		nFIRdesign = nFIR - 1;
		nFIRused = nFIR;
    }
    else {                            // nFIR odd
        nFIRdesign = nFIR - 2;        // Avoid this
        nFIRused = nFIR - 1;
    }
    nHalfFIR = (nFIRdesign - 1)/2;  // If nFIRdesign=199, nHalfFIR=99

    for (int kk = 0; kk<nFIRdesign; kk++)  // To be sure, zero the coefficients
      cf[kk] = 0.0f;

    // Convert dB to Voltage ratios, frequencies to fractions of sampling freq
    if(nBands <2 || nBands>50)  return ERR_EQ_BANDS;
    for (i=0; i<nBands; i++)  {
       aVolts[i]=adb[i];
       fNorm[i]=feq[i]/sample_rate_Hz;
    }

    /* Find FIR coefficients, the Fourier transform of the frequency
     * response. This is done by dividing the response into a sequence
     * of nBands rectangular frequency blocks, each of a different level.
     * We can precalculate the sinc Fourier transform for each rectangular band.
     * The linearity of the Fourier transform allows us to sum the transforms
     * of the individual blocks to get pre-windowed coefficients.
     *
     * Numbering example for nFIRdesign==199:
     * Subscript 0 to 98 is 99 taps;  100 to 198 is 99 taps; 99+1+99=199 taps
     * The center coef ( for nFIRdesign=199 taps, nHalfFIR=99 ) is a
     * special case that comes from sin(0)/0 and treated first:
     */
    cf[nHalfFIR] = 2.0f*(aVolts[0]*fNorm[0]);  // Coefficient "99"
    for(i=1; i<nBands; i++) {
        cf[nHalfFIR] += 2.0f*aVolts[i]*(fNorm[i]-fNorm[i-1]);
    }
    for (j=1; j<=nHalfFIR; j++) {          // Coefficients "100 to 198"
        q = MF_PI*(float32_t)j;
        // First, deal with the zero frequency end band that is "low-pass."
        cf[j+nHalfFIR] = aVolts[0]*sinf(fNorm[0]*2.0f*q)/q;
        //  and then the rest of the bands that have low and high frequencies
        for(i=1; i<nBands; i++)
            cf[j+nHalfFIR] += aVolts[i]*( (sinf(fNorm[i]*2.0f*q)/q) - (sinf(fNorm[i-1]*2.0f*q)/q) );
    }

    /* At this point, the cf[] coefficients are simply truncated sin(x)/x, creating
     * very high sidelobe responses. To reduce the sidelobes, a windowing function is applied.
     * This has the side affect of increasing the rate of cutoff for sharp frequency changes.
     * The only windowing function available here is that of James Kaiser.  This has a number
     * of desirable features.  These include being able to tradeoff sidelobe level
     * for rate of cutoff cutoff between frequency bands.
     * We specify it in terms of kdb, the highest sidelobe, in dB, next to a sharp cutoff. For
     * calculating the windowing vector, we need a Kaiser parameter beta, found as follows:
     */
    if (kdb<0) return ERR_EQ_SIDELOBES;
    if (kdb>50)
        beta = 0.1102f*(kdb-8.7f);
    else if  (kdb>20.96f && kdb<=50.0f)
        beta = 0.58417f*powf((kdb-20.96f), 0.4f) + 0.07886f*(kdb-20.96f);
    else
        beta=0.0f;
    // i0f is the floating point in & out zero'th order modified Bessel function
    kbes = 1.0f / i0f(beta);      // An additional derived parameter used in loop

    // Apply the Kaiser window, j = 0 is center coeff window value
    scaleXj2 = 2.0f/(float32_t)nFIRdesign;
    scaleXj2 *= scaleXj2;
    for (j=0; j<=nHalfFIR; j++) {  // For 199 Taps, this is 0 to 99
         xj2 = (int16_t)(0.5f+(float32_t)j);
         xj2 = scaleXj2*xj2*xj2;
         WindowWt=kbes*(i0f(beta*sqrtf(1.0-xj2)));
         cf[nHalfFIR + j] *= WindowWt;       // Apply the Kaiser window to upper half
         cf[nHalfFIR - j] = cf[nHalfFIR +j]; // and create the lower half
    }

    // Find the biggest to decide the scaling factor for the FIR filter.
    // Then we will scale the coefficients according to scale16
    float32_t cfmax = 0.0f;
    for (j=0; j<=nHalfFIR; j++)      // 0 to 99  for nFIRdesign=199
        if (cfmax < fabsf(cf[j])) cfmax=fabsf(cf[j]);
    // scale16 is a float number, such as 16384.0, that sets the maximum +/- 
    // value for coefficients.  This is a complex subject that needs more discussion
    // than we can put here.  The following scales the coefficients and converts to 16 bit:
    for (j=0; j<nFIRdesign; j++)
        cf16[j] = (int)(scale16*cf[j]/cfmax);
    // nFIRused id always even. nFIRdesign is always odd.  So add a zero
    cf16[nFIRdesign] = 0;
    // The following puts the numbers into the fir_inst structure
    arm_fir_init_q15(&fir_inst, nFIRused, (int16_t *)cf16, &StateQ15[0], (uint32_t)block_size);
    return 0;
}

/* Calculate response in dB.  Leave nFreq-point-result in array rdb[] that is supplied
 * by the calling .INO  See Parks and Burris, "Digital Filter Design," p27 (Type 1).
 */
void AudioFilterEqualizer::getResponse(uint16_t nFreq, float32_t *rdb, float32_t scale16)  {
    uint16_t i, j;
    float32_t bt;
    float32_t piOnNfreq;
    uint16_t nHalfFIR;
    float32_t cf[nFIR];

    nHalfFIR = (nFIRdesign - 1)/2;  // If nFIRdesign=199, nHalfFIR=99

    for (i=0; i<nFIRdesign; i++)
       cf[i] = ((float32_t)cf16[i]) / scale16;
    piOnNfreq = MF_PI / (float32_t)nFreq;
    for (i=0; i<nFreq; i++) {
        bt = cf[nHalfFIR];          // Center coefficient
        for (j=0; j<nHalfFIR; j++)  // Add in the others twice, as they are symmetric
            bt += 2.0f*cf[j]*cosf(piOnNfreq*(float32_t)((nHalfFIR-j)*i));
        rdb[i] = 20.0f*log10f(fabsf(bt));     // Convert to dB
    }
}