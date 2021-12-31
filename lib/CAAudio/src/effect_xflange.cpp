
#include <Arduino.h>
#include "effect_xflange.h"
#include "arm_math.h"

boolean AudioEffectXFlange::begin(void)
{
	int sampleRate = 44100;
	boolean all_ok = true;

	float maxDelayTime = 0.02;
	delayBufferSamples = (int)(maxDelayTime * (float)44100) + 1;
	if (delayBufferSamples < 1)
		delayBufferSamples = 1;

	for (int i = 0; i < delayBufferSamples; i++)
		delayData[i] = 0;

	delayWritePosition = 0;
	lfoPhase = 0.0f;
	inverseSampleRate = 1.0f / (float)sampleRate;
	twoPi = 2.0f * (float)M_PI;
	return(all_ok);
}





//==============================================================================

float AudioEffectXFlange::lfo(float phase, int waveform)
{
	float out = 0.0f;

	switch (waveform) {
	case waveformSine: {
		out = 0.5f + 0.5f * sinf(twoPi * phase);
		break;
	}
	case waveformTriangle: {
		if (phase < 0.25f)
			out = 0.5f + 2.0f * phase;
		else if (phase < 0.75f)
			out = 1.0f - 2.0f * (phase - 0.25f);
		else
			out = 2.0f * (phase - 0.75f);
		break;
	}
	case waveformSawtooth: {
		if (phase < 0.5f)
			out = 0.5f + phase;
		else
			out = phase - 0.5f;
		break;
	}
	case waveformInverseSawtooth: {
		if (phase < 0.5f)
			out = 0.5f - phase;
		else
			out = 1.5f - phase;
		break;
	}
	}

	return out;
}

void AudioEffectXFlange::update(void)
{
	audio_block_t* block;
	short* bp;


	// do passthru
	if (delay_offset_idx == FLANGE_DELAY_PASSTHRU) {
		// Just passthrough
		block = receiveWritable(0);
		if (block) {
			bp = block->data;
			// fill the delay line
			for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
				l_circ_idx++;
				if (l_circ_idx >= delay_length) {
					l_circ_idx = 0;
				}
				delayData[l_circ_idx] = *bp++;
			}
			// transmit the unmodified block
			transmit(block, 0);
			release(block);
		}
		return;
	}

	//          L E F T  C H A N N E L

	block = receiveWritable(0);
	float sr = 44100.0;

	if (block)
	{
		float currentDelay = fdelay / 10000;
		float currentWidth = fwidth / 10000.0;
		float currentDepth = fdepth;
		float currentFeedback = ffb;
		float currentFrequency = frate;
		int localWritePosition;
		float phase;
		float phaseMain;
		//		int channel = 0;
		localWritePosition = delayWritePosition;
		phase = lfoPhase;
		bp = block->data;
		for (int i = 0; i < AUDIO_BLOCK_SAMPLES ; i++) {
		int16_t out = 0;
		float wv = lfo(phase, fshape);
		float localDelayTime = (currentDelay + currentWidth * wv) * sr;
		float readPosition = fmodf((float)localWritePosition - localDelayTime + (float)delayBufferSamples, (float)delayBufferSamples);
		int localReadPosition = (int)floor(readPosition);

		if (fnone) {
			int16_t closestSample = delayData[localReadPosition % delayBufferSamples];
			out = closestSample;
		}
		if (flinear) {
			float fraction = readPosition - (float)localReadPosition;
			int16_t delayed0 = delayData[(localReadPosition + 0)];
			int16_t delayed1 = delayData[(localReadPosition + 1) % delayBufferSamples];
			out = delayed0 + fraction * (delayed1 - delayed0);
		}
		if (fcubic) {
			float fraction = readPosition - (float)localReadPosition;
			float fractionSqrt = fraction * fraction;
			float fractionCube = fractionSqrt * fraction;

			int16_t sample0 = delayData[(localReadPosition - 1 + delayBufferSamples) % delayBufferSamples];
			int16_t sample1 = delayData[(localReadPosition + 0)];
			int16_t sample2 = delayData[(localReadPosition + 1) % delayBufferSamples];
			int16_t sample3 = delayData[(localReadPosition + 2) % delayBufferSamples];

			float a0 = -0.5f * sample0 + 1.5f * sample1 - 1.5f * sample2 + 0.5f * sample3;
			float a1 = sample0 - 2.5f * sample1 + 2.0f * sample2 - 0.5f * sample3;
			float a2 = -0.5f * sample0 + 0.5f * sample2;
			float a3 = sample1;
			out = a0 * fractionCube + a1 * fractionSqrt + a2 * fraction + a3;
		}
		static float w1=0;
		int16_t in = (*bp);
		float w = in + 0.9 * w1;
		float y = w - w1;
		w1 = w;
		in=y;
		delayData[localWritePosition] = in + out * currentFeedback;
		//			if (isnan(in + out * currentFeedback))
		//				delayData[localWritePosition] = 0;
		if (++localWritePosition >= delayBufferSamples)
			localWritePosition -= delayBufferSamples;

		phase += currentFrequency * inverseSampleRate;
		if (phase >= 1.0f)
			phase -= 1.0f;
		*bp++ = in+out * currentDepth;
//		if(in<-10||in>10)
//		Serial.println(in);
//		Serial.println(in+out * currentDepth);
	}

	phaseMain = phase;

	delayWritePosition = localWritePosition;
	lfoPhase = phaseMain;
		// send the effect output to the left channel
	transmit(block, 0);
	release(block);
	}
}



