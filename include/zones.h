#pragma once
class MenuZoneSet : public MenuPara {
public:
	MenuZoneSet(int type, MenuTargetSet* ptargets) : MenuPara(ZONESETTINGS + type, "Zone " + String(type + 1), 0) {
		Paras[nump++] = new Parameter("Target", ptargets->me);
		paraID[nump - 1] = ptargets->me;
		Paras[nump++] = new Parameter("CC",(signed char *) &CC, 0, 127);
		//     Paras[nump++] = new Parameter("Function", &Func, 0, 1,selected);
		Paras[nump++] = new Parameter("Channel",(signed char *) &channel, 0, 15);
		Paras[nump++] = new Parameter("From Note", (signed char *)&Note, 0, 127, midiNamesLong);
		Paras[nump++] = new Parameter("to", (signed char *)&toNote, 0, 127, midiNamesLong);
		Paras[nump++] = new Parameter("Pitch",(signed char *) &Pitch, 0, 1, selected);
		Paras[nump++] = new Parameter("Modulation", (signed char *)&Mod, 0, 1, selected);
		for (int i = 1; i < nump; i++)
		{
			insertItem(Paras[i]->name);
			//        if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
		items[nump++] = "Back";
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
			if(debug==1)Serial.print("Off ");
			if(debug==1)Serial.print(me);
			if(debug==1)Serial.print("  ");
			if(debug==1)Serial.println(oldvalue);
			state = 0;
			eventtype = 0;
		}
		else
		{
			//if(debug==1)Serial.print("On ");
			//if(debug==1)Serial.print(me);
			//if(debug==1)Serial.print(" Touchsize: ");
			//if(debug==1)Serial.print(touchSize);
			//if(debug==1)Serial.print(" Target ");
			//if(debug==1)Serial.print(targets->UMO);
			//if(debug==1)Serial.print(" value: ");
			//if(debug==1)Serial.println(rawvalue);
			state = 1;
			eventtype = 1;
		}

		//	void action(bool on, byte eType, byte sourceCH, byte sourceNote, byte sourceVelocity, byte sourceCC, int rawPitch, int rawVelocity) {
		byte notefrom = Note;
		byte noteto = toNote;

		if (Pitch)
		{
			int pitch = map(rawvalue, 0, 1792, -8192, 8192);
			targets->action(on, 3, channel, pitch, 0, 0, map(rawvalue, 0, 1792, 0, 4096), map(rawvalue, 0, 1792, 0, 4096));
		}
		else if (Mod)
		{
			int mod = map(rawvalue, 0, 1792, 0, 127);
			targets->action(on, 2, channel, mod, 0, 1, map(rawvalue, 0, 1792, 0, 4096), map(rawvalue, 0, 1792, 0, 4096));
		}
		else if (Note && !on)
		{
			targets->action(0, 0, channel, oldvalue, 0, 1, 0, 0);
			oldvalue = 0;
		}
		else if (Note && on)
		{
			byte relnote = map(rawvalue, 0, 1792, notefrom, noteto);
			if(oldvalue!=0)
				targets->action(0, 0, channel, oldvalue, 0, 1, 0, 0);
			targets->action(on, 1, channel, relnote, touchSize, 0, map(rawvalue, 0, 1792, 0, 4096), map(rawvalue, 0, 1792, 0, 4096));
			oldvalue = relnote;
		}
		else if (targets->SynthPara)
		{
			byte pn = targets->SynthPara;
			Parameter* aPara = MenuPara::SynthMenu->getPara(pn);
			if (aPara == nullptr)
				return;
			//			if(debug==1)Serial.println(String(pn) + " " +aPara->name + " " +String(aPara->isFloat?1:0));
			if (aPara->isFloat)
			{
				//				if(debug==1)Serial.println(String((int)aPara->fvalue) + " " + items[0]);
				//				if(debug==1)Serial.println("audio " + String(eventtype) + " " + String(pn) + " " + mPara->Paras[pn]->name);// +" " + ((MenuPara*)Menus[SYNTHSETTINGS])->Paras[targets->SynthPara]->name);
				float mapvalue = fmap(rawvalue,0, 1792, aPara->fvstart, aPara->fvend);
				aPara->fvalue = mapvalue;
				//			if(debug==1)Serial.printf("values %f %d %d %f %f %f\n", rawvalue, emin, emax, aPara->fvstart, aPara->fvend, mapvalue);
				//			if(debug==1)Serial.println("audio "+ String(mapvalue) + " " + mPara->Paras[pn]->name);
				//			MenuPara::setSynthVal(targets->SynthPara);
				targets->action(on, 1, channel, 0, 0,0, rawvalue, mapvalue);
			}
			return;
		}
		{
			int cc = map(rawvalue, 0, 1792, 0, 127);
			float mapvalue = fmap(rawvalue, 0, 1792, 0, 1023);
			targets->action(on, 2, channel, cc, 0, CC, mapvalue, mapvalue, mapvalue);
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
	MenuTargetSet* targets = 0;
};
