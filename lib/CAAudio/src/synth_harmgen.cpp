/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
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

#include "synth_harmgen.h"
#include "utility/dspinst.h"

// data_waveforms.c
extern "C"
{
	extern const int16_t AudioWaveformSine[257];
}
void Envelope::noteOn(int fx, float dur, float ptf)
{
	actseg = 0;
	count = 0;
	noteon = 1;
	prev = levels[0];
	tf = ptf;
#if 0
		Serial.print(noteon);
		Serial.print(" on ");
		Serial.print(prev);
		Serial.print(" act ");
		Serial.println(actseg);
#endif
}

int16_t AudioSynthWaveformHarmGen::dowavetable()
{
	double remainder;
	//	int16_t *wbuffer=buffer+frame * framesize;
	float fa = 44100.0 / _frequency;
	wtphase += framesize / fa;
	//	wtphase++;
	//	frame=0;
	if (wtphase >= framesize)
	{
		wtphase -= framesize;
		if (wtphase < 0)
		{
			wtphase = 0;
		}
	}
	remainder = wtphase - floor(wtphase);
	int16_t output = ((1 - remainder) * buffer[1 + (long)wtphase + frame * framesize] + remainder * buffer[2 + (long)wtphase + frame * framesize]);
#if 0
	Serial.println(frame);
//	Serial.print(",");
//	Serial.println(output);
#endif
	return (output);
}

void AudioSynthWaveformHarmGen::update(void)
{
	audio_block_t *block, *modinput;
	int16_t magnitude15;
	uint32_t i, ph = 0, index, index2, scale;
	int32_t val = 0, val1, val2, inc;
	float mod = 0;
	bool ismod = false;
	modinput = receiveReadOnly();
	block = allocate();
	tone_type = WAVEFORM_SAWTOOTH;
	int intpf = floor(from - 1);
	if (intpf < 0)
		intpf = 0;
	float numh = to - from;

	Serial.print("update ");
	Serial.println(numh);
	if (note_on != state)
	{
		state = note_on;
		//	Serial.println(note_on);
	}
	mod = 0;
	if (modinput)
	{
		mod = modinput->data[0] / 32684.0;
		release(modinput);
		ismod = true;
		//	Serial.println(mod);
	}
	int intpl = ceil(from + numh * (1 - mod * hmod) - 1);
	//	int intpl = ceil(to-1);
	if (intpl < intpf)
		intpl = intpf + 1;
	int mp = min(nfreq, intpl);
	//float lp = 1;
	//float rp = 1;
	//float pf = 1.0;
	float pulse = 1;
	if (intpf < 0 || mp <= intpf)
	{
#if 0
		Serial.print(from);
		Serial.print(" ");
		Serial.print(intpf);
		Serial.print(" ");
		Serial.print(to);
		Serial.print(" ");
		Serial.print(mp);
		Serial.print(" ");
		Serial.println(mod);
#endif
		mp = nfreq;
		intpf = 0;
	}
	if (shape == WAVETABLE)
	{
		pulse = (mod + 1) / 2.0;
		int fc = frames;
		frame = (pulse * fc);
		//		Serial.println(frame);

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			//			if (frame >=frames)
			//				frame = 0;
			if (_frequency != 0)
			{
				val = dowavetable();
				float e = envelope[0]->update(solos > 0, loopend, loopmark, 0);
				if (e < 0)
					e = 0;
				e = e * ivol;

				block->data[i] = val * ivol;
				//				Serial.println(val*ivol);
			}
			else
				block->data[i] = 0;
		}
		phase_accumulator[0] = ph - phaselist[0];
		transmit(block);
		release(block);
		return;
	}
	for (int j = intpf; j < mp; j++)
	{
#if 0
		if (j == intpf && ffrac > 0)
			pf = ffrac;
		if (j == mp && lfrac > 0)
			pf = lfrac;
#endif
		fx = j;
		int waveform = tone_typelist[fx];
		ph = phase_accumulator[fx];
		inc = phase_increment[fx];
		if (envelope[fx]->doOffPhase)
			pulse = phaselist[fx] / 360.0;
		else if (envelope[fx]->doPhase)
			pulse = fabs(mod + 1) / 2.0;
		else
			pulse = 0;
		if (!envelope[fx]->doFM)
			ismod = false;
		if (pulse > 1)
			pulse = 1;
		//	Serial.print(ph);
		//	Serial.print(",");
		pulse_width = pulse * 4294967296.0;
		if (ismod)
			inc *= (1 + mod);
		if (!block)
		{
			if (fx == intpf)
				Serial.println("no block");
			// unable to allocate memory, so we'll send nothing
			ph += phase_increment[fx] * AUDIO_BLOCK_SAMPLES;
			phase_accumulator[fx] = ph;
			if (fx == nfreq - 1)
				return;
			continue;
		}
		if (doPhase)
		{
			waveform = WAVEFORM_PULSE_HG;
		}
		if (!fx && tone_typelist[fx] == WAVEFORM_SQUARE && 0)
		{
			Serial.print("ismod");
			Serial.print(" ");
			Serial.print(ismod ? 1 : 0);
			Serial.print(" Pulse");
			Serial.print(envelope[fx]->doPhase ? 1 : 0);
			Serial.print(" ");
			Serial.print(pulse);
			Serial.print(" Phase");
			Serial.print(envelope[fx]->doOffPhase ? 1 : 0);
			Serial.print(" shape ");
			Serial.println(waveform);
		}
		float e = magnitudes[fx];
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (fx == intpf)
				block->data[i] = 0;
			e = envelope[fx]->update(solos > 0, loopend, loopmark, fx);
			//				val = taylor(ph);
			//				block->data[i] += e*(val & 0xFFFF);
			//				block->data[i] +=(val >> 16)*e;
			//			if(!fx);
			//			Serial.println(e);
			magnitude = e * ivol;
			switch (waveform)
			{
			case WAVEFORM_SINE:
				index = ph >> 24;
				val1 = AudioWaveformSine[index];
				val2 = AudioWaveformSine[index + 1];
				scale = (ph >> 8) & 0xFFFF;
				val2 *= scale;
				val1 *= 0x10000 - scale;
				val = multiply_32x32_rshift32(val1 + val2, magnitude);
				ph += inc;
				//					Serial.println(ph);
				break;

			case WAVEFORM_ARBITRARY:
				if (!arbdata)
				{
					release(block);
					phase_accumulator[fx] += inc * AUDIO_BLOCK_SAMPLES;
					return;
				}
				// len = 256
				index = ph >> 24;
				index2 = index + 1;
				if (index2 >= 256)
					index2 = 0;
				val1 = *(arbdata + index);
				val2 = *(arbdata + index2);
				scale = (ph >> 8) & 0xFFFF;
				val2 *= scale;
				val1 *= 0x10000 - scale;
				val = multiply_32x32_rshift32(val1 + val2, magnitude);
				ph += inc;
				break;

			case WAVEFORM_SQUARE:
				magnitude15 = signed_saturate_rshift(magnitude, 16, 1);
				if (ph & 0x80000000)
				{
					val = -magnitude15;
				}
				else
				{
					val = magnitude15;
				}
				ph += inc;
				break;

			case WAVEFORM_SAWTOOTH:
				val = signed_multiply_32x16t(magnitude, ph);
				ph += inc;
				break;

			case WAVEFORM_TRIANGLE:
			{
				uint32_t phtop = ph >> 30;
				if (phtop == 1 || phtop == 2)
				{
					val = ((0xFFFF - (ph >> 15)) * magnitude) >> 16;
				}
				else
				{
					val = (((int32_t)ph >> 15) * magnitude) >> 16;
				}
				ph += inc;
			}
			break;

			case WAVEFORM_PULSE_HG:
				magnitude15 = signed_saturate_rshift(magnitude, 16, 1);
				if (ph < pulse_width)
				{
					val = magnitude15;
				}
				else
				{
					val = -magnitude15;
				}
				ph += inc;
				break;
			case WAVEFORM_NOISE:
			{
				int16_t lopa = 0, bpa = 0, hipa = 0;
				val = magnitude * noise();
				envelope[fx]->update_fixed(val, &lopa, &bpa, &hipa);
				val = lopa;
				break;
			}
			}

#if 0
			phase_accumulator = ph - phase_offset;
			index = ph >> 24;
			val1 = AudioWaveformSine[index];
			val2 = AudioWaveformSine[index + 1];
			scale = (ph >> 8) & 0xFFFF;
			val2 *= scale;
			val1 *= 0x10000 - scale;
#endif
			block->data[i] += val;

			//if(state==1&&fx==0&&i==AUDIO_BLOCK_SAMPLES-1)
			//	Serial.println(block->data[i]);
			//				if (state == 1 && fx == nfreq-1)
			//				{
			//					Serial.println(block->data[i]);
			//				}
			ph += inc;
			//		if(e>0&&fx==0)
			//		Serial.println(e);
		}
		phase_accumulator[fx] = ph - phaselist[fx];

		//	Serial.print(fx);
		//	Serial.print("  ");
		//	Serial.println(phase_increment[fx]);
	}
	//	for (i=0; i < AUDIO_BLOCK_SAMPLES&&state==1; i++)
	transmit(block);
	release(block);
}

int32_t AudioSynthWaveformHarmGen::noise(void)
{
	//	int32_t n1, n2, gain, sample;
	//	uint32_t lo, hi, val1, val2;
	int32_t n1, gain, sample;
	uint32_t lo, hi;

	gain = level;
	if (gain == 0)
		return 0;
	lo = seed;

	hi = multiply_16bx16t(16807, lo); // 16807 * (lo >> 16)
	lo = 16807 * (lo & 0xFFFF);
	lo += (hi & 0x7FFF) << 16;
	lo += hi >> 15;
	lo = (lo & 0x7FFFFFFF) + (lo >> 31);
	n1 = signed_multiply_32x16b(gain, lo);
	sample = n1;
	seed = lo;
	return sample;
}

bool segchange = true;
#define MULT(a, b) (multiply_32x32_rshift32_rounded(a, b) << 2)
float Envelope::update(bool isSolo, int loopend, int loopmark, int fx)
{
	//	if(fabs(prev)>0xffff)
	//		Serial.println(prev);
	if (!noteon || (isSolo && !solo) || mute)
	{
		if (noteon && 0)
		{
			Serial.print("fx ");
			Serial.print(fx);
			Serial.print("isSolo ");
			Serial.print(isSolo);
			Serial.print(" solo ");
			Serial.print(solo);
			Serial.print(" mute ");
			Serial.println(mute);
		}
		return 0;
	}
	if (noteon == 2)
	{
		//		noteon=3;
		lastm = -prev / (times[actseg]);
	}
	if (count >= times[actseg] * tf && noteon == 1)
	{
		actseg++;
		segchange = true;
		if (actseg >= segments - 2)
		{
			noteon = 0;
			lastm = -prev / (times[actseg] * tf);
		}
		//		prev=levels[actseg];
		if (actseg > segments - 1)
			actseg = segments - 1;
		count = 0;
	}
	if (noteon == 1)
		res = prev + slope[actseg] / tf;
	else
		res = prev + lastm;
	if (noteon > 1 && res <= 0)
	{
		//		Serial.print(noteon);
		//		Serial.println(" end");
		//		noteon=0;
		return (0);
	}
	prev = res;
	res = af * res;
	count++;
	return (res);
}
#if 0
if (count % 441 == 0 && fx == 0)
{
	Serial.println(res);
}
#endif

void Envelope::update_fixed(const int16_t in, int16_t *lp, int16_t *bp, int16_t *hp)
{
	int32_t input, inputprev;
	int32_t lowpass, bandpass, highpass;
	int32_t lowpasstmp, bandpasstmp, highpasstmp;
	int32_t fmult, damp;

	fmult = setting_fmult;
	damp = setting_damp;
	inputprev = state_inputprev;
	lowpass = state_lowpass;
	bandpass = state_bandpass;
	input = in << 12;
	lowpass = lowpass + MULT(fmult, bandpass);
	highpass = ((input + inputprev) >> 1) - lowpass - MULT(damp, bandpass);
	inputprev = input;
	bandpass = bandpass + MULT(fmult, highpass);
	lowpasstmp = lowpass;
	bandpasstmp = bandpass;
	highpasstmp = highpass;
	lowpass = lowpass + MULT(fmult, bandpass);
	highpass = input - lowpass - MULT(damp, bandpass);
	bandpass = bandpass + MULT(fmult, highpass);
	lowpasstmp = signed_saturate_rshift(lowpass + lowpasstmp, 16, 13);
	bandpasstmp = signed_saturate_rshift(bandpass + bandpasstmp, 16, 13);
	highpasstmp = signed_saturate_rshift(highpass + highpasstmp, 16, 13);
	*lp = lowpasstmp;
	*bp = bandpasstmp;
	*hp = highpasstmp;
	state_inputprev = inputprev;
	state_lowpass = lowpass;
	state_bandpass = bandpass;
}
