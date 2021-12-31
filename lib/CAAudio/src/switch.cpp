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

#include "switch.h"
#include "utility/dspinst.h"


void AudioSwitch::update(void)
{
	audio_block_t *in1 = nullptr, *in2 = nullptr, *out1 = nullptr, *out2 = nullptr;
	in1 = receiveReadOnly(0);
 	in2 = receiveReadOnly(1);
    if (!in1&&!in2)
		return;
	if(in1&&in2)
	{
		out1 = allocate();
		if (!out1)
		{
			release(in1);
			release(in2);
			return;
		}
		if (state ==0)
			memcpy(out1->data,in1->data,AUDIO_BLOCK_SAMPLES*2);
		else
			memcpy(out1->data,in2->data,AUDIO_BLOCK_SAMPLES*2);
	}
    if(in1&&!in2)
	{
		out1 = allocate();
		if (!out1)
		{
			release(in1);
			return;
		}
		out2 = allocate();
		if (state ==0)
		{
			memcpy(out1->data,in1->data,AUDIO_BLOCK_SAMPLES*2);
			if(out2)
				memset(out2->data,0,AUDIO_BLOCK_SAMPLES*2);
		}
		else
		{
			if(out2)
				memcpy(out2->data,in1->data,AUDIO_BLOCK_SAMPLES*2);
			memset(out1->data,0,AUDIO_BLOCK_SAMPLES*2);
		}
		transmit(out2,1);
		release(out2);
	}
	if(out1)
	{
		transmit(out1,0);
		release(out1);
	}
	if(in1) release(in1);
	if(in2) release(in2);
}


