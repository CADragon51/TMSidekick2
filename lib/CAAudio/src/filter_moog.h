

#ifndef filter_moog_h_
#define filter_moog_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#include "utility/dspinst.h"

class AudioFilterMoog: public AudioStream
{

public:
	AudioFilterMoog() : AudioStream(2, inputQueueArray) {
		frequency(1000);
		resonance(1);
		drive(3);
		octaveControl(1);
		y_a = 0;
		y_b = 0;
		y_c = 0;
		y_d = 0;
		y_d_1 = 0;
		frequency(1000);
		octaveControl(1.0); // default values
		state_inputprev = 0;
		state_lowpass = 0;
		state_bandpass = 0;
	}
		void frequency(float freq,bool save=true) {
		if (freq < 1) freq = 1;
		else if (freq > AUDIO_SAMPLE_RATE_EXACT/2.5) freq = AUDIO_SAMPLE_RATE_EXACT/2.5;
		setting_fcenter = (freq * (3.141592654/(AUDIO_SAMPLE_RATE_EXACT*2.0)))
			* 2147483647.0;
		// TODO: should we use an approximation when freq is not a const,
		// so the sinf() function isn't linked?
		setting_fmult = sinf(freq * (3.141592654/(AUDIO_SAMPLE_RATE_EXACT*2.0)))
			* 2147483647.0;
		mfrequency(freq,save);
	}
	void resonance(float qi) {
		q=qi;
		if (qi < 0.7) q = 0.7;
		else if (qi > 15.0) q = 15.0;
		// TODO: allow lower Q when frequency is lower
		setting_damp = (1.0 / q) * 1073741824.0;
	}
	void octaveControl(float n) {
		// filter's corner frequency is Fcenter * 2^(control * N)
		// where "control" ranges from -1.0 to +1.0
		// and "N" allows the frequency to change from 0 to 7 octaves
		if (n < 0.0) n = 0.0;
		else if (n > 6.9999) n = 6.9999;
		setting_octavemult = n * 4096.0;
		oct=n*MAX_INTINV;
	}

 	void drive(float d) {
		if (d > 10.0f) d = 10.0f;
		if (d < 0.1f) d = 0.1f;
		driv = d;
	}
	void moog(bool set) {
		isMoog=set;
	}
	void mfrequency(float freq,bool setf=true) {
	    g = 1 - expf(-2 * tanf(2 * M_PI * freq/(2 * AUDIO_SAMPLE_RATE_EXACT)));
		if(setf)
			cutoff=freq;
	}
	void moog_update(const int16_t *input_block,const int16_t *control_block,int16_t *lowpass_block);
	virtual void update(void);
	float g;
	float q;
	float driv;
	float oct;
	float cutoff;
	
	float y_a;
	float y_b;
	float y_c;
	float y_d;
	float y_d_1;
	const float MAX_INT = 32678.0;
	const float MAX_INTINV = 1.0/32678.0;
	void update_fixed(const int16_t *in,
		int16_t *lp, int16_t *bp, int16_t *hp);
	void update_variable(const int16_t *in, const int16_t *ctl,
		int16_t *lp, int16_t *bp, int16_t *hp);
	int32_t setting_fcenter;
	int32_t setting_fmult;
	int32_t setting_octavemult;
	int32_t setting_damp;
	int32_t state_inputprev;
	int32_t state_lowpass;
	int32_t state_bandpass;
    bool isMoog=false;
private:
	void update_fixed(const int16_t*in,int16_t *lp);
	void update_variable(const int16_t *in, const int16_t *ctl,	int16_t *lp);
	audio_block_t *inputQueueArray[2];
};

#endif
