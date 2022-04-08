bool selection = false;
#include "MD_MidiFile.h"
extern MD_MIDIFile SMF;
void saveTMS(void);
String readline(File file)
{
    String ret = "";
    char ch;
    while (file.available())
    {
        ch = file.read();
        if (ch == '\n' || ch == '\r')
        {
            return ret;
        }
        ret += ch;
    }
    return ret;
}
signed char SplitS(String what, char where, String *res, signed char nmax)
{
    int n = 0;
    int p[100];
    p[0] = 0;
    what = what + String(where);
    //  DBG(what);
    for (int i = 0; i < (int)what.length(); i++)
    {
        p[n] = what.indexOf(where, i);
        if (p[n] < 0)
            break;
        i = p[n];
        n++;
    }
    // DBG(what+"?"+String(p[0])+" ++++++ "+what.substring(0, p[0]));
    res[0] = what.substring(0, p[0]);
    for (int i = 1; i < n && i < nmax; i++)
    {
        res[i] = what.substring(p[i - 1] + 1, p[i]);
        //   DBG(res[i]);
    }
    return n;
}
void loadMap(String file)
{
    if (SD.exists(file.c_str()))
    {
        lastLoadMap = file;
        File frec = SD.open(file.c_str(), FILE_READ);
        if (frec)
        {
            for (int i = 0; i < 96; i++)
            {
                replchord[i] = 0;
                transpose[i] = 0;
                keychord[i] = 0;
                maxrepl = 0;
                actkeyidx = 0;
            }
            refresh = true;
            String fn;

            actMap= readline(frec);
            progressmode = false;
            if (actMap.indexOf("Progression") > -1)
                progressmode = true;
            FDBG(actMap + " " + SB(progressmode));
            // read from the file until there's nothing else in it:
            while (frec.available())
            {
                fn = readline(frec);
                if (fn.length() < 4)
                    continue;
                String res[2];
                signed char n = SplitS(fn, '=', (String *)res, 2);
                if (n > 1)
                {
                    DBG(fn + " " + res[0] + " " + res[0].toInt());
                    int ix = res[1].indexOf("+");
                    signed char n2 = 0;
                    if (ix > -1)
                        n2 = res[1].substring(ix).toInt();
                    if (maxrepl < 96)
                    {
                        keychord[maxrepl] = res[0].toInt();
                        replchord[maxrepl] = res[1].toInt();
                        transpose[maxrepl] = n2;

                        DBG(String(maxrepl) + " " + String(keychord[maxrepl]) + "=" + String(replchord[maxrepl]) + " + " + String(n2));
                        maxrepl++;
                    }
                }
            }
            frec.close();
        }
    }
    saveTMS();
}
void print(String out, bool inv, String line)
{
    selection = inv;
#if 0
    if (selection)
    {
        display.setTextColor(0, 1);
        display.print(out.replace("Portam.","").substring(0, 5));
//        display.setTextColor(1, 0);
//        display.print(out.substring(5));
        return;
    }
    else
        display.setTextColor(1, 0);

#else
    if (selection)
    {
        int16_t x = display.getCursorX();
        int16_t y = display.getCursorY();
        //        if(y>3)
        //        display.drawRect(4, y-2, 122, 13, 1);
        //        if (x > 5)
        display.setCursor(0, y);
        display.write(221);
        display.setCursor(x, y);
    }
#endif
    display.print(out);
    //  DBG(String(line)+" "+out);
}
void setInvertMode(bool inv = false)
{

#if 1
    if (inv)
        display.setTextColor(0, 1);
    else
        display.setTextColor(1, 0);

    selection = inv;
#else
    if (selection)
    {
        int16_t x = display.getCursorX();
        int16_t y = display.getCursorY();
        if (x > 5)
            display.setCursor(x - 5, y);
        display.write(221);
    }
#endif
}
void clearToEOL()
{
    int x = display.getCursorX();
    display.fillRect(x, display.getCursorY(), 127 - x, 7, 0);
}
#include "menu.h"
void loadPRS(String file)
{
    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);

    if (SD.exists(file.c_str()))
    {
        File frec = SD.open(file.c_str(), FILE_READ);
        if (frec)
        {
            //
            FDBG("loading preset data ");
            AudioNoInterrupts();
            mPara->restore(frec);
            AudioInterrupts();
            //
            FDBG("data loaded");
            frec.close();
        }
    }
}
int scidtoix(int sc)
{
    for (int i = 0; i < 100; i++)
    {
        if (scFP[i] == sc)
        {
            return i;
        }
    }
    return 0;
}

void saveAll(File frec)
{

    frec.write(lastMap);
    frec.write(scalebase);
    frec.write((char *)&g_scid,4);
    FDBG("s scale" + SN(g_scid));
    frec.write(octave);
    frec.write(semiTone);
    frec.write(s1index);
    frec.write(s2index);
    frec.write(preindex);
    frec.write(actPre);
    frec.write(recentPre);

    MenuSet *mes = (MenuSet *)Menus[SETTINGS];
    mes->save(frec);
    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
    mPara->save(frec);
    String geqnames[100];
    for (int e = 0; e < 8; e++)
    {
        MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
        mes->save(frec);
    }
#if 1
    for (int e = 0; e < 3; e++)
    {
        MenuMidiSet *mes = (MenuMidiSet *)Menus[MIDISETTINGS + e];
        mes->save(frec);
    }

    for (int e = 0; e < 6; e++)
    {
        MenuZoneSet *mes = (MenuZoneSet *)Menus[ZONESETTINGS + e];
        mes->save(frec);
    }
#endif
    for (int i = 0; i < 5; i++)
        frec.write(transposeit[i]);
    frec.println(seqFile);
    frec.println(lastLoadMap);
    FDBG("saved: " + seqFile + "&" + lastLoadMap);
    return;
}

signed char readrec(File frec)
{
    signed char res = 0;
    if (frec.available())
        res = frec.read();
    return (res);
}
float floatrec(File frec)
{
    float res = 0;
    if (frec.available())
        frec.read(&res, 4);
    if (res == 4)
        res = 0;
    return (res);
}
int intrec(File frec)
{
    int res = 0;
    if (frec.available())
        frec.read(&res, 4);
    if (res == 4)
        res = 0;
    return (res);
}

void Restore(File frec)
{
    lastMap = readrec(frec);
    scalebase = readrec(frec);
//    lastScale = readrec(frec);
    g_scid = intrec(frec);
    octave = readrec(frec);
    semiTone = readrec(frec);
    s1index = readrec(frec);
    s2index = readrec(frec);
    preindex = readrec(frec);
    actPre = readrec(frec);
    recentPre = readrec(frec);

    //	DBG("Restored oct " + String(octave) + " semi " + String(semiTone));
    MenuSet *mes = (MenuSet *)Menus[SETTINGS];
    mes->restore(frec);
    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
    DBG("synth restore");
    mPara->restore(frec);

    for (int e = 0; e < 8; e++)
    {
        MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
        mes->restore(frec);
    }
#if 1
    for (int e = 0; e < 3; e++)
    {
        MenuMidiSet *mes = (MenuMidiSet *)Menus[MIDISETTINGS + e];
        mes->restore(frec);
    }

    for (int e = 0; e < 6; e++)
    {
        MenuZoneSet *mes = (MenuZoneSet *)Menus[ZONESETTINGS + e];
        mes->restore(frec);
    }
#endif
    nx = 1;
    FDBG("l scale" + SN(g_scid));
    g_scid = g_scid << 1;
    //			g_scid = g_scid << 1;
    for (int i = 0; i < 12; i++)
    {
        int test = 1 << i;
        if ((g_scid & test) > 0)
            ln[nx++] = i;
        //
    }
    g_scid = g_scid >> 1;
    if (g_scid != 2047)
        Menus[SCALES]->scaleit(g_scid);
    //    DBG(String(g_scid) + " " + String(lastScale)+" "+scaleNames[g_scid]);
    for (int i = 0; i < 5; i++)
        transposeit[i] = readrec(frec);
#if 1
    if (frec.available())
    {
        seqFile = readline(frec);
        lastLoadMap = readline(frec);

        int err = SMF.load(seqFile.c_str());
        if (err != MD_MIDIFile::E_OK)
        {
            Serial.print(seqFile + " SMF load Error ");
            Serial.println(err);
        }
        else
            playSeq = false;
    }
    if (frec.available())
    {
        lastLoadMap = readline(frec);
        loadMap(lastLoadMap);
    }
    return;

    FDBG("lastseq:" + seqFile);
    FDBG("last map:" + lastLoadMap);
#endif
    return;
}
void saveTMS(void)
{

    if (SD.exists("TMSidekick.ini"))
    {
        SD.remove("TMSidekick.ini");
        //    if(debug==1)DBG("ini deleted");
    }
    File frec = SD.open("TMSidekick.ini", FILE_WRITE);
    if (frec)
    {
        DBG("ini created");
        saveAll(frec);
        frec.close();
        DBG("TBSidekick saved");
    }
}
float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int imap(int x, int in_min, int in_max, int out_min, int out_max)
{
    if (x < in_min)
        x = in_min;
    if (x > in_max)
        x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void printA4(String text)
{
    alpha4.clear();
    for (int i = 0; i < 4 && i < (int)text.length(); i++)
        alpha4.writeDigitAscii(i, text.c_str()[i]);
    alpha4.writeDisplay();
}
void printA4(int intext)
{
    String text = String(intext);
    alpha4.clear();
    for (int i = 0; i < 4 && i < (int)text.length(); i++)
        alpha4.writeDigitAscii(i, text.c_str()[i]);
    alpha4.writeDisplay();
}

void writeVarLen(unsigned long value)
{
    // Start with the first 7 bit block
    unsigned long buffer = value & 0x7f;

    // Then shift in 7 bit blocks with "has-more" bit from the
    // right for as long as `value` has more bits to encode.
    while ((value >>= 7) > 0)
    {
        buffer <<= 8;
        buffer |= HAS_MORE_BYTES;
        buffer |= value & 0x7f;
    }

    // Then unshift bytes one at a time for as long as the has-more bit is high.
    //    Serial.print(midiptr);
    //    Serial.print("\t");

    while (true)
    {
        midifile[midiptr] = (byte)(buffer & 0xff);
        //       Serial.print(midifile[midiptr], HEX);
        //       Serial.print(" ");
        midiptr++;
        if (buffer & HAS_MORE_BYTES)
        {
            buffer >>= 8;
        }
        else
        {
            break;
        }
    }
    //   Serial.print(":\t");
}
union intbyte
{
    int i;
    byte b[4];
};

void setTrackLength(int length)
{
    //    Serial.print("\n");
    //    Serial.print(midiptr);
    //    Serial.print("\t");
    intbyte x;
    x.i = length + 4;
    midifile[midiptr++] = 0x00;
    midifile[midiptr++] = 0xff;
    midifile[midiptr++] = 0x2f;
    midifile[midiptr++] = 0x00;
    //    Serial.print(" 00 0f 2f 00");
    //   memcpy(midifile + 18, x.b, 4);
    Serial.print(midifile[18], HEX);
    Serial.print(midifile[19], HEX);
    Serial.print(midifile[20], HEX);
    Serial.println(midifile[21], HEX);
    Serial.print(x.b[0], HEX);
    Serial.print(x.b[1], HEX);
    Serial.print(x.b[2], HEX);
    Serial.println(x.b[3], HEX);
    midifile[18] = x.b[3];
    midifile[19] = x.b[2];
    midifile[20] = x.b[1];
    midifile[21] = x.b[0];
}
void createMidiFile()
{
    midiptr = 0;
    byte header[] = {
        0x4D, 0x54, 0x68, 0x64, // "MThd" chunk
        0x00, 0x00, 0x00, 0x06, // chunk length (from this point on): 6 bytes
        0x00, 0x00,             // format: 0
        0x00, 0x01,             // number of tracks: 1
        0x01, 0xC2              // data rate: 450 ticks per quaver/quarter note
    };
    memcpy(midifile + midiptr, header, 14);
    midiptr += 14;
    byte track[] = {
        0x4D, 0x54, 0x72, 0x6B, // "MTrk" chunk
        0x00, 0x00, 0x00, 0x00  // chunk length placeholder
    };
    memcpy(midifile + midiptr, track, 8);
    midiptr += 8;
    byte tempo[] = {
        0x00,             // time delta for the first MIDI event: zero
        0xFF, 0x51, 0x03, // MIDI event type: "tempo" instruction
        0x06, 0xDD, 0xD0  // tempo value: 450,000μs per quaver/quarter note
    };
    //       Serial.print("\n");
    //       Serial.print(midiptr);
    //      Serial.print("\t");
    //       Serial.print("00 FF 51 03 06 DD D0\n");
    memcpy(midifile + midiptr, tempo, 7);
    midiptr += 7;
}
void writeToFile(byte eventType, byte b1, byte b2, int delta)
{
    static byte laste = 0;
    writeVarLen(delta);
    if (laste != eventType)
    {
        midifile[midiptr] = eventType - 1;
        //       Serial.print(midifile[midiptr], HEX);
        midiptr++;
    }
    laste = eventType;
    midifile[midiptr] = b1;
    //   Serial.print("\t");
    //   Serial.print(midifile[midiptr], HEX);
    //   Serial.print(" ");
    midiptr++;
    midifile[midiptr] = b2;
    //   Serial.print(midifile[midiptr], HEX);
    //   Serial.print(" ");
    midiptr++;
}
