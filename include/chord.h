#ifndef CHORD_H
#define CHORD_H
#include "Adafruit_SSD1306.h"
extern Adafruit_SSD1306 display;
extern String showtext(String text, int x, int y, String tclass);
extern String shownote(byte n, float basex, float basey);

class Chord
{
public:
  Chord(short nscid, int iscid, byte inv, int cindex)
  {
    num = 1;
    _scid = nscid;
    note[0] = 0;
    invid = inv;
    for (int i = 0; i < 12; i++)
    {
      int test = 1 << i;
      if ((nscid & test) > 0)
      {
        note[num++] = i + 1;
        //       Serial.println(String(_scid) + " note " + String(note[num - 1]));
      }
      //				Serial.println(String(i) + " " + String(test) + " " + String(scid & test) + " " + String(nx));
    }
    name = chordName[iscid];
    _cindex = cindex;

    // Serial.println(String(nscid) + " " + String(iscid) + " " + String(inv) + " \"" + name + " " + String(_cindex));
    //    inverseID[0] = _scid;
  }
  Chord(short nscid, int cindex)
  {
    num = 1;
    _scid = nscid;
    _cindex = cindex;
    note[0] = 0;
    for (int i = 0; i < 12; i++)
    {
      int test = 1 << i;
      if ((nscid & test) > 0)
      {
        note[num++] = i + 1;
        //       Serial.println(String(_scid) + " note " + String(note[num - 1]));
      }
      //				Serial.println(String(i) + " " + String(test) + " " + String(scid & test) + " " + String(nx));
    }
    name = chordName[_scid];
    inverseID[0] = _scid;
    //  Serial.println(String(_scid) + " " + String(0) + " " + String(0) + " \"" + name);

    for (int i = 1; i < num; i++)
    {
      inverseID[i] = doinverse(i);
      //     Serial.println(String(_scid) + " inv " + String(i)+ " "+String(inverseID[i]));
      //     if (cname[inverseID[i]] == "")
      //     {
      //       cname[inverseID[i]] = name;
      //     }
    }
  }
  Chord()
  {
    num = 0;
  }
  void noteOn(byte base, byte channel, byte velocity);
  void noteOff();

  void dodisplay(byte off, byte nb, bool sel = false)
  {
    //    Serial.print(String(_scid)+" "+midiNames[nb] + " " + name + " " + String(num)+"->");
    for (int i = 0; i < num; i++)
    {
      byte n = (note[i] + nb) % 12;
      //      Serial.print(String(i)+" " + midiNames[n]);
      if (n == 1 || n == 3 || n == 6 || n == 8 || n == 10)
      {
        //					off += 5;
        display.drawBitmap(-8 + off, 50 - ypos[n] * 3 - 8, flat, 7, 13, 1);
      }
      if (sel)
        display.fillRoundRect(off, 50 - ypos[n] * 3 - 2, 6, 5, 2, 1);
      else
        display.drawRoundRect(off, 50 - ypos[n] * 3 - 2, 6, 5, 2, 1);
      if (n == 0)
        display.drawFastHLine(off - 2, 50, 10, 1);
      //        if (j == 1)
      //        {
      //          display.drawFastVLine(off + 8, 20, 24, 1);
      //       }
    }
    //    Serial.println();
  }
  String blanks = "&nb;&nb;&nb;&nb;";
  String showChord(int x, int y, int nb, bool active, bool button = false)
  {
    String nl = "";
    for (int i = 0; i < num; i++)
    {
      int no = note[i] + nb;
      nl += shownote(no % 12, x, y);
    }
    String pn = midiNamesFlat[nb] +
                name.replace(String((char)248), "&deg;").replace(String((char)171), "ø");
    if (!button)
      nl += showtext(pn, x-10, y + 15, "small");
     //  Serial.println(nl);
    return nl;
  }
  String getName( )
  {
    String pn =name.replace(String((char)248), "&deg;").replace(String((char)171), "ø");
    return pn;
  }
  int doinverse(int inverse)
  {
    int mid = 0;
    int bn = note[inverse];
    for (int c = 0; c < num; c++)
    {
      mid |= 1 << ((note[c] + 12 - bn) % 12);
    }
    mid = mid >> 1;
    //	Serial.println(midiNames[basenote]+"done " + String(inverse) + "=>" + String(mid));
    return mid;
  }
  byte note[16];
  byte num = 0;
  byte basenote = 0;
  int delta[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  byte ypos[12] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};
  String name;
  int inverseID[6];
  int _scid;
  byte invid = 0;
  short _cindex = 0;
};

#endif
