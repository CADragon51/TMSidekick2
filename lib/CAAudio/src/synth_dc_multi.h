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

#ifndef synth_dc_multi_h_
#define synth_dc_multi_h_
#include "synth_dc.h"

class AudioSynthWaveformDcMulti : public AudioSynthWaveformDc
{
public:
	AudioSynthWaveformDcMulti() : AudioSynthWaveformDc(), state(0), magnitude(0),segments(0) {}
	// immediately jump to the new DC level

	void noteOn(){
		actseg=0;
		noteon=true;
		amplitude(levels[0]);
		Serial.println("Noteon:"+String(levels[0]));
	}
	void noteOff(float n, float milliseconds){
		noteon=false;
		amplitude(n,milliseconds);
	}
	void amplitude(float n) {
		if (n > 1.0) n = 1.0;
		else if (n < -1.0) n = -1.0;
		int32_t m = (int32_t)(n * 2147418112.0);
		__disable_irq();
		magnitude = m;
		state = 0;
		__enable_irq();
	}
	// slowly transition to the new DC level
	void amplitude(float n, float milliseconds) {
		if (milliseconds <= 0.0) {
			amplitude(n);
			return;
		}
		if (n > 1.0) n = 1.0;
		else if (n < -1.0) n = -1.0;
		int32_t c = (int32_t)(milliseconds*(AUDIO_SAMPLE_RATE_EXACT/1000.0));
		if (c == 0) {
			amplitude(n);
			return;
		}
		int32_t t = (int32_t)(n * 2147418112.0);
		__disable_irq();
		target = t;
		if (target == magnitude) {
			state = 0;
			__enable_irq();
			return;
		}
		increment = substract_int32_then_divide_int32(target, magnitude, c);
		if (increment == 0) {
			increment = (target > magnitude) ? 1 : -1;
		}
		state = 1;
		__enable_irq();
	}
	float read(void) {
		int32_t m = magnitude;
		return (float)m * (1.0 / 2147418112.0);
	}

	virtual void update(void);
	void multiseg(float *plevels,float *ptimes,int32_t  psegments)
	{
		times=ptimes;
		levels=plevels;
		segments=psegments;
		Serial.println("Multiseg:"+String(levels[0]));
	}

private:
	uint8_t  state;     // 0=steady output, 1=transitioning
	int32_t  magnitude; // current output
	int32_t  target;    // designed output (while transitiong)
	int32_t  increment; // adjustment per sample (while transitiong)

	float *times;
	float *levels;
	float maxlevel;
	int32_t  segments;
	int32_t  actseg;
	bool noteon;

};

#endif
