#pragma once
signed char readrec(File frec);
float floatrec(File frec);
byte offs = 124;
void printA4(String);
#include "submenu.h"
#include <NativeEthernet.h>
extern short idt, bp, synthmen[3], men, gra, cped, ledout, semp, semm, octp, octm, idt1, idt2, showstat, idjx, idjy, smen;
extern String showShape(int sp);
extern String showADSR(int);
extern void loadPRS(String);
class MenuSynth : public MenuPara
{
public:
	MenuSynth(int type) : MenuPara(SYNTHSETTINGS, "Synth Settings", 0)
	{
		parent = MAIN;
		actSubmenu = submod;
		int p = 0;
		//		DBG("cb:"+String( (int)filefunc,HEX));
		STACK;
		for (p = 0; p < NUMSYNTH; p++)
		{
			Paras[p] = new Parameter(synthParas[p], synparas[p]);
			//			DBG(String(p) + " " + String((int)Paras[p]));
		}
		STACK;
		for (p = FILE_OPEN; p < FILE_OPEN + 3; p++)
			Paras[p] = new Parameter(fileparas[p - FILE_OPEN], 0, p - FILE_OPEN + 1, filefunc);
		for (int s = 0; s < NUM_SUBMENUS; p++, s++)
		{
			Paras[p] = new Parameter(subMenus[s], subParent[s], p);
			//						DBG(String(p) + ":" + String(s) + " -> " + subMenus[s] + " -> " + String(subParent[s]));
		}
		for (int s = 0, p = submod; s < NUM_SUBMENUS; s++, p++)
		{
			submenus[p] = new SubMenu((byte *)submenustructure[s], Paras, p);

			DBG(" sub menu " + String(p) + " -> " + submenus[p]->subitems[0] + String(s));
		}
		//		FDBG("Size " + SN(sizeof(shapes)));
		Paras[VCO_SHAPE]->setForm(shapes, 0, 13);
		Paras[SUBSHAPE]->setForm(shapes, 0, 13);
		Paras[DETSHAPE]->setForm(shapes, 0, 13);
		Paras[FM_SHAPE]->setForm(shapes, 0, 13);
		Paras[PM_SHAPE]->setForm(shapes, 0, 13);
		Paras[AM_SHAPE]->setForm(shapes, 0, 13);
		Paras[VCF_SHAPE]->setForm(shapes, 0, 13);
		Paras[VCF_MODE]->setForm(vcftype, 0, 5);
		Paras[DIRECT]->setForm(selected, 0, 2);
		Paras[POLYMODE]->setForm(polymode, 0, 2);
		Paras[PORTAMENTOTYPE]->setForm(portatype, 0, 3);
		Paras[TRIGGER_1]->setForm(wavenames, 0, 100);
		Paras[TRIGGER_2]->setForm(wavenames, 0, 100);
		Parameter::aHIPara = Paras[VCF_HI];
		Parameter::aLOPara = Paras[VCF_LO];
		Parameter::aFPara = Paras[VCF_FREQ];
		Parameter::aDPara = Paras[VCO_DETUNE];
		Parameter::aRPara = Paras[ADSR_MAG];
		MenuPara::SynthMenu = this;
		//		DBG("SynthMenu");
		//		DBG((int)MenuPara::SynthMenu);
	}
	int getnumItems(void)
	{
		//		DBG(String((int)this, HEX) + " -> " + String((int)submenus[actSubmenu], HEX));
		return submenus[actSubmenu]->numsub;
	}
	bool isSlider(int k)
	{
		int i = id2synth[k];
		//		DBG(SN(i) + " " + Paras[i]->name + SN(Paras[i]->isFloat ? "f" : "i"));
		return Paras[i]->isFloat;
	}
	void checkControl(int type, int id, int select = 0, float value = 0, bool swval = false)
	{
		//		DBG(SN(type) + " : " + SN(id) + " " + SN(value) + " " + id2para[id]);
		//		return;
		int sp = id2para[id];
		Parameter *para = Paras[sp];
		//		int posy = 100;
		//		int option = value + 1;
		switch (type)
		{
		case SLIDER:
			para->fvalue = value;
			if (sp == ADSR_MAG)
			{
				for (int s = 1; s < 9; s++)
				{
					byte asp = id2synth[s];
					Parameter *apara = Paras[asp];
					if (apara->name.indexOf("Att") > -1 || apara->name.indexOf("Dec") > -1 || apara->name.indexOf("Rel") > -1)
					{
						apara->setSynthVal();
						webgui.setMonitor(apara->monid, apara->format());
					}
				}
			}
			webgui.setMonitor(id - 1, para->format());
			para->setSynthVal();
			if (para->name.indexOf("Att") > -1 || para->name.indexOf("Dec") > -1 || para->name.indexOf("Rel") > -1 || para->name.indexOf("Sus") > -1)
			{
				if (actSubmenu == MENU_VCA)
					webgui.setMonitor(synthmen[para->sc], showADSR(2));
				else if (actSubmenu == MENU_VCF)
					webgui.setMonitor(synthmen[para->sc], showADSR(1));
				else
					webgui.setMonitor(synthmen[para->sc], showADSR(0));
			}
			break;
		case OPTION:
			//			FDBG("option: " + SN(id) + " " + SN(sp) + " " + SN(value));
			// if (!sp)
			// {
			// 	FDBG("Open " + presetnames[option] + ".PRS");
			// 	loadPRS(presetnames[option]);
			// 	preindex = option;
			// 	return;
			// }
			para->fvalue = value;
			para->setSynthVal();
			//			if (para->fvalue)
			{
				//				int s = ((MenuSynth *)MenuPara::SynthMenu)->getValue(sp);
				//				webgui.remove(synthmen[para->sc]);
				//				synthmen[para->sc] = webgui.addStringDisplay(para->name + " " + shapes[s], 150, 100 + para->sc * 250);
				//				webgui.setMonitor(synthmen[para->sc], showShape(sp));
			}
			break;
		case CHECKBOX:
			//			DBG(SN(id) + " switched ");
			para->fvalue = swval;
			para->setSynthVal();
			break;
		}
	}
	void setLength(int myID)
	{
		if (lmyid == myID)
			return;
		lmyid = myID;

		int s = pmstart[myID];
		int e = pmend[myID];
		for (int k = s; k < e; k++)
		{
			int i = id2synth[k];
			String name = synthParas[i];
			int l = name.length();
			int lp = 0;
			if (l > maxiteml)
				maxiteml = l;
			if (Paras[i] != 0)
			{
				lp = Paras[i]->setLength();
				if (lp > maxvall)
					maxvall = lp;
			}
		}
		//		DBG(items[0] + " il: " + SN(maxiteml) + " ,vl: " + SN(maxvall));
	}
	int lmyid = 0;
	int orp = -1;
	int omyID;
	void setMonitor(void)
	{
		//		if (rot_pos == orp)
		//			return;
		orp = rot_pos;
		int actsb = actSubmenu;
		SubMenu *sb = submenus[actsb];
		omyID = sb->myID;
		String name = sb->Paras[sb->myID]->name;

		setLength(sb->myID);
		int widthl = maxiteml * 6;
		int widthr = maxvall * 6;
		int width = widthr + widthl;
		if (width < 150)
			width = 150;
		for (int i = 0; i < 20 && 0; i++)
		{
			if (menuId[i] > 0)
				webgui.remove(i);
			menuId[i] = -1;
		}

		sb->showMenu(rot_pos);
		String outmen = " <style>  table td  {  font-size : 14px; height:25px; width: " + String(width) + "px; } </style> ";
		outmen += "<style>.alnleft {font-size : 14px;text-align : left;  width: " + String(widthl) + "px ;}</style> ";
		outmen += "<table>";

		outmen += synthstr + "</table";
		if (sb->myID == subVCO || sb->myID == subVCA || sb->myID == subVCF)
		{
			webgui.remove(men);
			webgui.remove(smen);
			return;
		}
		if (sb->myID == subKeys || sb->myID == subSample)
		{
			webgui.remove(men);
			webgui.setMonitor(smen, outmen);
		}
		else
			webgui.remove(smen);
		if (synthstr.length() > 0)
			webgui.setMonitor(men, outmen);
		//		FDBG("outmen.length()");
		//		FDBG(outmen.length());
		if (outmen.length() < 250)
			webgui.remove(men);
	}
	String synthstr;
	void createControl(int myID)
	{
		int s = pmstart[myID];
		int e = pmend[myID];
		int base = 60;
		int shape = pmshape[myID];
		bool sws[20];
		int posx = base;
		int posy = 100;
		String swn = "";
		if (lmyid != myID)
			setLength(myID);
		synthstr = "";
		lmyid = myID;
		int widthl = maxiteml * 6;
		int widthr = maxvall * 6;
		//		DBG("right geometry: " + SN(widthl + widthr));
		int ns = 0;
		String outmen = "";
		maxg = 0;
		posx += widthl + widthr;

		int sci = 0;
		STACK;
		for (int k = s; k < e; k++)
		{
			int i = id2synth[k];
			String name = synthParas[i];
			//			FDBG("create " + name+" "+SN(i));
			if (Paras[i] == 0 || Paras[i]->form == 0)
				continue;
			String fx;
			int t = getType(i, fx) + 1;

			//			FDBG(SN(i) + " " + Paras[i]->name + " " + Paras[i]->form[0] + " " + SN(sci)+" "+SN(t)+" "+fx);
			int si = Paras[i]->getValue();
			String title = "f";
			if (t > 2 && t < 20)
			{
				int posx = 250 + sci * 260;
				int posy = 400;
				if (myID == MENU_VCF)
				{
					if (k == 28 + 4)
					{
						posx = 600;
						posy = 550;
					}
					if (k == 25 + 4)
					{
						posx = 1400;
						posy = 400;
						//						posx = 100;
						//						posy = 400;
					}
					//					FDBG(SN(k) + " " + name + " " + SN(posx) + "," + SN(posy));
				}
				if (myID == MENU_VCA)
				{
					if (k == 40 + 4)
					{
						posx = 600;
						posy = 550;
					}
				}
				if (myID == MENU_VCO)
				{
					if (k == 17)
					{
						posx = 1350;
						posy = 420;
					}
					if (k == 18)
					{
						posx = 1350;
						posy = 100;
						title = "title";
					}
					if (k == 16)
					{
						posx = 1350;
						posy = 700;
						title = "title";
					}
					else if (k == 21)
					{
						posx = 600;
						posy = 250;
					}
					else if (k == 24)
					{
						posx = 600;
						posy = 550;
					}
					//					FDBG(SN(myID) + " " + SN(k) + " " + name + " " + SN(posx) + "," + SN(posy) + " " + SN(sci) + " " + SN(synthmen[sci]));
				}
				else if (myID == MENU_KEYS)
				{
					posx = 420;
					posy = 150;
					title = "title";
				}
				//					FDBG(SN(myID) + " " + SN(k) + " " + name + " " + SN(posx) + "," + SN(posy) + " " + SN(sci) + " " + SN(synthmen[sci]) );
				STACK;
				webgui.remove(synthmen[sci]);
				String sh = showShape(i);
				synthmen[sci] = webgui.addStringDisplay(synthParas[i], posx - 70, posy + 50, "titlex");
				webgui.setMonitor(synthmen[sci], sh);
				webgui.remove(guiid[maxg]);
				guiid[maxg] = webgui.addOptions(Paras[i]->name, Paras[i]->vend + 1, (String *)Paras[i]->form, &onOptionSelect, posx, posy, si, title, "nomonitor"); //*Paras[i]->value);
																																									//				FDBG("set option " + SN(guiid[maxg]));																													//				FDBG(SN(synthmen[sci]) + " " + SN(guiid[maxg]) );
				id2para[guiid[maxg]] = i;
				Paras[i]->sc = sci;
				sci++;
				maxg++;
			}
			if (t > 20)
			{
				wavenames[0] = "Select";
				webgui.remove(guiid[maxg]);
				guiid[maxg] = webgui.addOptions(Paras[i]->name, maxsamples, wavenames, &onOptionSelect, 450, posy + 40 + (i - 13) * 80, si, "title"); //*Paras[i]->value);
																																					  //				FDBG(Paras[i]->name + " " + SN(maxsamples) + " " + SN(i) + " " + SN(guiid[maxg]));
				id2para[guiid[maxg]] = i;
				Paras[i]->sc = sci;
				sci++;
				maxg++;
			}
		}
		STACK;
		if (shape == -1)
		{
			webgui.remove(synthmen[sci]);
			synthmen[sci] = webgui.addStringDisplay("VCF and VCA ADSR", 248, posy + 260, "f");
			webgui.setMonitor(synthmen[0], showADSR(0));
			posx = 200;
		}

		for (int k = s; k < e; k++)
		{
			int i = id2synth[k];
			String name = synthParas[i];
			if (Paras[i] == 0)
				continue;
			if (Paras[i]->form != 0 && Paras[i]->vend > 1)
				continue;
			if (Paras[i]->isAnalog)
			{
				ns++;
			}
			else
			{
				name = name.replace("FFM ", "");
				name = name.replace("AM ", "");
				name = name.replace("PM ", "");
				name = name.replace("FM ", "");
				name = name.replace("VCO ", "");
				name = name.replace("Poly ", "");
				name = name.replace("Portam. ", "");
				name = name.replace("Sample", "");
				name = name.replace("ADSR", "");
				name = name.replace("VCA ", "").replace("VCF ", "");
				synthstr += "<tr>";
				synthstr += "<td> " + name + "</td>";
				//				synthstr += "<td class=\'alnright\'> &nbsp;</td>";
				synthstr += "</tr>";
			}
		}
		maxg = 0;
		int spx = posx, spy = posy, sf = 0;
		STACK;
		for (int k = s; k < e; k++)
		{
			int i = id2synth[k];
			if (!Paras[i]->isFloat)
				continue;
			String name = synthParas[i];
			//			DBG(name);
			if (k != 1 && k != 5)
			{
				name = name.replace("VCA ", "").replace("VCF ", "");
			}

			name = name.replace("VCO ", "");
			name = name.replace("FFM ", "");
			name = name.replace("Poly ", "");
			name = name.replace("Portam. ", "");
			name = name.replace("Sample", "");
			name = name.replace("ADSR", "");
			if (myID == MENU_VCO)
			{
				if (k <= 17)
				{
					spy = 350;
					spx = 700;
				}
				else if (k <= 20)
				{
					spy = 500;
					spx = 100;
					sf = k - 17;
				}
				else
				{
					spy = 200;
					spx = 100;
					sf = k - 20;
				}
				//				FDBG(SN(k) + " " + name + " " + SN(spx) + "," + SN(spy));
			}
			if (myID == MENU_KEYS)
				spx = 200;
			if (myID == MENU_VCF)
			{
				//
				if (k <= 24 + 4)
				{
					spy = 350;
					spx = 750;
				}
				else if (k <= 27 + 4)
				{
					spy = 500;
					spx = 100;
					sf = k - 24 - 4;
				}
				else
				{
					spy = 200;
					spx = 20;
					sf = k - 27 - 4;
					if (k == 29 + 4)
					{
						webgui.remove(synthmen[sci]);
						synthmen[sci] = webgui.addStringDisplay("VCF and VCA ADSR", 570, spy, "f");
						webgui.setMonitor(synthmen[sci], showADSR(1));
					}
					Paras[i]->sc = sci;
				}
			}
			if (myID == MENU_VCA)
			{
				//				FDBG(SN(myID) + " " + SN(k) + " " + name + " " + SN(posx) + "," + SN(posy));
				if (k <= 36 + 4)
				{
					spy = 200;
					spx = 180;
					if (k == 33 + 4)
					{
						webgui.remove(synthmen[sci]);
						synthmen[sci] = webgui.addStringDisplay("VCF and VCA ADSR", 390, spy, "f");
						webgui.setMonitor(synthmen[sci], showADSR(2));
					}
					Paras[i]->sc = sci;
				}
				else if (k <= 37 + 4)
				{
					spy = 350;
					spx = 750;
					sf = k - 36 - 4;
				}
				else
				{
					spy = 500;
					spx = 200;
					sf = k - 37 - 4;
				}
			}
			else if (shape == -1)
				Paras[i]->sc = sci;
			if (shape == -1)
			{
				//				FDBG(name + " " + SN(k) + " " + SN(spx));
				if (k == 1 || k == 5)
					spx += 40;
			}
			webgui.remove(Paras[i]->monid);
			int ret = webgui.addNumericDisplay(name, spx + 60 + sf * 80, spy + 200, "f", "nomonitor");
			Paras[i]->monid = ret;
			webgui.setMonitor(ret, Paras[i]->format());
			webgui.remove(guiid[maxg]);
			guiid[maxg] = webgui.addInputAnalog(name, 0, Paras[i]->fvend, Paras[i]->fvalue, &onSlider, spx + sf * 80, spy, "title", "controlx");
			if (guiid[maxg] == -1)
				guiid[maxg] = webgui.addInputAnalog(name, 0, Paras[i]->fvend, 0, &onSlider, spx + sf * 80, spy, "title", "controlx");
			//			FDBG(SN(k) + " " + name + " " + SN(spx + sf * 80) + "," + SN(spy));
			id2para[guiid[maxg]] = i;
			maxg++;
			sf++;
		}
		STACK;
		int inpx = 250 + 300;
		int inph = 29;
		int inpy = 100 + 10;
		int j = 0;
		if (myID == MENU_VCF)
			j = 2;
		if (myID == MENU_VCO)
			j = 3;
		for (int k = s; k < e; k++)
		{
			int i = id2synth[k];
			if (Paras[i]->isFloat)
				continue;
			//			DBG(Paras[i]->name + " " + Paras[i]->vend);
			if (Paras[i]->vend == 1)
			{
				//				DBG(SN(i) + " " + SN(j) + " " + Paras[i]->name);
				sws[0] = Paras[i]->getValue();
				webgui.remove(guiid[maxg]);
				guiid[maxg] = webgui.addSwitches(Paras[i]->name, 1, sws, &onSwitches, inpx, inpy + j++ * inph, "f", "nomonitor");
				id2para[guiid[maxg]] = i;
				maxg++;
			}
		}
		STACK;
		return;
	}

	float getValue(int what)
	{
		if (what < 0 || what > NUMSYNTH)
			return -1;
		return Paras[what]->fvalue;
	}
	void setValue(int what, float val)
	{
		if (what < 0 || what > NUMSYNTH)
			return;
		Paras[what]->fvalue = val;
		Paras[what]->setSynthVal();
	}
	int getType(int what, String &fo)
	{
		if (what < 0 || what > NUMSYNTH)
			return -1;
		if (Paras[what]->form != 0)
		{
			fo = Paras[what]->form[(int)Paras[what]->fvalue];
			return Paras[what]->vend;
		}
		return Paras[what]->vend;
	}
	void onBackButton(void)
	{
		printA4(String(actSubmenu));
		//
		orp = -1;
		//		DBG(String(submenus[actSubmenu]->rot_pos) + "********************* Back Synth click @" + String(Paras[actSubmenu]->parent));
		if (Paras[actSubmenu]->parent > 0)
		{
			//			submenus[actSubmenu]->displayMenu(0);
			actSubmenu = Paras[actSubmenu]->parent;
			//			DBG(SN(submenus[actSubmenu]->myID) + " " + SN(rot_pos));
			rot_pos = submenus[actSubmenu]->rot_pos;
			encoder->setPosition(rot_pos, __CALLER__);
			submenus[actSubmenu]->displayMenu(0);
			saveTMS();

			DBG(String(rot_pos) + " Synth click parent " + String(actSubmenu));
			return;
		}
		else
		{
			actSubmenu = submod;
			menuState = MAIN;
			isshowing = MAIN;
			if (Menus[SETTINGS]->hiRes > 0)
			{
				Menus[HIRES]->myEnc->write(imap(Menus[MAIN]->rot_pos, 0, 24, 0, 10000));
			}

			saveTMS();
			return;
		}
	}

	void onButton(bool cancel = false) override
	{
		//		if (rot_pos > submenus[actSubmenu]->numsub)
		//			rot_pos = 0;
		actPara = submenus[actSubmenu]->actPara(rot_pos - 1, actSubmenu);
		orp = -1;
		if (0)
		{
			FDBG(String(submenus[actSubmenu]->numsub) + " " + String(rot_pos) + " Synth click @" + String(actSubmenu) + " " + actPara->name + " " + String(actPara->submenu) + " " + String(actPara->parent));
			FDBG(String((int)actPara->parafunction));
			FDBG(submenus[actSubmenu]->subitems[0]);
		}
		//		rot_pos = rot_pos % (submenus[actSubmenu]->numsub + 1);
		if (!rot_pos)
		{
			onBackButton();
			return;
		}
		else
		{
			if (actPara->submenu > 0)
			{
				submenus[actSubmenu]->rot_pos = rot_pos;
				actSubmenu = actPara->submenu;
				//				DBG(actSubmenu);
				rot_pos = 1;
				encoder->setPosition(rot_pos, __CALLER__);
				//				if(actSubmenu!=subKeys)
				submenus[actSubmenu]->displayMenu(rot_pos);
				saveTMS();
				return;
			}
			if (actPara->parafunction > 0)
			{
				opencalled = false;
				actPara->parafunction(actPara->vstart);
				//				DBG("calling display " + String(actSubmenu) + " " + Paras[submenus[actSubmenu]->myID]->name);
				//				if (!opencalled)
				submenus[actSubmenu]->displayMenu(actPara->vstart);
				//				DBG("changing display to " + String(menuState));
				return;
			}
		}
		pararot_pos = 0;
		para_change = true;
		para_sel = !para_sel;
		if (para_sel)
		{
#if 1
			pararot_pos = actPara->getValue();
			paraencoder->setPosition(pararot_pos, __CALLER__);
			oldval = actPara->getValue();
			//			DBG("set enc " + String(actPara->isFloat ? "f" : "i") + "->" + String(actPara->fvend));
			if (!actPara->isFloat && !cancel && oldfvalue == -1)
				oldfvalue = oldval;
			if (Menus[SETTINGS]->hiRes > 0 && actPara->isFloat)
			{
				float tv = actPara->getValue();
				if (oldfvalue == -1 && !cancel)
					oldfvalue = tv;
				int setv = fmap(tv, 0, actPara->fvend, 0, 10000);

				//				FDBG("set enc " + String(tv) + "->" + String(oldfvalue));
				Menus[HIRES]->myEnc->write(setv);
			}
#endif
		}
		else if (cancel)
		{
			actPara->fvalue = oldfvalue;
			printA4(oldfvalue);
			oldfvalue = -1;
			show(true, __LINE__);
		}
	}
	virtual void displayMenu(String line) override
	{
		submenus[actSubmenu]->displayMenu(rot_pos);

		isshowing = myID;
		//		DBG("showing " + items[ml-1]);
	}
	void pararot_test()
	{
		paraencoder->tick();
		int newPos = paraencoder->getPosition(__CALLER__);
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
				DBG(pararot_dir);
			}
		}
		if (pararot_pos != newPos && pararot_dir)
		{
			pararot_pos = newPos;

			if (actPara)
			{
				//				DBG(pararot_dir);
				if (pararot_pos > actPara->fvend)
					pararot_pos = actPara->fvstart;
				else if (pararot_pos < actPara->fvstart)
					pararot_pos = actPara->fvend;
			}

			paraencoder->setPosition(pararot_pos, __CALLER__);
			//        Serial.print("pos:");
			//        Serial.print(pararot_pos);
			//        Serial.print(" dir:");
			//        DBG(pararot_dir);
		}
	}
	int oldval = -1;
	void show(bool force, String caller) override
	{
		nump = numitems - 1;

		static float foldval = -1;
		//		DBG(String(actSubmenu) + " " + actPara->name + " " + String(actPara->submenu) + " " + String(opencalled ? 1 : 0));
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

				DBG("Rot " + String(pararot_pos) + " " + String(actPara->fvalue));
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
					//					DBG("set enc "+String(m)+"->"+String(setv));
					//					Menus[HIRES]->myEnc->write(setv);
					val = Menus[HIRES]->rawvalue;
					//				DBG(actPara->name + " = " + String(val) + " " + String(actPara->fvstart) + " " + String(actPara->fvend));
					val = fmap(val, 0, 10000, actPara->fvstart, actPara->fvend);
					//				DBG(actPara->name + " = " + String(val));
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
				//				FDBG(actPara->name + " = " + String(actPara->fvalue));
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
			//			int orp = rot_pos;
			rot_test();
			//			if(orp!=rot_pos)
			//				DBG(SN(orp)+" # "+SN(rot_pos));
			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && oldmenuState == menuState)
			{
				return;
			}
			rot_pos = abs(rot_pos);
			//			rot_pos %= 8;

			oldmenuState = menuState;
		}

		displayMenu(__CALLER__);
	}
	void save(File frec)
	{
		// DBG("Saving # " + String(NUMSYNTH) + " data");
		for (int p = 1; p < NUMSYNTH; p++)
		{
			//			FDBG(SN(p)+" "+Paras[p]->name + " => " + String(Paras[p]->fvalue));
			frec.write((char *)&Paras[p]->fvalue, 4);
			frec.write((char *)&Paras[p]->source, 4);
		}
	}
	virtual void restore(File frec) override
	{
		//.println("Restoring # " + String(NUMSYNTH) + " data");
		for (int p = 1; p < NUMSYNTH; p++)
		{
			Paras[p]->fvalue = floatrec(frec);
			Paras[p]->source = intrec(frec);

			//			FDBG(SN(p)+" "+Paras[p]->name + " <= " + String(Paras[p]->fvalue));
			Paras[p]->setSynthVal();
		}
	}
	static void filefunc(signed char type)
	{
		//
		if (type == 1)
		{
			menuState = PRESETS;
			opencalled = true;
			//			DBG("Callback " + String(type));
			return;
		}
		String prename = "PRESET" + String(1) + ".PRS";
		if (type == 3)
		{
//			prename = "PRESET" + String(++actPre) + ".PRS";
//			if (actPre > 99)
//				actPre = 0;
//			recentPre = actPre;
		}
		if (SD.exists(prename.c_str()))
		{
			SD.remove(prename.c_str());
			//    DBG("ini deleted");
		}
		File frec = SD.open(prename.c_str(), FILE_WRITE);
		if (frec)
		{

			DBG(prename);
			MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
			mPara->save(frec);
			frec.close();

			DBG("Preset saved");
			((MenuSynth *)SynthMenu)->submenus[actSubmenu]->comment = "saved " + prename;
		}
	}
	SubMenu *submenus[100];
	static bool opencalled;
	const String polymode[2] = {"MONO", "POLY"};
	const String portatype[3] = {"_", "LIN", "EXP"};
	const String vcftype[5] = {"Off", "LP", "BP", "HP", "Moog"};
	// String ranges[7] = {"64", "32", "16", "8", "4", "2", "1"};
	// String taps[16] = {"0", "1", "2", "1-2", "3", "1-3", "2-3", "1-2-3", "4", "1-4", "2-4", "1-2-4", "3-4", "1-3-4", "2-3-4", "1-2-3-4"};
	// float vtaps[16] = {0, 1, 1, 0.5, 1, 0.5, 0.5, 1.0 / 3.0, 1, 0.5, 0.5, 1.0 / 3.0, 0.5, 1.0 / 3.0, 1.0 / 3.0, 0.25};
	const String shapes[13] = {"Sine", "Sawtooth", "Square", "Triangle", "Arbitrary", "Pulse", "Rev. Saw", "S&H", "Var. Tri", "BL Saw", "BL SAW R", "BL Square", "BL Pulse"};
};
