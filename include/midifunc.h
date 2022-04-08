
u_int8_t _channel = 0;
u_int8_t _velocity = 0;
MenuMidiSet *HostMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
MenuMidiSet *curMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
MenuMidiSet *USBMMS = (MenuMidiSet *)Menus[USB_MIDI_IN];
MenuMidiSet *SerialMMS = (MenuMidiSet *)Menus[MIDI_IN];
//#define SN(x) String(x)
//#define SB(x) String(x ? "t" : "f")
//#define SP(x) String((int)x, HEX)
short lastIn = 0, lastOut = 0;
byte scalenote(byte note, byte base)
{
	short oct = (note / 12) * 12;
	note = inscale[note % 12];
	note += base + oct;
	return note % 128;
}
Chord *lastChord;
// byte onnotes[128];
String unflat[12] = {"C", "d", "D", "e", "E", "F", "g", "G", "a", "A", "b", "B"};

void endchord(void)
{

	//	DBG("Timer hit:" + String(millis() - lastTime));
	//	FDBG("Timer hit @ " + SN(millis()));
	delete myTimer;
	isChordend = true;
	byte procMode = Menus[SETTINGS]->procMode;
	bool scaled = scFP[SCALES] != 2047 && procMode == 0;
	//	FDBG(SN(procMode) + " " + SB(scaled) + " " + curMMS->sourceNote);
	int orgnote = curMMS->sourceNote;
	int note = orgnote;
	inData = midiNamesLong[orgnote];
	outData = "";
	if (scaled)
	{
		//		byte on = orgnote;
		note = scalenote(orgnote, curMMS->baseNote);
		//		DBG("after scale " + String(note));
		curMMS->sourceNote = note;
		outData = midiNamesLong[note + semiTone + 12 * octave];
		//		printA4(unflat[on % 12] + SN(on / 12) + unflat[(note + semiTone) % 12] + SN(note / 12 + octave));
		onnotes[orgnote] = -note;
	}
	//	else
	//		printA4(unflat[orgnote % 12] + SN(orgnote / 12) + unflat[(orgnote + semiTone) % 12] + SN(orgnote / 12 + octave));

	DBG(SN(curMMS->sourceNote) + " " + SN(curMMS->SourceVelocity));
	DBG("onnotes: " + SN(onnotes[orgnote]));
	curMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
	curMMS->action(true, __CALLER__);
	chordptr = 0;
	isChord = false;
	DBG("key map " + String(onnotes[orgnote]));
}

void actNoteOn(signed char channel, signed char note, signed char velocity, MenuMidiSet *actMMS)
{
	curMMS = actMMS;
	basenote = 0;
	byte onote = note;
	onnotes[onote] = 0;
	outData = "";
	if (transport == RECORDING && lastEvent < 10000)
	{
		sequences[lastEvent].recinit(0x90, note, velocity, channel, lastEvent);
		lastEvent++;
	} //	debug = 0;
	byte ch = channel;
	if (curMMS->outCH > 0)
		ch = curMMS->outCH;
	_channel = ch;
	bool isTB = curMMS->isTB;
	byte procMode = Menus[SETTINGS]->procMode;
	isMap = curMMS->mapCH == channel && (procMode == 2 || procMode == 0);
//	isMap = (procMode == 2 || procMode == 0);
	scaled = (scFP[SCALES] != 2047 && procMode == 1) && !curMMS->isTB;
//	FDBG("note on: " + SN(note) + " in ch " + String(channel) + " out ch " + String(ch) + " map ch" + String(curMMS->mapCH) + " " + SB(isMap));
	actMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
	actMMS->sourceCH = channel;
	actMMS->sourceNote = note;
	actMMS->SourceCC = 0;
	actMMS->SourceVelocity = velocity;
	if (procMode == 3)
	{
		inData = midiNamesLong[note];
		printA4(inData);
		curMMS->action(true, __CALLER__);
		return;
	}
	if (scaled && !isMap)
	{
		//
		DBG("before scale " + String(note));
		byte on = note;
		note = scalenote(note, actMMS->baseNote);
		onnotes[on] = -note;
		//		printA4(unflat[on % 12] + SN(on / 12) + unflat[(note + semiTone) % 12] + SN(note / 12 + octave));
		inData = midiNamesLong[on];
		outData = midiNamesLong[note + semiTone + 12 * octave];
		if (note == 255)
			return;
		//		note += octave * 12 + semiTone;
		//
		DBG("after scale " + String(note));
		actMMS->sourceNote = note;
		curMMS->action(true, __CALLER__);
		//		onnotes[onote] = -note;
		return;
	}

	curMMS->sourceNote = note;
	if (menuState == NEWSCALE && !isMap && isTB)
	{
		int no = note % 12;
		if (bnx == 0)
		{
			//			fnote = no;
			ln[bnx++] = no;
			scend = false;
			g_scid = 0;
			//			DBG("no " + String(note) + " -> " + bnx);
			Menus[menuState]->showscale("Find Scale", ln, bnx, true, true);
		}
		else
		{
			//			DBG("no " + String(note) + " -> " + ln[bnx - 1]);
			//			no = no - fnote;
			//			if (no < 0)
			//				no += 12;
			//
			bool found = false;
			if (scend)
			{
				scalebase = note % 12;
				//				DBG("scale " + String(g_scid) + " -> " + midiNames[scalebase]);
				Menus[menuState]->showscale("", ln, bnx, true);
				//				scround = 0;
				for (int i = 0; i < 100; i++)
				{
					if (scFP[i] == g_scid)
					{
						saveTMS();
						break;
					}
				}
				return;
			}
			for (int x = 0; x < bnx; x++)
			{
				if (ln[x] == no)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				ln[bnx++] = no;
				//				DBG("no " + String(note) + " -> " + ln[bnx - 1]);
				g_scid = 0;
				for (int i = 0; i < bnx; i++)
				{
					int n = (12 + ln[i] - ln[0]) % 12;
					g_scid = (1 << n) | g_scid;
				}
				g_scid = g_scid >> 1;
				scalebase = ln[0] % 12;
				//				DBG("scale " + String(g_scid) + " -> " + scalebase);
				Menus[menuState]->showscale("Find", ln, bnx, true);
				//				scround = 0;
			}
		}
		return;
	}
	if (!Menu::isRat)
	{
		//		printA4(midiNamesFlat[note % 12] + String((int)note / 12));
		inData = midiNamesLong[note];
	}
	else
	{
		//		printA4(midiNamesFlat[note % 12] + String((int)note / 12) + "*");
		inData = midiNamesLong[note] + "*";
		myTime = millis();
		deltat = myTime - lastTime;
		lastTime = myTime;
		int n = ((MenuPara *)Menus[SETTINGS])->ratdiv;
		float Bpm = ((MenuPara *)Menus[SETTINGS])->BPM;
		if (trigger)
		{

			float del = deltat / n;
			if (Menu::useBPM && Bpm>10)
				del = 6000 / (n * Bpm);
			//			DBG("n " + String(n) + " on " + String(note) + " " + String(del));
			for (int i = 0; i < n; i++)
			{
				actMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
				actMMS->sourceCH = channel;
				actMMS->sourceNote = note;
				actMMS->SourceCC = 0;
				actMMS->SourceVelocity = velocity;
				actMMS->action(true, __CALLER__);
				delay(del);
				actMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
				actMMS->sourceCH = channel;
				actMMS->sourceNote = note;
				actMMS->SourceCC = 0;
				actMMS->SourceVelocity = 0;
				actMMS->action(true, __CALLER__);
				delay(5);
			}
		}
	}

	myTime = millis();
	deltat = myTime - lastTime;
	lastTime = myTime;

	//
	{
		DBG("Note On =>");
		DBG(channel);
		DBG(", note=");
		DBG(note);
		DBG(", velocity=");
		DBG(velocity);
		DBG("On ");
		DBG(deltat);
		DBG(" ");
		DBG(note);
		DBG("bypass " + String(Menus[SETTINGS]->procMode));
	}
	if (!isChord || !isMap)
	{
		isChord = true;
		chordptr = 0;
		id = 0;
		isChordend = false;
		_channel = channel;
		_velocity = velocity;
		chordnotes[0] = note;
		if (isMap)
		{
			actMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
			actMMS->sourceCH = channel;
			actMMS->sourceNote = note;
			actMMS->SourceCC = 0;
			actMMS->SourceVelocity = velocity;
			DBG(String(chordptr) + " chordptr " + SN(actMMS->sourceNote));
			lastTime = millis();
			myTimer = new IntervalTimer();
			DBG("Timer started " + SN(lastTime));
			myTimer->begin(endchord, 20000);
		} //
		  //			if (actMMS->mapEnabled) //&& actMMS->isTB
		  //				myTimer.begin(endchord, 20000);
	}

	chorddeltas[chordptr] = deltat;
	chordnotes[chordptr++] = note;
	DBG(SB(isChord) + " delta " + String(deltat));
	DBG(String(chordptr) + " added " + String(note) + " " + SB(isMap));
	if (isMap && !isTB && chordptr > 1)
	{
		delete myTimer;
		processChord();
		DBG(String(chordptr) + " chordptr end ");
		isChordend = true;
		return;
	}

	//		noteact[note] = true;
	if (procMode == 3 || !isMap)
	{
		actMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
		actMMS->sourceCH = channel;
		actMMS->sourceNote = note;
		actMMS->SourceCC = 0;
		actMMS->SourceVelocity = velocity;
		actMMS->action(true, __CALLER__);

		DBG("single note on " + String(actMMS->eventtype));
	}
}
void myNoteOn(u_int8_t channel, u_int8_t note, u_int8_t velocity)
{
	incoming[1] = HIGH;
	actNoteOn(channel, note, velocity, HostMMS);

	DBG("single note on host");
}
void usbNoteOn(u_int8_t channel, u_int8_t note, u_int8_t velocity)
{
	incoming[0] = HIGH;
	DBG("single note on usb");
	actNoteOn(channel, note, velocity, USBMMS);
}
void serNoteOn(u_int8_t channel, u_int8_t note, u_int8_t velocity)
{
	incoming[2] = HIGH;
	actNoteOn(channel, note, velocity, SerialMMS);

	DBG("single note on serial");
}
extern Chord *actChord;
void actNoteOff(u_int8_t channel, u_int8_t note, u_int8_t velocity, MenuMidiSet *actMMS)
{
	//  note += val
	curMMS = actMMS;
	byte ch = channel;
	if (transport == RECORDING && lastEvent < 10000)
	{
		sequences[lastEvent].recinit(0x80, note, velocity, channel, lastEvent);
		lastEvent++;
	}
	if (curMMS->outCH > 0)
		ch = curMMS->outCH;
	_channel = ch;
	//	bool isTB = curMMS->isTB;
	byte byPass = Menus[SETTINGS]->procMode;
	isMap = (byPass == 2 || byPass == 0) && curMMS->mapCH == channel;
	//	bool scaled = scFP[SCALES] != 2047 && byPass == 1;
	DBG("onnotes: " + SN(onnotes[note]));
	if (isMap && onnotes[note] > 0)
	{
		isChord = false;
		chordptr = 0;
		isChordend = true;
		DBG("############## " + SN(onnotes[note]));
		chords[onnotes[note]]->noteOff();
		onnotes[note] = 0;
		return;
	}
	//	if (menuState == NEWSCALE && actMMS->CH != channel && actMMS->isTB)
	basenote = 0;
	//	debug = 0;
	if (onnotes[note] < 0)
	{
		note = -onnotes[note];
		DBG(note);
		onnotes[note] = 0;
	}

	{
		DBG("Note Off, ch=");
		DBG(channel);
		DBG(", note=");
		DBG(note);
		DBG(", velocity=");
		DBG(velocity);
	}

	//	if (byPass == 3)
	{

		{
			actMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
			actMMS->sourceCH = channel;
			actMMS->sourceNote = note;
			actMMS->SourceCC = 0;
			actMMS->SourceVelocity = velocity;
			DBG("Off " + SN(note));
			actMMS->action(false, __CALLER__);
		}
		//		debug = 0;
		return;
	}

	//	noteact[note] = false;
	if (chordnotes[chordptr - 1] == note)
	{
		isChord = false;
	}
	short bid = baseNoteID[note];

	{
		DBG("Note Off =");
		DBG(bid);
		DBG(" note = ");
		DBG(note);
	}
	if (byPass < 2 && isMap)
	{
		if (bid < 2048 && actChord != nullptr)
		{
			DBG("noff " + String(actChord->name));
			actChord->noteOff();
			//
			baseNoteID[note] = 0;
		}
	}
	else
	{
		actMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
		actMMS->sourceCH = channel;
		actMMS->sourceNote = note;
		actMMS->SourceCC = 0;
		actMMS->SourceVelocity = velocity;
		actMMS->action(false, __CALLER__);
	}
}
void myNoteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
	incoming[1] = LOW;
	actNoteOff(channel, note, velocity, HostMMS);
}
void usbNoteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
	incoming[0] = LOW;
	actNoteOff(channel, note, velocity, USBMMS);
}
void serNoteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
	incoming[2] = LOW;
	actNoteOff(channel, note, velocity, SerialMMS);
}
void myAfterTouchPoly(unsigned char channel, unsigned char note, unsigned char velocity)
{
	if (HostMMS->mapCH != channel || HostMMS->isTB == 0)
		return;
	if (!isChordend && (Menus[SETTINGS]->procMode == 0 || Menus[SETTINGS]->procMode == 1))
	{
		//		myTimer.end();
		processChord();
		isChordend = true;
	}
	if (0)
	{

		DBG("AfterTouch Change, ch=");

		DBG(channel);

		DBG(", note=");

		DBG(note);

		DBG(", velocity=");

		DBG(velocity);
	}
	if (Menus[SETTINGS]->sendAfter % 2 == 1)
	{
		usbMIDI.sendAfterTouchPoly(note, velocity, channel, cable);
		MIDI1.sendAfterTouch(note, velocity, channel);
		MIDI2.sendAfterTouch(note, velocity, channel);
	}
	if (Menus[SETTINGS]->sendAfter > 1)
	{
		usbMIDI.sendAfterTouch(velocity, channel, cable);
		MIDI1.sendAfterTouch(velocity, channel);
		MIDI2.sendAfterTouch(velocity, channel);
	}
}
void actProgramChange(signed char ch, signed char program, MenuMidiSet *actMMS)
{
	if (actMMS->targets->UMO)
		usbMIDI.sendProgramChange(program, ch, cable);
	if (actMMS->targets->MO1)
		MIDI1.sendProgramChange(program, ch);
	if (actMMS->targets->MO2)
		MIDI2.sendProgramChange(program, ch);
}

void actControlChange(signed char channel, signed char control, signed char value, MenuMidiSet *actMMS)
{
	if (Menus[SETTINGS]->noPanic && control == 123)
		return;
	actMMS->eventtype = 2; // 0: note-off,1: Note on event; 2: CC event;3: pitch
	actMMS->sourceCH = channel;
	actMMS->sourceNote = 0;
	actMMS->SourceCC = control;
	actMMS->SourceVelocity = value;
	actMMS->action(true, __CALLER__);
	// usbMIDI.sendControlChange(control, value, channel, cable);
	//  MIDI.sendControlChange(control, value, channel);
}
void myControlChange(unsigned char channel, unsigned char control, unsigned char value)
{
	actControlChange(channel, control, value, HostMMS);
}
void usbControlChange(unsigned char channel, unsigned char control, unsigned char value)
{
	actControlChange(channel, control, value, USBMMS);
}
void serControlChange(unsigned char channel, unsigned char control, unsigned char value)
{
	actControlChange(channel, control, value, SerialMMS);
}
#if 0
void myProgramChange(signed char channel, signed char program) {
	DBG("Program Change, ch=");
	DBG(channel);
	DBG(", program=");
	DBG(program);
}

void myAfterTouchChannel(signed char channel, signed char pressure) {
	DBG("After Touch, ch=");
	DBG(channel);
	DBG(", pressure=");
	DBG(pressure);
	//  usbMIDI.sendAfterTouch(pressure, channel, cable);
}
#endif
void actPitchChange(u_int8_t channel, int pitch, MenuMidiSet *actMMS)
{

	DBG("Pitch Change, ch=");
	DBG(channel);
	DBG(", pitch=");
	DBG(pitch);
	actMMS->eventtype = 3; // 0: note-off,1: Note on event; 2: CC event;3: pitch
	actMMS->sourceCH = channel;
	actMMS->sourceNote = 0;
	actMMS->SourceCC = 0;
	actMMS->SourceVelocity = pitch;
	actMMS->action(true, __CALLER__);
}
void myPitchChange(u_int8_t channel, int pitch)
{
	actPitchChange(channel, pitch, HostMMS);
}
void usbPitchChange(u_int8_t channel, int pitch)
{
	actPitchChange(channel, pitch, USBMMS);
}
void usbProgamChange(u_int8_t channel, u_int8_t program)
{
	FDBG("PC: " + SN(program));
	actProgramChange(channel, program, USBMMS);
}
void serPitchChange(u_int8_t channel, int pitch)
{
	actPitchChange(channel, pitch, SerialMMS);
}
void usbSysEx(const uint8_t *data, uint16_t length, bool complete)
{
	String out = SP(data[0]) + " ";
	for (int i = 1; i < length; i++)
		out += SP(data[i]) + " ";
	FDBG(out);
	if (USBMMS->targets->UMO)
	{
		usbMIDI.sendSysEx(length, data);
	}
	if (USBMMS->targets->MO1)
	{
		MIDI1.sendSysEx(length, data);
	}
	if (USBMMS->targets->MO2)
	{
		MIDI2.sendSysEx(length, data);
	}
}
void setmidi()
{

	usbMIDI.setHandleNoteOn(usbNoteOn);
	usbMIDI.setHandleNoteOff(usbNoteOff);
	usbMIDI.setHandleControlChange(usbControlChange);
	usbMIDI.setHandlePitchChange(usbPitchChange);
	usbMIDI.setHandleProgramChange(usbProgamChange);
	usbMIDI.setHandleSysEx(usbSysEx);
	//	for (int i = 0; i < 128; i++)
	//		noteact[i] = false;
	myusb.begin();
	DBG("setting MIDI");
	midi1.setHandleNoteOn(myNoteOn);
	midi1.setHandleNoteOff(myNoteOff);
	midi1.setHandleAfterTouchPoly(myAfterTouchPoly);
	midi1.setHandleControlChange(myControlChange);
	midi1.setHandlePitchChange(myPitchChange);

	MIDI_SER.setHandleNoteOn(serNoteOn);
	MIDI_SER.setHandleNoteOff(serNoteOff);
	MIDI_SER.setHandleControlChange(serControlChange);
	MIDI_SER.setHandlePitchBend(serPitchChange);
	HostMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
	USBMMS = (MenuMidiSet *)Menus[USB_MIDI_IN];
	SerialMMS = (MenuMidiSet *)Menus[MIDI_IN];
	HostMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
	curMMS = HostMMS;
}
