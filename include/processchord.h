extern u_int8_t _channel;
extern u_int8_t _velocity;
// int cids[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// short Chordindex[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// short chordinv[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// short invptr = 0;
// short invmax = 0;
#if 0
#define DBG(x)      \
	if (debug == 1) \
	Serial.println(x)
#define FDBG(x) \
	Serial.println(x)

#define SN(x) String(x)

#define SP(x) String((int)x, HEX)
#endif
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
		DBG(String(c) + " " + String((chordnotes[c] + 12 - bn) % 12) + " " + String(chordnotes[c]) + " ");
	}
	mid = mid >> 1;
	DBG(midiNames[basenote] + " done " + String(inverse) + "=>" + String(mid));
	return mid;
}
bool inversion(bool emit = false)
{
	id = doinverse(0);
	int bn = chordnotes[0];
	bool found = false;
	DBG("id " + SN(id));
	if (chords[id] != nullptr)
	{
		String cn = chords[id]->name;
		byte cninv = chords[id]->note[chords[id]->invid];
		if (cninv > 0)
		{
			DBG(String(id * 100 + bn) + " " + midiNamesSharp[bn % 12] + cn + "/" + midiNamesSharp[cninv]);
		}
		else
		{
			DBG(String(id * 100 + bn) + " " + midiNamesSharp[bn % 12] + cn);
		}
		//		printA4(midiNamesFlat[bn % 12] + (id != 72 ? cn : ""));
		for (int i = 0; i < maxrepl; i++)
		{
			DBG("key " + String(keychord[i]) + " ?" + String(id * 100 + bn));

			if (keychord[i] == id * 100 + bn || (chordptr == 2 && keychord[i] == id * 100))
			{
				found = true;
				actkeyidx = i;
				DBG("found key " + String(keychord[i]) + " ?" + String(id * 100 + bn));
				break;
			}
		}
		if (menuState == NEWMAP)
		{
			if (!found && maxrepl < 96)
			{
				actkeyidx = maxrepl;
				keychord[actkeyidx] = id * 100 + bn;
				transpose[actkeyidx] = 0;
				replchord[actkeyidx] = id;
				maxrepl++;
			}
			//		int ki = keychord[actkeyidx];
			//		int ac = ki / 100;
			//		Chord *vc = chords[ac];
			//			DBG(String(actkeyidx) + " / " + String(maxrepl) + " " + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx]));
			Menus[NEWMAP]->showmap(true);
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
	return found;
}
void processChord()
{
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
	inData = midiNamesFlat[basenote % 12] + chordName[ack];

	//	FDBG("chord @ " + SN(nextEvent));
	if (!repl)
	{
		byte note = basenote;
		//		if (Menus[SETTINGS]->procMode < 2)
		//			note += octave * 12 + semiTone;
		//		chords[keychord[actkeyidx] / 100]->noteOn(note, _channel, _velocity);
		//		baseNoteID[basenote] = keychord[actkeyidx] / 100;
		display.setCursor(0, 7 * 7);
		clearToEOL();
		print(midiNames[scnote % 12], false, __LINE__);
		print(chordName[ack], false, __LINE__);
		print("->", false, __LINE__);
		print(midiNames[note % 12], false, __LINE__);
		print(chordName[ack], false, __LINE__);
		lastChord = chords[ack];
		for (int i = 0; i < chordptr; i++)
		{
			onnotes[chordnotes[i]] = ack;
			DBG(" --- " + SN(chordnotes[i]) + " -> " + SN(ack));
		}
		chords[ack]->noteOn(note, _channel, _velocity);
		isChord = false;
		isChordend = true;
		chordptr = 0;
		display.display();
		return;
	}
	else
	{
		byte note = basenote + transpose[actkeyidx];
		int tr = note % 12;
		//            FDBG(SN(scalebase) + " " + SN(i) + "? whole " + SN(mapwhole[scalebase][i]) + " -> " + SN(pos) + " " + isFlat[tr]);
		if (scnote != 5 && scnote != 0)
			note -= isFlat[tr];
		else
			note += isFlat[tr];

		//		DBG(String(actkeyidx) + " / " + String(maxrepl) + " " + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx]));
		//		DBG(String(actkeyidx) + " / " + chordName[(keychord[actkeyidx])] + " " + String(transpose[actkeyidx]) + " " + String(replchord[actkeyidx])));
		//		FDBG(String(note) + "=" + String(basenote) + " " + String(transpose[actkeyidx]));
		//		FDBG(String(actkeyidx) + " / " + chordName[ack] + " " + String(transpose[actkeyidx]) + " " + chordName[(replchord[actkeyidx])]);

		//		if (Menus[SETTINGS]->procMode<2)
		//		note += octave * 12 + semiTone;
		display.setCursor(0, 7 * 7);
		clearToEOL();
		display.print(midiNames[basenote % 12]);
		display.print(chordName[ack]);
		display.print("->");
		display.print(midiNames[note % 12]);
		display.print(chordName[replchord[actkeyidx]]);
		//		printA4(midiNamesFlat[basenote % 12] + chordName[replchord[actkeyidx]]);
		outData = midiNamesFlat[note % 12] + chordName[replchord[actkeyidx]];
		lastChord = chords[replchord[actkeyidx]];
		for (int i = 0; i < chordptr; i++)
		{
			onnotes[chordnotes[i]] = replchord[actkeyidx];
			DBG(" --- " + SN(chordnotes[i]) + " -> " + SN(replchord[actkeyidx]));
		}
		chords[replchord[actkeyidx]]->noteOn(note, _channel, _velocity);
		//		baseNoteID[basenote] = replchord[actkeyidx];
		isChord = false;
		chordptr = 0;
		isMap = true;
	}

	return;
}
