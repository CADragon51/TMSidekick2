#ifndef CHORD_H
#define CHORD_H
#include "Adafruit_SSD1306.h"
extern Adafruit_SSD1306 display;
extern String showText(String text, int x, int y, String tclass, String fill);
extern String showNote(byte n, float basex, float basey);
byte chypos[12] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};
class Chord
{
public:
  Chord(short nscid, int iscid, byte inv, int cindex)
  {
    num = 1;
    _mscid = iscid;
    _scid = nscid;
    note[0] = 0;
    invid = inv;
    for (int i = 0; i < 12; i++)
    {
      int test = 1 << i;
      if ((nscid & test) > 0)
      {
        note[num++] = i + 1;
        nn += SN(i + 1) + " ";
        //       Serial.println(String(_scid) + " note " + String(note[num - 1]));
      }
      //				Serial.println(String(i) + " " + String(test) + " " + String(scid & test) + " " + String(nx));
    }
    name = chordName[iscid];
    _cindex = cindex;
    inverseID[0] = _scid;
    //  Serial.println(String(_scid) + " " + String(0) + " " + String(0) + " \"" + name);

    for (int i = 1; i < num; i++)
    {
      inverseID[i] = doinverse(i);
      //     if (_scid == 72)
      //       Serial.println("##### " + String(_scid) + " inv " + String(i) + " " + String(inverseID[i]));
      //     if (cname[inverseID[i]] == "")
      //     {
      //       cname[inverseID[i]] = name;
      //     }
    }
    // Serial.println(String(nscid) + " " + String(iscid) + " " + String(inv) + " \"" + name + " " + String(_cindex));
    //    inverseID[0] = _scid;
  }
  Chord(short nscid, int cindex)
  {
    num = 1;
    _scid = nscid;
    _mscid = nscid;
    _cindex = cindex;
    note[0] = 0;
    for (int i = 0; i < 12; i++)
    {
      int test = 1 << i;
      if ((nscid & test) > 0)
      {
        note[num++] = i + 1;
        nn += SN(i + 1) + " ";
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
      //     if (_scid==72)
      //       Serial.println("##### "+String(_scid) + " inv " + String(i) + " " + String(inverseID[i]));
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
        display.drawBitmap(-8 + off, 50 - chypos[n] * 3 - 8, flat, 7, 13, 1);
      }
      if (sel)
        display.fillRoundRect(off, 50 - chypos[n] * 3 - 2, 6, 5, 2, 1);
      else
        display.drawRoundRect(off, 50 - chypos[n] * 3 - 2, 6, 5, 2, 1);
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
//      if(button)
//        FDBG(getName()+SN(i)+midiNamesLong[no]);
      nl += showNote(no, x, y + 6);
    }
    //   String iname = "";
    //   if(invid>0)
    //     iname = "/" + midiNamesFlat[invid] + " ";
    //   String pn = midiNamesFlat[nb] +name.replace(String((char)248), "&deg;").replace(String((char)171), "ø")+iname;
    if (!button)
      nl += showText(midiNamesFlat[nb] + getName(), x - 10, y + 21, "small", "white");
 //   Serial.println(nl);
    return nl;
  }
  String getName(int inv = 0)
  {
    String pn = name;
    pn.replace(String((char)248), "&deg;").replace(String((char)171), "ø");
    byte cninv = note[invid];
    if (inv > 0)
      cninv = chordnotes[inv] % 12;
    if (cninv > 0)
    {
      if (showflat)
        pn = pn + "/" + midiNamesFlat[cninv];
      else
        pn = pn + "/" + midiNamesSharp[cninv];
    }
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
  byte note[6];
  byte num = 0;
  byte basenote = 0;
  byte channel = 0;
  short delta[6] = {0, 0, 0, 0, 0, 0};

  String name;
  short inverseID[6];
  short _scid;
  short _mscid;
  byte invid = 0;
  short _cindex = 0;
  String nn = "";
};
EXTMEM Chord *chords[2048];
#endif
