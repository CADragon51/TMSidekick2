extern byte _channel, cable;
extern MenuMidiSet *curMMS;
extern int baseNoteID[128];
Chord *actChord = 0;
void Chord::noteOn(byte base, byte pchannel, byte velocity)
{
    //	short del = 0 * Menus[SETTINGS]->strum;
    //       if(debug==1)DBG(String(num)+" chord note on start del:" + String(del));
    channel = pchannel;
    for (int i = 0; i < num; i++)
    {
        //		FDBG(SN(millis()) + " " + SN(nextEvent - 1) + " " + SN(this->_scid) + " " + name + " " + " note:" + String(midiNamesLong[base + note[i]]));
        curMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
        curMMS->sourceCH = channel;
        curMMS->sourceNote = base + note[i];
        curMMS->SourceCC = 0;
        curMMS->SourceVelocity = velocity;
        curMMS->action(true, __CALLER__);
        //		if (del > 0)
        //			delay(del);
        //		baseNoteID[curMMS->sourceNote] = _scid;
    }
    //      if(debug==1)DBG("chord note on end");
    basenote = base;
    actChord = this;
}
void Chord::noteOff()
{
    //       if(debug==1)DBG("chord note off start");
    //	short del = Menus[SETTINGS]->strum;
    if (actChord == 0)
        return;
    STACK;
    for (int i = 0; i < num; i++)
    {
        //        FDBG(SN(i));
        curMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
        curMMS->sourceCH = channel;
        curMMS->sourceNote = basenote + note[i];
        curMMS->SourceCC = 0;
        curMMS->SourceVelocity = 0;
        curMMS->action(false, __CALLER__);
        //		if (del > 0)
        //			delay(del);
        baseNoteID[curMMS->sourceNote] = 0;
    }
    actChord = 0;
    STACK;

    //     if(debug==1)DBG("chord note off end");
}
bool Button::checkButton(bool nocall)
{
    int mreading = 1 - digitalRead(inp);
    hasChanged = false;
    if (mreading != mlastButtonState)
    {
        mlastDebounceTime = millis();
    }
    if ((millis() - mlastDebounceTime) > mdebounceDelay)
    {
        if (mreading != mbuttonState)
        {
            mbuttonState = mreading;
            if (mbuttonState == HIGH)
            {
                //
                FDBG("button click @" + String(inp));

                if (inp == triggerButton) //||(inp==4&&Menus[menuState]->isRat))
                {
                    hasChanged = true;
                    trigger = true;
                    Menus[EXT_SWITCH]->rawvalue = 1;
                    DBG(SN(Menus[EXT_SWITCH]->rawvalue));
                    return (true);
                }
                if (inp == cancelButton)
                {
                    //						DBG("cancel button click now ");
                    //						DBG("cancel button click ");
                    //						dumpDisplay();

                    DBG(seqOn);
                    if (transport == STOPPED)
                    {
                        transport = REPEAT;
                        nextEvent = 0;
                    }
                    else if (transport == REPEAT)
                    {
                        transport = STOPPED;
                        nextEvent = 0;
                        String notes[16];
                        //                       digitalWrite(13, LOW);
                        ledstate[13] = 0;
                        int nn = SplitS(seqOn, ',', notes, 16);
                        DBG(nn);
                        for (int n = 0; n < nn - 1; n++)
                        {
                            actNoteOff(sequences[0]._channel, notes[n].toInt(), 0, curMMS);
                        }
                        return true;
                    }
                    if (menuState == NEWMAP)
                        inversion(true);
                    if (!nocall)
                        onBackButton(Menus[menuState]);
                    trigger = true;
                    return true;
                }
                else if (inp == 29)
                {
                    *var = 1 - *var;
                    flash();
                    saveTMS();
                    onShow();
                }
                else
                {

                    DBG("button click " + String(inp));
                    onButton(Menus[menuState]);
                }
            }

            else if (inp == triggerButton || inp == cancelButton)
            {
                hasChanged = true;
                trigger = false;
                if (inp == triggerButton)
                {
                    Menus[EXT_SWITCH]->rawvalue = 0;
                    DBG(SN(Menus[EXT_SWITCH]->rawvalue));
                }
                return (false);
            }
        }
    }
    mlastButtonState = mreading;
    return (mreading);
}
void MenuPara::checkControl(int type, int id, int select, float value, bool swval)
{
    //		DBG(SN(type) + " : " + SN(id));
    int sp = 0;
    Parameter *para = 0;
    sp = id2para[id];
    para = ((MenuPara *)Menus[menId[id]])->Paras[sp];
    if (para == nullptr)
    {
        FDBG(SP(para) + " " + SN(sp) + " " + SN(menId[id]));
        return;
    } // Serial.println("myID =" + SN(((MenuPara *)Menus[menId[id]])) + " -> " + SN(para->name));

    FDBG(para->name + " " + SN(type) + " " + SN(sp) + " " + SN(value) + para->dumpValue());
    STACK;
    switch (type)
    {
    case SLIDER:
        //       FDBG(para->name + " " + SN(value));
        if (para->value != nullptr)
            *para->value = value;
        else if (para->tvalue != nullptr)
            *para->tvalue = value;
        else
            para->fvalue = value;
        FDBG(para->name + " " + SN(type) + " " + SN(sp) + " " + SN(value) + para->dumpValue());
        webgui.setMonitor(id - 1, para->format());
        STACK;
        if (para->name == "BPM")
        {
            STACK;
            mettime = 2000000 * 120.0 / Menu ::BPM;
            metTimer.end();
            if (metison)
            {
                metTimer.begin(playPattern, mettime / 48);
                patternc = -1;
            }
            STACK;
        }
        break;
    case OPTION:
        *para->value = value;
        //       FDBG(para->name + " " + SN(value));
        if (para->name == "Beatlength")
        {
            *para->value = value + 1;
            //           FDBG(para->name + " " + SN(*para->value));
            showKeys();
            showStatus(actpattern);
        }
        if (para->name == "Scale")
            Menus[SCALES]
                ->scaleit(scFP[(int)value]);
        if (para->name == "Syn. Para")
        {
            Parameter *SynthPara = MenuPara::SynthMenu->getPara((int)value);
            if (SynthPara)
            {
                //                FDBG(SN(SynthPara->source) + " ->" + SN(targets->me));
                //			((MenuTargetSet *)Menus[para->source])->SynthPara = 0;
                SynthPara->source = targets->me;
            }
        }
        //           DBG(scFP[(int)value]);
        break;
    case CHECKBOX:
        DBG(SN(id) + " switched " + SN(sp) + " " + para->name);
        *para->value = swval;
        webgui.setMonitor(idt, showConnections());
        break;
    }
    STACK;
    saveTMS();
    STACK;
}
int numswitches;
int me = 0;
void MenuTargetSet::createControl(String caller)
{
    int base = 300 + lastXPos;
    bool sws[1];
    int posx = base;
    int posy = 90;
    String swn = "";
    numswitches = 0;
    //        maxg = 0;
    //        DBG("myID :" + SN(myID));
    if (maxiteml == 0)
        setLength();
    if (maxg > 399)
        maxg = 0;
    int widthl = maxiteml * 6;
    int widthr = maxvall * 6;
    int j = 1;
    posx = base + widthl + widthr;
    //        maxg = 0;
    for (int i = 1; i < nump + 1; i++)
    {
        if (Paras[i] == 0)
            continue;
        if (!Paras[i]->isAnalog)
            continue;
        int ve = Paras[i]->vend;
        if (Paras[i]->isFloat)
            ve = Paras[i]->fvend;
        int vs = Paras[i]->vstart;
        if (Paras[i]->isFloat)
            vs = Paras[i]->fvstart;
        int vv = *Paras[i]->value;
        if (Paras[i]->isFloat)
            vv = Paras[i]->fvalue;
        String name = Paras[i]->name;
        //        FDBG(name);
        webgui.remove(Paras[i]->monid);
        int ret = webgui.addNumericDisplay(name, posx + j * 80 + 65, posy + 200, "f", "nomonitor");
        Paras[i]->monid = ret;

        webgui.setMonitor(ret, Paras[i]->format());
        webgui.remove(guiid[maxg]);
        guiid[maxg] = webgui.addInputAnalog(name, vs, ve, vv, &onSlider, posx + j * 80, posy, "title", "controlx");
        if (guiid[maxg] == -1)
            guiid[maxg] = webgui.addInputAnalog(name, vs, ve, vs, &onSlider, posx + j * 80, posy, "title", "controlx");
        DBG(Paras[i]->name + " " + SN(posx + maxg * 80) + "," + posy + " " + guiid[maxg]);
        id2para[guiid[maxg] % 1400] = i;
        menId[guiid[maxg] % 1400] = me;
        maxg++;
        j++;
    }
    j = 1;
    int lg = 0;
    int inpx = 250 + widthl + widthr + lastXPos;
    int inph = 29;
    int inpy = 100;
    if (maxvall > 16)
        inpx -= 50;
    for (int i = 1; i < nump + 1; i++)
    {
        if (Paras[i] == 0 || Paras[i]->isAnalog || Paras[i]->range == 0)
            continue;
        //       FDBG(Paras[i]->name + " " + Paras[i]->vend);
        if (Paras[i]->vend == 1)
        {
            sws[0] = *Paras[i]->value > 0;
            DBG(Paras[i]->name + " " + SB(sws[0]) + " " + SP(Paras[i]));
            webgui.remove(guiid[maxg]);
            guiid[maxg] = webgui.addSwitches(Paras[i]->name, 1, sws, &onSwitches, inpx, inpy + j++ * inph, "f", "nomonitor");
            id2para[guiid[maxg]] = i;
            //              DBG(Paras[i]->name + " " + SN(guiid[maxg]) + " " + SN(id2para[guiid[maxg]]));
            menId[guiid[maxg] % 400] = me;
            maxg++;
        }
        else if (Paras[i]->form != 0)
        {
            //               DBG(Paras[i]->name + " " + SN(Paras[i]->vend + 1) + " - " + SN(Paras[i]->form[0]) + " " + SN(*Paras[i]->value));
            int av = *Paras[i]->value;
            int pl = 8 * Paras[i]->mv;
            //                DBG("position " + SN(Paras[i]->mv) + " " + SN(pl) + " " + SN(inpx - pl));
            if (pl > 150)
                pl -= 40;
            if (Paras[i]->name == "Beatlength")
            {
                av = *Paras[i]->value - 1;
                FDBG(Paras[i]->name + " " + SN(av));
            }
            webgui.remove(guiid[maxg]);
            guiid[maxg] = webgui.addOptions(Paras[i]->name, Paras[i]->vend + 1, (String *)Paras[i]->form, &onOptionSelect, inpx - pl - 15, inpy + j++ * inph, av, "f", "nomonitor"); //*Paras[i]->value);
            lg++;
            id2para[guiid[maxg]] = i;
            menId[guiid[maxg]] = me;
            maxg++;
        }
    }
}
