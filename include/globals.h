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
#if 1
#define DBG(x)      \
	if (debug == 1) \
	Serial.println(x)
#define FDBG(x) \
	Serial.println(x)
#else
#define DBG(x)
#define FDBG(x)
#endif
#define SN(x) String(x)
#define SB(x) String(x ? "t" : "f")
#define SP(x) String((int)x, HEX)
#define __NAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __CALLER__ String(String(__LINE__) + " " + __NAME__)
String inData;
String outData;
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
	NUMSYNTH,
	FILE_OPEN,
	FILE_SAVE,
	FILE_SAVE_NEW,
};
short pmstart[100];
short pmend[100];
short pmshape[100];
int lastXPos = 0;
short guiid[400];
byte id2para[400];
byte menId[400];
int maxg = 0;
byte transposeit[6] = {1, 1, 1, 1, 1, 1};
bool progressmode = false;
static short iswhole[12][12] = {
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

static short isFlat[12] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
static short mapwhole[12][12] = {
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
byte id2synth[] = {
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
	VCO_SUB_MIX,
	VCO_AMP,
	NOISE, // 13
	VCO_SHAPE,
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

String fileparas[3] = {"Open", "Save", "Save as new"};
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
	"Noise"
};
const String subMenus[NUM_SUBMENUS] = {"Modules", "Sample", "VCO", "VCF", "VCA", "FM", "PM", "FFM", "ADSR", "AM", "ADSR", "Wave", "Presets", "Portamento"};
const signed char subParent[NUM_SUBMENUS] = {0, submod, submod, submod, submod, subVCO, subVCO, submod, submod, subVCA, subVCA, subVCO, submod, submod};
#define FILE_SAVE_NEW +1
#define SAMPLE1_PIN 40
synPara *synparas[100];
byte poly;
Button *Buttons[10];
// Chord *actChord;
Chord *chords[2048];
File root;
bool refresh = false;
bool isMap, scaled;
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);
int jj1 = 0;
int j2 = 0;
int j3 = 0;
EXTMEM Menu *Menus[NUM_MENUS];
#include "blobs.h"
EXTMEM String opts[100];
EXTMEM String scales[100];
EXTMEM String chordopt[63];
EXTMEM String samples[100];
EXTMEM String chordName[2048];
EXTMEM String filenames[100];
EXTMEM int onmidis[128];
EXTMEM unsigned char bitstream[1000000];
bool mapwhite = true;
int colidx = 0;
int fq = 0;
int eq = 0;
int commas[20];
int commidx = 0;
int bitidx = 0;
bool playSeq = 0;
EXTMEM byte midifile[80000];
EXTMEM byte midibase64[80000];
int midiptr = 0;
#define HAS_MORE_BYTES 0x80
String lastLoadMap = "";
String actMap = "";
class MidiEvent
{
public:
	MidiEvent() {}
	void init(int time, byte event, byte note, byte velocity, byte channel, short id)
	{
		_event = event;
		_note = note;
		_velocity = velocity;
		_channel = channel;
		_time = time;
		if (event)
			onmidis[note] = id;
		else
		{
			on_id = onmidis[note];
		}
		_id = id;
		DBG(SN(_time) + " " + SN(note) + " " + SN(onmidis[note]) + " " + SB(event) + " " + SN(_length));
	}
	void recinit(byte event, byte note, byte velocity, byte channel, short id)
	{
		_event = event;
		_note = note;
		_velocity = velocity;
		_channel = channel;
		if (starttime == 0)
			starttime = millis();
		_time = millis() - starttime;
		if (event == 0x90)
			onmidis[note] = _time;
		else
			_length = _time - onmidis[note];
		DBG(SN(_time) + " " + SN(note) + " " + SN(onmidis[note]) + " " + SN(event) + " " + SN(_length));
		_id = id;
	}
	byte _event;
	byte _note;
	byte _velocity;
	byte _channel;
	int _time;
	static int starttime;
	int _length = 0;
	short _id = 0;
	short on_id = -1;
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

EXTMEM MidiEvent sequences[10000];
short scFP[] = {628, 874, 726, 677, 1228, 1498, 435, 1241, 1128, 2047,
				1882, 853, 822, 854, 594, 1705, 694, 437, 429, 1243,
				1238, 730, 198, 742, 876, 1254, 593, 1372, 1436, 101,
				561, 1706, 1434, 821, 697, 698, 693, 1644, 1388, 1642,
				1452, 938, 1450, 1381, 1382, 746, 874, 1386, 1370, 660,
				1563, 1682, 1992, 1299, 1306, 1306, 793, 1366, 851, 858,
				209, 1365, 1237, 1462, 825, 330, 596, 1209, 717, 729, 725,
				810, 338, 870, 348, 686, 683, 685, 617, 227, 870, 427, 461, 682};
String wavenames[100];
String presetnames[100];
int maxsamples;
String midiNames[12] = {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};
String midiNamesSharp[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
String sel = "Note:";
String midiNamesFlat[12] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
String midiNamesOpt[13] = {"note", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
EXTMEM String midiNamesLong[128];
const String selected[2] = {"_", "x"};
EXTMEM const String pmode[4] = {"full", "scale", "map", "bypass"};
EXTMEM short replchord[96];
EXTMEM short transpose[96];
EXTMEM int keychord[96];
short maxrepl = 0;
short actkeyidx = -1;
EXTMEM short onnotes[128];
// short offnotes[128];
byte xpos[12], firstnote;

bool ledstate[100];
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
signed char SplitS(String what, char where, String *res, signed char nmax);
int baseNoteID[128];
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

// **************************config *****************
signed char lastMap = 0;
//int lastScale = 0;

signed char octave = 0;
signed char mapID = 0;
signed char semiTone = 0;
signed char s1index = 1;
signed char s2index = 1;
// String clefsvg;
// String mapFile;
// String scaleFile;
// String preFile;
// String eqFile;
String wav1File;
String wav2File;
signed char preindex = -1;
signed char actPre = 0;
signed char recentPre = 0;
String scaleNames[2048];
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
#include "background.h"
#endif
