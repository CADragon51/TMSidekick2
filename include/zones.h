#pragma once
class MenuZoneSet : public MenuPara {
public:
	MenuZoneSet(int type, MenuTargetSet* ptargets) : MenuPara(ZONESETTINGS + type, "Zone " + String(type + 1), 0) {
		Paras[nump++] = new Parameter("Target", ptargets->me);
		paraID[nump - 1] = ptargets->me;
		Paras[nump++] = new Parameter("CC",(signed char *) &CC, 0, 127);
		//     Paras[nump++] = new Parameter("Function", &Func, 0, 1,selected);
		Paras[nump++] = new Parameter("Channel",(signed char *) &channel, 0, 15);
		Paras[nump++] = new Parameter("From Note", (signed char *)&Note, 0, 127, midiNamesLong,128);
		Paras[nump++] = new Parameter("to", (signed char *)&toNote, 0, 127, midiNamesLong,128);
		Paras[nump++] = new Parameter("Pitch",(signed char *) &Pitch, 0, 1, selected,2);
		Paras[nump++] = new Parameter("Modulation", (signed char *)&Mod, 0, 1, selected,2);
		for (int i = 1; i < nump; i++)
		{
			insertItem(Paras[i]->name);
			//        DBG(items[0] + " " + Paras[i]->name);
		}
		numitems = nump;
		me = type;
		parent = TOUCH;
		targets = ptargets;
		targets->items[0] = items[0] + "->";
		rawvalue = 0;
	}
	void action(bool on) {
//		channel = 0;
		if (!on)
		{
			DBG("Off ");
			DBG(me);
			DBG("  ");
			DBG(oldvalue);
			state = 0;
			eventtype = 0;
		}
		else
		{
			//DBG("On ");
			//DBG(me);
			//DBG(" Touchsize: ");
			//DBG(touchSize);
			//DBG(" Target ");
			//DBG(targets->UMO);
			//DBG(" value: ");
			//DBG(rawvalue);
			state = 1;
			eventtype = 1;
		}

		//	void action(bool on, byte eType, byte sourceCH, byte sourceNote, byte sourceVelocity, byte sourceCC, int rawPitch, int rawVelocity) {
		byte notefrom = Note;
		byte noteto = toNote;

		if (Pitch)
		{
			int pitch = map(rawvalue, 0, 1792, -8192, 8192);
			targets->action(on, 3, channel, pitch, 0, 0, __CALLER__, map(rawvalue, 0, 1792, 0, 4096));
		}
		else if (Mod)
		{
			int mod = map(rawvalue, 0, 1792, 0, 127);
			targets->action(on, 2, channel, mod, 0, 1, __CALLER__, map(rawvalue, 0, 1792, 0, 4096));
		}
		else if (Note && !on)
		{
			targets->action(0, 0, channel, oldvalue, 0, 1, __CALLER__, 0);
			oldvalue = 0;
		}
		else if (Note && on)
		{
			byte relnote = map(rawvalue, 0, 1792, notefrom, noteto);
			if(oldvalue!=0)
				targets->action(0, 0, channel, oldvalue, 0, 1, 0, 0);
			targets->action(on, 1, channel, relnote, touchSize, 0, __CALLER__, map(rawvalue, 0, 1792, 0, 4096));
			oldvalue = relnote;
		}
		else if (targets->SynthPara)
		{
			byte pn = targets->SynthPara;
			Parameter* aPara = MenuPara::SynthMenu->getPara(pn);
			if (aPara == nullptr)
				return;
			//			DBG(String(pn) + " " +aPara->name + " " +String(aPara->isFloat?1:0));
			if (aPara->isFloat)
			{
				//				DBG(String((int)aPara->fvalue) + " " + items[0]);
				//				DBG("audio " + String(eventtype) + " " + String(pn) + " " + mPara->Paras[pn]->name);// +" " + ((MenuPara*)Menus[SYNTHSETTINGS])->Paras[targets->SynthPara]->name);
				float mapvalue = fmap(rawvalue,0, 1792, aPara->fvstart, aPara->fvend);
				aPara->fvalue = mapvalue;
				//			DBGf("values %f %d %d %f %f %f\n", rawvalue, emin, emax, aPara->fvstart, aPara->fvend, mapvalue);
				//			DBG("audio "+ String(mapvalue) + " " + mPara->Paras[pn]->name);
				//			MenuPara::setSynthVal(targets->SynthPara);
				aPara->setSynthVal();
				//				targets->action(on, 1, channel, 0, 0, 0, __CALLER__, mapvalue);
			}
			return;
		}
		{
			int cc = map(rawvalue, 0, 1792, 0, 127);
			float mapvalue = fmap(rawvalue, 0, 1792, 0, 1023);
			targets->action(on, 2, channel, cc, 0, CC, __CALLER__, mapvalue, mapvalue);
		}
	}
	void save(File frec)
	{
		frec.write(CC);
		frec.write(Pitch);
		frec.write(xOctave);
		frec.write(Note);
		frec.write(Mod);
		frec.write(channel);
		targets->save(frec);
		frec.write(xtoOctave);
		frec.write(toNote);
	}
	virtual void restore(File frec) override
	{
		CC = readrec(frec);
		Pitch = readrec(frec);
		xOctave = readrec(frec);
		Note = readrec(frec);
		Mod = readrec(frec);
		channel = readrec(frec);
		targets->restore(frec);
		xtoOctave = readrec(frec);
		toNote = readrec(frec);
	}
	byte CC = 0;
	byte Pitch = 0;
	byte xOctave = 3;
	byte Note = 0;
	byte xtoOctave = 3;
	byte toNote = 0;
	byte channel = 0;
	byte Mod = 0;
	int me;
	int state = 0;
	int oldvalue = 0;

};
