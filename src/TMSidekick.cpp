#include <Arduino.h>
#include <splash.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <SSD1306init.h>
#include <SSD1306AsciiSpi.h>
#include <SSD1306AsciiSoftSpi.h>
#include <SSD1306Ascii.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
void printA4(String);
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#include <USBHost_t36.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "setoled.h"
#include "CAAudio.h"
#include "AudioTool.h"

#include "note2freq.h"
#include "globals.h"
#include "button.h"
#include "chord.h"
#include "menu.h"
#include "trillin.h"
#include "processchord.h"
#include "midifunc.h"
#include "mappings.h"
#include "functions.h"
extern const int16_t myWaveform[256]; // defined in myWaveform.ino

elapsedMillis joy;
//elapsedMillis fftreset;
signed char Menu::contrast = 1;
short Menu::isshowing = 0;
Parameter *Parameter::aHIPara = 0;
Parameter *Parameter::aLOPara = 0;
Parameter *Parameter::aFPara = 0;
Parameter *Parameter::aDPara = 0;

//IntervalTimer midiMapTimer;
//IntervalTimer guitarHeroTimer;
//IntervalTimer volumeTimer;
int MenuPara::AudioTG[54] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0,
							 0, 0, 0, 0, 0, 0, 0, 0, 0};
int MenuPara::CV1TG = 0;
int MenuPara::CV2TG = 0;
int MenuPara::StrumTG = 0;
signed char Menu::actSubmenu = 0;
signed char Menu::useBPM = 0;
signed char Menu::hiRes = 0;
signed char Menu::isRat = 0;
signed char Menu::byPass = 0;
signed char Menu::sendAfter = 0;
signed char Menu::strum = 0;
signed char Menu::noPanic = 0;
signed char Menu::triggerCC = 0;
signed char Menu::ratdiv = 2;
short Menu::maparr[100];
byte Menu::mnx = 0;

float Menu::BPM = 120;

bool MenuSynth::opencalled = false;
int led = 13;
extern int fx;
//float fftmax[512];
void setup()
{
	AudioNoInterrupts();
	//	float a = 0.125, d = 0.125, s = 0.5, r = 0.75, M = 1024 * 32 - 1, A = 256 * a, R = 256 * r, S = M * s, D = d * 256;
	static byte leds[6] = {36, 34, 32, 30, 28, 13};
	File frec;
	Serial.begin(9600 * 16);
	pinMode(led, OUTPUT);
	setoled();
	debug = 0;
	AudioMemory(400);

	mixer12.gain(2, 0.4);
	noise1.amplitude(0);
	if (!display.begin(SSD1306_SWITCHCAPVCC))
	{
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}
	display.clearDisplay();
	display.drawBitmap(0, 0, tmslogo, 128, 64, 1);
	display.display();
	delay(1000); // Pause for 1 seconds
	createStruct();
	MIDI_SER.begin(MIDI_CHANNEL_OMNI);
	MIDI1.begin();
	MIDI2.begin();
	digitalWrite(led, HIGH);
	Wire.begin();
	//  Wire2.begin();
	if (debug == 1)
		Serial.print("Initializing SD card...");
	pinMode(11, OUTPUT);
	if (!SD.begin(chipSelect))
	{
		if (debug == 1)
			Serial.println("SD initialization failed!");
		return;
	}
	if (debug == 1)
		Serial.println("initialization done.");
	root = SD.open("/");
	if (debug == 1)
		Serial.println("reading SD ");
	if (debug == 1)
		Serial.println("done!");
	lastTime = millis();
	createmap();
	settrill();
	setButtons();
	if (debug == 1)
		Serial.println("Button set");
	setmidi();
	printDirectory(root, 0);
	if (debug == 1)
		Serial.println("Trying Restore");
	if (SD.exists("TMSIDEK.INI"))
	{
		frec = SD.open("TMSIDEK.INI", FILE_READ);
		if (frec)
		{
			if (debug == 1)
				Serial.println("loading last data ");
			Restore(frec);
			if (debug == 1)
				Serial.println("data loaded");
			frec.close();
		}
	}

	if (lastMap > 0 && lastMap < fx)
		Menus[MAPPINGS]->mapit();
	if (lastScale > 0 && lastScale < sfx)
		Menus[SCALES]->scaleit();

	if (Menus[SETTINGS]->byPass)
	{
		digitalWrite(28, HIGH);
		Buttons[5]->mlastButtonState = LOW;
		Buttons[5]->output = LOW;
	}
	menuState = MAIN;
	if (Menus[menuState])
		Menus[menuState]->show(true, 149);
	else if (debug == 1)
		Serial.println((int)Menus[menuState]);

	for (int v = 0; v < NUMVOICES; v++)
	{
		vcos1[v]->arbitraryWaveform(myWaveform, 172.0);
		sine21.arbitraryWaveform(myWaveform, 172.0);
		sine2.arbitraryWaveform(myWaveform, 172.0);
		sine1.arbitraryWaveform(myWaveform, 172.0);
		sine16.arbitraryWaveform(myWaveform, 172.0);
	}
	mixer1.gain(0, 0);
	mixer1.gain(1, 0);
	mixer1.gain(2, 1);
	mixer1.gain(3, 1);
	mixer3.gain(0, 1);
	dc9.amplitude(1);
	dc3.amplitude(1);
	filter1.octaveControl(2);
	prename = "PRESET" + String(recentPre) + ".PRS";
	if (preindex > 0)
		prename = presetnames[preindex];
	alpha4.begin(0x70); // pass in the address

	alpha4.writeDigitRaw(3, 0x0);
	alpha4.writeDigitRaw(0, 0xFFFF);
	alpha4.writeDisplay();
	delay(200);
	alpha4.writeDigitRaw(0, 0x0);
	alpha4.writeDigitRaw(1, 0xFFFF);
	alpha4.writeDisplay();
	delay(200);
	alpha4.writeDigitRaw(1, 0x0);
	alpha4.writeDigitRaw(2, 0xFFFF);
	alpha4.writeDisplay();
	delay(200);
	alpha4.writeDigitRaw(2, 0x0);
	alpha4.writeDigitRaw(3, 0xFFFF);
	alpha4.writeDisplay();
	delay(200);
	printA4("Hi!");
	for (int i = 0; i < 6; i++)
	{
		pinMode(leds[i], OUTPUT);
		if (debug == 1)
			Serial.println("led " + String(i));
		digitalWrite(leds[i], HIGH);
		delay(500);
	}
	if (debug == 1)
		Serial.println("Leds OK");

	AudioInterrupts();
}
unsigned long Rdeltat;
unsigned long myRTime;
unsigned long RlastTime;
void print(String out, int x, int y)
{
	display.setCursor(x, y);
	const char *outstr = out.c_str();
	display.setTextColor(1, 0);
	for (int16_t i = 0; i < (int)out.length(); i++)
	{
		char c = *outstr++;
		display.write(c);
	}
}
void loop()
{
	int to = 50;

	// bool isplaying = false;
	bool anyactive = false;
	if(Menus[SETTINGS]->byPass ==3)
	{
		AudioNoInterrupts();
		
	} 
	else
		AudioInterrupts();
	for (v = 0; v < NUMVOICES&&0; v++)
	{
		if (vcaenv[v]->isActive())
		{
			printA4(midiNamesFlat[v2note[v] % 12] + String(v2note[v] / 12));
			//			if (vcaenv[v]->isSustain())
			//				isplaying = true;
			//			break;
			anyactive = true;
			if (vtmillis[v] > 2000)
			{
				vcaenv[v]->noteOff();
				vcfenv[v]->noteOff();
				vtmillis[v] = 0;
				//	if (debug == 1)
				Serial.printf("**** removed:@ %f %d v=%d****\n", note_frequency[v2note[v]], v2note[v], v);
				usbMIDI.sendNoteOff(v2note[v], 0, _channel, cable);
				MIDI1.sendNoteOff(v2note[v], 0, _channel);
				MIDI2.sendNoteOff(v2note[v], 0, _channel);
				v2note[v] = -1;
				anyactive = false;
				onoff = 0;
			}
		}
	}
	digitalWrite(led, anyactive);

	//	if (!trigger)
	//		fclick1.setOn(false);
	if ((int)joy >= to)
	{
		checkTrill();
		for (int e = 0; e < 9; e++)
		{
			MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
			mes->probe();
		}
		joy = joy - to;
	}
	//			printA4(String(AudioMemoryUsageMax()));
	myusb.Task();
	midi1.read();
	MIDI_SER.read();
	for (int b = 0; b < 8; b++)
	{
		if (Buttons[b])
			Buttons[b]->loop();
	}
	//  if(debug==1)Serial.println("188 show");
	if (Menus[menuState])
		Menus[menuState]->show(refresh, 191);
	refresh = false;
#if 0
	if (isplaying && fft256_1.isActive() && 0)
	{
		int xi = 0;
		float fm = 0;
		int im = 0;
		for (int i = 0; i < 512; i++)
		{
			float x = fft256_1.read(i) * 256;
			if (x > 2)
			{
				//				Serial.println(String(i) + " " + String(x));
				if (xi == 0)
				{
					xi = i;
				}
				if (x > fm)
				{
					fm = x;
					im = i;
				}
			}
		}
		if (xi > 4)
			xi -= 4;
		display.clearDisplay();
		//float dx = 128.0 / (float)(256 - xi);
		//		Serial.println(String(fm));
		//		print("peak @ "+String(im*43)+"Hz",24, 0);

		for (int x = 0, xp = 0; x < 128; x++, xp++)
		{
			float y = fft256_1.read(x) * 256 * 20 / fm;
			//			float y1 = fft256_1.read(2 * x + 1) * 256 * 20 / fm;
			display.writeLine(xp, 63 - y, xp, 63, 1);
			//			display.writeLine(xp * 2 + 1, 63 - y1, xp * 2 + 1, 63, 1);
			if (2 * x == im)
				display.drawFastVLine(xp, 0, 63, 1);

			//			display.writeLine(x * 2 + 1, 63 - y, x * 2 + 1, 63, 1);
			if (y > fftmax[x])
			{
				fftmax[x] = y;
			}
			if (!xp)
				display.writeLine(0, 63 - fftmax[x] - 25, 0, 63 - fftmax[x] - 25, 1);
			else
				display.writeLine(xp - 1, 63 - fftmax[x - 1] - 25, xp, 63 - fftmax[x] - 25, 1);
		}
		display.display();
	}
#endif
}
