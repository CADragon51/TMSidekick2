
#ifndef effect_xflange_h_
#define effect_xflange_h_

#include "Arduino.h"
#include "AudioStream.h"


#define FLANGE_DELAY_PASSTHRU 0

class  AudioEffectXFlange : 
public AudioStream
{
public:
  AudioEffectXFlange(void): 
  AudioStream(1,inputQueueArray) { 
  }

  boolean begin(void);
  void bypass(float set)
  { 
	delay_offset_idx = set*100;
  }
 
 virtual void update(void);

    enum waveformIndex {
        waveformSine = 0,
        waveformTriangle,
        waveformSawtooth,
        waveformInverseSawtooth,
    };

    //======================================

 
    enum interpolationIndex {
        interpolationNearestNeighbour = 0,
        interpolationLinear,
        interpolationCubic,
    };

    //======================================

    int16_t delayData[2000];
    int delayBufferSamples;
    int delayWritePosition;

    float lfoPhase;
    float inverseSampleRate;
    float twoPi;
	float sampleRate=44100;
    float lfo (float phase, int waveform);

    //======================================

	void setmode(int set)
	{
		fcubic=false; //paramInterpolation
		flinear=false; //paramInterpolation
		fnone=false; //paramInterpolation
		if(set==7)
			fcubic=true; //paramInterpolation
		if(set==8)
			flinear=true; //paramInterpolation
		if(set==9)
			fnone=true; //paramInterpolation
	}
	void setfdelay(float val)
	{
		fdelay=val; // paramDelay
	
	}
	void setfdepth(float val)
	{
		fdepth=val; // paramDepth
	}
	void setffb(float val)
	{
		ffb=val; //paramFeedback
	}
	void setfrate(float val)
	{
		frate=val; // paramFrequency
	}
	void setfshape(int val)
	{
		fshape=val; //paramWaveform
	}
	void setfwidth(float val)
	{
		fwidth=val;//paramWidth
	}
	float delaytime;

	bool fcubic; //paramInterpolation
	bool flinear; //paramInterpolation
	bool fnone; //paramInterpolation
	float fdelay; // paramDelay
	float fdepth; // paramDepth
	float ffb; //paramFeedback
	float frate; // paramFrequency
	int fshape; //paramWaveform
	float fwidth;//paramWidth

private:
  audio_block_t *inputQueueArray[1];
  int delay_length;
  short l_circ_idx;
 int delay_depth;
 int delay_offset_idx;
 int delay_rate_incr;
  unsigned int l_delay_rate_index;
};

#endif
