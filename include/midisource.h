#pragma once
class MenuMidiSet : public MenuPara {
public:
	MenuMidiSet(int type, MenuTargetSet* ptargets) : MenuPara(MIDISETTINGS + type, paranamemidi[type], 0) {
		Paras[nump++] = new Parameter("Target", ptargets->me);
		paraID[nump - 1] = ptargets->me;
		Paras[nump++] = new Parameter("Channel", &CH, 0, 15);
		Paras[nump++] = new Parameter("Note Event", &Filter1, 0, 1,selected);
		Paras[nump++] = new Parameter("Scaling", &Filter2, 0, 1, selected);
		Paras[nump++] = new Parameter("BaseNote", &Filter3, 0, 11,midiNames);
		Paras[nump++] = new Parameter("Mapping", &mapEnabled, 0, 1, selected);
		Paras[nump++] = new Parameter("MIDI Ratchet", &isRat, 0, 1, selected);
		Paras[nump++] = new Parameter("TheoryBoard", &isTB, 0, 1, selected);
		Paras[nump++] = new Parameter("velocity", &useVel, 0, 1, selected);
		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//       if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
//		items[nump++] = "Back";
		numitems = nump;
		me = type;
		parent = MIDISET;
		targets = ptargets;
		targets->items[0] = items[0] + "->";
		rawvalue = 0;
	}
	void save(File frec)
	{
		frec.write(CH);
		frec.write(Filter1);
		frec.write(Filter2);
		frec.write(Filter3);
		frec.write(mapEnabled);
		frec.write(isTB);
		targets->save(frec);
		frec.write(useVel);

	}
	virtual void restore(File frec) override
	{
		CH = readrec(frec);
		Filter1 = readrec(frec);
		Filter2 = readrec(frec);
		Filter3 = readrec(frec);
		mapEnabled = readrec(frec);
		isTB = readrec(frec);
		targets->restore(frec);
		useVel = readrec(frec);

	}
	void action(bool on) {
		byte ch = sourceCH;
		if (CH > 0)
			ch = CH;
		targets->action(on, eventtype, ch, sourceNote, SourceVelocity, SourceCC, rawvalue, rawvalue);
		rawvalue = sourceNote;
	}
	int me;
	signed char CH = 0;
	signed char Filter1 = 0;
	signed char Filter2 = 0;
	signed char Filter3 = 0;
	signed char mapEnabled = 0;
	signed char sourceCH = 0;
	
	signed char SourceCC = 0;
	signed char isTB = 0;
	signed char isRat = 0;
	signed char useVel = 0;
	MenuTargetSet* targets = 0;
};
