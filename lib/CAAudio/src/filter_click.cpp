/* Audio Library for Teensy 3.X
 * Copyright (c) 2017, Paul Stoffregen, paul@pjrc.com
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
#include "filter_click.h"

void AudioFilterClick::update(void)
{
    audio_block_t *block, *block2;
 //   static int16_t lastsample = 0;
 //   static int lastdiff = 0;
 //   static int cc = 0;
    block = receiveWritable(0);
    block2 = receiveReadOnly(1);
    if (!block)
        return;
 //  int diff = block->data[0] - lastsample;
 //   int diff2 = diff - lastdiff;
  //  int delta = 0;
  //  int di = 0;
  //  if (!_on)
  //      cc = 0;
    //    if (_on)
    //        Serial.println(String(0) + "\t" + String(block->data[0]) + "\t" + abs(diff2));
 //   int i1 = -1, i2 = 0;
//    static int rc = 4;
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        #if 0
        if (abs(block2->data[i]) > epsilon&&i1==-1)
        {
            i1 = i;
            int j = 0;
            for (j = i + 1; j < AUDIO_BLOCK_SAMPLES; j++)
            {
                if (abs(block2->data[j]) < epsilon)
                {
                    i2 = j;
                    break;
                }
            }
            int fp = block->data[i1 - 2];
            int lp = block->data[i2];
            float incr = (lp - fp) / (i2 - i1);
            for (j = i1 - 1; j < i2; j++)
            {
                int ob = block->data[j];
                fp += incr;
                block->data[j] = fp ;
               if (_on)
                  Serial.println(String(j) + " \t" + String(ob) + "\t" + String(block->data[j]) + "\t" + String(block2->data[j]) + "\t" + String(i1) + "\t" + String(i2));
       
            }
           i1 = -1;
           i = i2;
       }
     else if (_on)
       Serial.println(String(i) + " \t" + String(block->data[i]) + "\t" + String(block->data[i]) + "\t" + String(block2->data[i]) + "\t" + String(i1) + "\t" + String(i2));
#endif
     }
     transmit(block);
     release(block);
     release(block2);
}
