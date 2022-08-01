bool selection = false;
#include "MD_MidiFile.h"
#include <Regexp.h>
extern MD_MIDIFile SMF;
void saveTMS(void);
String cdstack[20];
int cdptr = 0;
extern int freeram();
void push(String cd)
{
 //   FDBG("push "+cd);
    if (cd.indexOf("*") >0)
        return;
    if (cdptr < 20 && cd != "..")
        cdstack[cdptr++] = cd.replace("//", "/");
    //   FDBG("stack " + SN(cdptr - 1) + " " + cdstack[cdptr - 1]);
}
String pop(void)
{
    if (cdptr > 1)
        cdptr--;
 //   if (cdstack[cdptr - 1] == "..")
 //       FDBG("stack " + SN(cdptr - 1) + " " + cdstack[cdptr - 1]);
    return cdstack[cdptr - 1];
}
String peek(void)
{
    return cdstack[cdptr - 1];
}
void clearStack(void)
{
    cdptr = 0;
    push("/");
    int ix = 0;
    if (currentDirectory.substring(0, 1) != "/")
        currentDirectory = "/" + currentDirectory;
    //       FDBG(currentDirectory + " @ " + SN(n));
    while ((ix = currentDirectory.indexOf("/", ix + 1)) > -1)
    {
        push(currentDirectory.substring(0, ix));
 //       FDBG(peek());
    }
    push(currentDirectory);
//    FDBG(peek());
}
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

byte getVoice(byte note)
{
    //   FDBG("note " + SN(note) + " " + voices[note]);
    if (voices[note] == 255)
    {
        voices[note] = lastvoice++;
        if (lastvoice >= MAXVOI)
            lastvoice = 128;
        minstr[voices[note]] = note;
    }
    //   FDBG(SN(voices[note]) + " " + SN(minstr[voices[note]]));
    return voices[note];
}
int SplitS(String what, char where, String *res, int nmax)
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
    // FDBG(what+"?"+String(p[0])+" ++++++ "+what.substring(0, p[0]));
    res[0] = what.substring(0, p[0]);
    for (int i = 1; i < n && i < nmax; i++)
    {
        res[i] = what.substring(p[i - 1] + 1, p[i]);
        // FDBG(res[i]);
    }
    return n;
}
int SplitU(String what, char where, String *res, int nmax)
{
    int n = 0;
    int p[100];
    p[0] = 0;
    what = what + String(where);
    //  DBG(what);
    for (int i = 0; i < (int)what.length(); i++)
    {
        int s = what.substring(i, i + 1) == "\"" ? i : -1;
        //      FDBG(s);
        if (s > -1)
        {
            int e = what.indexOf('\"', s + 1);
            if (e > -1)
                i = e;
            //           FDBG(i);
            continue;
        }
        p[n] = what.indexOf(where, i);
        if (p[n] < 0)
            break;
        i = p[n];
        n++;
    }
    // FDBG(what+"?"+String(p[0])+" ++++++ "+what.substring(0, p[0]));
    res[0] = what.substring(0, p[0]);
    int m = 1;
    for (int i = 1; i < n && i < nmax; i++)
    {
        String r = what.substring(p[i - 1] + 1, p[i]);
        if (r.length() > 0)
        {
            res[m++] = r;
            //            FDBG(r);
        } //
    }
    return m;
}
void loadMap(String file)
{
    //    FDBG("file >" + file.replace(" ", "") + "<");
    STACK;
    lastLoadMap = file;

    File frec = SD.open(file.c_str(), FILE_READ);
    if (frec)
    {
        STACK;
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

        actMap = readline(frec);
        progressmode = false;
        if (actMap.indexOf("Progression") > -1)
            progressmode = true;
        //            FDBG(actMap + " " + SB(progressmode));
        // read from the file until there's nothing else in it:
        STACK;
        while (frec.available())
        {
            fn = readline(frec);
            if (fn.length() < 4)
                continue;
            String res[2];
            signed char n = SplitS(fn, '=', (String *)res, 2);
            if (n > 1)
            {
                //               FDBG(fn + " " + res[0] + " " + res[1]);
                int ix = res[1].indexOf("+");
                short n2 = 0;
                //               FDBG(SN(ix));
                if (ix > -1)
                {
                    String nx = res[1].substring(ix).replace("+-", "-");
                    n2 = nx.toInt();
                    //                  FDBG(SN(n2) + " " + res[1].substring(ix) + " < " + nx);
                }
                if (maxrepl < 96)
                {
                    keychord[maxrepl] = res[0].toInt();
                    replchord[maxrepl] = res[1].toInt();
                    STACK;
                    transpose[maxrepl] = n2;

                    //                   FDBG(String(maxrepl) + " " + String(keychord[maxrepl]) + "=" + String(replchord[maxrepl]) + " + " + String(transpose[maxrepl]) + " >" + SN(n2));
                    maxrepl++;
                }
            }
        }
        frec.close();
    }
    //    saveTMS();
}

String saveDrum(String file, bool show = false)
{
    File frec;
    String ret = "";
    char sep = ',';
    if (!show)
    {
        FDBG("saving drums to " + file + ".drm");
        String pname = file + ".drm";
        String bname = "bak_" + file + ".drm";
        if (SD.exists(pname.c_str()))
        {
            SD.remove(String("bak_").c_str());
            SD.rename(pname.c_str(), bname.c_str());
            SD.remove(pname.c_str());
        }
        frec = SD.open(pname.c_str(), FILE_WRITE);
    }

    //   FDBG("showing drums" + file + ".drm");
    String line = "";
    for (int v = 0; v < MAXVOI; v++)
    {
        if (minstr[v] == 0)
            continue;
        line = SN(minstr[v]);
        for (int p = 0; p < maxticks * MAXPAT; p++)
        {
            if (seqpattern[p][v] != -1)
                line += SN(p) + SN(delaypattern[p]) + SN(seqpattern[p][v]);
        }
        //            FDBG(line);

        if (line.length() > 10)
        {
            if (show)
                ret += line + "~";
            else
                frec.println(line);
        }
    }
    line = "K ";
    for (int i = 0; i < 128; i++)
    {
        //          FDBG(SN(triggerNote[i]) + SN(i)+SN(acttrigger[i]));
        if (triggerNote[i] > 0)
            line += SN(triggerNote[i]) + SN(i);
    }
    if (show)
        ret += line + "~";
    else
        frec.println(line);
    //        FDBG(keys);
    line = "B ";
    for (int i = 0; i < 6; i++)
    {
        //          FDBG(SN(triggerNote[i]) + SN(i)+SN(acttrigger[i]));
        line += coloring[i] + " ";
    }
    if (show)
        ret += line + "~";
    else
        frec.println(line);

    line = "C ";
    for (int p = 0; p < maxticks * MAXPAT; p++)
    {
        if (ccpattern[p] != 0 && ccpattern[p] < 128)
            line += SN(p) + SN(ccpattern[p]) + SN(ccval[p]);
    }
    //            FDBG(line);
    if (show)
        ret += line + "~";
    else
        frec.println(line);

    line = "P ";
    int bc = 0;
    for (int p = 0; p < MAXPAT; p++)
    {
        if (beatCount[p] <= bc)
        {
            bc = beatCount[p];
            continue;
        }
        line += String(p) + " " + String(beatCount[p]) + " ";
        bc = beatCount[p];
        //           keyl += String(triggerNote[p]) + " ";
    }
    if (show)
        ret += line + "~";
    else
        frec.println(line);
    line = "F ";
    for (int p = 0; p < MAXPAT; p++)
    {
        if (patfiles[p].length() > 0)
            line += String(p) + sep + patfiles[p] + sep;
        //               FDBG(SN(p)+line);
    }
    if (show)
        ret += line + "~";
    else
        frec.println(line);

    if (!show)
        frec.close();
    //   FDBG(ret);
    return ret;
}
EXTMEM String res[256];
EXTMEM String dirres[256];
EXTMEM char resbuf[256][1000];
EXTMEM char dirresbuf[256][1000];
EXTMEM short ires[2560];
int voice = 0;
void cleanpat(int l)
{
    int from = actpattern;
    int to = from + l;
    for (int p = from; p < to; p++)
    {
        ////        FDBG(SN(p) + SN(beatCount[p]));
        for (int s = p * maxticks; s < (p + 1) * maxticks; s++)
        {
            delaypattern[s] = 0;
            for (int v = 0; v < MAXVOI; v++)
            {
                seqpattern[s][v] = -1;
            }
        }
        beatCount[p] = 0;
        patfiles[p] = "";
        //           acttrigger[p] = 255;
    }
    //   FDBG(beatCount[to]);
    FDBG("cleaned from " + SN(from) + " to " + SN(to));
}
String cline = "";
void file2Drum(int cc)
{
    short note = ires[0];
    //   FDBG(__CALLER__);
    int v = 0;
    //    FDBG("pos " + SN(ires[1]));
    v = getVoice(note);
    if (v >= MAXVOI)
        return;
    //   int pos = actpattern + ires[1] / maxticks;
    //   int pos = ires[1] / maxticks;
    //   FDBG(SN(v) + SN(grp) + SN(pos) + " from " + SN(patvoicelow[pos][grp]) + " to " + SN(patvoicehigh[pos][grp]));

    for (int p = 1; p < cc - 1; p += 3)
    {
        short ipos = ires[p];
        short idel = ires[p + 1];
        short vel = ires[p + 2];
        short as = actpattern * maxticks + ipos;
        //       int pos = as / maxticks;
        seqpattern[as][v] = vel;
        delaypattern[as] = idel;
        //        Serial.println(SN(v) + " " + SN(note) + " " + SN(as) + " " + SN(seqpattern[as][v]) + " ");
        if (mvelo[v] < vel)
            mvelo[v] = vel;
        int pos = ires[p] / maxticks;
        if (patvoicelow[pos] > v)
            patvoicelow[pos] = v;
        if (patvoicehigh[pos] < v)
            patvoicehigh[pos] = v;
        //       FDBG(SN(v) + SN(grp) + SN(pos) + " from " + SN(patvoicelow[pos][grp]) + " to " + SN(patvoicehigh[pos][grp]));
    }

    //   Serial.println();
}

void file2File(int cl)
{
    cl = SplitS(cline, ',', res, 255);
    //   FDBG(cline + " " + SN(cl));
    //  lastColor = 0;
    for (int p = 0; p < cl - 2; p += 2)
    {
        short pos = res[p].toInt();
        patfiles[pos] = res[p + 1];
        int ix = res[p + 1].lastIndexOf(".mid ");
        if (ix > -1)
        {
            int np = patfiles[pos].substring(ix + 4).toInt();
            // patcolor[pos] = cyan[c++];
            // FDBG(SN(p) + SN(pos) + patfiles[pos] + SN(np));
            for (int pp = pos; pp < pos + np; pp++)
                patcolors[pp] = coloring[lastColor % 4];
            // c++;
            //          FDBG(SN(np) + SN(p) + SN(pos) + patcolors[pos]);
            //      FDBG("LC " + SN(lastColor));
            lastColor++;
        }
    }
    //   Serial.println();
    return;
}
void file2CC(int cl)
{
    int p = 0;
    while (p < cl)
    {
        short pos = ires[p++];
        short cc = ires[p++];
        short val = ires[p++];
        short as = actpattern * maxticks + pos;
        ccpattern[as] = cc;
        ccval[as] = val;
        //       Serial.println(SN(p)+"CC " + SN(pos) + SN(cc) + SN(val));
        //       Serial.println("CC " + SN(as) + SN(ccpattern[as]) + SN(ccval[as]));
    }
    //   Serial.println();
    return;
}
void file2Color(int cl)
{
    int p = 0;
    cl = SplitS(cline, ' ', res, 255);

    while (p < cl && p < 6)
    {
        coloring[p] = res[p];
        //       FDBG("color " + SN(p) + coloring[p]);
        p++;
        //       Serial.println("CC " + SN(as) + SN(ccpattern[as]) + SN(ccval[as]));
    }
    //   Serial.println();
    return;
}
void file2Key(int cl)
{
    int p = 0;
    while (p < cl)
    {
        short act = ires[p++];
        short note = ires[p++];

        triggerNote[note] = act;
        acttrigger[act - 1] = note;
        //        FDBG("key " + SN(act - 1) + SN(note));
        //       Serial.println("CC " + SN(as) + SN(ccpattern[as]) + SN(ccval[as]));
    }
    //   Serial.println();
    return;
}
void file2Pat(int cl)
{
    int p = 0;
    while (p < cl)
    {
        short sp = ires[p++];
        short bc = ires[p++];
        //        FDBG(SN(p) + SN(bc));
        while (bc > 0)
        {
            beatCount[sp++] = bc--;
        }
    }
}
int midi2Line(File frec)
{

    int cc = 0;
    char lc = 0;
    char c = frec.read();
    while (frec.available() && c >= 32)
    {
        if (c != 32 || lc != c)
            cline += c;
        lc = c;
        c = frec.read();
    }
    // cline.replace("_073", "_07 3");
    //   FDBG(cline);
    cc = 0;
    for (unsigned int i = 0; i < cline.length(); i++)
    {
        ires[cc++] = cline.substring(i, i + 6).toInt();
        //      FDBG(SN(cc-1) + SN(ires[cc-1]));
        for (i++; i < cline.length(); i++)
        {
            if (cline.substring(i, i + 1) == " ")
                break;
        }
    }
    return cc;
}
void loadDrum(String file)
{

    FDBG("loading " + file);
    voice = -1;
    if (SD.exists(file.c_str()))
    {
        File frec = SD.open(file.c_str(), FILE_READ);
        //       int rea = frec.size();
        //       frec.close();
        int cc = 0;
        ires[cc] = 0;
        //        midifile[rea++] = 10;
        FSTACK;
        //       FDBG(rea);
        while (frec.available())
        {
            char c = (char)frec.read();
            if (c == -1)
                break;
            if (c < '0')
                continue;
            //           Serial.print(c, HEX);
            if (c > '@')
            {
                frec.read();
                cline = "";
            }
            else
                cline = c;
            cc = midi2Line(frec);
            if (c == 'K')
                file2Key(cc);
            if (c == 'B')
                file2Color(cc);
            if (c == 'P')
            {
                //               FDBG(cline + " " + SN(cc));
                file2Pat(cc);
            }
            else if (c == 'F')
                file2File(cc);
            else if (c == 'C')
                file2CC(cc);
            else if (cc > 2)
                file2Drum(cc);
        }
        frec.close();
    }

    //   for (grp = 0; grp < 2;grp++)
    //       for (int pa = 0;  pa < MAXPAT; pa+=4)
    //          FDBG(SN(grp) + SN(pa) + " from " + SN(patvoicelow[pa][grp]) + " to " + SN(patvoicehigh[pa][grp]));
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

long readlong(byte *fp)
{
    unsigned char c[4];

    memcpy((char *)c, fp, 4);
    return (long)((c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3]);
}

/*  READSHORT  --  Read short from a file (byte-order independent)  */

short readshort(byte *fp)
{
    unsigned char c[2];

    memcpy((char *)c, fp, 2);
    return (short)((c[0] << 8) | c[1]);
}

/*  READVARLEN	--  Parse variable length value from MIDI file	*/

long readVarLen(byte *fp)
{
    long value;
    int ch;

    if ((value = *fp) & 0x80)
    {
        value &= 0x7F;
        do
        {
            value = (value << 7) | ((ch = *fp++) & 0x7F);
        } while (ch & 0x80);
    }
    return value;
}

/*  High level input functions.  */

/*  READMIDIFILEHEADER	--  Read file header structure.  */
/*  MIDI command codes  */

typedef enum
{

    /* Channel voice messages */

    NoteOff = 0x80,
    NoteOn = 0x90,
    PolyphonicKeyPressure = 0xA0,
    ControlChange = 0xB0,
    ProgramChange = 0xC0,
    ChannelPressure = 0xD0,
    PitchBend = 0xE0,

    /* Channel mode messages */

    ChannelMode = 0xB8,

    /* System messages */

    SystemExclusive = 0xF0,
    SystemCommon = 0xF0,
    SystemExclusivePacket = 0xF7,
    SystemRealTime = 0xF8,
    SystemStartCurrentSequence = 0xFA,
    SystemContinueCurrentSequence = 0xFB,
    SystemStop = 0xFC,

    /* MIDI file-only messages */

    FileMetaEvent = 0xFF
} midi_command;

/*  MIDI file meta-event codes  */

typedef enum
{
    SequenceNumberMetaEvent = 0,
    TextMetaEvent = 1,
    CopyrightMetaEvent = 2,
    TrackTitleMetaEvent = 3,
    TrackInstrumentNameMetaEvent = 4,
    LyricMetaEvent = 5,
    MarkerMetaEvent = 6,
    CuePointMetaEvent = 7,

    ChannelPrefixMetaEvent = 0x20,
    PortMetaEvent = 0x21,
    EndTrackMetaEvent = 0x2F,

    SetTempoMetaEvent = 0x51,
    SMPTEOffsetMetaEvent = 0x54,
    TimeSignatureMetaEvent = 0x58,
    KeySignatureMetaEvent = 0x59,

    SequencerSpecificMetaEvent = 0x7F
} midifile_meta_event;

struct mhead
{
    char chunktype[4]; /* Chunk type: "MThd" */
    long length;       /* Length: 6 */
    short format;      /* File format */
    short ntrks;       /* Number of tracks in file */
    short division;    /* Time division */
};

/*  MIDI track header  */

#define MIDI_Track_Sentinel "MTrk"

struct mtrack
{
    char chunktype[4]; /* Chunk type: "MTrk" */
    long length;       /* Length of track */
};

byte *readMidiFileHeader(byte *fp, struct mhead *h)
{
    memcpy((char *)h->chunktype, fp, sizeof h->chunktype);
    fp += sizeof h->chunktype;
    h->length = readlong(fp);
    fp += 4;
    h->format = readshort(fp);
    fp += 2;
    h->ntrks = readshort(fp);
    fp += 2;
    h->division = readshort(fp);
    fp += 2;
    return fp;
}

/*  READMIDITRACKHEADER  --  Read track header structure.  */

byte *readMidiTrackHeader(byte *fp, struct mtrack *t)
{
    memcpy((char *)t->chunktype, fp, sizeof t->chunktype);
    fp += sizeof t->chunktype;
    t->length = readlong(fp);
    fp += 4;
    return fp;
}
long vlength(byte *trk, int &x)
{
    long value;
    byte ch;
    if ((value = trk[x++]) & 0x80)
    {
        value &= 0x7F;
        //       FDBG(" x " + SN(x) + " " + SN(value));
        do
        {
            ch = trk[x];
            value = (value << 7) | (ch & 0x7F);
            //           FDBG(" x " + SN(x) + " " + SN(ch));
            x++;
        } while (ch & 0x80);
    }
    return value;
}

extern void update_pat(bool);
extern String lastMidiFile;
extern int lastbc;
byte posplus[MAXVOI];
extern String showLine(float x1, float y1, float x2, float y2, int width);
extern int sizeMIDI(String file, int &v);
extern void playTime(void);
void previewMIDI(String pfile)
{
    FDBG("preview " + pfile);
    pfile.replace(" /", "/");
    int nx = pfile.indexOf(".mid|");
    String file = pfile;
    if (nx > -1)
        file = pfile.substring(0, nx + 4);
    FDBG("Loading " + file+" "+SB(SD.exists(file.c_str())));
    int err = SMF.load(file.c_str());
    if (err != MD_MIDIFile::E_OK)
    {
        FDBG("SMF load Error " + SN(err));

        return;
    }
    //   FDBG("Loaded " + file + " " + SN(SMF.getTicksPerQuarterNote())+" "+lastMidiFile);
    String header = " <svg height=\"200\" width=\"1400\">";
    String svgend = "</svg>";
    String nl = header;
    //	nl += showLine(0, 50, 1220, 50);
    float delta = 1220 / lastbc;
    int inc = 1;
    if (delta < 10)
        inc = 4;
    else if (delta < 20)
        inc = 2;
    for (int i = 0; i < lastbc + inc; i += inc)
    {
        nl += showLine(i * delta, 30, i * delta, 60,1);
        nl += showText(String(i + 1), i * delta, 20,"","#ffffff");
    }
    nl += svgend;
    webgui.setMonitor(beatnrid, nl);
    playSeq = false;
    //    FSTACK;
    lasttick = millis();
    SMF.restart();
    //    FSTACK;
    SMF.looping(true);
    transport = REPEAT;
 //   playTimer.begin(playTime, 1000);
    lastMidiFile = pfile;
    // Serial.print("freeram = ");
    // Serial.println(freeram());
    //    FSTACK;
}
void loadMIDI(String pfile, int np, int v)
{
//    FDBG(pfile);
    // Serial.print("freeram = ");
    // Serial.println(freeram());
    String file = pfile;
    struct mhead mh;
    byte *fp = midifile;
    cleanpat(np);
    patfiles[actpattern] = pfile + " " + SN(np) + "|" + SN(v);
    String pcol = coloring[lastColor % 4];
    //    FDBG(patfiles[actpattern] + " LC " + SN(actpattern) + SN(lastColor) + SN(pcol));
    lastColor++;
    for (int a = actpattern, b = np; a < actpattern + np; a++, b--)
    {
        beatCount[a] = b;
        patcolors[a] = pcol;
        //       FDBG(SN(a) + " PC " + SN(beatCount[a]) + patcolors[a]);
    }
    //   FDBG(actpattern + np);
    fp = readMidiFileHeader(fp, &mh);
    struct mtrack mt;
    fp = readMidiTrackHeader(fp, &mt);
    if (mh.ntrks > 1)
    {
        fp = readMidiTrackHeader(fp + mt.length, &mt);
    }
    FSTACK;
    int type;
    byte event = 0;
    int tempo = SMF.getTicksPerQuarterNote() / 12;
    float tf = tempo / 8;
    byte note, vel;
    int i = fp - midifile;
    long abstime = 0; /* Absolute time in track */
                      //   FDBG(pfile + " Loading " + file + " " + SN(np) + SN(tempo) + SN(tf) + SN(i));

    while (i < mt.length + fp - midifile)
    {
        long tlapse = vlength(midifile, i);
        abstime += tlapse;
        if (midifile[i] >= 0x80)
            event = midifile[i++];
        if (event == FileMetaEvent)
        {
            type = midifile[i++];
            int len = vlength(midifile, i);
            if (type == EndTrackMetaEvent) // FF 2F 00 End of Track
                break;
            i += len;
            continue;
        }
        event &= 0xf0;
        if (event == PolyphonicKeyPressure) /* Aftertouch */
            i += 2;
        if (event == ControlChange) /* Control change */
            i += 2;
        if (event == ProgramChange) /* Program change */
            i++;
        if (event == ChannelPressure) /* Channel pressure (aftertouch) */
            i++;
        if (event == PitchBend) /* Pitch bend */
            i += 2;
        if (event == SystemExclusive || event == SystemExclusivePacket)
        {
            int len = vlength(midifile, i);
            i += len;
        }
        if (event == NoteOn || event == NoteOff)
        {
            note = midifile[i++];
            vel = midifile[i++];
            if (event == NoteOff)
                vel = 0;
            int pos = abstime / tempo;
            short as = actpattern * maxticks + pos;
            int v = getVoice(note);
            //           FDBG("seq " + SN(pos) + SN(v) + SN(abstime) + SN(vel));
            if (v >= MAXVOI)
                continue;
            int lim = MAXPAT * maxticks;
            if (as >= lim)
                break;
            int pp = as / maxticks;
            if (patvoicelow[pp] > v)
                patvoicelow[pp] = v;
            if (patvoicehigh[pp] < v)
                patvoicehigh[pp] = v;
            if (mvelo[v] < vel)
                mvelo[v] = vel;
            //           FDBG("seq " + SN(pos) + SN(pp) + SN(as) + SN(v) + SN(vel) + SN(patvoicelow[pp]) + SN(patvoicehigh[pp]));
            seqpattern[as][v] = vel;
            int del = (abstime - pos * tempo) / tf;
            if (del > delaypattern[as])
                delaypattern[as] = del;
        }
        else
            Serial.println(event, HEX);
    }
    SMF.close();
    update_pat(true);
    FSTACK;
    // Serial.print("freeram = ");
    // Serial.println(freeram());
}
int sizeMIDI(String file, int &v)
{
    v = 0;
    byte lv[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool ismid = SD.exists(file.c_str());
    // FDBG(file+" "+SB(ismid));
    // Serial.print(__CALLER__);
    // Serial.print(" freeram = ");
    // Serial.println(freeram());
    if (ismid)
    {
        struct mhead mh;
        //        long track1;

        File frec = SD.open(file.c_str(), FILE_READ);
        frec.readBytes((char *)midifile, frec.size());
        byte *fp = midifile;
        //       Serial.print(frec.size());
        frec.close();
        fp = readMidiFileHeader(fp, &mh);
        struct mtrack mt;
        fp = readMidiTrackHeader(fp, &mt);
        if (mh.ntrks > 1)
        {
            fp = readMidiTrackHeader(fp + mt.length, &mt);
        }
        int type;
        byte event = 0;
        float tempo = mh.division / 12;
#if 0
    
        uint16_t beatTime;
        int err = SMF.load(file.c_str());
        if (err != MD_MIDIFile::E_OK)
        {
            FDBG("SMF load Error " + SN(err));

            return 0;
        }
        beatTime = 60000 / SMF.getTempo();
        tempo = (tempo*120)/beatTime;
        #endif 
        int maxpc = 0;
        byte note;
        // check actual length
        int s = fp - midifile;
        int e = mt.length + fp - midifile;
        long abstime = 0; /* Absolute time in track */
        int i = s;
#if 0
        Serial.print(" tlen ");
        Serial.print(mt.length);
        Serial.print(" nt ");
        Serial.print(mh.ntrks);
        Serial.print(" ");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(e);
#endif
        while (i < e)
        {
            //            Serial.println(i);
            //            Serial.print(SN(i)+"->");
            long tlapse = vlength(midifile, i);
            abstime += tlapse;
            //            Serial.println(SN(i) + SN(abstime) + SN(midifile[i])+ SN(midifile[i+1]));

            if (midifile[i] >= 0x80)
                event = midifile[i++];
            if (event == FileMetaEvent)
            {
                type = midifile[i++];
                int len = vlength(midifile, i);
                if (type == EndTrackMetaEvent) // FF 2F 00 End of Track
                    break;
                i += len;
                //                Serial.println(" -> "+SN(i));
                continue;
            }
            event &= 0xf0;
            //            Serial.println(" ");
            if (event == PolyphonicKeyPressure) /* Aftertouch */
                i += 2;
            if (event == ControlChange) /* Control change */
                i += 2;
            if (event == ProgramChange) /* Program change */
                i++;
            if (event == ChannelPressure) /* Channel pressure (aftertouch) */
                i++;
            if (event == PitchBend) /* Pitch bend */
                i += 2;
            if (event == SystemExclusive || event == SystemExclusivePacket)
            {
                int len = vlength(midifile, i);
                i += len;
            }
            if (event == NoteOn || event == NoteOff)
            {
                note = midifile[i++];
                i++;
 //               int pos = int(abstime / tempo + 0.5);
                if (maxpc < abstime)
                    maxpc = abstime;
                //           FDBG(SN(pos)+SN(note)+SN(v)+SN(maxpc));
                for (int r = 0; r < 32; r++)
                {
                    if (lv[r] == note)
                        break;
                    if (lv[r] == 0)
                    {
                        v++;
                        lv[r] = note;
                        break;
                    }
                }
            }
            else
                Serial.println(event, HEX);
        }
        float np = float(maxpc/tempo) / 12.0 + 0.5;
        Serial.println("maxpc " + SN(maxpc));
        lastbc = maxpc / mh.division + 1;
        if (lastbc == 0)
            lastbc = 1;
        //       FDBG(SN(v) + SN(bcc));
        //       FSTACK;
        // Serial.print("freeram = ");
        // Serial.println(freeram());
        lastbc /= 4;
        return np;
    }

    return 0;
}

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
    frec.write((char *)&g_scid, 4);
    frec.write(octave);
    frec.write(semiTone);
    frec.write(sindex[0]);
    frec.write(sindex[1]);
    frec.write(preindex);
    frec.write(lastcc);
    frec.write(lastccval);

    MenuSet *mes = (MenuSet *)Menus[SETTINGS];
    mes->save(frec);
    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
    mPara->save(frec);
    for (int e = 0; e < 8; e++)
    {
        MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
        mes->save(frec);
    }

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

    for (int i = 0; i < 21; i++)
        frec.write(transposeit[i]);
    //    FDBG("pattern " + SN(pattern) + " " + SN(mclick1) + " " + SN(mclick2));
    frec.write((char *)&actpattern, 4);
    frec.write(beatlength);
    STACK;
    // for (int i = 0; i < 128; i++)
    // {
    //     frec.write(triggerNote[i]);
    // }
    //    FDBG("trigger notes");
    STACK;
    frec.println(history);
    STACK;
    FDBG("saving: " + lastLoadMap);

    frec.println(lastLoadMap);
    frec.println(currentDirectory);
    frec.println(wavenames[0]);
    frec.println(wavenames[1]);
    frec.close();
    FDBG("saved: " + currentDirectory);
    saveDrum("TMS");
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
    return (res);
}
int intrec(File frec)
{
    int res = 0;
    if (frec.available())
        frec.read(&res, 4);
    return (res);
}
extern int pwx;
void Restore(File frec)
{

    FDBG("Restoring TBSidekick " + SN((int)frec.size()));
    actpattern = 0;
    STACK;
    lastMap = readrec(frec);
    scalebase = readrec(frec);
    //    lastScale = readrec(frec);
    g_scid = intrec(frec);
    octave = readrec(frec);
    semiTone = readrec(frec);
    sindex[0] = readrec(frec);
    sindex[1] = readrec(frec);
    preindex = readrec(frec);
    lastcc = 0 * readrec(frec);
    lastccval = 0 * readrec(frec);
    //	DBG("Restored oct " + String(octave) + " semi " + String(semiTone));

    MenuSet *mes = (MenuSet *)Menus[SETTINGS];
    mes->restore(frec);

    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
    DBG("synth restore");
    mPara->restore(frec);
    //    FDBG(__CALLER__);
    for (int e = 0; e < 8; e++)
    {
        MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
        mes->restore(frec);
    }
    //   FDBG(__CALLER__);

    for (int e = 0; e < 3; e++)
    {
        MenuMidiSet *mes = (MenuMidiSet *)Menus[MIDISETTINGS + e];
        mes->restore(frec);
    }
    //   FDBG(__CALLER__);

    for (int e = 0; e < 6; e++)
    {
        MenuZoneSet *mes = (MenuZoneSet *)Menus[ZONESETTINGS + e];
        mes->restore(frec);
    }
    //   FDBG(__CALLER__);

    g_nx = 1;
    //    FDBG("l scale" + SN(g_scid));
    g_scid = g_scid << 1;
    //			g_scid = g_scid << 1;
    for (int i = 0; i < 12; i++)
    {
        int test = 1 << i;
        if ((g_scid & test) > 0)
            g_ln[g_nx++] = i;
        //
    }
    g_scid = g_scid >> 1;
    FSTACK;
    if (g_scid != 2047)
        Menus[SCALES]->scaleit(g_scid);
    //    DBG(String(g_scid) + " " + String(lastScale)+" "+scaleNames[g_scid]);
    for (int i = 0; i < 21; i++)
    {
        transposeit[i] = readrec(frec);
        //        FDBG("trans " + SN(i) + SN(transposeit[i]));
    }
    FSTACK;
    actpattern = intrec(frec);
    beatlength = readrec(frec);
    beatlength = 4;
    if (actpattern < 0 || actpattern > MAXPAT)
        actpattern = 0;

    // for (int i = 0; i < 128; i++)
    // {
    //     byte t = readrec(frec);
    //     triggerNote[i] = t;
    //     acttrigger[t % MAXPAT][t / MAXPAT] = i;
    //     //     Serial.print(SN(triggerNote[i]) + " "+SN(triggerNote[i+MAXPAT])+" ");
    // }
    //   FDBG("trigger notes");
    //   FDBG("pattern " + SN(pattern) + " " + SN(mclick1) + " " + SN(mclick2));

    if (frec.available())
    {
        String ll = frec.readString(300);
        //    String res[3];
        ll.replace("\r", "");
        SplitS(ll, '\n', res, 5);
    }
    history = res[0];
    FDBG("hist " + history);
    lastLoadMap = res[1];
    previousDir = currentDirectory;
    currentDirectory = res[2].length() > 0 ? res[2] : "/";
    currentDirectory.replace("//", "/");
    currentDirectory.replace("//", "/");
    FDBG("cd " + currentDirectory);
    clearStack();
    wavenames[0]=res[3];
    wavenames[1]=res[4];
    frec.close();
    FSTACK;
    return;
}
void saveTMS(void)
{

    if (SD.exists("settings.tms"))
    {
        SD.remove("settings.bak");
        SD.rename("settings.tms", "settings.bak");
        //    if(debug==1)DBG("ini deleted");
    }
    STACK;
    File frec = SD.open("settings.tms", FILE_WRITE);
    if (frec)
    {
        //        FDBG("ini created");
        saveAll(frec);
        FDBG("TBSidekick saved" + SN((int)frec.size()));
        frec.close();
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
    x.i = length - 4;
    midifile[midiptr++] = 0x00;
    midifile[midiptr++] = 0xff;
    midifile[midiptr++] = 0x2f;
    midifile[midiptr++] = 0x00;
    //    Serial.print(" 00 0f 2f 00");
    //   memcpy(midifile + 18, x.b, 4);
    Serial.println(x.i, HEX);
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
    // FDBG(SN(laste) + " " + SN(eventType));
    if (laste != eventType)
    {
        midifile[midiptr] = eventType;
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
extern void showStatus(int pos);

void playPattern(void)
{
    //  static long last = millis();
    //   FDBG(millis() - last);
    //  last = millis();
    int inc = 1;
    if (metisback)
        inc = -1;
    //    startpattern = -1;
    patternc += inc;
    int bl = (actpattern + beatCount[actpattern]) * maxticks;

    if (editMode)
        bl = (actpattern + 1) * maxticks;
    bool beatend = patternc >= bl;
    if (metisback)
        beatend = patternc < actpattern * maxticks;
    if (beatend || (((mettrigger != oldtrigger && triggerNote[mettrigger] > 0) || newpatternc > -1) && patternc % (Menu::ratdiv * 12) == 0))
    {
        oldtrigger = mettrigger;
        //       FDBG("bl " + SN(bl) + " " + SN(patternc) + " " + SN(beatCount[actpattern]) + SB(beatend));
        if (!editMode)
        {
            //            FDBG("met " + SN(mettrigger) + SN(triggerNote[mettrigger]));
            digitalWrite(13, HIGH);
            //       FDBG(midiNamesLong[mettrigger] + " " + SN(triggerNote[mettrigger]));
            if (mettrigger && triggerNote[mettrigger] > 0)
            {
                actpattern = (triggerNote[mettrigger]) - 1;
                sa = (actpattern / 64) * 64;
                //               oldtrigger = mettrigger;
                //            FDBG(SN(mettrigger) + " " + SN(beatCount[actpattern] * beatlength * maxticks) + " " + SN(actpattern));
            }
            patternc = actpattern * maxticks;
            if (newpatternc > -1)
            {
                patternc = newpatternc;
                actpattern = patternc / maxticks;
                newpatternc = -1;
            }
            //           FDBG("patternc " + SN(actpattern));
            if (metisback)
            {
                patternc = maxticks * (actpattern + beatCount[actpattern] * beatlength) - 1;
                //               FDBG(SN(mettrigger) + " " + SN(beatCount[actpattern] * beatlength * maxticks) + " " + SN(actpattern));
            }
            bl = patternc + beatCount[actpattern] * maxticks;
        }
        else
        {
            patternc = (actpattern)*maxticks;
            if (metisback)
                patternc = (1 + actpattern) * maxticks;
        } //       FDBG(SN(patternc) + " " + SN(bl)+" "+SN(actpattern));
    }

    if (patternc % (beatlength * maxticks) == 0)
    {
        digitalWrite(13, HIGH);
    }
    else if (patternc % 12 == 0)
        digitalWrite(13, LOW);

    patcnt = patternc / maxticks;
    delay(630.0 * delaypattern[patternc] / Menu ::BPM);

    if (ccpattern[patternc] > 0)
    {
        byte cc = ccpattern[patternc];
        byte val = ccval[patternc];
        usbMIDI.sendControlChange(cc, val, 10, cable);
        MIDI1.sendControlChange(cc, val, 10);
        MIDI2.sendControlChange(cc, val, 10);
        if (transport == RECORDING && menuState == SETTINGS)
        {
            if (lastEvent == 0)
                MidiEvent::starttime = 0;
            sequences[lastEvent++].init(0xB0, cc, val, 10);
        }
    }
    int aa = patcnt;
    // FDBG(SN(aa) + SN(patvoicelow[aa]) + " to " + SN(patvoicehigh[aa]));
    for (int v = patvoicelow[aa]; v <patvoicehigh[aa] && v < MAXVOI; v++)
    //   for (int v = 0; v < MAXVOI; v++)
    {
        short ptest = seqpattern[patternc][v];
        //       FDBG("voice " + SN(v) + SN(ptest));
        if (ptest == -1)
            continue;
        //       FDBG(patternc);
        byte midinr = minstr[v];
        byte midivel = ptest;
        {
            //          FDBG(SN(midinr) + " " + SN(midivel));
            if (!Menu::useVel&&midivel>0)
                midivel = 127;
            usbMIDI.sendNoteOn(midinr, midivel, 10, cable);
            MIDI1.sendNoteOn(midinr, midivel, 10);
            MIDI2.sendNoteOn(midinr, midivel, 10);
            //           FDBG("note on " + SN(midinr) + SN(midivel));
            if (transport == RECORDING && menuState == SETTINGS)
            {
                if (lastEvent == 0)
                    MidiEvent::starttime = 0;
                sequences[lastEvent++].init(0x90, midinr, midivel, 10);
            }
        }
    }
}
