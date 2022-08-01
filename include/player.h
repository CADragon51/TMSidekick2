int c = 0;
int lt = 10;
long lm = 0;
void playnextMidi(void)
{
    static float count = 0;
    //    float Bpm = ((MenuPara *)Menus[SETTINGS])->BPM;
    //    count += Bpm / 120.0;
    if (nextEvent == 0)
    {
        lm = millis();
        count = 0;
    }
    else
    {
        count += millis() - lm;
//        FDBG(SN(millis() - lm) + " " + SN(count) + " " + SN(sequences[nextEvent]._time));
    }
    lm = millis();
    //	printA4(lt);
    if (count < sequences[nextEvent]._time)
        return;
    int e = nextEvent++;

    if (nextEvent >= lastEvent)
    {
        nextEvent = 0;
        if (transport != REPEAT)
        {
            transport = STOPPED;
 //           playTimer.end();
            webgui.setMonitor(sbp, offled);
            digitalWrite(13, LOW);
        }
    }
    //  FDBG("seq " +SN(e)+" "+ SN(sequences[e]._event));
    if (sequences[e]._event == 0x80 || sequences[e]._velocity==0)
    {
        webgui.setMonitor(sbp, offled);
        actNoteOff(sequences[e]._channel, sequences[e]._note, sequences[e]._velocity, curMMS);
        seqOn.replace(SN(sequences[e]._note) + ",", "");
    }
    else if (sequences[e]._event == 0x90)
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
    //	FDBG(sequences[e]._id);
}
bool first = true;
int lastplay = 0;