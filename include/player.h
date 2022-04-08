int c = 0;
int lt = 10;

void playnextMidi(void)
{
    static float count = 0;
//    float Bpm = ((MenuPara *)Menus[SETTINGS])->BPM;
//    count += Bpm / 120.0;
    count ++;
    //	printA4(lt);
    if (count < lt)
        return;
    count = 0;
    int e = nextEvent++;
    if (e == 0)
        seqOn = "";
//    FDBG(SN(midiplay) + " " + SN(lt) + " " + SN(sequences[e]._time) + " " + SP(sequences[e]._event) + " " + SN(sequences[e]._note) + " " + SN(sequences[e]._velocity));
    lt = sequences[e + 1]._time - sequences[e]._time;
    if (lt < 20)
        lt = 0;
    else if (sequences[e]._event == 0x80 && qfactor > 0)
    {
        lt = qfactor;
    }

    //	midiplay -= lt;
    if (nextEvent >= lastEvent || lt < 0)
    {
        nextEvent = 0;
        lt = 10;
        if (transport != REPEAT)
        {
            transport = STOPPED;
            webgui.setMonitor(sbp, offled);
            digitalWrite(13, LOW);
            ledstate[13] = 0;
        }
    }
    if (sequences[e]._event == 0x80)
    {
        webgui.setMonitor(sbp, offled);
        actNoteOff(sequences[e]._channel, sequences[e]._note, sequences[e]._velocity, curMMS);
        seqOn.replace(SN(sequences[e]._note) + ",", "");
    }
    if (sequences[e]._event == 0x90)
    {
        webgui.setMonitor(sbp, greenled);
        actNoteOn(sequences[e]._channel, sequences[e]._note, sequences[e]._velocity, curMMS);
        seqOn += SN(sequences[e]._note) + ",";
    }
    if (sequences[e]._event == 0xB0)
    {
        webgui.setMonitor(sbp, blueled);
        actControlChange(sequences[e]._channel, sequences[e]._note, sequences[e]._velocity, curMMS);
    }
    digitalWrite(13, sequences[e]._event == 0x90 ? 1 : 0);
    ledstate[13] = sequences[e]._event == 0x90 ? 1 : 0;
    //	FDBG(sequences[e]._id);
}
bool first = true;
int lastplay = 0;