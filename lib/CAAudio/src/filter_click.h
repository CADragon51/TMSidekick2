

#ifndef filter_click_h_
#define filter_click_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#include "utility/dspinst.h"

class AudioFilterClick : public AudioStream
{

public:
    AudioFilterClick() : AudioStream(2, inputQueueArray)
    {
    }
    void setNum(byte nums)
    {
        numSamples = nums;
    }
    void setThreshold(float peps)
    {
        epsilon = peps * 32 * 1024;
    }
    virtual void update(void);
    void setOn(bool isOn)
    {
        _on = isOn;
    }

private:
    audio_block_t *inputQueueArray[2];
    float epsilon =400;
    byte numSamples = 7;
    bool _on;
};

#endif
