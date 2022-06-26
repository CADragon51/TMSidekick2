
extern u_int8_t _velocity;
extern MenuMidiSet *curMMS;
#include "chord.h"
extern Chord *lastChord;
int doinverse(int inverse)
{
	if (inverse > chordptr)
		inverse = 0;
	int mid = 0;
	int bn = chordnotes[inverse];
	basenote = bn % 12;
	for (int c = 0; c < chordptr; c++)
	{
		mid |= 1 << ((chordnotes[c] + 12 - bn) % 12);
		//		DBG(String(c) + " " + String((chordnotes[c] + 12 - bn) % 12) + " " + String(chordnotes[c]) + " ");
	}
	mid = mid >> 1;
	if (chords[mid] == 0)
		return 0;
	//	FDBG(midiNames[basenote] + " done " + String(inverse) + "=>" + String(mid) + " " + SN(chords[mid]->invid) + " " + SN(chords[mid]->_mscid));
	STACK;
	return mid;
}
bool inversion(bool emit = false)
{
	id = doinverse(0);
	if (id == 0)
		return false;
	int bn = chordnotes[0];
	bool found = false;
	//	FDBG("in id " + SN(id));
	STACK;
	//	inData = midiNamesSharp[bn % 12] + String(id);

	if (chords[id] != 0)
	{
		String cn = chords[id]->getName(true);
		int inv = chords[id]->invid;
		if (inv > 0)
		{
			for (int i = 1; i < chords[id]->num; i++)
			{
				if (chords[id]->inverseID[i] == chords[id]->_mscid)
				{
					//					FDBG(SN(i) + " " + chords[id]->inverseID[i]);
					//					int rid = chords[id]->_mscid;
					cn = chords[id]->getName();
					bn = chordnotes[i];
				}
			}
		}
		STACK;
		MIDIinData = midiNamesSharp[bn % 12] + cn + String(bn / 12);
		//		FDBG(String(id * 100 + bn) + " ...... " + MIDIinData);

		//		printA4(midiNamesFlat[bn % 12] + (id != 72 ? cn : ""));
		for (int i = 0; i < maxrepl && !originalmode; i++)
		{
			//			FDBG("key " + String(keychord[i]) + " ?" + String(id * 100 + bn % 12));

			if (keychord[i] == id * 100 + bn % 12)
			{
				found = true;
				actkeyidx = i;
				//				FDBG("found key " + String(keychord[i]) + " ?" + String(id * 100 + bn));
				break;
			}
		}
		STACK;
		//		FDBG(__CALLER__);
		if (menuState == NEWMAP || newmapmode)
		{
			//			FDBG(SB(found) + " ===== " + SN(maxrepl));
			if ((!found && maxrepl < 96) || maxrepl == 0)
			{
				actkeyidx = maxrepl;
				keychord[actkeyidx] = id * 100 + bn % 12;
				//				STACK;
				transpose[actkeyidx] = 0;
				replchord[actkeyidx] = id;
				maxrepl++;
				//				FDBG(String(actkeyidx) + " / " + String(maxrepl) + " " + SN(keychord[actkeyidx]) + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx]));
			}
			//		int ki = keychord[actkeyidx];
			//		int ac = ki / 100;
			//		Chord *vc = chords[ac];
			//			DBG(String(actkeyidx) + " / " + String(maxrepl) + " " + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx]));
			//			STACK;
			Menus[NEWMAP]->showmap(true);
			String nl = noteLine(false);
			webgui.setMonitor(idt, nl);
		}
	}
	else
	{
		DBG(String(id * 100 + bn));
		//		printA4(midiNamesFlat[bn % 12] + String(id));
	}
	if (emit && menuState == NEWMAP)
	{

		String name = "M" + String(g_scid * 100 + scalebase) + ".map";

		if (SD.exists(name.c_str()))
		{
			SD.remove(name.c_str());
			//    if(debug==1)DBG("ini deleted");
		}
		File frec = SD.open(name.c_str(), FILE_WRITE);
		if (frec)
		{

			DBG("Mapping saved");
			DBG("M" + String(g_scid) + String(scalebase) + ".map");
			DBG(scaleNames[g_scid] + " " + midiNamesFlat[scalebase]);
			frec.println(scaleNames[g_scid] + " " + midiNamesFlat[scalebase]);

			for (int i = 0; i < maxrepl; i++)
			{
				DBG(String(keychord[i]) + "=" + String(replchord[i]) + "+" + String(transpose[i]));
				frec.println(String(keychord[i]) + "=" + String(replchord[i]) + "+" + String(transpose[i]));
			}
			frec.close();
		}
	}
	STACK;
	return found;
}
void processChord(byte pchannel)
{
	STACK;
	bool repl = false;
	byte procMode = Menus[SETTINGS]->procMode;
	scaled = (scFP[SCALES] != 2047 && procMode == 0);
	//	if (Menus[SETTINGS]->procMode)
	//		return;
	//	debug = 0;
	if (chordptr == 2)
	{
		if (chordnotes[0] > chordnotes[1])
		{
			int sw = chordnotes[0];
			chordnotes[0] = chordnotes[1];
			chordnotes[1] = sw;
		}
	}
	if (chordptr > 1)
		repl = inversion();
	//
	//	DBG("pc ID:" + SN(id));
	int actkeyc = keychord[actkeyidx];
	int scnote = chordnotes[0] % 12;
	int basenote = (actkeyc % 100);
	int ack = actkeyc / 100;
	//	FDBG("\n"+SN(scnote) + " ----- " + SN(actw) + "=>" + SN(actr) + "=" + SN(basenote));
	STACK;
	if (chordptr == 2)
	{
		if (progressmode)
		{
			int actw = __builtin_ctz(ack);
			actkeyidx = mapwhole[scnote][actw];
		}
		basenote = chordnotes[0];
		if (scaled)
		{
			int oct = basenote / 12;
			FDBG("scaling " + SN(basenote % 12) + "->" + SN(inscale[basenote % 12]));
			basenote = inscale[basenote % 12] + oct * 12;
		}

	} //	chordptr = 0;
	  //	FDBG("repl " + SN(repl) + " " + String(actkeyidx) + " / " + chordName[(keychord[actkeyidx] / 100)] + " " + String(transpose[actkeyidx]) + " " + chordName[(replchord[actkeyidx])]);
	//	printA4(midiNamesFlat[basenote % 12] + chordName[(keychord[actkeyidx] / 100)]);
	//	if (!originalmode)
	//		inData = midiNamesFlat[basenote % 12] + chordName[ack];
	//	else
	//		inData = midiNamesFlat[chordnotes[0] % 12] + chordName[id];
	//	if (!originalmode)
	//		FDBG(SN(actkeyidx) + " " + SN(keychord[actkeyidx]) + " " + inData);
	//	FDBG("chord @ " + SN(nextEvent));
	if (!repl || originalmode)
	{
		STACK;
		byte note = chordnotes[0];
//		if (Menus[SETTINGS]->procMode < 2)
//			note += octave * 12 + semiTone;
//		chords[keychord[actkeyidx] / 100]->noteOn(note, _channel, _velocity);
//		baseNoteID[basenote] = keychord[actkeyidx] / 100;
#if 0
		display.setCursor(0, 7 * 7);
		clearToEOL();
		print(midiNames[scnote % 12], false, __LINE__);
		print(chordName[ack], false, __LINE__);
		print("->", false, __LINE__);
		print(midiNames[note % 12], false, __LINE__);
		print(chordName[ack], false, __LINE__);
		display.display();
#endif
		lastChord = chords[id];
		for (int i = 0; i < chordptr; i++)
		{
			onnotes[chordnotes[i]] = id;
			//			if (lastChord == 0)
			//				FDBG(" --- " + SN(chordnotes[i]) + " -> " + SN(id));
		}
		if (lastChord == 0)
			return;
		//		FDBG(SN(id) + " " + SP(chords[id]) + " " + SN(note));

		STACK;
		outData = MIDIinData;
		if (!(menuState == NEWMAP || newmapmode))
		{
			//			FDBG(pchannel);
			chords[id]->noteOn(note, pchannel, _velocity);
		}
		isChord = false;
		isChordend = true;
		chordptr = 0;
		STACK;
		return;
	}
	else
	{
		STACK;
		byte oct = chordnotes[0] / 12;
		byte note = basenote % 12 + transpose[actkeyidx] + oct * 12;
		int tr = note % 12;
		if (g_scid == 2047)
		{
			if (scnote != 5 && scnote != 0)
				note -= isFlat[tr];
			else
				note += isFlat[tr];
		}
		//		DBG(String(actkeyidx) + " / " + String(maxrepl) + " " + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx]));
		//		DBG(String(actkeyidx) + " / " + chordName[(keychord[actkeyidx])] + " " + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx])));
		//		FDBG(SN(basenote)+String(note) + "=" + String(chordnotes[0]) + " " + String(transpose[actkeyidx])+SN(oct));
		//		FDBG(String(actkeyidx) + " / " + chordName[ack] + " " + String(transpose[actkeyidx]) + " " + chordName[(replchord[actkeyidx])]);

		//		if (Menus[SETTINGS]->procMode<2)
		//		note += octave * 12 + semiTone;
		display.setCursor(0, 7 * 7);
		clearToEOL();
		display.print(midiNamesFlat[basenote % 12]);
		display.print(chordName[ack]);
		display.print("->");
		display.print(midiNamesFlat[note % 12]);
		display.print(chordName[replchord[actkeyidx]]);
		//		printA4(midiNamesFlat[basenote % 12] + chordName[replchord[actkeyidx]]);
		STACK;
		Chord *cho = chords[replchord[actkeyidx]];

		outData = midiNamesFlat[note % 12] + cho->getName() + String(note / 12);
		lastChord = cho;
		for (int i = 0; i < chordptr; i++)
		{
			onnotes[chordnotes[i]] = replchord[actkeyidx];
			//			DBG(" --- " + SN(chordnotes[i]) + " -> " + SN(replchord[actkeyidx]));
		}
		STACK;
		//	FDBG(pchannel);
		cho->noteOn(note, pchannel, _velocity);
		//		baseNoteID[basenote] = replchord[actkeyidx];
		isChord = false;
		chordptr = 0;
		isMap = true;
	}
	STACK;

	return;
}
