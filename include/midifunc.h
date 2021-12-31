
u_int8_t _channel = 0;
u_int8_t _velocity = 0;
MenuMidiSet *HostMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
MenuMidiSet *curMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
MenuMidiSet *USBMMS = (MenuMidiSet *)Menus[USB_MIDI_IN];
MenuMidiSet *SerialMMS = (MenuMidiSet *)Menus[MIDI_IN];

void endchord(void)
{
	if (debug == 1)
		Serial.println("Timer hit:" + String(millis() - lastTime));
	myTimer.end();
	processChord();
	isChordend = true;
}
byte scalenote(byte note, byte base)
{
	//	Serial.print("scale it: " + String(base) + " " + String(note));
	//	int i = base;

	if (note < 128 - base && basescale[note] != -1)
	{
		note = basescale[note];
	}
	else
		return 255;
	note += base;
	//	Serial.println(" scaled: " + String(note));
	return note;
}
Chord lastChord;
void actNoteOn(signed char channel, signed char note, signed char velocity, MenuMidiSet *actMMS)
{
	_channel = channel;
	basenote = 0;
	//	debug = 0;
	if (Menus[SETTINGS]->byPass >= 2)
	{
		usbMIDI.sendNoteOn(note, velocity, channel, cable);
		MIDI1.sendNoteOn(note, velocity, channel);
		MIDI2.sendNoteOn(note, velocity, channel);
		if (actMMS->CH == channel && actMMS->isTB != 0)
		{
			lastChord.note[lastChord.num++] = note;
		}
		onnotes[note]++;
		if (Menus[SETTINGS]->byPass >= 3 && channel != 2)
		{
			byte polye = 1;

			if (poly)
				polye = NUMVOICES;
			synthnoteOn(note, polye);
		}
		return;
	}
	curMMS = actMMS;
	if (actMMS->Filter2 && !Menus[SETTINGS]->byPass)
	{
		if (debug == 1)
			Serial.println("before scale " + String(note));
		note = scalenote(note, actMMS->Filter3);
		if (note == 255)
			return;
		if (debug == 1)
			Serial.println("after scale " + String(note));
	}

	curMMS->sourceNote = note;
	if (menuState == NEWSCALE && actMMS->CH != channel && actMMS->isTB)
	{
		int no = note % 12;
		if (bnx == 0)
		{
			//			fnote = no;
			ln[bnx++] = no;
			scend = false;
			scid = 0;
			//			Serial.println("no " + String(note) + " -> " + bnx);
			Menus[menuState]->showscale("Find Scale", ln, bnx, true, true);
		}
		else
		{
			//			Serial.println("no " + String(note) + " -> " + ln[bnx - 1]);
			//			no = no - fnote;
			//			if (no < 0)
			//				no += 12;
			//			if(debug==1)
			bool found = false;
			if (scend)
			{
				scalebase = note % 12;
				//				Serial.println("scale " + String(scid) + " -> " + midiNames[scalebase]);
				Menus[menuState]->showscale("", ln, bnx, true);
				//				scround = 0;

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
				//				Serial.println("no " + String(note) + " -> " + ln[bnx - 1]);
				scid = 0;
				for (int i = 0; i < bnx; i++)
				{
					int n = (12 + ln[i] - ln[0]) % 12;
					scid = (1 << n) | scid;
				}
				scid = scid >> 1;
				scalebase = ln[0] % 12;
				//				Serial.println("scale " + String(scid) + " -> " + scalebase);
				Menus[menuState]->showscale("Find", ln, bnx, true);
				//				scround = 0;
			}
		}
		return;
	}
	if (!Menu::isRat)
		printA4(midiNamesFlat[note % 12] + String((int)note / 12));
	else
	{
		printA4(midiNamesFlat[note % 12] + String((int)note / 12) + "*");
		myTime = millis();
		deltat = myTime - lastTime;
		lastTime = myTime;
		int n = ((MenuPara *)Menus[SETTINGS])->ratdiv;
		float Bpm = ((MenuPara *)Menus[SETTINGS])->BPM;
		if (trigger)
		{

			float del = deltat / n;
			if (Menu::useBPM)
				del = 6000 / (n * Bpm);
			//			Serial.println("n " + String(n) + " on " + String(note) + " " + String(del));
			for (int i = 0; i < n; i++)
			{
				actMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
				actMMS->sourceCH = channel;
				actMMS->sourceNote = note;
				actMMS->SourceCC = 0;
				actMMS->SourceVelocity = velocity;
				actMMS->action(true);
				delay(del);
				actMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
				actMMS->sourceCH = channel;
				actMMS->sourceNote = note;
				actMMS->SourceCC = 0;
				actMMS->SourceVelocity = 0;
				actMMS->action(true);
				delay(5);
			}
		}
	}

	{
		myTime = millis();
		deltat = myTime - lastTime;
		lastTime = myTime;
		if (debug == 1)
			Serial.println(String(myTime) + " delta " + String(deltat));
		if (!isChord || !actMMS->mapEnabled)
		{
			isChord = true;
			chordptr = 0;
			id = 0;
			isChordend = false;
			_channel = channel;
			_velocity = velocity;
			if (debug == 1)
				Serial.println(String(chordptr) + " chordptr ");
			//			if (actMMS->mapEnabled) //&& actMMS->isTB
			//				myTimer.begin(endchord, 20000);
		}
		else if ((actMMS->mapEnabled && !actMMS->isTB) || chordptr > 15)
		{
			processChord();
			isChordend = true;
			//			myTimer.end();
		}
		chorddeltas[chordptr] = deltat;
		chordnotes[chordptr++] = note;
		if (debug == 1)
			Serial.println(String(chordptr) + " added " + String(note));
#if 0 
if (0) 
		{
			if (debug == 1)
				Serial.print("Note On =>");

			if(debug==1)Serial.print(channel, DEC);
			if(debug==1)Serial.print(", note=");
			if(debug==1)Serial.print(note, DEC);
			if(debug==1)Serial.print(", velocity=");
			if(debug==1)Serial.println(velocity, DEC);
			if(debug==1)Serial.print("On ");
			if(debug==1)Serial.print(deltat, DEC);
			if(debug==1)Serial.print(" ");

			if (debug == 1)
				Serial.println(note, DEC);
		}
#endif
		//		noteact[note] = true;
		if (debug == 1)
			Serial.println("bypass " + String(Menus[SETTINGS]->byPass));
		if (Menus[SETTINGS]->byPass || !actMMS->mapEnabled)
		{
			if (actMMS->mapEnabled)
				myTimer.end();
			actMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
			actMMS->sourceCH = channel;
			actMMS->sourceNote = note;
			actMMS->SourceCC = 0;
			actMMS->SourceVelocity = velocity;
			actMMS->action(true);
			if (debug == 1)
				Serial.println("single note on " + String(actMMS->eventtype));
		}
	}
}
void myNoteOn(u_int8_t channel, u_int8_t note, u_int8_t velocity)
{
	actNoteOn(channel, note, velocity, HostMMS);
	if (debug == 1)
		Serial.println("single note on host");
}
void usbNoteOn(u_int8_t channel, u_int8_t note, u_int8_t velocity)
{
	actNoteOn(channel, note, velocity, USBMMS);
}
void serNoteOn(u_int8_t channel, u_int8_t note, u_int8_t velocity)
{
	actNoteOn(channel, note, velocity, SerialMMS);
	if (debug == 1)
		Serial.println("single note on serial");
}

void actNoteOff(u_int8_t channel, u_int8_t note, u_int8_t velocity, MenuMidiSet *actMMS)
{
	//  note += val
	if (actMMS->CH == channel && actMMS->isTB)
	{
		isChord = false;
		chordptr = 0;
		isChordend = true;
	}
	//	if (menuState == NEWSCALE && actMMS->CH != channel && actMMS->isTB)
	return;
	basenote = 0;
	//	debug = 0;
	_channel = channel;
	if (Menus[SETTINGS]->byPass >= 2)
	{
		byte polye = 1;

		if (poly)
			polye = NUMVOICES;
		offnotes[note]++;
		if (actMMS->CH == channel && actMMS->isTB != 0)
		{
			for (int i = 0; i < lastChord.num; i++)
			{
				note = lastChord.note[i];
				usbMIDI.sendNoteOff(note, 0, channel, cable);
				MIDI1.sendNoteOff(note, 0, channel);
				MIDI2.sendNoteOff(note, 0, channel);
				Serial.println("Off " + String(note));
			}
			lastChord.num = 0;
		}
		else
		{
			usbMIDI.sendNoteOff(note, 0, channel, cable);
			MIDI1.sendNoteOff(note, 0, channel);
			MIDI2.sendNoteOff(note, 0, channel);

			if (Menus[SETTINGS]->byPass >= 3 && channel != actMMS->CH)
				synthnoteOff(note, polye);
		}
		//		debug = 0;
		return;
	}
	curMMS = actMMS;
	if (0)
	{
		if (debug == 1)
			Serial.print("Note Off, ch=");
		if (debug == 1)
			Serial.print(channel, DEC);
		if (debug == 1)
			Serial.print(", note=");
		if (debug == 1)
			Serial.print(note, DEC);
		if (debug == 1)
			Serial.print(", velocity=");
		if (debug == 1)
			Serial.println(velocity, DEC);
	}
	if (actMMS->Filter2 && !Menus[SETTINGS]->byPass)
	{
		note = scalenote(note, actMMS->Filter3);
		if (note == 255)
			return;
	}
	//	noteact[note] = false;
	if (chordnotes[chordptr - 1] == note)
	{
		isChord = false;
	}
	if (debug == 1)
		Serial.print("Note Off =");
	if (debug == 1)
		Serial.println(note, DEC);
	if (!Menus[SETTINGS]->byPass && actMMS->mapEnabled)
	{
		if (chords[baseNoteID[note]])
		{
			chords[baseNoteID[note]]->noteOff();
			if (debug == 1)
				Serial.println("noff " + String(baseNoteID[note]));
			baseNoteID[note] = 0;
		}
		else
		{
			actMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
			actMMS->sourceCH = channel;
			actMMS->sourceNote = note + octave * 12 + semiTone;
			actMMS->SourceCC = 0;
			actMMS->SourceVelocity = velocity;
			actMMS->action(false);
			if (debug == 1)
				Serial.println("single note off");
		}
	}
	else
	{
		actMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
		actMMS->sourceCH = channel;
		actMMS->sourceNote = note;
		actMMS->SourceCC = 0;
		actMMS->SourceVelocity = velocity;
		actMMS->action(false);
	}
}
void myNoteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
	actNoteOff(channel, note, velocity, HostMMS);
}
void usbNoteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
	actNoteOff(channel, note, velocity, USBMMS);
}
void serNoteOff(unsigned char channel, unsigned char note, unsigned char velocity)
{
	actNoteOff(channel, note, velocity, SerialMMS);
}
void myAfterTouchPoly(unsigned char channel, unsigned char note, unsigned char velocity)
{
	if (HostMMS->CH != channel || HostMMS->isTB == 0)
		return;
	if (!isChordend && !Menus[SETTINGS]->byPass)
	{
		myTimer.end();
		processChord();
		isChordend = true;
	}
	if (0)
	{
		if (debug == 1)
			Serial.print("AfterTouch Change, ch=");
		if (debug == 1)
			Serial.print(channel, DEC);
		if (debug == 1)
			Serial.print(", note=");
		if (debug == 1)
			Serial.print(note, DEC);
		if (debug == 1)
			Serial.print(", velocity=");
		if (debug == 1)
			Serial.println(velocity, DEC);
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

void actControlChange(signed char channel, signed char control, signed char value, MenuMidiSet *actMMS)
{
#if 1
	if (debug == 1)
		Serial.print("Control Change, ch=");
	if (debug == 1)
		Serial.print(channel, DEC);
	if (debug == 1)
		Serial.print(", control=");
	if (debug == 1)
		Serial.print(control, DEC);
	if (debug == 1)
		Serial.print(", value=");
	if (debug == 1)
		Serial.println(value, DEC);
#endif
	if (Menus[SETTINGS]->noPanic && control == 123)
		return;
	actMMS->eventtype = 2; // 0: note-off,1: Note on event; 2: CC event;3: pitch
	actMMS->sourceCH = channel;
	actMMS->sourceNote = 0;
	actMMS->SourceCC = control;
	actMMS->SourceVelocity = value;
	actMMS->action(true);
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
	if(debug==1)Serial.print("Program Change, ch=");
	if(debug==1)Serial.print(channel, DEC);
	if(debug==1)Serial.print(", program=");
	if(debug==1)Serial.println(program, DEC);
}

void myAfterTouchChannel(signed char channel, signed char pressure) {
	if(debug==1)Serial.print("After Touch, ch=");
	if(debug==1)Serial.print(channel, DEC);
	if(debug==1)Serial.print(", pressure=");
	if(debug==1)Serial.println(pressure, DEC);
	//  usbMIDI.sendAfterTouch(pressure, channel, cable);
}
#endif
void actPitchChange(u_int8_t channel, int pitch, MenuMidiSet *actMMS)
{
	if (debug == 1)
		Serial.print("Pitch Change, ch=");
	if (debug == 1)
		Serial.print(channel, DEC);
	if (debug == 1)
		Serial.print(", pitch=");
	if (debug == 1)
		Serial.println(pitch, DEC);
	actMMS->eventtype = 3; // 0: note-off,1: Note on event; 2: CC event;3: pitch
	actMMS->sourceCH = channel;
	actMMS->sourceNote = 0;
	actMMS->SourceCC = 0;
	actMMS->SourceVelocity = pitch;
	actMMS->action(true);
}
void myPitchChange(u_int8_t channel, int pitch)
{
	actPitchChange(channel, pitch, HostMMS);
}
void usbPitchChange(u_int8_t channel, int pitch)
{
	actPitchChange(channel, pitch, USBMMS);
}
void serPitchChange(u_int8_t channel, int pitch)
{
	actPitchChange(channel, pitch, SerialMMS);
}

void setmidi()
{
	usbMIDI.setHandleNoteOn(usbNoteOn);
	usbMIDI.setHandleNoteOff(usbNoteOff);
	usbMIDI.setHandleControlChange(usbControlChange);
	usbMIDI.setHandlePitchChange(usbPitchChange);
	//	for (int i = 0; i < 128; i++)
	//		noteact[i] = false;
	myusb.begin();
	Serial.println("setting MIDI");
	midi1.setHandleNoteOn(myNoteOn);
	midi1.setHandleNoteOff(myNoteOff);
	midi1.setHandleAfterTouchPoly(myAfterTouchPoly);
	midi1.setHandleControlChange(myControlChange);
	//	midi1.setHandlePitchChange(myPitchChange);

	MIDI_SER.setHandleNoteOn(serNoteOn);
	MIDI_SER.setHandleNoteOff(serNoteOff);
	MIDI_SER.setHandleControlChange(serControlChange);
	MIDI_SER.setHandlePitchBend(serPitchChange);
	HostMMS = (MenuMidiSet *)Menus[HOST_MIDI_IN];
	USBMMS = (MenuMidiSet *)Menus[USB_MIDI_IN];
	SerialMMS = (MenuMidiSet *)Menus[MIDI_IN];
}
