#include <Arduino.h>
#include <splash.h>
#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
void printA4(String);
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#include "base64.h"
#include <USBHost_t36.h>
#include <SD.h>
//#include <SPI.h>
#include <Wire.h>
#define CS_PIN 38
#define RST_PIN 10
#define DC_PIN 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
						 &SPI1, DC_PIN, RST_PIN, CS_PIN);
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
#include "webinter.h"
extern const int16_t myWaveform[256]; // defined in myWaveform.ino

// elapsedMillis fftreset;
signed char Menu::contrast = 1;
short Menu::isshowing = 0;
Parameter *Parameter::aHIPara = 0;
Parameter *Parameter::aLOPara = 0;
Parameter *Parameter::aFPara = 0;
Parameter *Parameter::aDPara = 0;
Parameter *Parameter::aRPara = 0;

// IntervalTimer midiMapTimer;
// IntervalTimer guitarHeroTimer;
// IntervalTimer volumeTimer;
// int MenuPara::AudioTG[54] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
// 							 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 							 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 							 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 							 0, 0, 0, 0, 0, 0, 0, 0, 0,
// 							 0, 0, 0, 0, 0, 0, 0, 0, 0};
// int MenuPara::CV1TG = 0;
// int MenuPara::CV2TG = 0;
// int MenuPara::StrumTG = 0;
signed char Menu::actSubmenu = 0;
signed char Menu::useBPM = 0;
signed char Menu::hiRes = 0;
signed char Menu::isRat = 0;
signed char Menu::procMode = 0;
signed char Menu::sendAfter = 0;
signed char Menu::strum = 0;
signed char Menu::noPanic = 0;
signed char Menu::triggerCC = 0;
signed char Menu::ratdiv = 2;
// const unsigned int inPort = 8888;
float Menu::BPM = 120;
byte mac[] = {
	0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

// Enter the IP address of the server you're connecting to:

bool MenuSynth::opencalled = false;
int led = 13;
extern int fx;
RotaryEncoder *Menu::encoder;
RotaryEncoder *Menu::paraencoder;
Encoder *Menu::myEnc;
elapsedMillis joy;
elapsedMillis web;
elapsedMillis midiplay;
#include "midiplay.h"
const int buttonPin = 15;		// the number of the pushbutton pin
const int ledPin = LED_BUILTIN; // the number of the LED pin
								// LED_BUILTIN is set to the correct LED pin independent of which board is used

// Variables will change:
int ledState = HIGH;	   // the current state of the output pin
int buttonState;		   // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;	// the debounce time; increase if the output flickers

void setup()
{
	FSTACK;
	AudioNoInterrupts();
	#if 1
	//	float a = 0.125, d = 0.125, s = 0.5, r = 0.75, M = 1024 * 32 - 1, A = 256 * a, R = 256 * r, S = M * s, D = d * 256;
 static byte leds[6] = {36, 34, 32, 30, 28, 13};
	File frec;
	Serial.begin(9600 * 16);
	pinMode(led, OUTPUT);
	Wire.begin();
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
	//	setoled();
	AudioMemory(400);
	STACK;
	//	Udp.begin(inPort);
	// Check for Ethernet hardware present
	//	EthernetClass::setSocketSize(10 * 1024);
	Ethernet.begin(mac, ip);
	STACK;
	if (Ethernet.hardwareStatus() == EthernetNoHardware)
	{
		FDBG("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
		while (true)
		{
			delay(1); // do nothing, no point running without Ethernet hardware
		}
	}
	if (Ethernet.linkStatus() == LinkOFF)
	{
		FDBG("Ethernet cable is not connected.");
	}
	FDBG("Ethernet connected.");
	Udp.begin(localPort);

	//	delay(5000);
	FDBG("waiting for UDP...");
	STACK;

	if (Udp.parsePacket())
	{
		int packetSize = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		if (packetSize)
		{
			Serial.print("Received packet of size ");
			DBG(packetSize);
			Serial.print("From ");
			server = Udp.remoteIP();
			webgui.connect(&client, server);
			String ipa = "";
			for (int i = 0; i < 4; i++)
			{
				ipa += String(server[i]);
				if (i < 3)
				{
					ipa += ".";
				}
			}
			// read the packet into packetBufffer
			Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
			//			DBG("Contents:");
			//			DBG(packetBuffer);
			Menus[VERSION]->insertItem("GUI");
			Menus[VERSION]->insertItem(ipa);

			// send a reply to the IP address and port that sent us the packet we received
		}
		STACK;
	}
	STACK;
	noise1.amplitude(0);
	if (!display.begin(SSD1306_SWITCHCAPVCC))
	{
		DBG(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}
	STACK;
	display.clearDisplay();
	display.setTextColor(1, 0);
	//	display.drawBitmap(0, 0, tmslogo, 128, 64, 1);
	STACK;
	display.display();
	//	delay(1000); // Pause for 1 seconds
	STACK;
	createStruct();
	MIDI_SER.begin(MIDI_CHANNEL_OMNI);
	MIDI1.begin();
	MIDI2.begin();
	digitalWrite(led, HIGH);

	//  Wire2.begin();
#if 1
	Serial.print("Initializing SD card...");
	pinMode(11, OUTPUT);
	if (!SD.begin(chipSelect))
	{

		FDBG("SD initialization failed!");
		return;
	}

	FDBG("initialization done.");

//	root = SD.open("/");
#endif

	SMF.begin(&SD.sdfs);
	//	SMF.load("Audio.mid");
	SMF.setMidiHandler(midiCallback);
	SMF.setSysexHandler(sysexCallback);
	FSTACK;
	clearPat();
	lastTime = millis();
	FSTACK;
	createmap();
	settrill();
	setButtons();
	printA4(__LINE__);
	STACK;
	DBG("Button set");
	setmidi();
	for (int i = 0; i < 84; i++)
		Menus[SCALES]->insertItem(scaleNames[scFP[i]]);
	STACK;
	// printDirectory(SD.open("/"), 0);
	for (int i = 3; i < Menus[MAPPINGS]->numitems && jj1 < 100; i++)
		opts[jj1++] = Menus[MAPPINGS]->items[i];
	j3 = 0;
	chordopt[0] = "Chord";
	for (int c = 11; c < 63; c++)
	{
		chordopt[c - 10] = chordName[chordIds[c]];
		chordopt[c - 10].replace(String((char)248), "&deg;").replace(String((char)171), "ø");
	}
	FDBG("Trying Restore");
	STACK;

	if (SD.exists("settings.tms"))
	{
		frec = SD.open("settings.tms", FILE_READ);
		if (frec)
		{

			FDBG("loading last data ");
			Restore(frec);

			FDBG("data loaded");
			frec.close();
		}
	}
	STACK;

	//	if (lastMap > 0 && lastMap < fx)
	//		Menus[MAPPINGS]->mapit(__CALLER__);
	int ia = 0;
	String ipa = "";
	while (1)
	{
		ipa += String(ip[ia], DEC);
		if (++ia >= 4)
			break;
		ipa += ".";
	}

	Menus[VERSION]->insertItem(ipa);

	//	websetup();

	if (Menus[SETTINGS]->procMode)
	{
		digitalWrite(28, HIGH);
		Buttons[5]->mlastButtonState = LOW;
		Buttons[5]->output = LOW;
	}
	menuState = MAIN;
	display.setTextColor(1, 0);
	STACK;
	if (Menus[menuState])
		Menus[menuState]->show(true, __CALLER__);
		//	else if (debug == 1)
		//		DBG((int)Menus[menuState]);

#include "synthsetup.h"
	STACK;
	root = SD.open("/");
	STACK;
	printDirectory(root, 0);
	STACK;
	actpattern = 0;
	loadDrum("TMS.drm");
	loadMap(lastLoadMap);
	#endif
}
#include "player.h"
int to = 100;
int to2 = 100;
int to3 = 10;
int loopc = to;
void loop()
{

	// bool isplaying = false;
#if 1
	// bool anyactive = false;
	myusb.Task();
	midi1.read();
	usbMIDI.read();
	MIDI_SER.read();
	if (transport == PLAYING || transport == REPEAT)
	{
		if (!playSeq )
		{
//			FDBG(SB(SMF.isEOF()));
			if (!SMF.isEOF())
			{
				if (SMF.getNextEvent())
					tickMetronome();
			}
			else
			{
				transport = STOPPED;
			}
		}
		else if ((int)midiplay >= to3 && lastEvent > 0)
		{
			//		FDBG("lastplay "+SN(lastplay));
			midiplay = midiplay - to3;
			//		printA4(midiplay);
			playnextMidi();
		}
	}
#if 0
	if (Menus[SETTINGS]->procMode == 3)
	{
		AudioNoInterrupts();
	}
	else
		AudioInterrupts();

	for (v = 0; v < NUMVOICES && 0; v++)
	{
		//		if (vcaenv[v]->isActive())
		{
			printA4(midiNamesFlat[v2note[v] % 12] + String(v2note[v] / 12));
			//			if (vcaenv[v]->isSustain())
			//				isplaying = true;
			//			break;
			//			anyactive = true;
			if (vtmillis[v] > 2000)
			{
				vcaenv[v]->noteOff();
				vcfenv[v]->noteOff();
				vtmillis[v] = 0;
				//
				Serial.printf("**** removed:@ %f %d v=%d****\n", note_frequency[v2note[v]], v2note[v], v);
				usbMIDI.sendNoteOff(v2note[v], 0, _channel, cable);
				MIDI1.sendNoteOff(v2note[v], 0, _channel);
				MIDI2.sendNoteOff(v2note[v], 0, _channel);
				v2note[v] = -1;
				//				anyactive = false;
				onoff = 0;
			}
		}
	}
#endif
	//	digitalWrite(led, anyactive);

	//	if (!trigger)
	//		fclick1.setOn(false);
	loopc--;
	if (loopc <= 0)
	{
		loopc = to;

		for (int e = 0; e < 9; e++)
		{
			MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
			mes->probe();
		}
		checkTrill();
		// read the state of the switch into a local variable:
		int reading = digitalRead(buttonPin);

		// check to see if you just pressed the button
		// (i.e. the input went from LOW to HIGH), and you've waited long enough
		// since the last press to ignore any noise:

		// If the switch changed, due to noise or pressing:
		if (reading != lastButtonState)
		{
			// reset the debouncing timer
			lastDebounceTime = millis();
		}

		if ((millis() - lastDebounceTime) > debounceDelay)
		{
			// whatever the reading is at, it's been there for longer than the debounce
			// delay, so take it as the actual current state:

			// if the button state has changed:
			if (reading != buttonState)
			{
				buttonState = reading;

				// only toggle the LED if the new button state is HIGH
				if (buttonState == HIGH)
				{
					ledState = !ledState;
				}
			}
		}

		// set the LED:
		digitalWrite(ledPin, ledState);

		// save the reading. Next time through the loop, it'll be the lastButtonState:
		lastButtonState = reading;
	}

	//			printA4(String(AudioMemoryUsageMax()));
	for (int b = 0; b < 7; b++)
	{
		if (Buttons[b])
			Buttons[b]->loop();
	}
#endif
	//	DBG((int)Menus[menuState]);
#if 1 
if (Menus[menuState])
	Menus[menuState]->show(false, __CALLER__);

	if ((int)web >= to2)
	{
		if (!foundserver)
			DBG("waiting for UDP...");
		if (Udp.parsePacket() && !foundserver)
		{
			int packetSize = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
			if (packetSize)
			{
				Serial.print("Received packet of size ");
				DBG(packetSize);
				Serial.print("From ");
				server = Udp.remoteIP();
				webgui.connect(&client, server);
				Menus[MAIN]->rot_pos = 1;
				websetup();
				for (int i = 0; i < 4; i++)
				{
					Serial.print(server[i], DEC);
					if (i < 3)
					{
						Serial.print(".");
					}
				}
				Serial.print(", port ");
				FDBG(Udp.remotePort());

				// read the packet into packetBufffer
				Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
				foundserver = true;
				String ipa = "";
				for (int i = 0; i < 4; i++)
				{
					ipa += String(server[i]);
					if (i < 3)
					{
						ipa += ".";
					}
				}
				Menus[VERSION]->insertItem("GUI");
				Menus[VERSION]->insertItem(ipa); // send a reply to the IP address and port that sent us the packet we received
			}
		}
		if (foundserver)
			webloop();
		web = web - to2;
	}
#endif
}
#include "tmsfunctions.h"