#ifndef GLOBALS_H
#define GLOBALS_H
#include <MIDI.h>
#include <Trill.h>

#include "Adafruit_LEDBackpack.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

class Button;
class Chord;
class Menu;
class Parameter;
int debug = 0;
bool showflat = true;

#define SN(x) String(x) + " "
#define SB(x) String(x ? "t" : "f")
#define SP(x) String((int)x, HEX)
#define __NAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __CALLER__ String(String(__LINE__) + " " + __NAME__)

#define DBG(x)
	String history = "help";
byte voices[128];
// #define DBG(x)
// 	if (debug == 1)
// 	Serial.println(x)
#define FDBG(x) \
	Serial.println(__CALLER__+" "+x)

//#define FDBG(x)
String lastCall="stack";
//#define FSTACK Serial.println(__CALLER__)
//#define STACK lastCall = __CALLER__
#define STACK
#define FSTACK
String MIDIinData;
String outData;
float g_xoff = 0.0;
int oldsa = 0;

class synPara
{
public:
	synPara(byte spara, float sendvalue, String dim, bool pisFloat, float startv = 0)
	{
		para = spara;
		endvalue = sendvalue;
		dimension = dim;
		isFloat = pisFloat;
		startvalue = startv;
	}
	byte para = 0;
	float endvalue = 0;
	float startvalue = 0;
	String dimension = "";
	bool isFloat = false;
};
#define triggerButton 15 // 3
#define cancelButton 12	 // ４

const signed char MAIN = 0;
const signed char MIDISET = 1;
const signed char SYNTHSETTINGS = 2;
const signed char MAPPINGS = 3;
const signed char SCALES = 4;
const signed char SETTINGS = 5;
const signed char EXTERNALS = 6;
const signed char TOUCH = 7;
const signed char VERSION = 8;
const signed char PRESETS = 12;
const signed char NEWSCALE = 13;
const signed char SHOWSCALE = 14;
const signed char SHOWMAP = 16;
const signed char NEWMAP = 15;
const signed char EXTSETTINGS = 17;
const signed char MIDISETTINGS = EXTSETTINGS + 9;
const signed char ZONESETTINGS = MIDISETTINGS + 3;
const signed char TARGETS = ZONESETTINGS + 6;
const signed char CALIBRATION = TARGETS + 20;
const signed char NUM_MENUS = 100;

enum synparatype
{
	NO_AUDIO = 0,
	PORTAMENTOTIME,
	PORTAMENTOTYPE,
	POLYMODE,
	ADSR_MAG,
	AM_DEPTH,
	AM_RATE,
	AM_SHAPE,
	DIRECT,
	FM_DEPTH,
	FM_RATE,
	FM_SHAPE,
	PM_DEPTH,
	PM_RATE,
	PM_SHAPE,
	SAMPLE_AMP,
	SAMPLE_TIME,
	TRIGGER_1,
	TRIGGER_2,
	VCA_BP,
	VCA_ATTACK,
	VCA_DECAY,
	VCA_RELEASE,
	VCA_SUSTAIN,
	VCF_ATTACK,
	VCF_DECAY,
	VCF_DEPTH,
	VCF_FREQ,
	VCF_HI,
	VCF_LO,
	VCF_MODE,
	VCF_Q,
	VCF_RATE,
	VCF_RELEASE,
	VCF_SHAPE,
	VCF_SUSTAIN,
	VCO_AMP,
	VCO_DETUNE,
	VCO_FREQ,
	VCO_MIX,
	VCO_FMRANGE,
	VCO_RANGE,
	VCO_SHAPE,
	VCO_SUB_MIX,
	NOISE,
	SUBSHAPE,
	SUBOCT,
	DETSHAPE,
	DETOFF,
	NUMSYNTH,
	FILE_OPEN,
	FILE_SAVE,
	FILE_SAVE_NEW,
};
EXTMEM short pmstart[100];
EXTMEM short pmend[100];
EXTMEM short pmshape[100];
int lastXPos = 0;
EXTMEM short guiid[1400];
EXTMEM byte id2para[1400];
EXTMEM byte menId[1400];
int maxg = 0;
EXTMEM String perc[127];
#if 0
EXTMEM String perc[127] = {
	"[Racktom 1]",
	"[Racktom 2]",
	"[Racktom 2]",
	"[Floortom 1]",
	"[Floortom 1]",
	"[Snare]",
	"hatsTrig",
	"hatsTipTrig",
	"hatsTipTrig",
	"Closed Pedal",
	"Closed",
	"Open 1",
	"Open 1",
	"Open 2",
	"Open 2",
	"Open 3",
	"Open 3",
	"hatsTrig",
	"hatsTipTrig",
	"hatsTipTrig",
	"Closed Pedal",
	"Closed",
	"Open Pedal",
	"Open 1",
	"Open 2",
	"Open 3",
	"[Crash A]",
	"[Crash A]",
	"[Crash A]",
	"[Crash B]",
	"[Crash B]",
	"Metronome Click",
	"Metronome Bell",
	"Acoustic Bass Drum",
	"Bass Drum 1",
	"Side Stick",
	"Acoustic Snare",
	"Hand Clap",
	"Electric Snare",
	"Low Floor Tom",
	"Closed Hi-Hat",
	"High Floor Tom",
	"Pedal Hi-Hat",
	"Low Tom",
	"Open Hi-Hat",
	"Low-Mid Tom",
	"Hi-Mid Tom",
	"Crash Cymbal 1",
	"High Tom",
	"Ride Cymbal 1",
	"Chinese Cymbal",
	"Ride Bell",
	"Tambourine f#",
	"Splash Cymbal",
	"Cowbell ",
	"Crash Cymbal 2",
	"Vibraslap x",
	"Ride Cymbal 2",
	"Hi Bongo",
	"Low Bongo",
	"Mute Hi Conga",
	"Open Hi Conga",
	"Low Conga",
	"High Timbale",
	"Low Timbale",
	"High Agogo",
	"Low Agogo",
	"Cabasa ",
	"Maracas ",
	"Short Whistle",
	"Long Whistle",
	"Short Guiro",
	"Long Guiro",
	"Claves ",
	"Hi Wood Block",
	"Low Wood Block",
	"Mute Cuica",
	"Open Cuica",
	"Mute Triangle",
	"Open Triangle",
	"Flams",
	"Crescendo",
	"Left Brush Hit",
	"Right Brush Hit",
	"Right Brushed",
	"Left Brushed",
	"Slap Crescendo",
	"Right Muted",
	"Right Open",
	"Right BassTone",
	"Left Open Slap",
	"Right Open Slap",
	"Left Closed Slap",
	"Right Closed Slap",
	"Left Heel",
	"Right Heel",
	"Crescendo",
	"Flams",
	"Slap Flams",
	"FX",
	"c2Left Open",
	"c2Right Open",
	"c2Left Muted",
	"c2Right Muted",
	"c2Left BassTone",
	"c2Right BassTone",
	"c2Crescendo",
	"c2Flams",
	"c2FX",
	"c2Left Open",
	"c2Right Open",
	"c2Left Muted",
	"c2Right Muted",
	"c2Left BassTone",
	"c2Right BassTone",
	"c2Crescendo",
	"c2Flams",
	"[Bells] Crescendo",
	"[Cymbal 1] *Crescendo",
	"[Waterfall] Crescendo",
	"[Cymbal 2] Crescendo",
	"[Crickets] Crescendo",
	"[Shekere] Beat",
	"[Shekere] Off Beat",
	"[Shekere] On Beat",
	"[Shekere] Shakings",
};
#endif
EXTMEM byte transposeit[32] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
bool progressmode = false;
bool newmapmode = false;
bool originalmode = false;
String mapComment;
const short iswhole[12][12] PROGMEM = {
	{-1, 2, -3, 4, 5, -6, 7, -8, 9, -10, 11},
	{1, -2, 3, 4, -5, 6, -7, 8, -9, 10, 11},
	{-1, 2, 3, -4, 5, -6, 7, -8, 9, 10, -11},
	{1, 2, -3, 4, -5, 6, -7, 8, 9, -10, 11},
	{1, -2, 3, -4, 5, -6, 7, 8, -9, 10, -11},
	{-1, 2, -3, 4, -5, 6, 7, -8, 9, -10, 11},
	{1, -2, 3, -4, 5, 6, -7, 8, -9, 10, 11},
	{-1, 2, -3, 4, 5, -6, 7, -8, 9, 10, -11},
	{1, -2, 3, 4, -5, 6, -7, 8, 9, -10, 11},
	{-1, 2, 3, -4, 5, -6, 7, 8, -9, 10, -11},
	{1, 2, -3, 4, -5, 6, 7, -8, 9, -10, 11},
	{1, -2, 3, -4, 5, 6, -7, 8, -9, 10, -11}};

const short isFlat[12] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
const short mapwhole[12][12] PROGMEM = {
	{-1, 0, -1, 1, 2, -1, 3, -1, 4, -1, 5, 6},
	{0, -1, 1, 2, -1, 3, -1, 4, -1, 5, 6, 7},
	{-1, 0, 1, -1, 2, -1, 3, -1, 4, 5, -1, 6},
	{0, 1, -1, 2, -1, 3, -1, 4, 5, -1, 6, 7},
	{0, -1, 1, -1, 2, -1, 3, 4, -1, 5, -1, 6},
	{-1, 0, -1, 1, -1, 2, 3, -1, 4, -1, 5, 6},
	{0, -1, 1, -1, 2, 3, -1, 4, -1, 5, 6, 7},
	{-1, 0, -1, 1, 2, -1, 3, -1, 4, 5, -1, 6},
	{0, -1, 1, 2, -1, 3, -1, 4, 5, -1, 6, 7},
	{-1, 0, 1, -1, 2, -1, 3, 4, -1, 5, -1, 6},
	{0, 1, -1, 2, -1, 3, 4, -1, 5, -1, 6, 7},
	{0, -1, 1, -1, 2, 3, -1, 4, -1, 5, -1, 6}};
#define CLICK 0
#define RELEASE 1
#define CHECKBOX 2
#define SLIDER 3
#define OPTION 4

enum
{
	submod = FILE_SAVE_NEW + 1,
	subSample,
	subVCO,
	subVCF,
	subVCA,
	subFM,
	subPM,
	subFLFO,
	subFADSR,
	subAM,
	subAADSR,
	subVCOWAVE,
	subFile,
	subKeys
};
const byte id2synth[] PROGMEM = {
	ADSR_MAG,
	VCF_ATTACK,
	VCF_DECAY,
	VCF_SUSTAIN,
	VCF_RELEASE,
	VCA_ATTACK,
	VCA_DECAY,
	VCA_SUSTAIN,
	VCA_RELEASE, // 7
	VCO_MIX,	 // 8
	VCO_DETUNE,
	DETOFF,
	VCO_SUB_MIX,
	SUBOCT,
	VCO_AMP,
	NOISE, // 13
	SUBSHAPE,
	VCO_SHAPE,
	DETSHAPE,
	FM_DEPTH, // 14
	FM_RATE,
	FM_SHAPE,
	PM_DEPTH, // 17
	PM_RATE,
	PM_SHAPE, // 19
	VCF_FREQ, // 20
	VCF_LO,
	VCF_HI,
	VCF_Q, // 24
	VCF_MODE,
	VCF_DEPTH, // 25
	VCF_RATE,
	VCF_SHAPE,
	VCF_ATTACK,
	VCF_DECAY,
	VCF_SUSTAIN,
	VCF_RELEASE,
	VCA_ATTACK,
	VCA_DECAY,
	VCA_SUSTAIN,
	VCA_RELEASE, // 7
	VCA_BP,		 // 28
	AM_DEPTH,	 // 29
	AM_RATE,
	AM_SHAPE,
	PORTAMENTOTIME, // 32
	PORTAMENTOTYPE,
	POLYMODE,
	DIRECT, // 35
	SAMPLE_AMP,
	SAMPLE_TIME,
	TRIGGER_1,
	TRIGGER_2,

};

EXTMEM String fileparas[3] = {"Open", "Save", "Save as new"};
String prename = "";
String seqFile = "";
#define MENU_PARAMETERS submod
#define MENU_SAMPLE subSample
#define MENU_VCO subVCO
#define MENU_VCF subVCF
#define MENU_VCA subVCA
#define MENU_VCFADSR subFADSR
#define MENU_FILES subFile
#define MENU_KEYS subKeys
String seqOn = "";
int qfactor = 0;
#define NUM_SUBMENUS (subKeys - submod + 1)
const signed char submenustructure[NUM_SUBMENUS][7] = {
	{subVCO, subFADSR, subVCF, subVCA, subSample, subFile, subKeys},
	{DIRECT, SAMPLE_AMP, SAMPLE_TIME, TRIGGER_1, TRIGGER_2, 0, 0},
	{subVCOWAVE, subFM, subPM, VCO_MIX, VCO_DETUNE, VCO_SUB_MIX, NOISE},
	{subFLFO, subFADSR, VCF_FREQ, VCF_LO, VCF_HI, VCF_MODE, VCF_Q},
	{subAM, subAADSR, VCA_BP, 0, 0, 0, 0},
	{FM_DEPTH, FM_RATE, FM_SHAPE, 0, 0, 0, 0},
	{PM_DEPTH, PM_RATE, PM_SHAPE, 0, 0, 0, 0},
	{VCF_DEPTH, VCF_RATE, VCF_SHAPE, 0, 0, 0, 0},
	{VCF_ATTACK, VCF_DECAY, VCF_SUSTAIN, VCF_RELEASE, 0, 0, 0},
	{AM_DEPTH, AM_RATE, AM_SHAPE, 0, 0, 0, 0},
	{VCA_ATTACK, VCA_DECAY, VCA_SUSTAIN, VCA_RELEASE, 0, 0, 0},
	{VCO_AMP, VCO_FMRANGE, VCO_SHAPE, 0, 0, 0, 0},
	{FILE_OPEN, FILE_SAVE, FILE_SAVE_NEW, 0, 0, 0, 0},
	{PORTAMENTOTIME, PORTAMENTOTYPE, POLYMODE, 0, 0, 0, 0},
};

EXTMEM String synthParas[] = {
	"_",
	"PTime",
	"PType",
	"Poly",
	"ADSR Range",
	"AM Depth",
	"AM Rate",
	"AM Shape",
	"Direct",
	"FM Depth",
	"FM Rate",
	"FM Shape",
	"PM Depth",
	"PM Rate",
	"PM Shape",
	"Sample Amp",
	"Sample Time",
	"Wave 1",
	"Wave 2",
	"VCA Mix",
	"VCA Attack",
	"VCA Decay",
	"VCA Release",
	"VCA Sustain",
	"VCF Attack",
	"VCF Decay",
	"FFM Depth",
	"VCF Freq",
	"VCF Hi",
	"VCF Lo",
	"VCF Mode",
	"VCF Q",
	"FFM Rate",
	"VCF Release",
	"FFM Shape",
	"VCF Sustain",
	"VCO Amp",
	"VCO Detune",
	"VCO Freq",
	"VCO Mix",
	"VCO FM Range",
	"VCO Range",
	"VCO Shape",
	"VCO Sub Mix",
	"Noise",
	"Sub Shape",
	"Sub Octave",
	"Det Shape",
	"Det Offset"};
EXTMEM const String subMenus[NUM_SUBMENUS] = {"Modules", "Sample", "VCO", "VCF", "VCA", "FM", "PM", "FFM", "ADSR", "AM", "ADSR", "Wave", "Presets", "Portamento"};
const signed char subParent[NUM_SUBMENUS] = {0, submod, submod, submod, submod, subVCO, subVCO, submod, submod, subVCA, subVCA, subVCO, submod, submod};
#define FILE_SAVE_NEW +1
#define SAMPLE1_PIN 40
EXTMEM synPara *synparas[100];
byte poly;
Button *Buttons[10];
// Chord *actChord;

File root;
bool refresh = false;
bool isMap, scaled;
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);
int jj1 = 0;
int j3 = 0;
EXTMEM Menu *Menus[NUM_MENUS];
#include "blobs.h"
EXTMEM String opts[100];
EXTMEM const String scales[] = {
	"12 Bar Blues",
	"Lydian Dom.",
	"Aeolian",
	"Altered Locrian ",
	"Augmented ",
	"Bebop Maj",
	"Byzantine M7",
	"Byzantine",
	"Chinese",
	"Chromatic ",
	"Dom. Bebop",
	"Dorian b2",
	"Dorian b5",
	"Dorian",
	"Egypthian",
	"Enigmatic ",
	"Half diminished ",
	"Harm. Maj 7",
	"Harm. Min 7",
	"Harmonic Maj.",
	"Harmonic Min ",
	"Hindu",
	"Hirayoshi",
	"Hungarian Gypsy",
	"Hungarian Maj ",
	"Hungarian Min",
	"Insen ",
	"Ionian #2",
	"Ionian Aug #2",
	"Istrian ",
	"Iwato ",
	"Lead. Wh. Tone",
	"Locrian #5",
	"Locrian #6",
	"Locrian Dom.",
	"Locrian Maj",
	"Locrian",
	"Lydian #2 #6",
	"Lydian #2",
	"Lydian #6",
	"Lydian Aug#2",
	"Lydian Aug. Dom.",
	"Lydian Aug.",
	"Lydian b#",
	"Lydian b3",
	"Lydian Dom. b6",
	"Lydian Dom.",
	"Lydian",
	"Major",
	"Man Gong",
	"Melodic Maj.",
	"Melodic Maj.M2",
	"Melodic Maj.M3",
	"Melodic Maj.M4",
	"Melodic Maj.M6",
	"Melodic Maj.M6",
	"Melodic Maj.M7",
	"Melodic Min.",
	"Mixolydian b9",
	"Mixolydian",
	"Miyako-bushi",
	"Neapol. Maj.",
	"Neapol. Min",
	"Octatonic ",
	"Oriental",
	"Pentatonic Maj.",
	"Pentatonic Min.",
	"Persian ",
	"Phrygian b4",
	"Phrygian min.",
	"Phrygian",
	"Prometheus",
	"Ritsusen Yo",
	"Ukrainian Dorian ",
	"Scale of harmonics ",
	"Semi Locrian b4",
	"Sup. Locrian bb3",
	"Superlocrian",
	"Tritone ",
	"Two-semitone tritone ",
	"Ukrainian Dorian ",
	"Ultr. Locrian bb3",
	"Ultraphrygian",
	"Whole tone ",
};
EXTMEM String chordopt[63];
EXTMEM String samples[100];
EXTMEM String chordName[2048];
EXTMEM String filenames[100];
EXTMEM int onmidis[128];
EXTMEM unsigned char bitstream[100000];
bool mapwhite = true;
int colidx = 0;
int fq = 0;
int eq = 0;
int commas[20];
int commidx = 0;
int bitidx = 0;
bool playSeq = 0;
EXTMEM byte midifile[1000000];
EXTMEM byte midibase64[1000000];
int midiptr = 0;
#define HAS_MORE_BYTES 0x80
EXTMEM String lastLoadMap = "";
String actMap = "";
class MidiEvent
{
public:
	MidiEvent() {}
	void init(byte event, byte note, byte velocity, byte channel)
	{
		_event = event;
		_note = note;
		_velocity = velocity;
		_channel = channel;
		if (starttime == 0)
			starttime = millis();
		_time = millis() - starttime;
		//		_id = id;
		//		FDBG(show());
	}
	String show()
	{
		return String(SN(_time) + " " + SN(_event) + " " + SN(_note) + " " + SN(_velocity));
	}
	byte _event;
	byte _note;
	byte _velocity;
	byte _channel;
	int _time;
	static int starttime;
	//	int _length = 0;
	//	short _id = 0;
	//	short on_id = -1;
};
enum trans
{
	REWIND,
	PLAYING,
	STOPPED,
	RECORDING,
	REPEAT,
	SAVESEQ,
	PAUSE
};
int MidiEvent::starttime = 0;
short lastEvent = 0;

short transport = STOPPED;
int nextEvent = 0;

EXTMEM MidiEvent sequences[100000];
const short scFP[] PROGMEM = {628, 874, 726, 677, 1228, 1498, 435, 1241, 1128, 2047,
							  1882, 853, 822, 854, 594, 1705, 694, 437, 429, 1243,
							  1238, 730, 198, 742, 876, 1254, 593, 1372, 1436, 101,
							  561, 1706, 1434, 821, 697, 698, 693, 1644, 1388, 1642,
							  1452, 938, 1450, 1381, 1382, 746, 874, 1386, 1370, 660,
							  1563, 1682, 1992, 1299, 1306, 1306, 793, 1366, 851, 858,
							  209, 1365, 1237, 1462, 825, 330, 596, 1209, 717, 729, 725,
							  810, 338, 870, 348, 686, 683, 685, 617, 227, 870, 427, 461, 682};
EXTMEM String wavenames[100];
EXTMEM String presetnames[100];
EXTMEM int patnames[500];
EXTMEM String patvals[500];
int maxsamples;
byte lastmet[8] = {0, 0, 0, 0, 0, 0, 0, 0};
// String xmidiNames[12] = {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};
String midiNamesSharp[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
String sel = "Note:";
String midiNamesFlat[12] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
EXTMEM String midiNamesOptF[13] = {"note", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
EXTMEM String midiNamesOptS[13] = {"note", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
EXTMEM String midiNamesLong[128];
const String selected[2] = {"_", "x"};
EXTMEM const String pmode[4] = {"full", "scale", "map", "bypass"};
EXTMEM int replchord[96];
EXTMEM short transpose[96];
EXTMEM int keychord[96];
short maxrepl = 0;
short actkeyidx = -1;
EXTMEM short onnotes[128];
// short offnotes[128];
EXTMEM byte xpos[12], firstnote;

Trill trillSensor;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_SER);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI2);

bool isChord = false;
bool isChordend = false;
// boolean noteact[128];
byte cable = 0;
signed char chordnotes[16];
short chorddeltas[16];
// extern AudioPlaySdWav           playSdWav1;
#define MCP4725_ADDR 0x60
bool foundserver = false;
const int chipSelect = BUILTIN_SDCARD;
const uint8_t con[] = {0, 0x60, 0xA0, 0xFF};
elapsedMillis led1;
elapsedMillis led2;
elapsedMillis led3;
elapsedMillis led4;
elapsedMillis led5;
extern u_int8_t _channel;
int SplitS(String what, char where, String *res, int nmax);
EXTMEM int baseNoteID[128];
signed char basenote = 0;
signed char chordptr = 0;
signed char sleepNoW = 0;
int deltat = 0;
int id = 0, bn = 0;
signed char checked = 0;
signed char extSW = 0;
boolean touchActive = false;
enum externals
{
	JOY_X = EXTSETTINGS,
	JOY_Y,
	POTI_1,
	POTI_2,
	EXT_PED_1,
	EXT_PED_2,
	FSR,
	HIRES,
	EXT_SWITCH,

};
enum midis
{
	USB_MIDI_IN = MIDISETTINGS,
	HOST_MIDI_IN,
	MIDI_IN
};
signed char extPins[] = {A8, A9, A6, A7, A17, A15, A16, 24, triggerButton};
int reso = 1;
const int NUMVOICES = 6;

elapsedMillis vtmillis[NUMVOICES];
bool incoming[3];
float outgoing[7];
AudioEffectEnvelope *vcaenv[] = {&envelope1, &envelope4, &envelope6, &envelope8, &envelope10, &envelope12};							  // xy=1660,&665
AudioEffectEnvelope *vcfenv[] = {&envelope2, &envelope3, &envelope5, &envelope7, &envelope9, &envelope11};							  // xy=1668,&1114
AudioFilterStateVariable *filters[] = {&filter1, &filter2, &filter3, &filter4, &filter5, &filter6};									  // xy=1329,&1196
AudioMixer4 *vcomix[] = {&mixer2, &mixer9, &mixer13, &mixer17, &mixer20, &mixer23};													  // xy=1185,&951
AudioMixer4 *vcfmod[] = {&mixer19, &mixer22, &mixer15, &mixer26, &mixer27, &mixer25};												  // xy=1515,&1173
AudioMixer4 *vcfout[] = {&mixer6, &mixer10, &mixer14, &mixer18, &mixer21, &mixer24};												  // xy=1110,&394
AudioSynthMVWaveformModulated *vcos1[] = {&waveformMod1, &waveformMod3, &waveformMod5, &waveformMod7, &waveformMod9, &waveformMod11}; // xy=1025,&792
short onoff = 0;																													  // AudioSynthWaveformModulated* vcos2[] = { &waveformMod2,&waveformMod4,&waveformMod6,&waveformMod8,&waveformMod10,&waveformMod12 }; //xy=1008,&1177
class Parameter;
Parameter *aDTPara = 0;
byte scalebase = 0;
bool scend = false;
int clickc = 0;
bool actpat[2];
#define editMode actpat[0]
#define MAXPAT 256
#define MAXVOI 32
String g_search;
EXTMEM String patfiles[MAXPAT];

EXTMEM short triggerNote[128];
EXTMEM byte acttrigger[MAXPAT];
EXTMEM short actbeatID[MAXPAT];
EXTMEM String patcolors[MAXPAT];
int sa = 0;
int se;
float g_xpos = 0;
short actred=-1;
short startvoice = -1;
bool startOver = false;
#define maxticks 12
IntervalTimer metTimer;
IntervalTimer playTimer;
int numq = 1;
float metpart = 1;
float mettime = 2000000;
bool metison = false;
bool metisback = false;
short patternc = -1,newpatternc=-1;
byte patcnt = 0;
int actpattern = 0;
int startpattern = -1;
int lastpattern = -1;
EXTMEM short seqpattern[MAXPAT * maxticks][MAXVOI];
EXTMEM byte delaypattern[MAXPAT * maxticks];
EXTMEM byte velpattern[MAXPAT * maxticks][MAXVOI];
byte zerobase = 1;
byte minstr[MAXVOI];
byte mvelo[MAXVOI];
String currentDirectory = "/";
String previousDir = "/";
byte oldtrigger = 0;
bool seqswitch[maxticks];
EXTMEM int patidt[4][2];
int metopt[4];
int instopt[4];
int voiceidt[4];
int velopt[4];
int metid[4];
byte beatlength = 4;
void click(void);
// String pout = "";
bool beatstate = false;
byte mettrigger = 24;
int keyidt, beatidt, ccidt, nltidt, progidt, beatnrid;
short mbase = 100, ledbase = mbase + 40, sbase = 200 + 35, tbase = sbase + 15, fbase = tbase + 40, xbase = fbase + 20, base5 = xbase + 80, base6 = base5 + 20, base7 = base6 + 30, base8 = base7 + 50, base9 = base8 + 50, base10 = base9 + 50;
int mtarget = 0;
int centerx[10], centery[10];
int ccopt, lastcc = 0;
byte ccvalopt = 0, lastccval = 0;
int ccmetid;
EXTMEM byte ccpattern[MAXPAT * maxticks];
EXTMEM byte ccval[MAXPAT * maxticks];
EXTMEM byte patvoicelow[MAXPAT];
EXTMEM byte patvoicehigh[MAXPAT];
EXTMEM String counts[128];
EXTMEM String pcount[4] = {"1", "17", "33", "49"};
bool patset = false;
long lastmill = 0;
byte lastBeat = 0;
EXTMEM byte beatCount[MAXPAT];
byte lastvoice = 0;
int patternidt = 0;
int patternidt2 = 0;
int ccpatternidt = 0;
int ccvpatternidt = 0;
EXTMEM String coloring[6] = {
	"cyan",
	"#66CDAA",
	"#008B8B",
	"#5F9EA0",
	"gold",
	"beige"};
EXTMEM String orgcoloring[6] = {
	"cyan",
	"#66CDAA",
	"#008B8B",
	"#5F9EA0",
	"gold",
	"beige"};
byte lastColor = 0;
// **************************config *****************
signed char lastMap = 0;
// int lastScale = 0;
// String metpat[8] = {"q", "ee", "3eee", "ssss", "e.ss", "esse", "e3sss", "3sss e"};
// short metval[8] = {1, 65, 273, 585, 1281, 1601, 1353, 85};
uint32_t lasttick = 0;
signed char octave = 0;
signed char mapID = 0;
signed char semiTone = 0;
signed char sindex[2] = {0, 0};
signed char windex = 0;
// String clefsvg;
// String mapFile;
// String scaleFile;
// String preFile;
// String eqFile;
String wavFile[2];

signed char preindex = -1;
// signed char actPre = 0;
// signed char recentPre = 0;
EXTMEM String scaleNames[2048];
signed char contrast;
int g_scid = 0;
IntervalTimer *myTimer;
short inscale[12];

// **************************************************
short menuState = MAIN;
short oldmenuState = MAIN;
bool nodisplay = false;
unsigned long lastTime;
unsigned long onTime;
unsigned long myTime;
//	File gfrec;

bool trigger = false;

#include "gfunctions.h"
#include "svgdraw.h"
#endif
