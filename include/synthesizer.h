#pragma once
signed char readrec(File frec);
float floatrec(File frec);
byte offs = 124;
void printA4(String);
class SubMenu
{
public:
	SubMenu(byte *psubitems, Parameter **pParas, byte id)
	{
		Paras = pParas;
		myID = id;
		subitems = psubitems;
		numsub = 0;
//		Serial.println(Paras[myID]->name);
		for (int s = 0; s < 16; s++)
		{
			if (subitems[s] > 0)
				numsub++;
			else
				break;
			Parameter *mPara = Paras[subitems[s]];
			//			Serial.println(mPara->name + " " + String(mPara->fvalue));
			if (Paras[myID]->name == "ADSR")
			{
				if (mPara->name.startsWith("VCF"))
					mPara->name.replace("VCF ", "");
				if (mPara->name.startsWith("VCA"))
					mPara->name.replace("VCA ", "");
			}
			if (Paras[myID]->name == "Wave")
			{
				if (mPara->name.startsWith("VCO"))
					mPara->name.replace("VCO ", "");
			}
			else
				mPara->name.replace(Paras[myID]->name + " ", "");
			if (mPara->name.endsWith("Shape"))
			{
				hasShape = true;
				shapeix = s;
			}
			//			if(debug==1)Serial.println("Shape?" + mPara->name + " " + String(shapeix));
		}
	}
	Parameter *actPara()
	{
		return subPara;
	}
	void displayValue(int line, Parameter *aPara)
	{
		byte soff = 0;
		String oval = aPara->format();
		if (Paras[myID]->name == "EQ")
		{
			display.setCursor(6, 7 * 8);
			//		print(String("                   "),false);
			print(aPara->name, false);
			print(":", false);
			print(oval, false);
			display.display();
			return;
		}
		if (Paras[myID]->name == "ADSR")
			soff = 3 * 8;
		else if (numsub < 7)
			soff = 5;
		display.setCursor(6, soff + (line)*8);
		//		print(String("                   "),false);
		print(aPara->name, false);

		display.setCursor(offs - 6 * oval.length(), 8 * (line) + soff);
		print(oval, true);
		display.display();
	}
	void displayMenu(int rot_pos)
	{
		//		printA4(String(myID));
		display.clearDisplay();
		rot_pos %= numsub + 1;
		display.setTextSize(1);				 // Normal 1:1 pixel scale
		display.setTextColor(SSD1306_WHITE); // Draw white text
		display.cp437(true);				 // Use full 256 char 'Code Page 437' font
		display.setCursor(0, 0);
		if (rot_pos == 0)
			display.setTextColor(0, 1);
		display.write(17);
		//		if (debug == 1)			Serial.println("myID " + String(myID));
		//		if (debug == 1)			Serial.println(Paras[myID]->name);

#if 1
		byte l = Paras[myID]->name.length();
		byte ml = 21;
		byte pad = (ml - l) / 2;
		byte soff = 0;

		if (Paras[myID]->name == "ADSR")
			pad += 3;
		display.setCursor(pad * 6, 2);
		print(Paras[myID]->name, false);
		//		display.writeFastHLine(0, 7, 128, 1);
		display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
		if (rot_pos == 0)
			subPara = Paras[myID];
		{
			if (Paras[myID]->name == "ADSR")
				soff = 24;
			else if (numsub < 7)
				soff = 5;
			for (int s = 0; s < numsub; s++)
			{
				display.setCursor(6, 8 * (s + 1) + soff);
				Parameter *mPara = Paras[subitems[s]];
				print(mPara->name, s + 1 == rot_pos);
				if (s + 1 == rot_pos)
					subPara = mPara;
				if (mPara->submenu || mPara->name.indexOf("Open") > -1)
				{
					display.setCursor(120, 8 * (s + 1) + soff);
					display.write(16);
				}
				else if (mPara->parafunction)
				{
					display.setCursor(120, 8 * (s + 1) + soff);
					display.write(15);
				}
				else
				{
					String oval = mPara->format();
					display.setCursor(offs - 6 * oval.length(), 8 * (s + 1) + soff);
					print(oval, s + 1 == rot_pos);
				}
			}
			if (Paras[myID]->name == "ADSR")
			{
				Parameter *APara = Paras[subitems[0]];
				Parameter *DPara = Paras[subitems[1]];
				Parameter *SPara = Paras[subitems[2]];
				Parameter *RPara = Paras[subitems[3]];
				float a = APara->fvalue;
				float d = DPara->fvalue;
				float s = SPara->fvalue;
				float r = RPara->fvalue;
				float tl = (64 - 16) / (a + d + r);
				//			if(debug==1)Serial.println("tl=" + String(tl));
				//			if(debug==1)Serial.println("A=" + String(a) + " D=" + String(d) + " S=" + String(s) + " R=" + String(r));
				//			if(debug==1)Serial.println("A=" + String(a*tl) + " D=" + String(d * tl) + " S=" + String(s) + " R=" + String(r));
				display.writeLine(0, 31, int(a * tl), 31 - 24, 1);
				display.writeLine(int(a * tl), 31 - 24, int(a * tl) + int(d * tl), 31 - s * 24, 1);
				display.writeLine(int(a * tl) + int(d * tl), 31 - s * 24, int(a * tl) + int(d * tl) + 16, 31 - s * 24, 1);
				display.writeLine(int(a * tl) + int(d * tl) + 16, 31 - s * 24, int(a * tl) + int(d * tl) + 16 + r * tl, 31, 1);
			}
			else if (hasShape)
			{
				drawShape();
			}
#endif
			if (comment.length() > 0)
			{
				display.setCursor(8, 8 * 7);
				print(comment);
			}
		}
		display.display();
	}
	byte bot = 63;
	byte top = 63 - 24;
	byte left = 8;
	byte right = 64 + 8;
	byte mid = 32 + 8;
	byte q1 = 16 + 8;
	byte q3 = 48 + 8;
	float f = 44.0 / 7 * 64;
	String comment = "";
	void drawShape(void)
	{
		byte shape = (byte)Paras[subitems[shapeix]]->fvalue;
		//		if(debug==1)Serial.println("Shape " + String(shape));
		switch (shape)
		{
		case WAVEFORM_SINE:
			for (int i = left; i < right; i++)
				display.drawPixel(i, 52 - 12 * sin((i - left) * f), 1);
			break;
		case WAVEFORM_SAWTOOTH:
		case 9:
			display.writeLine(left, bot, mid, top, 1);
			display.writeLine(mid, top, mid, bot, 1);
			display.writeLine(mid, bot, right, top, 1);
			display.writeLine(right, top, right, bot, 1);
			break;
		case WAVEFORM_SQUARE:
		case 11:
			display.writeLine(left, bot, left, top, 1);
			display.writeLine(left, top, mid, top, 1);
			display.writeLine(mid, top, mid, bot, 1);
			display.writeLine(mid, bot, right, bot, 1);
			display.writeLine(right, bot, right, top, 1);
			break;
		case WAVEFORM_TRIANGLE:
			display.writeLine(left, bot, q1, top, 1);
			display.writeLine(q1, top, mid, bot, 1);
			display.writeLine(mid, bot, q3, top, 1);
			display.writeLine(q3, top, right, bot, 1);
			break;
		case WAVEFORM_ARBITRARY:

			for (int i = left; i < right; i++)
				display.drawPixel(i, 52 - 3 * myWaveform[(i - left) * 4] / 8192, 1);
			break;
		case WAVEFORM_PULSE:
		case 12:
			display.writeLine(left, bot, left, top, 1);
			display.writeLine(left, top, q1, top, 1);
			display.writeLine(q1, top, q1, bot, 1);
			display.writeLine(q1, bot, right, bot, 1);
			display.writeLine(right, bot, right, top, 1);
			break;
		case WAVEFORM_SAWTOOTH_REVERSE:
		case 10:
			display.writeLine(left, bot, left, top, 1);
			display.writeLine(left, top, mid, bot, 1);
			display.writeLine(mid, bot, mid, top, 1);
			display.writeLine(mid, top, right, bot, 1);
			break;
		case WAVEFORM_TRIANGLE_VARIABLE:
			display.writeLine(left, bot, q1 / 2, top, 1);
			display.writeLine(q1 / 2, top, mid, bot, 1);
			display.writeLine(mid, bot, q3 - q1 / 2, top, 1);
			display.writeLine(q3 - q1 / 2, top, right, bot, 1);
			break;
		case WAVEFORM_SAMPLE_HOLD:
			byte y = 63 - random(24);
			byte y1 = 63 - random(24);
			for (int i = left; i < right; i += 4)
			{
				display.drawLine(i, y, i + 4, y, 1);
				display.drawLine(i + 4, y, i + 4, y1, 1);
				y = y1;
				y1 = 63 - random(24);
			}
			break;
		}
	}
	Parameter **Paras;
	Parameter *subPara = 0;
	byte myID;
	byte *subitems;
	byte numsub = 0;
	bool hasShape = false;
	byte shapeix = 0;
};
extern float32_t dbBand1[];

class MenuSynth : public MenuPara
{
public:
	MenuSynth(int type) : MenuPara(SYNTHSETTINGS, "Synth Setting", 0)
	{
		parent = MAIN;
		actSubmenu = submod;
		int p = 0;
		//		if(debug==1)Serial.println("cb:"+String( (int)filefunc,HEX));
		for (p = 1; p < NUMSYNTH; p++)
		{
			//			String spd=synthParas[p].toUpperCase().replace(" ","_");
				Paras[p] = new Parameter(synthParas[p], synparas[p]->endvalue, p, synparas[p]->dimension, synparas[p]->isFloat);
			//Serial.println("synparas["+spd+"]=new synPara("+spd+","+String(endvalue[p])+",\""+dimensions[p]+"\","+String(isFloat[p]?"true":"false")+");");
		}
		for (p = FILE_OPEN; p < FILE_OPEN +3; p++)
			Paras[p] = new Parameter(fileparas[p - FILE_OPEN], 0, p - FILE_OPEN + 1, filefunc);
		for (int s = 0; s < NUM_SUBMENUS; p++, s++)
		{
			Paras[p] = new Parameter(subMenus[s], subParent[s], p);
//					if(debug==1)	Serial.println(String(p) + ":" + String(s) + " -> " + subMenus[s] + " -> " + String(subParent[s]));
		}
		for (int s = 0, p = submod; s < NUM_SUBMENUS; s++, p++)
		{
			submenus[p] = new SubMenu((byte *)submenustructure[s], Paras, p);
			if(debug==1)
			Serial.println("sub menu " + String(p) + " -> " + submenus[p]->subitems[0]);
		}

		Paras[VCO_SHAPE]->setForm(shapes);
		Paras[FM_SHAPE]->setForm(shapes);
		Paras[PW_SHAPE]->setForm(shapes);
		Paras[AM_SHAPE]->setForm(shapes);
		Paras[VCF_SHAPE]->setForm(shapes);
		Paras[VCF_MODE]->setForm(vcftype);
		Paras[TRIGGER_1]->setForm(selected);
		Paras[TRIGGER_2]->setForm(selected);
		Paras[DIRECT]->setForm(selected);
		Paras[VCA]->setForm(selected);
		Paras[POLYMODE]->setForm(polymode);
		Paras[PORTAMENTOTYPE]->setForm(portatype);
		Parameter::aHIPara = Paras[VCF_HI];
		Parameter::aLOPara = Paras[VCF_LO];
		Parameter::aFPara = Paras[VCF_FREQ];
		Parameter::aDPara = Paras[VCO_DETUNE];
		MenuPara::SynthMenu = this;
		//		if(debug==1)Serial.println("SynthMenu");
		//		if(debug==1)Serial.println((int)MenuPara::SynthMenu);
	}
	void onBackButton(void)
	{
		printA4(String(actSubmenu));
		if (debug == 1)
			Serial.println(String(rot_pos) + " Back Synth click @" + String(Paras[actSubmenu]->parent));
		if (Paras[actSubmenu]->parent > 0)
		{
			submenus[actSubmenu]->displayMenu(0);
			actSubmenu = Paras[actSubmenu]->parent;
			submenus[actSubmenu]->displayMenu(0);
			saveTMS();
			if (debug == 1)
				Serial.println(String(rot_pos) + " Synth click parent " + String(actSubmenu));
			return;
		}
		else
		{
			actSubmenu = submod;
			menuState = MAIN;
			if (Menus[SETTINGS]->hiRes > 0)
			{
				Menus[HIRES]->myEnc->write(imap(Menus[MAIN]->rot_pos, 0, 24, 0, 10000));
			}

			saveTMS();
			return;
		}
	}

	void onButton(bool cancel) override
	{
		//		if (rot_pos > submenus[actSubmenu]->numsub)
		//			rot_pos = 0;
		actPara = submenus[actSubmenu]->actPara();
		//		if (debug == 1)
		Serial.println(String(submenus[actSubmenu]->numsub) + " " + String(rot_pos) + " Synth click @" + String(actSubmenu) + " " + actPara->name + " " + String(actPara->submenu) + " " + String(actPara->parent));
		//		if(debug==1)Serial.println(String(rot_pos) + " Synth click @" + String((int)submenus[actSubmenu]) );
		rot_pos = rot_pos % (submenus[actSubmenu]->numsub + 1);
		if (!rot_pos)
		{
			onBackButton();
			return;
		}
		else
		{
			if (actPara->submenu > 0)
			{
				actSubmenu = actPara->submenu;
				submenus[actSubmenu]->displayMenu(rot_pos);
				saveTMS();
				return;
			}
			if (actPara->parafunction > 0)
			{
				opencalled = false;
				actPara->parafunction(actPara->vstart);
				submenus[actSubmenu]->displayMenu(actPara->vstart);
				return;
			}
		}
		pararot_pos = 0;
		para_change = true;
		para_sel = !para_sel;
		if (para_sel)
		{
			pararot_pos = actPara->fvalue;
			paraencoder->setPosition(pararot_pos);
			oldval = actPara->fvalue;
			//			Serial.println("set enc " + String(actPara->isFloat ? "f" : "i") + "->" + String(actPara->fvend));
			if (!actPara->isFloat && !cancel && oldfvalue == -1)
				oldfvalue = oldval;
			if (Menus[SETTINGS]->hiRes > 0 && actPara->isFloat)
			{
				float tv = actPara->fvalue;
				if (oldfvalue == -1 && !cancel)
					oldfvalue = tv;
				int setv = fmap(tv, 0, actPara->fvend, 0, 10000);
				if (debug == 1)
					Serial.println("set enc " + String(tv) + "->" + String(oldfvalue));
				Menus[HIRES]->myEnc->write(setv);
			}
		}
		else if (cancel)
		{
			actPara->fvalue = oldfvalue;
			printA4(oldfvalue);
			oldfvalue = -1;
			show(true, 349);
		}
	}
	virtual void displayMenu(void) override
	{
		submenus[actSubmenu]->displayMenu(rot_pos);

		isshowing = myID;
		//		if(debug==1)Serial.println("showing " + items[ml-1]);
	}
	void pararot_test()
	{
		paraencoder->tick();
		int newPos = paraencoder->getPosition();
		pararot_dir = (int)paraencoder->getDirection();
		if (Menus[SETTINGS]->hiRes && !pararot_dir)
		{
			int val = Menus[HIRES]->rawvalue;
			pararot_dir = Menus[HIRES]->rawdir;
			newPos = map(val, 1, 10000, 0, 24);
			if (newPos == pararot_pos)
				pararot_dir = 0;
			if (pararot_dir && debug == 1)
			{
				Serial.print("pos:");
				Serial.print(pararot_pos);
				Serial.print(" new:");
				Serial.print(newPos);
				Serial.print(" dir:");
				Serial.println(pararot_dir);
			}
		}
		if (pararot_pos != newPos && pararot_dir)
		{
			pararot_pos = newPos;

			if (actPara)
			{
				//				if(debug==1)Serial.println(pararot_dir);
				if (pararot_pos > actPara->fvend)
					pararot_pos = actPara->fvstart;
				else if (pararot_pos < actPara->fvstart)
					pararot_pos = actPara->fvend;
			}

			paraencoder->setPosition(pararot_pos);
			//        if(debug==1)Serial.print("pos:");
			//        if(debug==1)Serial.print(pararot_pos);
			//        if(debug==1)Serial.print(" dir:");
			//        if(debug==1)Serial.println(pararot_dir);
		}
	}
	int oldval = -1;
	void show(bool force, int caller) override
	{
		nump = numitems - 1;

		static float foldval = -1;
		//		if(debug==1)Serial.println(String(actSubmenu) + " " + actPara->name + " " + String(actPara->submenu) + " " + String(opencalled ? 1 : 0));
		if (opencalled)
		{
			force = true;
			((MenuSynth *)SynthMenu)->submenus[actSubmenu]->comment = "opened " + presetnames[preindex];
			opencalled = false;
		}
		if (para_sel && actPara)
		{
			if (!actPara->isFloat)
			{
				pararot_test();
				pvalue = pararot_pos;
				if (oldval == pvalue)
					return;
				if (debug == 1)
					Serial.println("Rot " + String(pararot_pos) + " " + String(actPara->fvalue));
				oldval = pvalue;
				actPara->fvalue = pvalue;
				submenus[actSubmenu]->displayMenu(rot_pos);
				printA4(String(pvalue));
			}
			else
			{
				float val = 0;
				if (Menus[SETTINGS]->hiRes > 0)
				{
					//					float tv = *actPara->tvalue;
					//					float m=actPara->fvend/10000;
					//					int setv = tv*m;
					//					Serial.println("set enc "+String(m)+"->"+String(setv));
					//					Menus[HIRES]->myEnc->write(setv);
					val = Menus[HIRES]->rawvalue;
					val = fmap(val, 0, 10000, actPara->fvstart, actPara->fvend);
				}
				else
				{
					val = Menus[ZONESETTINGS]->rawvalue;
					val = fmap(val, 0, 1792, actPara->fvstart, actPara->fvend);
				}
				if (val == foldval)
					return;
				//				actPara->fvalue = fmap(val, 0, 1792, actPara->fvstart, actPara->fvend);

				actPara->fvalue = val;
				if (debug == 2)
					Serial.println(actPara->name + " = " + String(actPara->fvalue));
				foldval = val;
				printA4(String(val));
				submenus[actSubmenu]->displayMenu(rot_pos);
			}
			submenus[actSubmenu]->displayValue(rot_pos, actPara);
			actPara->setSynthVal();
			return;
		}

		if (!force)
		{
			rot_test();
			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && oldmenuState == menuState)
			{
				return;
			}
			rot_pos = abs(rot_pos);
			//			rot_pos %= 8;

			oldmenuState = menuState;
		}

		displayMenu();
	}
	void save(File frec)
	{
		Serial.println("Saving # " + String(NUMSYNTH) + " data");
		for (int p = 1; p < NUMSYNTH; p++)
		{
			if (debug == 2)
				Serial.println(Paras[p]->name + " = " + String(Paras[p]->fvalue));
			frec.write((char *)&Paras[p]->fvalue, 4);
		}
	}
	virtual void restore(File frec) override
	{
		Serial.println("Restoring # " + String(NUMSYNTH) + " data");
		for (int p = 1; p < NUMSYNTH; p++)
		{
			Paras[p]->fvalue = floatrec(frec);
			if(debug==1)Serial.println(Paras[p]->name + " = " + String(Paras[p]->fvalue));
			if (p != TRIGGER_1 && p != TRIGGER_2)
				Paras[p]->setSynthVal();
		}
#if 0
		for (int b = EQBAND1; b <= EQBAND16; b++)
		{
			Paras[b]->fvalue = dbBand1[b - EQBAND1];
			//			Serial.println("EQ band " + String(b - EQBAND1) + " " + String(Paras[b]->fvalue));
		}
#endif
	}
	static void filefunc(signed char type)
	{
		//		if (debug == 1)
		Serial.println("Callback " + String(type));
		if (type == 1)
		{
			menuState = PRESETS;
			opencalled = true;
			return;
		}
		//		String prename = "PRESET" + String(recentPre) + ".PRS";
		if (type == 3)
		{
			prename = "PRESET" + String(++actPre) + ".PRS";
			if (actPre > 99)
				actPre = 0;
			recentPre = actPre;
		}
		if (SD.exists(prename.c_str()))
		{
			SD.remove(prename.c_str());
			//    if(debug==1)Serial.println("ini deleted");
		}
		File frec = SD.open(prename.c_str(), FILE_WRITE);
		if (frec)
		{
			if (debug == 1)
				Serial.println(prename);
			MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
			mPara->save(frec);
			frec.close();
			if (debug == 1)
				Serial.println("Preset saved");
			((MenuSynth *)SynthMenu)->submenus[actSubmenu]->comment = "saved " + prename;
		}
	}
	SubMenu *submenus[100];
	static bool opencalled;
};
