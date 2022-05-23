MD_MIDIFile SMF;
#define USE_MIDI 0 // set to 1 to enable MIDI output, otherwise debug output

#if USE_MIDI // set up for direct MIDI serial output

#define DEBUG(x)
#define DEBUGX(x)
#define DEBUGS(s)
#define SERIAL_RATE 31250

#else // don't use MIDI to allow printing debug statements

#define DEBUG(x) Serial.print(x)
#define DEBUGX(x) Serial.print(x, HEX)
#define DEBUGS(s) Serial.print(F(s))
#define SERIAL_RATE 57600

#endif // USE_MIDI

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
#if 0
	if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
	{
		Serial.write(pev->data[0] | pev->channel);
		Serial.write(&pev->data[1], pev->size - 1);
	}
	else
		Serial.write(pev->data, pev->size);

	DEBUG("\n");
	DEBUG(millis());
	DEBUG("\tM T");
	DEBUG(pev->track);
	DEBUG(":  Ch ");
	DEBUG(pev->channel + 1);
	DEBUG(" Data ");
	for (uint8_t i = 0; i < pev->size; i++)
	{
		DEBUGX(pev->data[i]);
		DEBUG(' ');
	}
	DBG(pev->data[0]);


    if (pev->data[0] == 0xb0)
    {
        if (curMMS->targets->UMO)
            usbMIDI.sendControlChange(pev->data[1], pev->data[2], pev->channel + 1);
        if (curMMS->targets->MO1)
            MIDI1.sendControlChange(pev->data[1], pev->data[2], pev->channel + 1);
        if (curMMS->targets->MO2)
            MIDI2.sendControlChange(pev->data[1], pev->data[2], pev->channel + 1);
        if (curMMS->targets->SYNTH && pev->data[1]<NUMSYNTH)
        {
            Parameter *mP = MenuPara::SynthMenu->getPara(pev->data[1]);
            //					FDBG("CC " + SN(sourceCC));
            if (mP)
            {
                mP->fvalue = fmap(pev->data[2], 0, 128, mP->fvstart, mP->fvend);
                mP->setSynthVal(__CALLER__);
            }
        }
//        FDBG("control " + SN(pev->data[1]));
    }
#endif
//    FDBG(SN(pev->channel + 1) + " " + SP(pev->data[0]));
    if (pev->data[0] == 0xc0)
    {
        actProgramChange(pev->channel + 1, pev->data[1], curMMS);
    }
    if (pev->data[0] == 0x80)
    {
//        FDBG(SN(pev->channel + 1) + " " + SP(pev->data[0]));
        webgui.setMonitor(sbp, offled);
        actNoteOff(pev->channel + 1, pev->data[1], pev->data[2], curMMS);
    }
    if (pev->data[0] == 0xB0)
    {
        webgui.setMonitor(sbp, blueled);
        actControlChange(pev->channel + 1, pev->data[1], 0, curMMS);
    }
    else if (pev->channel != curMMS->outCH && curMMS->outCH != 0)
        return;
    if (pev->data[0] == 0x90)
    {
        webgui.setMonitor(sbp, greenled);
        actNoteOn(pev->channel + 1, pev->data[1], pev->data[2], curMMS);
    }
}
void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs
// to be processed through the midi communications interface. Most sysex events cannot
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{
    DEBUG("\nS T");
    DEBUG(pev->track);
    DEBUG(": Data ");
    for (uint8_t i = 0; i < pev->size; i++)
    {
        DEBUGX(pev->data[i]);
        DEBUG(' ');
    }
    if (curMMS->targets->UMO)
    {
        usbMIDI.sendSysEx(pev->size, pev->data);
    }
    if (curMMS->targets->MO1)
    {
        MIDI1.sendSysEx(pev->size, pev->data);
    }
    if (curMMS->targets->MO2)
    {
        MIDI2.sendSysEx(pev->size, pev->data);
    }
}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
    STACK;
    midi_event ev;

    // All sound off
    // When All Sound Off is received all oscillators will turn off, and their volume
    // envelopes are set to zero as soon as possible.
    ev.size = 0;
    ev.data[ev.size++] = 0xb0;
    ev.data[ev.size++] = 123;
    ev.data[ev.size++] = 0;
    for (int t = 0; t < 16; t++)
    {
        ev.channel = t;
        midiCallback(&ev);
    }
    ev.size = 0;
    ev.data[ev.size++] = 0xb0;
    ev.data[ev.size++] = 120;
    ev.data[ev.size++] = 0;
    for (int t = 0; t < 16; t++)
    {
        ev.channel = t;
        midiCallback(&ev);
    }
    for (v = 0; v < NUMVOICES; v++)
    {
        vcaenv[v]->noteOff();
        vcfenv[v]->noteOff();
//        FDBG(SN(v) + " " + SB(vcfenv[v]->isSustain()));
    }
    STACK;
}
void tickMetronome(void)
// flash a LED to the beat
{
    static uint32_t lastBeatTime = 0;
    static boolean inBeat = false;
    uint16_t beatTime;

    beatTime = 60000 / SMF.getTempo(); // msec/beat = ((60sec/min)*(1000 ms/sec))/(beats/min)
    if (!inBeat)
    {
        if ((millis() - lastBeatTime) >= beatTime)
        {
            lastBeatTime = millis();
            digitalWrite(13, HIGH);
            ledstate[13] = 1;
            inBeat = true;
        }
    }
    else
    {
        if ((millis() - lastBeatTime) >= 100) // keep the flash on for 100ms only
        {
            digitalWrite(13, LOW);
            ledstate[13] = 0;
            inBeat = false;
        }
    }
}