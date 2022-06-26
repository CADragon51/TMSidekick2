#pragma once
class MenuMidiSet : public MenuPara
{
public:
	MenuMidiSet(int type, MenuTargetSet *ptargets) : MenuPara(MIDISETTINGS + type, paranamemidi[type], 0)
	{
		Paras[nump++] = new Parameter("Target", ptargets->me);
		paraID[nump - 1] = ptargets->me;
		Paras[nump++] = new Parameter("OutCh", &outCH, 0, 16);
		Paras[nump++] = new Parameter("Split CH", &splitCH, 0, 16);
		Paras[nump++] = new Parameter("SplitNote", &splitNote, 0, 127, midiNamesLong, 128);
		//	Paras[nump++] = new Parameter("Scale", &scaleID, 0, 81);
		//	Paras[nump - 1]->setForm(scales);

		Paras[nump++] = new Parameter("MapCH", &mapCH, 1, 16);
		Paras[nump++] = new Parameter("MIDI Ratchet", &isRat, 0, 1, selected, 2);
		Paras[nump++] = new Parameter("TheoryBoard", &isTB, 0, 1, selected, 2);
		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//       if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
		numitems = nump;
		me = type;
		parent = MIDISET;
		targets = ptargets;
		targets->items[0] = items[0] + "->";
		rawvalue = 0;
		setLength();
	}
	void save(File frec)
	{
		frec.write(outCH);
		frec.write(splitCH);
		//		frec.write(scaleID);
		frec.write(splitNote);
		frec.write(mapCH);
		frec.write(isTB);
		targets->save(frec);
		frec.write(useVel);
	}
	virtual void restore(File frec) override
	{
		outCH = readrec(frec);
		splitCH = readrec(frec);
		//	byte scaleID = readrec(frec);
		splitNote = readrec(frec);
		mapCH = readrec(frec);
		isTB = readrec(frec);
		targets->restore(frec);
		readrec(frec);
	}
	void action(bool on, String from)
	{
		byte ch = sourceCH;
//		if (eventtype != 2)
		{
			if (outCH > 0 &&ch == mapCH)
				ch = outCH;
			else if (sourceNote >= splitNote)
				ch = splitCH;
		}
		STACK;
//		FDBG(useVel);
		if (!Menu::useVel)
			SourceVelocity = 127;
//		FDBG(SB(on) + " " + SN(sourceCH) + SN(outCH) + SN(mapCH) + SN(ch));
		targets->action(on, eventtype, ch, sourceNote, SourceVelocity, SourceCC, from + "->" + __CALLER__, 80);
		//		rawvalue = sourceNote;
	}
	int me;
	signed char outCH = 0;
	signed char splitCH = 0;
	signed char splitNote = 0;
	signed char mapCH = 0;
	signed char sourceCH = 0;

	signed char SourceCC = 0;
	signed char isTB = 0;
	signed char isRat = 0;

};
