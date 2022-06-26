extern short menuState;
extern MenuMidiSet *curMMS;

String midiPorts = {"USB,HOST,Serial"};
String externals = {"Joystick X,Joystick Y,Pot 1,Pot 2,Pedal 1,Pedal 2,FSR,Hi-Res,Switch"};
#include "globals.h"
void onButton(Menu *menu)
{
	//	)
	FDBG("ID " + String(menu->myID) + " " + menu->items[0] + " clicked @"+menu->rot_pos);
	if (menu->rot_pos == 0)
		onBackButton(menu);
	else
		menu->onButton(false);
}
void onBackButton(Menu *menu)
{
	// DBG("cancel Rat?" + String((int)Menu::isRat));
	if (Menu::isRat == 0)
	{
		DBG("cancel button " + String(menu->para_sel ? "sel" : "unsel")+"@"+SN(menu->rot_pos));
//		if (menuState == MAIN)
//		{
//			curMMS->targets->action(false, 2, curMMS->outCH, 0, 0, 123, 0, 0);
//		}
		if (!menu->para_sel)
		{
			if (menu != MenuPara::SynthMenu)
			{
				DBG(String(menu->myID) + " par " + String(menu->parent));
				menuState = menu->parent;
				Menu *newmenu = Menus[menuState];
				newmenu->rot_test();
//				DBG("rpp " + SN(newmenu->rot_pos));
				if (newmenu->rot_pos > newmenu->numitems || newmenu->rot_pos < 1)
					newmenu->rot_pos = 1;
				DBG("Back " + SN(Menus[menuState]->rot_pos) + " of " + SN(Menus[menuState]->numitems));
//				newmenu->encoder->setPosition(newmenu->rot_pos, __CALLER__);
				newmenu->para_change = false;
				//				if (Menus[SETTINGS]->hiRes > 0)
				//				{
				//					Menus[HIRES]->myEnc->write(imap(menu->rot_pos, 0, 24, 0, 10000));
				//				}
				saveTMS();
				Menus[menuState]->setMenu();
				return;
			}
			else
			{
				//				MenuPara::SynthMenu->rot_pos = 0;
				//				)
				DBG("onBackButton");
				//				((MenuSynth *)MenuPara::SynthMenu)->onBackButton();
				((MenuSynth *)MenuPara::SynthMenu)->onBackButton();
				return;
			}
		}
		// DBG("cancel button");
		if (menu != MenuPara::SynthMenu)
			((MenuPara *)menu)->onButton(true);
		else
			MenuPara::SynthMenu->onButton(true);
	}
}
void onShow(void)
{
	if (menuState > -1 && Menus[menuState])
		Menus[menuState]->show(true, __LINE__);
}
void dumpDisplay(void)
{
	#if 0
	uint8_t *buffer = display.getBuffer();
	int r = 0;
	if (menuState != SYNTHSETTINGS)
		DBG("unsigned char " + Menus[menuState]->items[0].replace(" ", "_") + String(Menus[menuState]->rot_pos) + "[1024]={");
	else
		DBG("unsigned char " + Menus[menuState]->items[0].replace(" ", "_") + String(((MenuSynth *)Menus[menuState])->actSubmenu) + "[1024]={");
	for (int n = 0; n < 1024; n++)
	{
		Serial.print("0x");
		Serial.printf("%02x", buffer[n], HEX);
		Serial.print(",");
		r++;
		if (r % 32 == 0)
			DBG();
	}
	DBG("};");
	#endif
}


void setButtons(void)
{
	Buttons[0] = new Button(37, 36, &octave, -1, -5, led1, 1);
	Buttons[1] = new Button(35, 34, &octave, 1, 5, led2, 0);
	Buttons[2] = new Button(33, 32, &semiTone, -1, -12, led3, 3);
	Buttons[3] = new Button(31, 30, &semiTone, 1, 12, led4, 2);
	Buttons[4] = new Button(7); //encoder
	Buttons[5] = new Button(29, 28, &Menus[SETTINGS]->procMode, 2, 3, led5, 5);
//	Buttons[6] = new Button(4);
	Buttons[6] = new Button(cancelButton);
//	Buttons[7] = new Button(12);
}

void createStruct(void)
{
#include "assigns.h"

	STACK;
	for (int m = 0; m < NUM_MENUS; m++)
	{
		Menus[m] = 0;
	}
	Menus[MAPPINGS] = new Menu(MAPPINGS, "Mappings", "Edit Map,Show Map,Chromatic");
//	Menus[MAPPINGS]->mapit();
//	filenames[3]="chromatic interval.map";
	Menus[NEWMAP] = new Menu(NEWMAP, "Edit Map", "", MAPPINGS);
	Menus[SCALES] = new Menu(SCALES, "Scales", "Find Scale,Show Scale");
	Menus[PRESETS] = new Menu(PRESETS, "Synth Preset", "", SYNTHSETTINGS);
	Menus[NEWSCALE] = new Menu(NEWSCALE, "Find Scale", "", SCALES);
	Menus[SHOWSCALE] = new Menu(SHOWSCALE, "Show Scale", "", SCALES);
	Menus[SHOWMAP] = new Menu(SHOWMAP, "Show Map", "", MAPPINGS);

	Menus[SETTINGS] = new MenuSet();

	Menus[VERSION] = new Menu(VERSION, "About", "Version 1.99, TMS");
	Menus[MIDISET] = new Menu(MIDISET, "MIDI In", midiPorts);
	Menus[EXTERNALS] = new Menu(EXTERNALS, "Controls", externals);
	Menus[TOUCH] = new MenuTouch();
	STACK;
	Menus[SYNTHSETTINGS] = new MenuSynth(SYNTHSETTINGS);
	String mainMS = "";
#define NUMMAIN 9
	for (int m = 1; m < NUMMAIN + 1; m++)
	{
		mainMS = mainMS + Menus[m]->items[0];
		mainMS = mainMS + ",";
	}

	Menus[MAIN] = new Menu(MAIN, "Main Menu", mainMS);

	DBG("MAIN " + String(MAIN));
	int t = TARGETS;
	int c = CALIBRATION;

	for (int e = 0; e <= EXT_SWITCH - EXTSETTINGS; e++, t++, c++)
	{
		Menus[t] = new MenuTargetSet(t, EXTSETTINGS + e);
		Menus[c] = new MenuCalibrateSet(c, EXTSETTINGS + e);
		Menus[EXTSETTINGS + e] = new MenuExtSet(e, (MenuTargetSet *)Menus[t], (MenuCalibrateSet *)Menus[c], extPins[e]);
	//	FDBG(Menus[TARGETS + e]->items[0] + " Target back to " + String(e));
		// if(debug==1)DBG(" MES " + String(EXTSETTINGS + e) + " MTS " + String(TARGETS + e));
	}
	// if(debug==1)DBG("EXTSETTINGS done " + String(TARGETS));

	Menus[SCALES]->setState(NEWSCALE, 2);
	Menus[MAPPINGS]->setState(NEWMAP, 2);
	Menus[EXTERNALS]->setState(EXTSETTINGS, 9);
	int e = 0;
	for (e = 0; e < 3; e++, t++)
	{
		Menus[t] = new MenuTargetSet(t, MIDISETTINGS + e);
		Menus[MIDISETTINGS + e] = new MenuMidiSet(e, (MenuTargetSet *)Menus[t]);
		//   if(debug==1)Serial.print(Menus[TARGETS + e+8]->items[0]+" Target back to "+Menus[Menus[TARGETS + e+8]->parent]->items[0]);
		//    if(debug==1)DBG(" MMS " + String(MIDISETTINGS + e) + " MTS " + String(TARGETS + e + 8));
	}
	Menus[MIDISET]->setState(MIDISETTINGS, 3);
	for (e = 0; e < 6; e++, t++)
	{
		Menus[t] = new MenuTargetSet(t, ZONESETTINGS + e);
		//		if(debug==1)DBG(String(e) + " Zones  " + String(t) + " Zones  " + String(ZONESETTINGS + e));
		Menus[ZONESETTINGS + e] = new MenuZoneSet(e, (MenuTargetSet *)Menus[t]);
		//		if(debug==1)DBG(String(e) + " Zones  " + String(t) + " Zones  " + String(ZONESETTINGS + e));
		//		if(debug==1)Serial.print(Menus[TARGETS + e+14]->items[0]+" Target back to "+Menus[Menus[TARGETS + e+14]->parent]->items[0]);
		//		if(debug==1)DBG(" MZS " + String(ZONESETTINGS + e) + " MTS " + String(TARGETS + e + 14));
	}

	//	if(debug==1)DBG("ZONESETTINGS " + String(t));
	//	if(debug==1)DBG("SynthMenu");
	//	if(debug==1)DBG((int)MenuPara::SynthMenu);

	Menus[TOUCH]->setState(ZONESETTINGS, 6);
	Menus[MAIN]->setState(MIDISET, NUMMAIN);
}
