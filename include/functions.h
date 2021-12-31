extern short menuState;
String midiPorts = {"USB,HOST,Serial"};
String externals = {"Pedal 1,Pedal 2,FSR,Switch,Joystick X,Joystick Y,Pot 1,Pot 2,Hi-Res Encoder,Back"};
#include "globals.h"
void onButton(Menu *menu)
{
	if (debug == 1)
		Serial.println("ID "+String(menu->myID)+" "+menu->items[0] + " clicked");
	menu->onButton(false);
}
void onBackButton(Menu *menu)
{
	// Serial.println("cancel Rat?" + String((int)Menu::isRat));
	if (Menu::isRat == 0)
	{
		// Serial.println("cancel button " + String(menu->para_sel ? "sel" : "unsel"));
		if (!menu->para_sel)
		{
			if (menu != MenuPara::SynthMenu)
			{
				menuState = menu->parent;
				menu->para_change = false;
				if (Menus[SETTINGS]->hiRes > 0)
				{
					
					Menus[HIRES]->myEnc->write(imap(menu->rot_pos,0,24,0,10000));
				}
				//			if(debug==1)Serial.println("par " + String(parent));
				saveTMS();
				return;
			}
			else
			{
				MenuPara::SynthMenu->rot_pos = 0;
				if (debug == 1)
					Serial.println("onBackButton");
				//				((MenuSynth *)MenuPara::SynthMenu)->onBackButton();
				Menus[menuState]->show(true, 31);
			}
		}
		// Serial.println("cancel button");
		if (menu != MenuPara::SynthMenu)
			((MenuPara *)menu)->onButton(true);
		else
			MenuPara::SynthMenu->onButton(true);
	}
}
void onShow(void)
{
	if (menuState > -1 && Menus[menuState])
		Menus[menuState]->show(true, 12);
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

void setButtons(void)
{
	Buttons[0] = new Button(37, 36, &octave, false, -5, &led1, 1);
	Buttons[1] = new Button(35, 34, &octave, true, 5, &led2, 0);
	Buttons[2] = new Button(33, 32, &semiTone, false, -12, &led3, 3);
	Buttons[3] = new Button(31, 30, &semiTone, true, 12, &led4, 2);
	Buttons[4] = new Button(7);
	Buttons[5] = new Button(29, &Menus[SETTINGS]->byPass);
	Buttons[6] = new Button(4);
	Buttons[7] = new Button(12);
}

void createStruct(void)
{
	synparas[AM_DEPTH] = new synPara(AM_DEPTH, 1.00, "", true);
	synparas[AM_RATE] = new synPara(AM_RATE, 10.00, "Hz", true);
	synparas[AM_SHAPE] = new synPara(AM_SHAPE, 8.00, "", false);
	synparas[DIRECT] = new synPara(DIRECT, 1.00, "", false);
	synparas[FM_DEPTH] = new synPara(FM_DEPTH, 1.00, "", true);
	synparas[FM_RATE] = new synPara(FM_RATE, 10.00, "Hz", true);
	synparas[FM_SHAPE] = new synPara(FM_SHAPE, 8.00, "", false);
	synparas[PW_DEPTH] = new synPara(PW_DEPTH, 1.00, "", true);
	synparas[PW_RATE] = new synPara(PW_RATE, 10.00, "Hz", true);
	synparas[PW_SHAPE] = new synPara(PW_SHAPE, 8.00, "", false);
	synparas[SAMPLE_AMP] = new synPara(SAMPLE_AMP, 1.00, "", true);
	synparas[SAMPLE_TIME] = new synPara(SAMPLE_TIME, 2000.00, "ms", true);
	synparas[TRIGGER_1] = new synPara(TRIGGER_1, 1.00, "", false);
	synparas[TRIGGER_2] = new synPara(TRIGGER_2, 1.00, "", false);
	synparas[VCA] = new synPara(VCA, 1.00, "", false);
	synparas[VCA_ATTACK] = new synPara(VCA_ATTACK, 2000.00, "ms", true);
	synparas[VCA_DECAY] = new synPara(VCA_DECAY, 2000.00, "ms", true);
	synparas[VCA_RELEASE] = new synPara(VCA_RELEASE, 2000.00, "ms", true);
	synparas[VCA_SUSTAIN] = new synPara(VCA_SUSTAIN, 1.00, "", true);
	synparas[VCF_ATTACK] = new synPara(VCF_ATTACK, 2000.00, "ms", true);
	synparas[VCF_DECAY] = new synPara(VCF_DECAY, 2000.00, "ms", true);
	synparas[VCF_DEPTH] = new synPara(VCF_DEPTH, 1.00, "", true);
	synparas[VCF_FREQ] = new synPara(VCF_FREQ, 2000.00, "Hz", true);
	synparas[VCF_HI] = new synPara(VCF_HI, 2500.00, "Hz", true);
	synparas[VCF_LO] = new synPara(VCF_LO, 500.00, "Hz", true);
	synparas[VCF_MODE] = new synPara(VCF_MODE, 3.00, "", false);
	synparas[VCF_Q] = new synPara(VCF_Q, 8.00, "", true);
	synparas[VCF_RATE] = new synPara(VCF_RATE, 10.00, "Hz", true);
	synparas[VCF_RELEASE] = new synPara(VCF_RELEASE, 2000.00, "ms", true);
	synparas[VCF_SHAPE] = new synPara(VCF_SHAPE, 8.00, "", false);
	synparas[VCF_SUSTAIN] = new synPara(VCF_SUSTAIN, 1.00, "", true);
	synparas[VCO_AMP] = new synPara(VCO_AMP, 1.00, "", true);
	synparas[VCO_DETUNE] = new synPara(VCO_DETUNE, 10.00, "Hz", true);
	synparas[VCO_FREQ] = new synPara(VCO_FREQ, 109.00, "Hz", true);
	synparas[VCO_MIX] = new synPara(VCO_MIX, 1.00, "", true);
	synparas[VCO_FMRANGE] = new synPara(VCO_FMRANGE, 12.00, "", true);
	synparas[VCO_RANGE] = new synPara(VCO_RANGE, 6.00, "", false);
	synparas[VCO_SHAPE] = new synPara(VCO_SHAPE, 12.00, "", false);
	synparas[VCO_SUB_MIX] = new synPara(VCO_SUB_MIX, 1.00, "", true);
	synparas[NOISE] = new synPara(NOISE, 1.00, "", true);
	synparas[PORTAMENTOTIME] = new synPara(PORTAMENTOTIME, 1000, "ms", true);
	synparas[PORTAMENTOTYPE] = new synPara(PORTAMENTOTYPE, 3, "", false);
	synparas[POLYMODE] = new synPara(POLYMODE, 1, "", false);
	for (int m = 0; m < NUM_MENUS; m++)
	{
		Menus[m] = 0;
	}
	Menus[MAPPINGS] = new Menu(MAPPINGS, "Mapping", "Show Chords");
	Menus[NEWMAP] = new Menu(NEWMAP, "Mapping" , MAPPINGS);
	Menus[SCALES] = new Menu(SCALES, "Scales", "Find scale");
	Menus[PRESETS] = new Menu(PRESETS, "Synth Preset", "", SYNTHSETTINGS);
	Menus[NEWSCALE] = new Menu(NEWSCALE, "Find scale", "", SCALES);
	Menus[SHOWSCALE] = new Menu(SHOWSCALE, "Scales", "", SCALES);
	Menus[SHOWMAP] = new Menu(SHOWMAP, "Mappings", "", MAPPINGS);

	Menus[SETTINGS] = new MenuSet();

	Menus[VERSION] = new Menu(VERSION, "Version", "1.10");
	Menus[MIDISET] = new Menu(MIDISET, "MIDI In", midiPorts);
	Menus[EXTERNALS] = new Menu(EXTERNALS, "Controller", externals);
	Menus[SAMPLES] = new Menu(SAMPLES, "Sample Select", "Samples 1,Samples 2");
	Menus[FSR] = new Menu(FSR, "Samples 1", "", SAMPLES);
	Menus[SWITCH] = new Menu(SWITCH, "Samples 2", "", SAMPLES);
	Menus[TOUCH] = new MenuTouch();
	Menus[SYNTHSETTINGS] = new MenuSynth(SYNTHSETTINGS);

	String mainMS = "";
	#define NUMMAIN 9
	for (int m = 1; m < NUMMAIN+1; m++)
	{
		mainMS = mainMS + Menus[m]->items[0];
		mainMS = mainMS + ",";
	}

	Menus[MAIN] = new Menu(MAIN, "Main Menu", mainMS);

	if (debug == 1)
		Serial.println("MAIN " + String(MAIN));
	int t = TARGETS;
	int c = CALIBRATION;

	for (int e = 0; e <= HIRES - EXTSETTINGS; e++, t++, c++)
	{
		Menus[t] = new MenuTargetSet(t, EXTSETTINGS + e);
		Menus[c] = new MenuCalibrateSet(c, EXTSETTINGS + e);
		Menus[EXTSETTINGS + e] = new MenuExtSet(e, (MenuTargetSet *)Menus[t], (MenuCalibrateSet *)Menus[c], extPins[e]);
		//if(debug==1)Serial.print(Menus[TARGETS + e]->items[0]+" Target back to "+Menus[Menus[TARGETS + e]->parent]->items[0]);
		//if(debug==1)Serial.println(" MES " + String(EXTSETTINGS + e) + " MTS " + String(TARGETS + e));
	}
	// if(debug==1)Serial.println("EXTSETTINGS done " + String(TARGETS));

	Menus[SAMPLES]->setState(FSR, 2);
	Menus[SCALES]->setState(NEWSCALE, 2);
	Menus[MAPPINGS]->setState(NEWMAP, 2);
	Menus[EXTERNALS]->setState(EXTSETTINGS, 9);
	int e = 0;
	for (e = 0; e < 3; e++, t++)
	{
		Menus[t] = new MenuTargetSet(t, MIDISETTINGS + e);
		Menus[MIDISETTINGS + e] = new MenuMidiSet(e, (MenuTargetSet *)Menus[t]);
		//   if(debug==1)Serial.print(Menus[TARGETS + e+8]->items[0]+" Target back to "+Menus[Menus[TARGETS + e+8]->parent]->items[0]);
		//    if(debug==1)Serial.println(" MMS " + String(MIDISETTINGS + e) + " MTS " + String(TARGETS + e + 8));
	}
	Menus[MIDISET]->setState(MIDISETTINGS, 3);
	for (e = 0; e < 6; e++, t++)
	{
		Menus[t] = new MenuTargetSet(t, ZONESETTINGS + e);
		//		if(debug==1)Serial.println(String(e) + " Zones  " + String(t) + " Zones  " + String(ZONESETTINGS + e));
		Menus[ZONESETTINGS + e] = new MenuZoneSet(e, (MenuTargetSet *)Menus[t]);
		//		if(debug==1)Serial.println(String(e) + " Zones  " + String(t) + " Zones  " + String(ZONESETTINGS + e));
		//		if(debug==1)Serial.print(Menus[TARGETS + e+14]->items[0]+" Target back to "+Menus[Menus[TARGETS + e+14]->parent]->items[0]);
		//		if(debug==1)Serial.println(" MZS " + String(ZONESETTINGS + e) + " MTS " + String(TARGETS + e + 14));
	}

	//	if(debug==1)Serial.println("ZONESETTINGS " + String(t));
	//	if(debug==1)Serial.println("SynthMenu");
	//	if(debug==1)Serial.println((int)MenuPara::SynthMenu);

	Menus[TOUCH]->setState(ZONESETTINGS, 6);
	Menus[MAIN]->setState(MIDISET, NUMMAIN);
}
