extern u_int8_t _channel;
extern u_int8_t _velocity;
int cids[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short Chordindex[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short chordinv[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short invptr = 0;
short invmax = 0;
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
		//		Serial.print(String(c) + " " + String((chordnotes[c] + 12 - bn) % 12) + " " + String(chordnotes[c]) + " ");
	}
	mid = mid >> 1;
	//	Serial.println(midiNames[basenote]+"done " + String(inverse) + "=>" + String(mid));
	return mid;
}
void inversion(bool emit = false)
{
	static int lastid = -1;
	static int inverse = 0, invfirst = 0;
	static short scround = -1;
	static short ulast = 1;
	//	debug = 0;
	basenote = 127;
	//	int ixn = 0;
	int lid = 0;
	id = doinverse(0);
	int bn = chordnotes[0];
	lid = id * 1000 + 10 * (bn % 12);
#if 0 
	//	Serial.println("lid "+String(lid));
	if (lid != lastid)
	{
		scround++;
		invptr = -1;
		lastid = lid;
		invmax = 0;

bool ifo = false;

		for (int c = 0; c < chordptr; c++)
		{

			for (int u=ulast-1; u < scround; u++)
			{
				Serial.println(midiNames[chordnotes[c] % 12] + " check " + midiNames[chordnotes[ln[u]] % 12] + " => " + String(u));
				if (chordnotes[c] == ln[u])
				{
					inverse = c;
					ifo = true;
					ulast = u;
					break;
				}
			}
			if(ifo)
				break;
	}
	Serial.println(midiNames[chordnotes[inverse] % 12] + " try " + midiNames[chordnotes[ln[ulast]] % 12] + " => " + String(ulast));


		for (int c = inverse; c < chordptr; c++)
		{
			id = doinverse(c);
			//			Serial.println("id " + String(id));
			if (cname[id] != "")
			{
				if (invptr == -1)
				{
					invptr = invmax;
					invfirst = invmax;
				}
				if (debug == 1)
					Serial.println(midiNames[chordnotes[c] % 12] + " check " + String(c) + " => " + cname[id]);
				chordinv[invmax] = c;
				Chordindex[invmax++] = id;
				if (debug == 1)
					Serial.println(String(invmax - 1) + " " + String(chordinv[invmax - 1]) + " checked " + String(Chordindex[invmax - 1]) + " => " + cname[id]);
			}
		}
	}

		if (!emit)
		{
			if (debug == 1)
			{
				Serial.println(invptr);
				Serial.println(invmax);
				Serial.println(invfirst);
				Serial.println(chordinv[invptr]);
				Serial.println(Chordindex[invptr]);
				Serial.println(scround);
				Serial.println(midiNamesFlat[ln[scround]]);
			}
			id = Chordindex[invptr];
			inverse = chordinv[invptr++];
			if (invptr >= invmax)
				invptr = invfirst;
			bn = chordnotes[inverse];
			if (debug == 1)
			{
				Serial.println(String(scid) + " sb " + String(scalebase) + "=" + String(id) + " " + midiNamesFlat[bn % 12] + cname[id]);
				Serial.println(String(id) + " cname " + cname[id]);
			}
			basenote = bn % 12;
			printA4(midiNamesFlat[basenote] + (id != 72 ? cname[id] : ""));
			//	Serial.println(midiNamesFlat[ln[ulast]]);
			//		display.clearDisplay();
			//			lastinv = inverse;
			//		Serial.println("==> " + String(ixn) + ":" + midiNamesShort[basenote] + " id " + String(id) + "->" + cname[id]);
		}


		//	Serial.println(String(scround)+ " "+String(id)+" "+String(inverse));
		cids[scround] = lid + inverse;
		//	Serial.println(String(scround) + " " + String(id) + " " + String(inverse)+" "+String(cids[scround]));

		//		if (debug == 1)
		//			Serial.println(cids[scround] + " " + String(scround) + " " + String(scid) + " " + String(id) + " inversion #" + String(inverse));


printA4(midiNamesFlat[basenote % 12] + (id != 72 ? cname[id] : ""));
	byte nf = 127;
	for (int i = 0; i < bnx; i++)
	{
		int n = (ln[i] + scalebase - ln[0]) % 12;
		//		Serial.print(" s " + String(n));
		if (n == basenote % 12)
		{
			nf = i;
			break;
		}
	}
	for (int i = 0; i < bnx && nf == 127; i++)
	{
		int n = (ln[i] + scalebase - ln[0]) % 12;
		//		Serial.print(" s " + String(n));
		if (n == basenote % 12)
		{
			nf = i;
			break;
		}
		for (int j = 0; j < chords[id]->num; j++)
		{
			byte cn = (chords[id]->note[j] + basenote % 12) % 12;
			//			Serial.print(" " + String(cn));
			if (cn == n)
			{
				nf = i;
				break;
			}
		}
		//		Serial.println();

		if (nf != 127)
			break;
	}
	if (nf != 127)
	{
		Menus[menuState]->showscale("", ln, bnx, true, true);
		//		Serial.println(String(xpos[nf]) + " " + String(firstnote));
		chords[id]->dodisplay(xpos[nf], basenote % 12, false);
		display.setCursor(xpos[nf] - 8, 12);
		//		print("     ");
		//		display.setCursor(xpos[nf]-8, 14);
		print(midiNamesSharp[basenote] + (id != 72 ? cname[id] : ""));
		display.display();
	}
#endif
	if (chords[id] != nullptr)
	{
		String cn = chords[id]->name;
		byte cninv = chords[id]->note[chords[id]->invid];
		if(cninv>0)
			Serial.println(String(id * 10000 + bn) + " " + midiNamesSharp[bn % 12] + cn + "/" + midiNamesSharp[cninv]);
			else
		Serial.println(String(id * 10000 + bn) + " " + midiNamesSharp[bn % 12] + cn );
		printA4(midiNamesFlat[bn % 12] + (id != 72 ? cn : ""));
	}
	else
	{
		Serial.println(String(id * 10000 + bn));
		printA4(midiNamesFlat[bn % 12] + String(id));
	}
	if (emit || scround > 11 && 0)
	{
		Serial.print("const int A" + String(scalebase) + "_" + String(scid) + "[12] PROGMEM={" + String(cids[0]));
		for (int i = 1; i < 12; i++)
		{
			if (i < scround)
				Serial.print("," + String(cids[i]));
			else
				Serial.print(",0");
			cids[i] = 0;
		}
		Serial.println("};");
		lastid = 0;
		inverse = 0;
		scround = -1;
		printA4(String(scid));

		//		printA4(scid);
		if (scalebase == 6)
		{
			Serial.print("const int *A_" + String(scid) + "[12]={");
			Serial.print("A" + String(0) + "_" + String(scid));
			for (int i = 1; i < 12; i++)
			{
				Serial.print(",A" + String(i) + "_" + String(scid));
			}
			Serial.println("};");
		}
	}
}
void processChord()
{

	if (Menus[SETTINGS]->byPass)
		return;
	//	debug = 0;
	basenote = 127;
	if (chordptr > 2)
		inversion();
	if (debug == 1)
		Serial.println(id);

	return;

#if 0
	for (int n = 0; n < chordptr; n++)
	{
		if (debug == 1)
		{
			Serial.print(midiNames[chordnotes[n] % 12]);
			Serial.print(" : ");
			Serial.print(chordnotes[n]);
			Serial.print(" ");
		}
		if (chordnotes[n] < basenote)
			basenote = chordnotes[n];
	}
	id = 0;
	for (int n = 0; n < chordptr; n++)
	{
		Serial.print((chordnotes[n] - basenote) % 12);
		Serial.print(" ");
		id |= 1 << ((chordnotes[n] - basenote) % 12);
	}
	id = id >> 1;
	if (debug == 1 && invert[id][0] < 0)
	{
		Serial.print(" id ");
		Serial.print(id);
		Serial.print(" inv ");
		for (int n = 1; n < 6; n++)
		{
			Serial.print(invert[id][n]);
			if (invert[id][n] > 0)
				Serial.print(" " + cname[invert[id][n]]);
			Serial.print(" , ");
		}
		Serial.print(" basenote ");
		Serial.print(basenote);
		Serial.println(" ID " + String(id) + " #  " + String(chordptr));
	}

	if (id == 0)
	{
		if (debug == 1)
			Serial.println("send :" + String(millis() - lastTime));
		chords[id]->noteOn(basenote + octave * 12 + semiTone, _channel, _velocity);
		//    actChord = chords[ox];
		baseNoteID[basenote] = id;
		//		debug = 0;
		return;
	}
 
	if (invert[id][0] > 0)
	{
		int li = id;
		for (int i = 1; i < invert[id][0] + 1; i++)
		{
			if (invert[id][i] < li)
			{
				li = invert[id][i];
				basenote = chordnotes[i];
			}
		}
		id = li;
	}

	if (invert[id][0] < 0)
	{
		basenote = chordnotes[2];
		if (invert[id][0] == -2)
		{
			basenote = chordnotes[2];
			id = invert[id][2] > 0 ? invert[id][2] : invert[id][1];
			Serial.println("hello ->" + String(id) + " " + midiNamesShort[basenote % 12] + cname[id]);
		}
		if (invert[id][0] == -3)
		{
			basenote = chordnotes[1];
			id = invert[id][2] > 0 ? invert[id][2] : invert[id][1];
			Serial.println("hello ->" + String(id) + " " + midiNamesShort[basenote % 12] + cname[id]);
		}
		if (debug == 1)
		{
			Serial.print(" basenote ");
			Serial.print(basenote);
			Serial.print(" id ");
			Serial.print(id);
			Serial.println("->" + midiNamesShort[basenote % 12] + cname[id]);
		}
	}

	//	debug = 0;
	if (menuState == NEWMAP && id >= chordIds[0])
	{
		bool found = false;
		if (mapnx == 0)
		{
			//			fnote = no;
			mln[mapnx] = id;
			mlnchord[mapnx][0] = chordptr;
			for (int c = 0; c < chordptr; c++)
			{
				mlnchord[mapnx][c + 1] = chordnotes[c];
				mlninv[mapnx][c] = actinv[c];
			} //			Serial.println("no " + String(note) + " -> " + bnx);
			mapnx++;
		}
		else
		{
			//			no = no - fnote;
			//			if (no < 0)
			//				no += 12;
			for (int x = 0; x < mapnx; x++)
			{
				if (mln[x] == id)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				mlnchord[mapnx][0] = chordptr;
				for (int c = 0; c < chordptr; c++)
				{
					mlnchord[mapnx][c + 1] = chordnotes[c];
					mlninv[mapnx][c] = actinv[c];
				} //			Serial.println("no " + String(note) + " -> " + bnx);
				mln[mapnx++] = id;
			}
		}
		//	replchord[mln[mapnx - 1]] = 0;
		//	transpose[mln[mapnx - 1]] = 0;
		Menus[menuState]->showmap("New Mapping", mln, mapnx, true, true);
		if (debug == 1)
			Serial.println(String(mapnx) + " ---> " + String(mln[mapnx - 1]) + " " + String(replchord[mln[mapnx - 1]]));
	}

	int ox = id;
	int ofs = 0;
	//	if(debug==1)Serial.print(" ? " + String(ob) + " " + String(ox) + " " + String(basenote));
	//	if(debug==1)Serial.println(" ox " + midiNames[ob] + cname[ox]);
	if (debug == 1)
		Serial.println(".... bn " + String(basenote) + " ox  " + String(ox) + " cn " + midiNames[basenote % 12] + cname[ox]);
	String rn = String(ox);
	//	debug = 0;

	if (curMMS->mapEnabled && 0)
	{
		rn = replchord[ox];
		if (rn != String(ox))
		{
			int nox = rn.toInt();
			if (debug == 1)
				Serial.println("mapon " + rn + " nox " + String(nox) + "#");
		}
	}
	if (debug == 1)
	{
		Serial.println(" rn " + rn + " " + String(ox));
		Serial.println(" chords " + String(chords[ox]->num));
	}

	if (chords[ox] && rn == String(ox))
	{
		int mn = basenote - ofs + octave * 12 + semiTone;
		for (int c = 0; c < chordptr; c++)
		{
			chords[ox]->delta[c] = chorddeltas[c];
		}
		if (debug == 1)
			Serial.println(" mv" + mn);
		chords[ox]->noteOn(basenote - ofs + octave * 12 + semiTone, _channel, _velocity);
		//    actChord = chords[ox];
		baseNoteID[basenote] = ox;
		//   oled.print(" " + midiNames[mn%12] + cname[ox] );
		oled.setCursor(0, 7);
		oled.clearToEOL();
		oled.write(14);
		oled.write(32);
		oled.print(ox);
		oled.write(32);
		oled.print(midiNames[mn % 12] + cname[ox]);
		return;
	}

	if (rn != String(ox) && curMMS->mapEnabled)
	{
		ind1 = rn.indexOf('+');
		int nox = 0;
		int nb = 0;
		if (debug == 1)
			Serial.println("ox Index: " + String(ind1));
		String newnotes = notes[rn.toInt()];
		//    oled.print("=>" + rn);

		nox = rn.toInt(); //captures first data String
		if (ind1 != -1)
		{
			nox = rn.substring(0, ind1).toInt(); //captures first data String
			nb = rn.substring(ind1 + 1).toInt(); //captures second data String
			//     oled.print("->" + rn);

			newnotes = notes[nox];
			if (debug == 1)
				Serial.println("->******" + String(nox) + "******@" + String(nb));
		}
		if (debug == 1)
			Serial.println("->" + newnotes + "@" + String(nb));

		int mn = basenote + octave * 12 + semiTone + nb; // -ofs + octave * 12 + semiTone + nb;
		if (debug == 1)
			Serial.println(" " + String(mn) + " " + String(nox));
		for (int c = 0; c < chordptr; c++)
		{
			chords[nox]->delta[c] = chorddeltas[c];
		}

		chords[nox]->noteOn(mn, _channel, _velocity);
		//    actChord = chords[nox];
		baseNoteID[basenote] = nox;
		oled.setCursor(0, 7);
		oled.clearToEOL();
		oled.write(14);
		oled.write(32);
		oled.print(nox);
		oled.write(32);
		oled.print(midiNames[mn % 12] + cname[nox]);
		if (debug == 1)
			Serial.println(String(id) + "->" + midiNames[mn % 12] + cname[nox]);
	}
#endif
}
