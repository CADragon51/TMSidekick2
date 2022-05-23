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
#pragma once
#ifndef synth_mvwaveform_h_
#define synth_mvwaveform_h_

#include <Arduino.h>
#include "AudioStream.h"
#include "arm_math.h"
#include <synth_waveform.h>
// waveforms.c
extern "C"
{
	extern const int16_t AudioWaveformSine[257];
}

class BandLimitedWaveform;
class AudioSynthMVWaveformModulated : public AudioStream
{
public:
	AudioSynthMVWaveformModulated(void) : AudioStream(2, inputQueueArray),
										  modulation_factor(32768),
										  arbdata(NULL), sample(0), tone_offset(0),
										  modulation_type(0)
	{
		setPortamento(10);
		setTau(1);
	}
	uint32_t ftop = (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
	void frequency(float freqFrom, float freqTo)
	{
		//	Serial.println("Freq from " + String(freqFrom) + " to " + String(freqTo)+" t "+String(tau));
		if (tau == 0)
		{
			frequency(freqTo);
			return;
		}
		float ff[3];
		float ft[3];
		ff[0] = freqTo;
		ff[1] = ff[0] + detune;
		ff[2] = ff[0] * harm[2];
		ft[0] = freqFrom;
		ft[1] = ft[0] + detune;
		ft[2] = ft[0] * harm[2];
		if (freqFrom < 0.0)
		{
			freqFrom = 0.0;
		}
		else if (freqFrom > AUDIO_SAMPLE_RATE_EXACT / 2)
		{
			freqFrom = AUDIO_SAMPLE_RATE_EXACT / 2;
		}
		if (freqTo < 0.0)
		{
			freqTo = 0.0;
		}
		else if (freqTo > AUDIO_SAMPLE_RATE_EXACT / 2)
		{
			freqTo = AUDIO_SAMPLE_RATE_EXACT / 2;
		}
		ff[0] = freqFrom;
		ff[1] = ff[0] + detune;
		ff[2] = ff[0] * harm[2];
		ft[0] = freqTo;
		ft[1] = ft[0] + detune;
		ft[2] = ft[0] * harm[2];
		upw = freqTo - freqFrom;
		if (freqTo == 0 || portatime == 0)
			return;
		for (int i = 0; i < 3; i++)
		{
			phase_increment[i] = ft[i] * ftop;
			from_phase_increment[i] = ff[i] * ftop;
			if (phase_increment[i] > 0x7FFE0000u)
				phase_increment[i] = 0x7FFE0000;
			if (from_phase_increment[i] > 0x7FFE0000u)
				from_phase_increment[i] = 0x7FFE0000;
			float df = ft[i] - ff[i];
			df /= portatime;
			//			Serial.println(String(df,5) + " pt " + String(portatime) + " t " + String(tau));
			if (tau == 2)
			{
				if (freqTo > freqFrom)
					si[i] = logf(ft[i] - ff[i]) / portatime;
				else
					si[i] = -logf(ff[i] - ft[i]) / portatime;
				si[i] = exp(si[i]);
				s0[i] = si[i];
			}
			else if (tau == 1)
			{
				delta_phase_increment[i] = ftop * df;
				//				Serial.println(String(df, 5) + " pt " + String(portatime) + " t " + String(delta_phase_increment[i] ));
			}
		}
	}
	void frequency(float freq)
	{
		if (freq < 0.0)
		{
			freq = 0.0;
		}
		else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2)
		{
			freq = AUDIO_SAMPLE_RATE_EXACT / 2;
		}
		phase_increment[0] = freq * ftop;
		if (phase_increment[0] > 0x7FFE0000u)
			phase_increment[0] = 0x7FFE0000;
		phase_increment[1] = (detune + harm[1] * freq) * ftop;
		if (phase_increment[1] > 0x7FFE0000u)
			phase_increment[1] = 0x7FFE0000;
		phase_increment[2] = harm[2] * freq * ftop;
		if (phase_increment[2] > 0x7FFE0000u)
			phase_increment[2] = 0x7FFE0000;
		upw = 0;
	}
	void setPortamento(float time)
	{
		portatime = 44.1 * time;
	}
	void setTau(int ptau)
	{
		tau = ptau;
	}
	int getTau(void)
	{
		return tau;
	}
	void amplitude(float n, int m)
	{ // 0 to 1.0
		if (n < 0)
		{
			n = 0;
		}
		else if (n > 1.0)
		{
			n = 1.0;
		}
		magnitude[m] = n * 65536.0;
	}
	void offset(float n)
	{
		if (n < -1.0)
		{
			n = -1.0;
		}
		else if (n > 1.0)
		{
			n = 1.0;
		}
		tone_offset = n * 32767.0;
	}
	void begin(short t_type)
	{
		tone_type[0] = t_type;
		tone_type[1] = t_type;
		tone_type[2] = t_type;
		for (int o = 0; o < 3; o++)
		{
			if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
				band_limit_waveform[o].init_square(phase_increment[o]);
			else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
				band_limit_waveform[o].init_pulse(phase_increment[o], 0x80000000u);
			else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
				band_limit_waveform[o].init_sawtooth(phase_increment[o]);
		}
	}
	void begin(short t_type, byte osc)
	{
		tone_type[osc] = t_type;
		if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
			band_limit_waveform[osc].init_square(phase_increment[osc]);
//		Serial.println("set shape " + String(osc) + " " + String(t_type));
	}
	void setDetune(float t_detune)
	{
		detune = t_detune;
	}
	void setHarm(float t_detune, int osc)
	{
		if (osc == 2)
		{
			harm[osc] = 1.0 / t_detune;
		}
		if (osc == 1)
			harm[osc] = offfreq[(int)t_detune];
//		Serial.println("set harm " + String(osc) + " " + String(harm[osc]));
	}
	void begin(float t_amp, float t_freq, u_int8_t t_type)
	{

		amplitude(t_amp, 0);
		frequency(t_freq);
		tone_type[0] = t_type;
	}
	void arbitraryWaveform(const int16_t *data, float maxFreq)
	{
		arbdata = data;
	}
	void frequencyModulation(float octaves)
	{
		if (octaves > 12.0)
		{
			octaves = 12.0;
		}
		else if (octaves < 0.1)
		{
			octaves = 0.1;
		}
		modulation_factor = octaves * 4096.0;
		modulation_type = 0;
	}
	void phaseModulation(float degrees)
	{
		if (degrees > 9000.0)
		{
			degrees = 9000.0;
		}
		else if (degrees < 30.0)
		{
			degrees = 30.0;
		}
		modulation_factor = degrees * (65536.0 / 180.0);
		modulation_type = 1;
	}
	virtual void update(void);

private:
	audio_block_t *inputQueueArray[2];
	uint32_t phase_accumulator[3] = {0, 0, 0};
	uint32_t phase_increment[3] = {0, 0, 0};
	uint32_t from_phase_increment[3] = {0, 0, 0};
	int32_t delta_phase_increment[3] = {0, 0, 0};
	uint32_t modulation_factor;
	int32_t magnitude[3] = {0, 0, 0};
	float offfreq[12] = {1, 1.059463094, 1.122462048, 1.189207115, 1.25992105, 1.334839854, 1.414213562, 1.498307077, 1.587401052, 1.681792831, 1.781797436, 1.887748625};

	const int16_t *arbdata;
	//	uint32_t phasedata[3][AUDIO_BLOCK_SAMPLES];
	int16_t sample; // for WAVEFORM_SAMPLE_HOLD
	int16_t tone_offset;
	float suboct = 2;
	float detoff = 0;
	float detune = 0;
	uint8_t tone_type[3];
	uint8_t modulation_type;
	float portatime = 0;
	BandLimitedWaveform band_limit_waveform[3];
	float s0[3] = {0.0, 0.0, 0.0};
	float si[3];
	int tau = 0;
	float upw = 0;
	float harm[3] = {1, 0.5, 0};
};

#endif
