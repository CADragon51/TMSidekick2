/* Audio Library for Teensy 3.X
 * Copyright (c) 2018, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Arduino.h>
#include "synth_mvwaveform.h"
#include "arm_math.h"
#include "utility/dspinst.h"

// uncomment for more accurate but more computationally expensive frequency modulation
//#define IMPROVE_EXPONENTIAL_ACCURACY

//--------------------------------------------------------------------------------

void AudioSynthMVWaveformModulated::update(void)
{
	audio_block_t *block, *moddata, *shapedata;
	int16_t magnitude15;
	uint32_t i, ph = 0, index, index2, scale, priorphase[3], inc;
	int32_t val = 0, val1, val2;
	float mod = 0;
	bool ismod = false;
	float smod = 0;
	bool isshape = false;

	moddata = receiveReadOnly(0);
	shapedata = receiveReadOnly(1);
	mod = 0;
	smod = 0;
	if (moddata)
	{
		mod = moddata->data[0] / 32684.0;
		release(moddata);
		ismod = true;
		//	Serial.println(mod);
	}
	if (shapedata)
	{
		smod = shapedata->data[0] / 32684.0;
		isshape = true;
	}

	// Pre-compute the phase angle for every output sample of this update

	// If the amplitude is zero, no output, but phase still increments properly
	if (magnitude[0]+ magnitude[1]+magnitude[2]== 0)
	{
		if (shapedata)
			release(shapedata);
		return;
	}
	block = allocate();
	if (!block)
	{
		if (shapedata)
			release(shapedata);
		Serial.println("no block");
		return;
	}
//	Serial.println(magnitude[2]);
	for (int p = 0; p < 3; p++)
	{
		ph = phase_accumulator[p];
		inc = phase_increment[p];
		if (ismod)
				inc *= (1 + mod);
		if (isshape && p == 1 && detune==0)
			inc *= (1 + smod);
		uint32_t tinc = inc;
		float sinc = from_phase_increment[p] / ftop;
		float finc = tinc / ftop;
		float otinc = tinc / ftop;

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{

			// Now generate the output samples using the pre-computed phase angles
			//			int32_t vx = 0;
			if (upw != 0 && portatime > 0 && tau > 0) //  portamento
			{
				if (tau == 1) // linear
				{
					from_phase_increment[p] += delta_phase_increment[p];
					inc = from_phase_increment[p];
					if (inc >= tinc && upw > 0)
						upw = 0;
					if (inc <= tinc && upw < 0)
						upw = 0;
//					if (p == 0)
//						Serial.println("from " + String(sinc) + " to " + String(finc) + " inc " + String(delta_phase_increment[p]/ftop) + " -> " + String(inc / ftop));
				}
				else if (tau == 2) //exponential
				{
					s0[p] = s0[p] * si[p];
					if (upw > 0)
					{
						inc = from_phase_increment[p] + s0[p] * ftop;
						if (inc > tinc)
							inc = tinc;
					}
					else
					{
						finc = otinc + (sinc - otinc) * s0[p];

						inc = finc * ftop;
						if (finc <= otinc)
							inc = tinc;
//						if (p == 0)
//							Serial.println("inc " + String(sinc) + " from " + String(finc) + " target " + String(otinc) + " s0 " + String(s0[p]) + " inc " + String(inc / ftop));
					}
					if (inc == tinc)
						upw = 0;
				}
			}
			if (p ==0)
				block->data[i] = 0;
			switch (tone_type[p])
			{
			case WAVEFORM_SINE:
				index = ph >> 24;
				val1 = AudioWaveformSine[index];
				val2 = AudioWaveformSine[index + 1];
				scale = (ph >> 8) & 0xFFFF;
				val2 *= scale;
				val1 *= 0x10000 - scale;
				val = multiply_32x32_rshift32(val1 + val2, magnitude[p]);
				break;

			case WAVEFORM_ARBITRARY:
				if (!arbdata)
				{
					release(block);
					return;
				}
				// len = 256
				//				ph = phasedata[p][i];
				index = ph >> 24;
				index2 = index + 1;
				if (index2 >= 256)
					index2 = 0;
				val1 = *(arbdata + index);
				val2 = *(arbdata + index2);
				scale = (ph >> 8) & 0xFFFF;
				val2 *= scale;
				val1 *= 0x10000 - scale;
				val = multiply_32x32_rshift32(val1 + val2, magnitude[p]);
				break;

			case WAVEFORM_PULSE:
				if (shapedata)
				{
					magnitude15 = signed_saturate_rshift(magnitude[p], 16, 1);
					uint32_t width = ((shapedata->data[i] + 0x8000) & 0xFFFF) << 16;
					if (ph < width)
					{
						val = magnitude15;
					}
					else
					{
						val = -magnitude15;
					}
					break;
				} // else fall through to orginary square without shape modulation

			case WAVEFORM_SQUARE:
				magnitude15 = signed_saturate_rshift(magnitude[p], 16, 1);
				if (ph & 0x80000000)
				{
					val = -magnitude15;
				}
				else
				{
					val = magnitude15;
				}
				break;

			case WAVEFORM_SAWTOOTH:
				val = signed_multiply_32x16t(magnitude[p], ph);
				break;

			case WAVEFORM_SAWTOOTH_REVERSE:
				val = signed_multiply_32x16t(0xFFFFFFFFu - magnitude[p], ph);
				break;

			case WAVEFORM_BANDLIMIT_PULSE:
				if (shapedata)
				{
					uint32_t width = ((shapedata->data[i] + 0x8000) & 0xFFFF) << 16;
					val = band_limit_waveform[p].generate_pulse(ph, width, i);
					val = (int16_t)((val * magnitude[p]) >> 16);
					break;
				} // else fall through to orginary square without shape modulation

			case WAVEFORM_BANDLIMIT_SQUARE:
				val = band_limit_waveform[p].generate_square(ph, i);
				val = (int16_t)((val * magnitude[p]) >> 16);
				break;

			case WAVEFORM_BANDLIMIT_SAWTOOTH:
			case WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE:
				val = band_limit_waveform[p].generate_sawtooth(ph, i);
				val = (int16_t)((val * magnitude[p]) >> 16);
				val = tone_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE ? (int16_t)-val : (int16_t) + val;
				break;

			case WAVEFORM_TRIANGLE_VARIABLE:
				if (shapedata)
				{
					uint32_t width = (shapedata->data[i] + 0x8000) & 0xFFFF;
					uint32_t rise = 0xFFFFFFFF / width;
					uint32_t fall = 0xFFFFFFFF / (0xFFFF - width);
					uint32_t halfwidth = width << 15;
					uint32_t n;
					if (ph < halfwidth)
					{
						n = (ph >> 16) * rise;
						val = ((n >> 16) * magnitude[p]) >> 16;
					}
					else if (ph < 0xFFFFFFFF - halfwidth)
					{
						n = 0x7FFFFFFF - (((ph - halfwidth) >> 16) * fall);
						val = (((int32_t)n >> 16) * magnitude[p]) >> 16;
					}
					else
					{
						n = ((ph + halfwidth) >> 16) * rise + 0x80000000;
						val = (((int32_t)n >> 16) * magnitude[p]) >> 16;
					}
					ph += inc;
					break;
				} // else fall through to orginary triangle without shape modulation

			case WAVEFORM_TRIANGLE:
			{
				uint32_t phtop = ph >> 30;
				uint32_t phs = ph >> 15;
				int32_t phn = 65535 - phs;
				int32_t phn3 = phs - 2 * 65535;

				if (phtop == 1 || phtop == 2)
				{
					val = (phn * magnitude[p]) >> 16;
				}
				else if (phtop == 0)
				{
					val = (phs * magnitude[p]) >> 16;
				}
				else
				{
					val = (phn3 * magnitude[p]) >> 16;
				}
			}
			break;
			case WAVEFORM_SAMPLE_HOLD:
				if (ph < priorphase[p])
				{ // does not work for phase modulation
					sample = random(magnitude[p]) - (magnitude[p] >> 1);
				}
				priorphase[p] = ph;
				val = sample;
				break;
			}
			block->data[i] += val;
//			Serial.println(val);
			ph += inc;
			//			ph += inc;
		}
		phase_accumulator[p] = ph;
	}
	if (shapedata)
		release(shapedata);
	//	for (i = 0; i < AUDIO_BLOCK_SAMPLES && phase_increment[0] > 0; i++)
	//		Serial.println(block->data[i]);

	transmit(block, 0);
	release(block);
}
