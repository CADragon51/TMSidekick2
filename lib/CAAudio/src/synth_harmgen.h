
#ifndef synth_harmgen_h_
#define synth_harmgen_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#define WAVEFORM_SINE              0
#define WAVEFORM_SAWTOOTH          1
#define WAVEFORM_SQUARE            2
#define WAVEFORM_TRIANGLE          3
#define WAVEFORM_ARBITRARY         4
#define WAVEFORM_NOISE      5

#define WAVEFORM_PULSE_HG      6
#define WAVETABLE     7
class Envelope 
{
public:
	Envelope() :  noteon(0),segments(0){
		frequency(1000);
		state_inputprev = 0;
		state_lowpass = 0;
		state_bandpass = 0;
	}
	// immediately jump to the new DC level
	void noteOn(int fx,float dur,float ptf);
	void noteOn(){
		actseg=0;
		count=0;
		noteon=1;
		prev =levels[0];
		tf=1.0;
		#if 1
		Serial.print(noteon);
		Serial.print(" on ");
		Serial.print(prev);
		Serial.print(" act ");
		Serial.println(actseg);
	#endif
	}

	void frequency(float freq) {
		if (freq < 1) freq = 1;
		else if (freq > AUDIO_SAMPLE_RATE_EXACT/2.5) freq = AUDIO_SAMPLE_RATE_EXACT/2.5;
		setting_fcenter = (freq * (3.141592654/(AUDIO_SAMPLE_RATE_EXACT*2.0)))			* 2147483647.0;
		// TODO: should we use an approximation when freq is not a const,		// so the sinf() function isn't linked?
		setting_fmult = sinf(freq * (3.141592654/(AUDIO_SAMPLE_RATE_EXACT*2.0)))* 2147483647.0;
		Serial.println(setting_fcenter);
		Serial.println(setting_fmult);
	}
	void update_fixed(const int16_t in,int16_t *lp, int16_t *bp, int16_t *hp);

	void noteOff(float n, float milliseconds){
//		actseg=segments-2;
//		count=0;
		noteon=0;
	}
	void noteOff(){
		noteon=0;
	}
	float damps[77];
	void multiseg(const float *plevels,float *pslope,int32_t  psegments,const float *ptimes,float *pamps)
	{

		levels=(float *)plevels;
//		segments=psegments;
		slope=pslope;
		if(pamps==0)
		{
		amps=damps;
		for(int i=0;i<77;i++)
			amps[i]=1.0;
		}
		times=(float *)ptimes;
	}

	void multiseg(int s,float pamps)
	{
		amps[s]=pamps;
//		Serial.println(amps[s]);
	}
	float update(bool isSolo,int loopend,int loopmark,int fx);
	int noteon;
	int32_t  segments;
	bool show=false;
	int32_t  count; // sample count;
	float *amps;
	bool solo=false;
	bool mute=false;
    float af=1.0;
	float tf=1.0;
	bool debug=false;
	bool doFM=false;
	bool doPhase=false;
	bool doOffPhase=false;
	int32_t state_inputprev;
	int32_t state_lowpass;
	int32_t state_bandpass;
	int32_t setting_fcenter;
	int32_t setting_fmult;
	int32_t setting_octavemult;
	int32_t setting_damp;

private:
	double prev=0.0;
	int32_t  actseg;
	float res;
	float lastm=0;
	float *slope;
	float *levels;
	float *times;
};

class AudioSynthWaveformHarmGen : public AudioStream
{
public:
	AudioSynthWaveformHarmGen() : AudioStream(1, inputQueueArray){
		for(int i=0;i<64;i++)
		{
			magnitudes[i]=16384;
			envelope[i]=new Envelope;
		}
		nfreq=2;
//		ismulti=false;
		envelope[0]->solo=false;
		segments(77);
		setUp();
	}
	const float times0[77] = { 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22 };
const float levels0[77] = { 0.9, 0.678372, 0.495464, 0.301508, 0.339783, 0.498546, 0.598159, 0.645641, 0.642171, 0.585037, 0.502955, 0.418925, 0.359227, 0.289685, 0.234677, 0.183282, 0.127093, 0.0963231, 0.0848258, 0.0835139, 0.0991046, 0.11642, 0.127684, 0.139657, 0.147896, 0.153429, 0.158572, 0.159887, 0.155609, 0.153936, 0.143464, 0.126615, 0.110659, 0.0980336, 0.0879634, 0.0771719, 0.0672569, 0.0575699, 0.0495689, 0.0440098, 0.0399621, 0.039779, 0.0397175, 0.0390421, 0.039631, 0.0400186, 0.0373525, 0.0344221, 0.0340408, 0.032551, 0.0316066, 0.0289313, 0.0259956, 0.0236706, 0.021577, 0.0182963, 0.0172926, 0.0166325, 0.014334, 0.0143506, 0.0133971, 0.0119883, 0.0118358, 0.0110001, 0.0109675, 0.0109391, 0.0100006, 0.00941304, 0.00972209, 0.00844575, 0.00786468, 0.00704925, 0.00575545, 0.00635456, 0.00540625, 0.004776, 0.00458493 };
const float times1[77] = { 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22, 23.22 };
const float levels1[77] = { 0.0622159, 0.10114, 0.203122, 0.293699, 0.350318, 0.391812, 0.377322, 0.316547, 0.257969, 0.192751, 0.116965, 0.0951556, 0.152981, 0.211564, 0.274588, 0.313656, 0.326535, 0.32345, 0.29629, 0.257122, 0.200618, 0.141424, 0.0803016, 0.0175151, 0.0379016, 0.0837862, 0.118376, 0.139474, 0.148802, 0.149768, 0.141587, 0.121111, 0.101661, 0.0744591, 0.0462583, 0.0258496, 0.0275142, 0.0452851, 0.0627125, 0.0740816, 0.0807103, 0.0813423, 0.0784272, 0.0700038, 0.0576817, 0.0476025, 0.0333835, 0.0219651, 0.0175689, 0.0207928, 0.0275006, 0.0330718, 0.0375197, 0.0401784, 0.0388967, 0.0373528, 0.0330946, 0.0274361, 0.0228917, 0.0172748, 0.0139798, 0.0135771, 0.0148655, 0.0170235, 0.0186617, 0.0199667, 0.0197922, 0.0189367, 0.0179988, 0.0155708, 0.0138243, 0.0112574, 0.00959996, 0.00837181, 0.00860643, 0.00916598, 0.0097144 };
float slopes0[77];
float slopes1[77];

float *pslopes[2]={slopes0,slopes1};
const float *ptimes[2]={times0,times1};
const float *plevels[2]={levels0,levels1};

	void setUp()
    {
		short numseg=77;
      for (int j = 0; j < numseg; j++)
      {
        float nl = 0;
        if (j < numseg - 1)
          nl = levels0[j + 1];
        slopes0[j] = (nl - levels0[j]) / times0[j];
//		Serial.println(String((nl - levels0[j]))+","+String(levels0[j])+","+String(times0[j])+","+String(1000*slopes0[j]));
      }
      for (int j = 0; j < numseg; j++)
      {
        float nl = 0;
        if (j < numseg - 1)
          nl = levels1[j + 1];
        slopes1[j] = (nl - levels1[j]) / times1[j];
// 		Serial.println(slopes1[j]);
     }
      //     // // Serial.println(" ");
		envelope[0]->multiseg(levels0,slopes0,77,times0,0);
		envelope[1]->multiseg(levels1,slopes1,77,times1,0);
    }

	void setme(int n)
	{
		iam=n;
	}
	int iam=0;
	void partials(int nf) {
		if(nf>64)
			nf=64;
		nfreq=nf;
//				Serial.print("partials ");
//				Serial.println(nfreq);
	}
	int16_t dowavetable();
	int framesize;
	int frames; 
	int frame=0;
	float wtphase =0;
	void segments(int ns) {
		if(ns>128)
			ns=128;
		nseq=ns;
		for(int i=0;i<nfreq;i++)
		{
			envelope[i]->segments=nseq;
//			Serial.print(i);
//			Serial.print(" ");
//			Serial.println(nseq);
		}
	}
	// maximum unmodulated carrier frequency is 11025 Hz
	// input = +1.0 doubles carrier
	// input = -1.0 DC output
	void frequency(float freqin) {
//		Serial.print(" ");
//			return;
		for(int i=0;i<nfreq;i++)
		{
			float freq=freqin*(i+1);

			#if 0
			if(freqlist!=0)
			{
				freq=freqin*freqlist[i]+offlist[i];
				if(subharmlist[i]>0&&freqlist[i]>1)
					freq=freqin*1.0/freqlist[i]+offlist[i];

			}
			#endif
			if (freq < 0.0) freq = 0.0;
			else if (freq > AUDIO_SAMPLE_RATE_EXACT/4) freq = AUDIO_SAMPLE_RATE_EXACT/4;

			phase_increment[i] = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
//			phase_accumulator[i] = sync?0:phaselist[i]* (4294967296.0 / 360.0);
			phase_accumulator[i] = 0;
			if(!i)
				_frequency=freq;
			envelope[i]->frequency(freq);


			Serial.print("nfreq ");
			Serial.println(freq);

		}
//		Serial.println();
	}
	float _frequency;
	void phase(float *anglein) {
		for(int i=0;i<nfreq;i++)
		{
			float angle=anglein[i];
			if (angle < 0.0) angle = 0.0;
			else if (angle > 360.0) {
				angle = angle - 360.0;
				if (angle >= 360.0) continue;
			}
			phase_accumulator[i] = angle * (4294967296.0 / 360.0);
		}
	}
	void amplitude(float *nin) {
		for(int i=0;i<nfreq;i++)
		{
			float n=nin[i];
			if (n < 0) n = 0;
			else if (n > 1.0) n = 1.0;
			magnitudes[i] = n * 65536.0;
			phase_accumulator[i] = phaselist[i]* (4294967296.0 / 360.0);
		}
	}
#if 0
	void setfmax(float n,int omni,int part) {
		int fs=part,fe=part+1;
		if(omni)
		{
			fs=0;
			fe=nfreq;
		}
//		for(int i=fs;i<fe;i++)
//			amps[i]=n;
//		Serial.println(fmax);
	}
#endif
	void amplitude(float n) {
		for(int i=0;i<nfreq;i++)
		{
			if (n < 0) n = 0;
			else if (n > 1.0) n = 1.0;
			magnitudes[i] = n * 65536.0;
			phase_accumulator[i] = phaselist[i]* (4294967296.0 / 360.0);
		}
	}
	void setFreq(float *pfreqlist, float *pofflist, float *pphaselist,uint16_t *ptone_typelist,uint8_t *psubharmlist)
	{
			freqlist=pfreqlist;
			offlist=pofflist;
			phaselist=pphaselist;
			tone_typelist=ptone_typelist;
			subharmlist=psubharmlist;
//			Serial.print("set f");
//			Serial.println(freqlist[1]);
	}
	
	void noteOn(int fx,float dur){
//			Serial.print("nfreq ");
//			Serial.print(nfreq);
//			Serial.println(" ");
		note_on=1;
		for(int i=0;i<nfreq;i++)
		{
			envelope[i]->noteOn();
//			Serial.print(i);
//			Serial.print(" ");
		}
//		Serial.println(" on ");
	}
	void noteOn(float nf){
		nfreq=2;
		frequency(nf) ;
		Serial.print("nfreq ");
			Serial.print(nfreq);
		Serial.print("nf ");
			Serial.print(nf);
			Serial.println(" ");
		note_on=1;
//		for(int i=0;i<nfreq;i++)
//		{
//			envelope[i]->noteOn();
//		}
		Serial.println(" on ");
	}
	void noteOff(float n, float milliseconds){
		note_on=0;
		for(int i=0;i<nfreq;i++)
		{
			envelope[i]->noteOff(n, milliseconds);
		}
//		Serial.println(" off ");
	}
	void noteOff(){
		note_on=0;
		for(int i=0;i<nfreq;i++)
		{
			envelope[i]->noteOff();
		}
		Serial.println(" off ");
	}
	void multiseg(float *plevels,float *pslope,int32_t  psegments,float *pphase,int p,float *ptimes,float *pamps)
	{
		phase_accumulator[p] = pphase[p] * (4294967296.0 / 360.0);
		envelope[p]->multiseg(plevels,pslope,psegments,ptimes,pamps);
//		ismulti=true;
		m_time=ptimes[0];
	}
	void multiseg(int p,int s,float val)
	{
		envelope[p]->multiseg(s,val);
		Serial.print("multi ");
		Serial.print(p);
		Serial.print(" , ");
		Serial.print(s);
		Serial.print(" = ");
		Serial.println(envelope[p]->amps[s]);
	}
	void setshow(int part)
	{
	}
	void setsolo(int part,bool set)
	{
			envelope[part]->solo=set;
			if(set)
				solos++;
			else
				solos--;
			if(solos<0)
				solos=0;
//			Serial.println(String(part)+":"+String(envelope[part]->solo)+"->"+String(solos));
	}
	void setmute(int part,bool mute)
	{
		envelope[part]->mute=mute;
	}
	void setdoFM(int part,bool set)
	{
		if(part>=0)
			envelope[part]->doFM=set;
		else
			doFM=set;
	}
	void setdoPhase(int part,bool set)
	{
		if(part>=0)
			envelope[part]->doPhase=set;
		else
			doPhase=set;
	}
	void setdoHM(bool set)
	{
		doHM=set;
	}
	void setdoOffPhase(int part,bool set)
	{
		envelope[part]->doOffPhase=set;
	}
	void settf(int part,float val)
	{
		tf[part]=val;
	}
	void noise_amplitude(float n) {
		if (n < 0.0) n = 0.0;
		else if (n > 1.0) n = 1.0;
		level = (int32_t)(n * 65536.0);
	}
	int32_t noise(void);

	int32_t  level=65536;//=max
	uint32_t seed=1;  // must start at 1
	static uint16_t instance_count;

	virtual void update(void);
	int note_on=false;
	Envelope *envelope[64];
	float m_time;
	int nfreq;
	int nseq;
	int solos=0;
	int loopmark;
	int loopend;
	float delay;
	int hmod=0;
	int shape=0;
	float from;
	float to;
	float ivol=1.0;
	int state=-1;
	float *freqlist=0;
	float *offlist=0;
	float *phaselist=0;
	uint16_t *tone_typelist=0;
	uint8_t *subharmlist=0;
	uint32_t phase_accumulator[64];
	float tf[64]={1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
	uint32_t phase_increment[64];
	audio_block_t *inputQueueArray[1];
	int32_t magnitudes[64];
	int32_t  magnitude;
	uint32_t pulse_width;
	const int16_t *arbdata;
	int16_t  sample; // for WAVEFORM_SAMPLE_HOLD
	short    tone_type;
	uint16_t  tone_offset;
	int fx = 0;
	bool sync;
	uint32_t phasedata[AUDIO_BLOCK_SAMPLES];
	bool	doFM=false;
	bool	doPhase=false;
	bool	doHM=false;
	int16_t *buffer;
};

#endif
