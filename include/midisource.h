#pragma once
class MenuMidiSet : public MenuPara {
public:
	MenuMidiSet(int type, MenuTargetSet* ptargets) : MenuPara(MIDISETTINGS + type, paranamemidi[type], 0) {
		Paras[nump++] = new Parameter("Target", ptargets->me);
		paraID[nump - 1] = ptargets->me;
		Paras[nump++] = new Parameter("OutCh", &outCH, 0, 16);
		Paras[nump++] = new Parameter("Note Event", &Filter1, 0, 1,selected,2);
		Paras[nump++] = new Parameter("BaseNote", &baseNote, 0, 11, midiNames,12);
	//	Paras[nump++] = new Parameter("Scale", &scaleID, 0, 81);
	//	Paras[nump - 1]->setForm(scales);

		Paras[nump++] = new Parameter("MapCH", &mapCH, 1, 16);
		Paras[nump++] = new Parameter("MIDI Ratchet", &isRat, 0, 1, selected,2);
		Paras[nump++] = new Parameter("TheoryBoard", &isTB, 0, 1, selected,2);
		Paras[nump++] = new Parameter("velocity", &useVel, 0, 1, selected,2);
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
		frec.write(Filter1);
//		frec.write(scaleID);
		frec.write(baseNote);
		frec.write(mapCH);
		frec.write(isTB);
		targets->save(frec);
		frec.write(useVel);

	}
	virtual void restore(File frec) override
	{
		outCH = readrec(frec);
		Filter1 = readrec(frec);
//	byte scaleID = readrec(frec);
		baseNote = readrec(frec);
		mapCH = readrec(frec);
		isTB = readrec(frec);
		targets->restore(frec);
		useVel = readrec(frec);
	}
	void action(bool on,String from) {
		byte ch = sourceCH;
		if (outCH > 0)
			ch = outCH;
		targets->action(on, eventtype, ch, sourceNote, SourceVelocity, SourceCC, from+"->"+__CALLER__, 80);
		//		rawvalue = sourceNote;
	}
	int me;
	signed char outCH = 0;
	signed char Filter1 = 0;
	signed char baseNote = 0;
	signed char mapCH = 0;
	signed char sourceCH = 0;
	
	signed char SourceCC = 0;
	signed char isTB = 0;
	signed char isRat = 0;
	signed char useVel = 0;

};
