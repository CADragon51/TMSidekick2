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
class synPara
{
public:
	synPara(byte spara, float sendvalue, String dim, bool pisFloat)
	{
		para = spara;
		endvalue = sendvalue;
		dimension = dim;
		isFloat = pisFloat;
	}
	byte para = 0;
	float endvalue = 0;
	String dimension = "";
	bool isFloat = false;
};
const signed char MAIN = 0;
const signed char MIDISET = 1;
const signed char SYNTHSETTINGS = 2;
const signed char MAPPINGS = 3;
const signed char SCALES = 4;
const signed char SETTINGS = 5;
const signed char EXTERNALS = 6;
const signed char TOUCH = 7;
const signed char SAMPLES = 8;
const signed char VERSION = 9;
const signed char FSR = 10;
const signed char SWITCH = 11;
const signed char PRESETS = 12;
const signed char NEWSCALE = 13;
const signed char SHOWSCALE = 14;
const signed char SHOWMAP = 15;
const signed char NEWMAP = 16;
const signed char EXTSETTINGS = 17;
const signed char MIDISETTINGS = EXTSETTINGS + 9;
const signed char ZONESETTINGS = MIDISETTINGS + 3;
const signed char TARGETS = ZONESETTINGS + 6;
const signed char CALIBRATION = TARGETS + 20;
const signed char NUM_MENUS = 100;

const signed char NO_AUDIO = 0;
const signed char AM_DEPTH = 1;
const signed char AM_RATE = 2;
const signed char AM_SHAPE = 3;
const signed char DIRECT = 4;
const signed char FM_DEPTH = 5;
const signed char FM_RATE = 6;
const signed char FM_SHAPE = 7;
const signed char PW_DEPTH = 8;
const signed char PW_RATE = 9;
const signed char PW_SHAPE = 10;
const signed char SAMPLE_AMP = 11;
const signed char SAMPLE_TIME = 12;
const signed char TRIGGER_1 = 13;
const signed char TRIGGER_2 = 14;
const signed char VCA = 15;
const signed char VCA_ATTACK = 16;
const signed char VCA_DECAY = 17;
const signed char VCA_RELEASE = 18;
const signed char VCA_SUSTAIN = 19;
const signed char VCF_ATTACK = 20;
const signed char VCF_DECAY = 21;
const signed char VCF_DEPTH = 22;
const signed char VCF_FREQ = 23;
const signed char VCF_HI = 24;
const signed char VCF_LO = 25;
const signed char VCF_MODE = 26;
const signed char VCF_Q = 27;
const signed char VCF_RATE = 28;
const signed char VCF_RELEASE = 29;
const signed char VCF_SHAPE = 30;
const signed char VCF_SUSTAIN = 31;
const signed char VCO_AMP = 32;
const signed char VCO_DETUNE = 33;
const signed char VCO_FREQ = 34;
const signed char VCO_MIX = 35;
const signed char VCO_FMRANGE = 36;
const signed char VCO_RANGE = 37;
const signed char VCO_SHAPE = 38;
const signed char VCO_SUB_MIX = 39;
const signed char NOISE = VCO_SUB_MIX + 1;
const signed char PORTAMENTOTIME = NOISE + 1;
const signed char PORTAMENTOTYPE = PORTAMENTOTIME + 1;
const signed char POLYMODE = PORTAMENTOTYPE + 1;

const signed char FILE_OPEN = POLYMODE + 1;
const signed char FILE_SAVE = FILE_OPEN + 1;
const signed char FILE_SAVE_NEW = FILE_SAVE + 1;

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

String fileparas[3] = {"Open", "Save", "Save as new"};
String prename = "";
#define NUM_SUBMENUS (subKeys - submod + 1)
const signed char submenustructure[NUM_SUBMENUS][7] = {
	{subVCO, subVCF, subVCA, subSample, subFile, subKeys, 0},
	{DIRECT, SAMPLE_AMP, SAMPLE_TIME, TRIGGER_1, TRIGGER_2, 0, 0},
	{subVCOWAVE, subFM, subPM, VCO_MIX, VCO_DETUNE, VCO_SUB_MIX, 0},
	{
		subFLFO,
		subFADSR,
		VCF_FREQ,
		VCF_LO,
		VCF_HI,
		VCF_MODE,
		VCF_Q,
	},
	{subAM, subAADSR, VCA, NOISE, 0, 0, 0},
	{FM_DEPTH, FM_RATE, FM_SHAPE, 0, 0, 0, 0},
	{PW_DEPTH, PW_RATE, PW_SHAPE, 0, 0, 0, 0},
	{VCF_DEPTH, VCF_RATE, VCF_SHAPE, 0, 0, 0, 0},
	{VCF_ATTACK, VCF_DECAY, VCF_SUSTAIN, VCF_RELEASE, 0, 0, 0},
	{AM_DEPTH, AM_RATE, AM_SHAPE, 0, 0, 0, 0},
	{VCA_ATTACK, VCA_DECAY, VCA_SUSTAIN, VCA_RELEASE, 0, 0, 0},
	{VCO_AMP, VCO_FMRANGE, VCO_SHAPE, 0, 0, 0, 0},
	{FILE_OPEN, FILE_SAVE, FILE_SAVE_NEW, 0, 0, 0, 0},
	{PORTAMENTOTIME, PORTAMENTOTYPE, POLYMODE, 0, 0, 0, 0},
};

String synthParas[] = {"_", "AM Depth", "AM Rate", "AM Shape", "Direct", "FM Depth", "FM Rate", "FM Shape", "PM Depth", "PM Rate", "PM Shape", "Sample Amp", "Sample Time", "Trigger 1", "Trigger 2", "VCA", "VCA Attack", "VCA Decay", "VCA Release", "VCA Sustain", "VCF Attack", "VCF Decay", "FFM Depth", "VCF Freq", "VCF Hi", "VCF Lo", "VCF Mode", "VCF Q", "FFM Rate",
					   "VCF Release", "FFM Shape", "VCF Sustain", "VCO Amp", "VCO Detune", "VCO Freq", "VCO Mix", "VCO FM Range",
					   "VCO Range", "VCO Shape", "VCO Sub Mix", "Noise",
					   "Portamento Time", "Portamento Type", "Poly Mode"};
String vcftype[5] = {"Off", "LP", "BP", "HP", "Moog"};
String ranges[7] = {"64", "32", "16", "8", "4", "2", "1"};
String taps[16] = {"0", "1", "2", "1-2", "3", "1-3", "2-3", "1-2-3", "4", "1-4", "2-4", "1-2-4", "3-4", "1-3-4", "2-3-4", "1-2-3-4"};
float vtaps[16] = {0, 1, 1, 0.5, 1, 0.5, 0.5, 1.0 / 3.0, 1, 0.5, 0.5, 1.0 / 3.0, 0.5, 1.0 / 3.0, 1.0 / 3.0, 0.25};
const String shapes[] = {"Sine", "Sawtooth", "Square", "Triangle", "Arbitrary", "Pulse", "Rev. Saw", "S&H", "Var. Tri", "BL Saw", "BL SAW R", "BL Square", "BL Pulse"};
const String subMenus[NUM_SUBMENUS] = {"Parameters", "Sample", "VCO", "VCF", "VCA", "FM", "PM", "FFM", "ADSR", "AM", "ADSR", "Wave", "File", "Keys"};
const signed char subParent[NUM_SUBMENUS] = {0, submod, submod, submod, submod, subVCO, subVCO, subVCF, subVCF, subVCA, subVCA, subVCO, submod, submod};
#define NUMSYNTH POLYMODE + 1
#define FSR_PIN 40
synPara *synparas[NUMSYNTH];
byte poly;
Button *Buttons[10];
//Chord *actChord;
Chord *chords[1200];
File root;
bool refresh = false;
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);

Menu *Menus[NUM_MENUS];
#include "blobs.h"

String cname[2048];
String filenames[100];
int scFP[100];
String wavenames[100];
String presetnames[100];

String midiNames[12] = {"C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab", "A", "A#/Bb", "B"};
String midiNamesSharp[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
String midiNamesFlat[12] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
String midiNamesLong[128];
const String selected[2] = {"_", "x"};
const String polymode[2] = {"MONO", "POLY"};
const String portatype[4] = {"_", "LIN", "EXP", "LOG"};
short replchord[2048];
short transpose[2048];
short onnotes[128];
short offnotes[128];
byte xpos[12],firstnote;

signed char basescale[128];
Trill trillSensor;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_SER);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI2);

bool isChord = false;
bool isChordend = false;
//boolean noteact[128];
byte cable = 0;
signed char chordnotes[16];
short chorddeltas[16];
//extern AudioPlaySdWav           playSdWav1;
#define MCP4725_ADDR 0x60

const int chipSelect = BUILTIN_SDCARD;
const uint8_t con[] = {0, 0x60, 0xA0, 0xFF};
elapsedMillis led1;
elapsedMillis led2;
elapsedMillis led3;
elapsedMillis led4;
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
	EXT_PED_1 = EXTSETTINGS,
	EXT_PED_2,
	FSR_IN,
	EXT_SWITCH,
	JOY_X,
	JOY_Y,
	POTI_1,
	POTI_2,
	HIRES
};
enum midis
{
	USB_MIDI_IN = MIDISETTINGS,
	HOST_MIDI_IN,
	MIDI_IN
};
signed char extPins[] = {A17, A15, A16, 3, A8, A9, A6, A7, 24};
int reso = 1;
const int NUMVOICES = 6;
int debug = 0;
elapsedMillis vtmillis[NUMVOICES];

AudioEffectEnvelope *vcaenv[] = {&envelope1, &envelope4, &envelope6, &envelope8, &envelope10, &envelope12};							  //xy=1660,&665
AudioEffectEnvelope *vcfenv[] = {&envelope2, &envelope3, &envelope5, &envelope7, &envelope9, &envelope11};							  //xy=1668,&1114
AudioFilterStateVariable *filters[] = {&filter1, &filter2, &filter3, &filter4, &filter5, &filter6};									  //xy=1329,&1196
AudioMixer4 *vcomix[] = {&mixer2, &mixer9, &mixer13, &mixer17, &mixer20, &mixer23};													  //xy=1185,&951
AudioMixer4 *vcfmod[] = {&mixer19, &mixer22, &mixer15, &mixer26, &mixer27, &mixer25};												  //xy=1515,&1173
AudioMixer4 *vcfout[] = {&mixer6, &mixer10, &mixer14, &mixer18, &mixer21, &mixer24};												  //xy=1110,&394
AudioSynthMVWaveformModulated *vcos1[] = {&waveformMod1, &waveformMod3, &waveformMod5, &waveformMod7, &waveformMod9, &waveformMod11}; //xy=1025,&792
short onoff = 0;																													  //AudioSynthWaveformModulated* vcos2[] = { &waveformMod2,&waveformMod4,&waveformMod6,&waveformMod8,&waveformMod10,&waveformMod12 }; //xy=1008,&1177
class Parameter;
Parameter *aDTPara = 0;
byte invID[2048];
byte scalebase = 0;
bool scend = false;

#include "scchords.h"
	// **************************config *****************
signed char lastMap = 0;
signed char lastScale = 0;

signed char octave = 0;
signed char mapID = 0;
signed char semiTone = 0;
signed char s1index = 1;
signed char s2index = 1;
String mapFile;
String scaleFile;
String preFile;
String eqFile;
String wav1File;
String wav2File;
signed char preindex = -1;
signed char actPre = 0;
signed char recentPre = 0;
String scaleNames[2048];
signed char contrast;
int scid = 0;
IntervalTimer myTimer;

// **************************************************
short menuState = MAIN;
short oldmenuState = MAIN;
unsigned long lastTime;
unsigned long onTime;
unsigned long myTime;
//	File gfrec;

bool trigger = false;

#include "gfunctions.h"
#endif
