extern void midiSilence(void);

void update_pat(bool scroll = true)
{
    if (menuState != SETTINGS)
        return;
    //   FDBG(__CALLER__);
    int ap = actpattern;
    if (patvoicelow[ap] < MAXVOI - 4 && !startOver)
        startvoice = patvoicelow[ap];

    webgui.remove(ccmetid);
    //   FDBG(__CALLER__);
    int end = startvoice + 4;

    for (int v = 0; v < 4; v++)
    {

        webgui.remove(instopt[v]);
        webgui.remove(velopt[v]);
        webgui.remove(metopt[v]);
        webgui.remove(metid[v]);
        //       webgui.remove(voiceidt[v]);
    }
    showCCs();
    showStatus(actpattern, scroll);

    int ypos = 400;
    websetMonitor(patternidt, " Pattern #" + String(actpattern + zerobase));
    for (int i = 0; i < maxticks; i++)
        seqswitch[i] = ccpattern[actpattern * maxticks + i] > 0;
    // ccpattern[actpattern * maxticks + i] > 0;
    ccmetid = webgui.addSwitches("steps", maxticks, seqswitch, &onSwitches, 680, ypos, "x", "nomonitor");
    //   websetMonitor(ccpatternidt, "CC " + String(lastcc < 128 ? lastcc : 0));
    //   websetMonitor(ccvpatternidt, "CC Value " + String(lastccval < 128 ? lastccval : 0));
    //   FDBG(__CALLER__);
    int sv = startvoice >= 0 ? startvoice : 0;
    end = sv + 4;
    for (int v = sv, vv = 0; v < end && v < MAXVOI; v++, vv++)
    {
        //       FDBG("update voice " + SN(v));
        int ypos = 450 + (vv)*50;

        int inx = minstr[v] - 2;
        if (inx < 1 || inx > 127)
            inx = 1;
        if (pwx < 1 || pwx > 127)
            pwx = 16;
        //        FDBG("instr " + SN(inx) + " " + SN(mvelo[v]));
        //      webgui.remove(voiceidt[v]);
        websetMonitor(voiceidt[vv], String(v + zerobase));
        instopt[vv] = webgui.addOptions("instr", 127, perc, &onOptionSelect, 200, ypos, inx, "f");
        velopt[vv] = webgui.addOptions("velo", 128, counts, &onOptionSelect, 400, ypos, mvelo[v], "f");
        metopt[vv] = webgui.addOptions("Pattern", pwx, patvals, &onOptionSelect, 500, ypos, lastmet[v], "f", "metro"); //*Paras[i]->value)                                                                                                                                     //  FDBG(SN(pwx) + " " + patvals[0]);

        for (int i = 0; i < 2; i++)
        {
            String pas = pat2string(actpattern + i, v);
            String r = showRhythm(pas, i * 10 + v);
            //          FDBG("pattern " + SN(i) + "/" + SN(v) + " " + pas + " " + r);
            websetMonitor(patidt[vv][i], r);
        }

        for (int i = 0; i < maxticks; i++)
        {
            seqswitch[i] = seqpattern[actpattern * maxticks + i][v] > 0;
        }
        metid[vv] = webgui.addSwitches("steps", maxticks, seqswitch, &onSwitches, 680, ypos, "x", "nomonitor");
        //        FDBG(__CALLER__);
    }
    //    FDBG(__CALLER__);
}
void addPattern(short patin)
{
    //   FDBG(pat2string(patin) + " " +SN(patin));
    for (int i = 0; i < pwx; i++)
    {
        //       FDBG(patnames[i]);
        if (patnames[i] == patin)
            return;
    }
    if (pwx < 100)
    {
        patnames[pwx] = patin;
        patvals[pwx] = pat2string(patin, -1);
        //        FDBG(pwx);
        pwx++;
    }
}
bool cleared = false;
void onMessage(String value, int id);
void setDisplay(String outString)
{
    //    FDBG(outString);
    int block = 1000;
    webgui.remove(men);
    FSTACK;
    if (Menus[menuState]->isPara)
    {
        for (int i = 0; i < maxg; i++)
            webgui.remove(guiid[i]);
    }
    if (cliitem > 0)
        webgui.remove(cliitem);
    cliitem = webgui.addStringDisplay("cli", 50, 210, "f", "nomonitor");
    int ol = outString.length();
    //  FDBG(ol);
    if (ol < block)
        websetMonitor(cliitem, outString);
    else
    {
        int n = ol / block;
        //       int r = ol % block;
        String tmp = outString.substring(0, block);
        webgui.setMonitor(cliitem, tmp, 1);
        //       FDBG(SN(r) + SN(n) + SN(ol));
        for (int32_t i = 1; i < n; i++)
        {
            tmp = outString.substring(i * block, (i + 1) * block);
            webgui.setMonitor(cliitem, tmp, 2);
        }
        tmp = outString.substring(n * block);
        webgui.setMonitor(cliitem, tmp, 3);
    }
    cleared = true;
    addedDisplay = true;
}
void setBPM(int argc, String *argv)
{
    Menu ::BPM = argv[1].toInt();
    //          pattern = argv[2].toInt();
    mettime = 2000000 * 120.0 / Menu ::BPM;
    //            if (metison)
    clickc = 0;
    beatlength = argv[2].toInt();
    metTimer.end();
    //          if (metison)
    {
        playSeq = true;

        metTimer.begin(playPattern, mettime / 48);
        patternc = -1;
    }
}
void clearPat(int from, int to)
{

    for (int p = from; p <= to; p++)
    {
        beatCount[p] = 0;
        patfiles[p] = "";
        acttrigger[p] = 255;
        if (actbeatID[p] > -1 && menuState == SETTINGS)
            webgui.remove(actbeatID[p]);
        actbeatID[p] = -1;
    }

    lastvoice = 0;
    for (int v = 0; v < MAXVOI; v++)
    {
        for (int p = from; p <= 12 * to; p++)
        {
            {
                seqpattern[p][v] = -1;
                if (!v)
                    delaypattern[p] = 0;
            }
        }
    }
    update_pat(true);
}
void clearPat(void)
{

    for (int p = 0; p < 128; p++)
    {
        triggerNote[p] = -1;
        voices[p] = 255;
    }

    for (int p = 0; p < MAXPAT; p++)
    {
        beatCount[p] = 0;
        patfiles[p] = "";
        acttrigger[p] = 255;
        if (actbeatID[p] > -1 && menuState == SETTINGS)
            webgui.remove(actbeatID[p]);
        actbeatID[p] = -1;
    }

    lastvoice = 0;
    for (int v = 0; v < MAXVOI; v++)
    {
        for (int p = 0; p < 12 * MAXPAT; p++)
        {
            {
                seqpattern[p][v] = -1;
                if (!v)
                    delaypattern[p] = 0;
            }
        }
    }
    update_pat(true);
}
void setTrans(int argc, String *argv)
{
    int what = argv[1].toInt() - 1;
    if (what == -1)
    {
        argv[1].toLowerCase();
        if (argv[1] == "m-1")
            what = 17;
        if (argv[1] == "m-2")
            what = 18;
        if (argv[1] == "synth")
            what = 19;
        if (argv[1] == "cv1")
            what = 20;
        if (argv[1] == "usb")
            what = 16;
    }
    int how = argv[2].toInt();
    if (argv[2] == "on")
        how = 127;
    transposeit[what] = how;
    saveTMS();
    return;
}
void saveData(int argc, String *argv)
{
    if (argc > 2 && argv[1] == "drum")
    {
        saveDrum(argv[2]);
        return;
    }
    if (argc > 1 && argv[1] == "pat")
    {
        //           for (int p = 0; p < maxpattern; p++)
        //           {
        //               addPattern(pattern[p][0]);
        //           }
        for (int v = 0; v < startvoice + 4 && v < MAXVOI; v++)
        {
            int ypos = 500 + v * 50;
            webgui.remove(metopt[v]);
            metopt[v] = webgui.addOptions("Pattern", pwx, patvals, &onOptionSelect, 300, ypos, 0, "title", "metro"); //*Paras[i]->value);
        }
        FDBG("Saving patterns.pat");
        if (SD.exists("patterns.pat"))
        {
            SD.remove("patterns.pat");
            //           FDBG("deleted");
        }
        File frec = SD.open("patterns.pat", FILE_WRITE);
        if (frec)
        {
            for (int i = 0; i < pwx; i++)
            {
                frec.println(SN(patnames[i]) + " " + pat2string(patnames[i], -1));
            }
            frec.close();
        }
        return;
    }
    if (argc > 1)
    {
        String pname = argv[1];
        FDBG("Saving " + pname);
        int lE = lastEvent;
        if (argv[1].indexOf(".mid") > -1 && lE > 0)
        {
            seqFile = pname;
            if (SD.exists(pname.c_str()))
            {
                SD.remove(pname.c_str());
                //    if(debug==1)DBG("ini deleted");
            }
            File frec = SD.open(pname.c_str(), FILE_WRITE);
            if (frec)
            {
                createMidiFile();
                int delta = 0;
                int lasttime = 0;

                //            FDBG("midi " + SN(midiptr));
                for (int s = 0; s < lE; s++)
                {
                    delta = sequences[s]._time - lasttime;
                    //                FDBG(" " + SB(sequences[s]._event) + " delta " + SN(delta) + " " + SN(midiptr-22));
                    writeToFile(sequences[s]._event + sequences[s]._channel, sequences[s]._note, sequences[s]._velocity, delta);
                    lasttime = sequences[s]._time;
                }
                setTrackLength(midiptr - 14);
                Serial.printf("\n%04x ", 0);
                for (int i = 0; i < midiptr; i++)
                {
                    Serial.printf("%02x ", midifile[i]);
                    if (i % 16 == 15)
                        Serial.printf("\n%04x ", i + 1);
                }
                Serial.println();
                frec.write(midifile, midiptr);
                frec.close();
            }
        }
        if (argv[1].indexOf(".map") > -1)
        {
            //            comment = argv[argc].replace(argv[1], "").replace("save ", "").replace("sav ", "").replace("sa ", "");
            if (progressmode)
                comment = "Progression: " + comment + " " + mapComment;
            FDBG(comment);
            if (SD.exists(argv[1].c_str()))
            {
                SD.remove(argv[1].c_str());
                FDBG("deleted");
            }
            File frec = SD.open(argv[1].c_str(), FILE_WRITE);
            if (frec)
            {
                FDBG("Mapping saved to " + argv[1]);
                //                   FDBG(comment);
                frec.println(comment);

                for (int i = 0; i < maxrepl; i++)
                {
                    //            DBG(String(keychord[i]) + "=" + String(replchord[i]) + "+" + String(transpose[i]));
                    frec.println(String(keychord[i]) + "=" + String(replchord[i]) + "+" + String(transpose[i]));
                }
                frec.close();
            }
        }
        if (argv[1].indexOf(".prs") > -1)
        {
            if (SD.exists(argv[1].c_str()))
            {
                SD.remove(argv[1].c_str());
                //    DBG("ini deleted");
            }
            File frec = SD.open(argv[1].c_str(), FILE_WRITE);
            if (frec)
            {
                MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
                mPara->save(frec);
                frec.close();

                FDBG("Preset saved");
            }
        }
    }
}
void showData(int argc, String *argv)
{
    if (argc < 2)
        return;
    if (argv[1].indexOf("seq") > -1)
    {

        String event = "step time event note velocity~";
        for (int s = 0; s < lastEvent; s++)
        {
            event += SN(s) + " " + sequences[s].show() + "~";
        }
        String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + event + "</textarea>";
        setDisplay(out);
        //           FDBG(out);
    }
    if (argv[1].indexOf("drm") > -1)
    {

        String event = saveDrum("", true);

        String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + event + "</textarea>";
        setDisplay(out);
        //           FDBG(out);
    }
    if (argv[1].indexOf("cal") > -1)
    {
        String cal = "";

        for (int s = JOY_X; s < EXT_SWITCH; s++)
        {
            MenuExtSet *em = (MenuExtSet *)Menus[s];
            cal += em->name + " " + SN(em->emin) + " " + SN(em->emax) + " " + SN(em->rawvalue) + "~";
            FDBG(em->name + " " + SN(em->emin) + " " + SN(em->emax) + " " + SN(em->rawvalue));
        }
        String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=30>" + cal + "</textarea>";
        setDisplay(out);
    }
}
void impData(int argc, String *argv)
{
    o = "";
    webgui.remove(inbuts[3]);
    webgui.remove(impid);
    webgui.remove(showstat);
    webgui.remove(men);
    for (int i = 0; i < maxg; i++)
        webgui.remove(guiid[i]);
    cleared = true;
    impid = webgui.addInputString("import", &onMessage, 0, 200, "f", "scroll");
    impFile = argv[1];
    inimport = true;
    addedDisplay = true;
    digitalWrite(13, HIGH);
}
void procVoice(String proc, int from, int to)
{
    if (proc == "cp")
    {
        FDBG("copy voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            seqpattern[i][to] = seqpattern[i][from];
        }
    }
    if (proc == "mv")
    {
        FDBG("move voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            seqpattern[i][to] = seqpattern[i][from];
            seqpattern[i][from] = -1;
        }
    }
    if (proc == "sw")
    {
        FDBG("swap voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            short sw = seqpattern[i][to];
            seqpattern[i][to] = seqpattern[i][from];
            seqpattern[i][from] = sw;
        }
    }
    if (proc == "rm")
    {
        FDBG("delete voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            seqpattern[i][from] = -1;
        }
    }
    actpattern = startpattern;
    update_pat(false);
}
String mos = "";
EXTMEM String g_out;
void expData(int argc, String *argv)
{
    webgui.remove(inbuts[3]);
    webgui.remove(impid);
    webgui.remove(showstat);
    webgui.remove(men);
    cleared = true;

    if (argc == 3 && argv[1].indexOf("seq") > -1)
    {
        int l = 0;
        //        FDBG(argv[2]);
        createMidiFile();
        int delta = 0;
        int lasttime = 0;
        //            FDBG("midi " + SN(midiptr));
        for (int s = 0; s < lastEvent; s++)
        {
            delta = sequences[s]._time - lasttime;
            //                FDBG(" " + SB(sequences[s]._event) + " delta " + SN(delta) + " " + SN(midiptr-22));
            writeToFile(sequences[s]._event + sequences[s]._channel - 1, sequences[s]._note, sequences[s]._velocity, delta);
            lasttime = sequences[s]._time;
        }
        setTrackLength(midiptr - 14);
        FDBG(" Length " + SN(midiptr - 14));
        l = midiptr;
        // Serial.printf("\n%04x ", 0);
        // for (int i = 0; i < midiptr; i++)
        // {
        //     Serial.printf("%02x ", midifile[i]);
        //     if (i % 16 == 15)
        //         Serial.printf("\n%04x ", i + 1);
        // }
        // Serial.println();
        encode_base64(midifile, l, midibase64);
        o = (char *)midibase64;
        g_out = "<textarea class=\"scrollabletextbox\" id=\"export-data\" rows=10 cols=40 name=\"" + argv[2] + "\">" + o + "</textarea><input type=\"button\" id=\"dwn-btn\" value=\"export\" onclick=\"onclickbtn('" + argv[2] + "')\"/>";
        setDisplay(g_out);
        //     FDBG(out);
        return;
    }
    File myFile = SD.open(argv[1].c_str());
    int l = myFile.size();
    FDBG(argv[1] + " " + SP(myFile));
    if (myFile)
    {
        myFile.readBytes((char *)midifile, l);
        // close the file:
        myFile.close();
    }
    else
        return;
    // Serial.printf("\n%04x ", 0);
    // for (int i = 0; i < l; i++)
    // {
    //     Serial.printf("%02x ", midifile[i]);
    //     if (i % 16 == 15)
    //         Serial.printf("\n%04x ", i + 1);
    // }
    encode_base64(midifile, l, midibase64);
    o = (char *)midibase64;
    //   FDBG(o);
    //   FDBG(o.length());
    //            FDBG(o);
    //    if (l < 100000)
    {
        g_out = "<textarea class=\"scrollabletextbox\" id=\"export-data\" rows=10 cols=40 name=\"" + argv[1] + "\">" + o + "</textarea><input type=\"button\" id=\"dwn-btn\" value=\"export\" onclick=\"onclickbtn('" + argv[1] + "')\"/>";
        setDisplay(g_out);
    }
}

void onMessage(String value, int id)
{
    Serial.println(String(id) + " Message from the GUI: " + value);
    if (id == impid)
    {
        //       FDBG(strlen((char *)(bitstream + fq + 1)));
        //       FDBG(fq + 1);
        int l = decode_base64(bitstream + fq + 1, midifile);
        if (SD.exists(impFile.c_str()))
        {
            SD.remove(impFile.c_str());
            //    if(debug==1)DBG("ini deleted");
        }
        File frec = SD.open(impFile.c_str(), FILE_WRITE);
        if (frec)
        {
            int n = frec.write(midifile, l);
            frec.close();
            Serial.println(SN(n) + " Bytes written");
        }
        webgui.remove(impid);
        inimport = false;
        cleared = true;

        //        digitalWrite(13, LOW);
        return;
    }
    if (id == mapvers)
        version = value;
    if (id == prenameinp)
        presave = value;
    if (id == mapcom)
        comment = value;
    if (id == cli)
    {
        String argv[20];
        history = value;
        //        FDBG("set: " + history);

        int argc = SplitS(value.replace("  ", " "), ' ', argv, 20);
        String starts = argv[0].substring(0, 2);
        FDBG("starts with " + starts + " # " + SN(argc));
        webgui.remove(cliitem);
        //       cliitem = -1;
        if (starts == "bp" && argc > 2)
        {
            setBPM(argc, argv);
            return;
        }
        if (starts == "cl")
        {
            clearPat();
        }
        if (starts == "tr" && argc == 3)
        {
            setTrans(argc, argv);

            return;
        }
        if (starts == "rm" && argc > 1)
        {
            String pname = argv[1];
            if (pname.substring(0, 2) == "*.")
            {
                rmDirectory(SD.open("/"), argv[1]);
            }
            else
            {
                FDBG("deleting " + pname);
                if (SD.exists(pname.c_str()))
                {
                    SD.remove(pname.c_str());
                    //    if(debug==1)DBG("ini deleted");
                }
            }
        }
        if (starts == "mv" && argc > 2)
        {
            String pname = argv[2];
            FDBG("renaming " + pname);
            if (SD.exists(argv[1].replace("'", "`").c_str()))
            {
                SD.rename(argv[1].c_str(), argv[2].c_str());
                //    if(debug==1)DBG("ini deleted");
            }
        }
        if (starts == "cp" && argc > 2)
        {
            String pname = argv[2];
            FDBG("copy to " + pname);
            if (SD.exists(argv[1].replace("'", "`").c_str()))
            {
                File frec = SD.open(argv[1].c_str());
                File freccp = SD.open(pname.c_str(), FILE_WRITE);
                frec.read(midifile, frec.size());
                freccp.write(midifile, frec.size());
                frec.close();
                freccp.close();
            }
        }
        if (starts == "sc" && argc > 1)
        {
            g_scid = argv[1].toInt();
            g_scid = (g_scid - 1) / 2;
            Menus[SCALES]->scaleit(g_scid);
            //           FDBG(SN(g_scid) + " " + scaleNames[g_scid]);
            saveTMS();
        }
        if (starts == "sa")
        {
            argv[0] = value;
            saveData(argc, argv);
            return;
        }
        if (starts == "sx")
        {
            g_xoff = argv[1].toFloat();
            oldsa = -1;
            showStatus();
            FDBG("g_xoff=" + SN(g_xoff));
            return;
        }
        if (starts == "vo")
        {
            FDBG("vo " + argv[1] + " " + argv[2] + " " + SN(startpattern) + SN(actpattern));
            if (startpattern > -1 && actpattern > startpattern)
                procVoice(argv[1], argv[2].toInt(), argv[3].toInt());
            return;
        }
        if (starts == "am")
        {
            int nv = argv[1].toInt();
            //           FDBG(SN(startpattern) + SN(lastpattern) + SN(nv));
            for (int a = startpattern * maxticks; a <= lastpattern * maxticks; a++)
            {
                for (int v = 0; v < MAXVOI; v++)
                {
                    if (seqpattern[a][v] > 0)
                    {
                        int vel = velpattern[a][v] + nv;
                        if (vel > 127)
                            vel = 127;
                        seqpattern[a][v] = vel;
                    }
                }
            }
            return;
        }
        if (starts == "fl")
        {
            showflat = true;
            refresh = true;
        }
        if (starts == "#")
        {
            showflat = false;
            refresh = true;
        }
        if (starts == "sh")
        {
            showData(argc, argv);
        }
        if (starts == "ex")
        {
            expData(argc, argv);
        }
        if (starts == "im" && argc > 1)
        {
            impData(argc, argv);
        }
        if (starts == "ma" && argc > 1)
        {
            if (argv[1] == "n")
            {
                newmapmode = true;
                maxrepl = 0;
                websetMonitor(idt, noteLine(false));
            }
            else if (argv[1] == "a")
                newmapmode = true;
            else if (argv[1] == "e")
                newmapmode = false;
            else if (argv[1] == "o")
                originalmode = true;
            else if (argv[1] == "r")
                originalmode = false;
            else if (argv[1] == "p" && argc > 2)
            {
                // mapComment = value;
                int pargc = SplitS(value.replace("-", " "), ' ', res, 20);
                FDBG("split " + SN(argc) + res[2]);
                static String progression = "I,II,III,IV,V,VI,VII,"
                                            "IM7,IIM7,IIIM7,IVM7,VM7,VIM7,VIIM7,"
                                            "i,ii,iii,iv,v,vi,vii,"
                                            "i7,ii7,iii7,iv7,v7,vi7,vii7,"
                                            "I7,II7,III7,IV7,V7,VI7,VII7";
                static int np = 0;
                static String pres[70];
                static int p2k[7] = {1, 3, 5, 6, 8, 10, 11};
                static int g2k[10] = {72, 1096, 68, 516, 584, 9, 11};
                if (!np)
                    np = SplitS(progression, ',', pres, 70);
                progressmode = true;
                //               FDBG(__CALLER__);
                for (int i = 0; i < maxrepl && argv[2] == "n"; i++)
                {
                    keychord[i] = chordIds[i] * 100;
                    replchord[i] = chordIds[i];
                    transpose[i] = 0;
                }
                int as = 2;
                if (argv[2] == "n")
                    as = 3;
 //               FDBG(__CALLER__);
                for (int i = as, r = 0; i < pargc; i++, r++)
                {
                    //                FDBG(SN(i) + ":" + argv[i]+" "+SN(np));
                    //                   FDBG(argv[i]);
                    for (int p = 0; p < np; p++)
                    {

                        if (res[i] == pres[p])
                        {
                            int k = p2k[p % 7] - 1;
                            //                       int ik = i - 1;
                            //                           FDBG(SN(p / 7) + " " + SN(p % 7) + " " + res[p] + " " + SN(k) + " " + SN(ik) + " " + SN(keychord[ik]));
                            //                           FDBG(SN(p) + SN(k) + res[p]);
                            replchord[r] = g2k[p / 7];
                            FSTACK;
                            FDBG("target " + SN(k) + SN(keychord[0] % 100) + SN(keychord[r] % 100));
                            transpose[r] = (k - (keychord[r] % 100) % 12) % 12;
                        }
                    }
                }
                //               FDBG(__CALLER__);
                //          for (int i = 0; i < maxrepl; i++)
                //              FDBG(SN(i) + " " + SN(keychord[i]) + " " + SN(replchord[i]) + " " + SN(transpose[i]));
                websetMonitor(idt, noteLine(false));
            }
        }
        if (starts == "pc" && argc > 1)
        {
            if (argc == 2)
                actProgramChange(curMMS->outCH, argv[1].toInt(), curMMS);
            else
                actProgramChange(argv[2].toInt(), argv[1].toInt(), curMMS);
        }
        if (starts == "lo" && argc > 1)
        {
            mos = argv[2];
            int r = argv[1].indexOf("/");
            int rp = argv[1].indexOf(".");
            int ra = argv[1].indexOf("*");
            if (ra > -1 || (r > -1 && rp == -1))
            {
                if (!argv[1].startsWith("/"))
                    argv[1] = "/" + argv[1];
                if (argv[1].endsWith("/"))
                    argv[1] = argv[1].substring(0, argv[1].length() - 1);
                g_search = argv[1];
                int n = loadDirectory(SD.open("/"), g_search, "");
                res[0] = g_search + "/";
                preid = webgui.addOptions("files", n, res, &onOptionSelect, 50, 160, 0, "f");
                addedDisplay = true;
            }
            bool exist = SD.exists(argv[1].c_str());
            FDBG(argv[1] + " ok?" + SB(exist));
            if (!exist)
                return;

            if (argv[1].toLowerCase().endsWith(".mid"))
            {
                int v = 0;
                //       FDBG(ldir + "/" + entry.name());
                int bc = sizeMIDI(argv[1], v);
                String size = String(bc) + "/" + String(v);

                loadMIDI(argv[1] + "#" + size, argv[2]);
            }
            if (argv[1].toLowerCase().endsWith(".prs"))
            {
                loadPRS(argv[1]);
                refresh = true;
            }
            if (argv[1].toLowerCase().endsWith(".map"))
            {
                loadMap(argv[1]);
                refresh = true;
            }
            if (argv[1].toLowerCase().endsWith(".drm"))
            {
                loadDrum(argv[1]);
                if (menuState == SETTINGS)
                {
                    update_pat(true);
                }
                return;
            }
        }
        if (starts == "pv" && argc > 1)
        {
            mos = argv[2];
            int r = argv[1].indexOf("/");
            int rp = argv[1].indexOf(".");
            int ra = argv[1].indexOf("*");
            if (ra > -1 || (r > -1 && rp == -1))
            {
                if (!argv[1].startsWith("/"))
                    argv[1] = "/" + argv[1];
                if (argv[1].endsWith("/"))
                    argv[1] = argv[1].substring(0, argv[1].length() - 1);
                g_search = argv[1];
                int n = loadDirectory(SD.open("/"), g_search, "");
                res[0] = g_search + "/";
                preid = webgui.addOptions("files", n, res, &onOptionSelect, 50, 160, 0, "f");
                addedDisplay = true;
            }
        }
        if (starts == "pl" && argc == 2)
        {
            if (argv[1].toLowerCase() == "seq")
                playSeq = true;
            else
            {
                playSeq = false;
                SMF.restart();
            }
            transport = PLAYING;
        }
        if (starts == "mo" && argc == 2 && !argv[1].toLowerCase().endsWith(".ini") && !argv[1].toLowerCase().endsWith(".prs"))
        {
            File myFile = SD.open(argv[1].c_str());
            String more = "";
            //           FDBG(argv[1] + " " + SP(myFile));
            if (myFile)
            {
                //               FDBG(argv[1]);
                while (myFile.available())
                {
                    String fi = readline(myFile);
                    if (fi.length() > 1)
                    {
                        FDBG(fi);
                        fi.replace("\n", "");
                        more = more + fi + "~";
                    }
                }
            }
            // close the file:
            myFile.close();
            g_out = "<textarea class =\"scrollabletextbox\" name=\"note\" rows=10 cols=160>" + more + "</textarea>";
            FDBG(g_out.length());
            setDisplay(g_out);
        }
        if (starts == "he")
        {
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + help + "</textarea>";
            setDisplay(out);
        }
        if (starts == "ls")
        {
            if (!argv[1].startsWith("/"))
                argv[1] = "/" + argv[1];
            if (argv[1].endsWith("/"))
                argv[1] = argv[1].substring(0, argv[1].length() - 1);
            if (argv[1].indexOf(".") > -1)
                g_search = argv[1];
            else
                g_search = argv[1] + "/";
            dirout = "";
            //           FDBG(__CALLER__);
            loadDirectory(SD.open("/"), argv[1], "");
            //           FDBG(__CALLER__);
            dirout = g_search + "~" + dirout;

            //            String dir = printDirectory(SD.open("/"), argv[1], "");
            //          FDBG(dirout);
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=140>" + dirout + "</textarea>";
            //           FDBG(out);
            setDisplay(out);
        }
    }
}
enum ms
{
    NONE,
    TRANS,
    CHORDS
};
byte mstate = NONE;

void onOptionSelect(int option, int id)
{
    FDBG("option " + SN(id) + " @ " + SN(menuState) + SN(option));
    if (id == actgrpopt)
    {
        actpattern = option * 64;
        selopt = option;
        if (patvoicelow[actpattern] < MAXVOI - 4)
            startvoice = patvoicelow[actpattern];

        sa = (actpattern / 64) * 64;
        update_pat(true);
        return;
    }
    for (int v = startvoice; v < MAXVOI && v < startvoice + 4; v++)
    {
        if (id == metopt[v])
        {
            //           FDBG("met " + SN(option) + " " + SN(patnames[option]));
            lastmet[v] = option;
            int pos = actpattern * maxticks;
            //       FDBG("pos " + SN(pos) + " " + SN(actpattern) + " "+ SN(actpat));
            for (int i = 0; i < maxticks; i++)
            {
                bool pt = (patnames[option] & (1 << i)) > 0;
                if (pt)
                {
                    seqpattern[i + pos][v] = 100;
                }
                else
                    seqpattern[i + pos][v] = -1;
                //           FDBG(metid[i]);
            }
            update_pat(false);
            return;
        }
        if (id == instopt[v])
        {
            minstr[v] = option + 2;
            //           FDBG("met instr " + SN(minstr[v]));
            saveTMS();
            return;
        }
        if (id == velopt[v])
        {
            mvelo[v] = option;
            saveTMS();
            return;
        }
    }
    //   FDBG(SN(id) + " " + SN(upopt) + " " + SN(option) + " " + SN(mclick1) + " " + SN(mclick2));
    if (menuState == MAPPINGS)
    {
        STACK;
        for (int i = 0; i < 20; i++)
        {
            int tdx = i; //+actkeyidx;
                         //           FDBG(SN(i) + " " + SN() + " " + transId[i]);

            if (transId[i] == id && mstate == TRANS)
            {
                int b = keychord[tdx] % 100;
                //               int tr = (scalebase + transpose[tdx] + b) % 12;

                //              FDBG("tdx " +SN(tdx)+ SN(transpose[tdx]) +SN(b%12)+ SN(option) + SN((tr) % 12));
                if (option)
                    transpose[tdx] = option - 1 - b % 12;
                //               FDBG(SN(option) + " " + SN(tdx) + " " + SN(transpose[tdx]));
                STACK;
                webgui.remove(transId[i]);
                mstate = NONE;
                refresh = true;
                return;
            }
            if (chordselId[i] == id)
            {
                STACK;
                webgui.remove(chordselId[i]);
                if (option)
                    replchord[tdx] = chordIds[option + 10];
                mstate = NONE;
                STACK;
                refresh = true;
                //                refresh = true;
                return;
            }
        }
    }
    if (id == baseid)
    {
        scalebase = option;
        Menus[SCALES]->scaleit(g_scid);
        //       FDBG(String(id) + " Basenote selected: " + option);
        saveTMS();
    }
    if (id == preid)
    {
        //       replchord[actkeyidx] = chordIds[option];

        FDBG(String(id) + " selected: " + SN(option) + " " + res[option]);
        String opt = res[option];
        opt.toLowerCase();
        int nx = opt.indexOf("#");
        String file = opt;
        if (nx > -1)
            file = opt.substring(0, nx);
        FDBG("Loading " + file);

        if (opt.indexOf(".map") > -1)
        {
            loadMap(file);
        }
        else if (opt.indexOf(".prs") > -1)
        {
            loadPRS(file);
        }
        if (opt.indexOf(".mid") > -1)
        {
            previewMIDI(g_search.replace("*", "") + "/" + opt);
            return;
        }
        if (opt.indexOf(".drm") > -1)
        {
            loadDrum(file);
            if (menuState == SETTINGS)
            {
                update_pat(true);
            }
        }
        webgui.remove(preid);
        return;
    }

    if (id == scaleid)
    {
        g_scid = scFP[option];
        FDBG("option " + SN(g_scid) + " ->" + scaleNames[scFP[option]] + "<");
        Menus[SCALES]->scaleit(g_scid);
        if (g_scid == 2047)
            Menus[MAPPINGS]->mapit();
        saveTMS();
    }
    if (menuState == SYNTHSETTINGS)
    {
        ((MenuSynth *)MenuPara::SynthMenu)->checkControl(OPTION, id, 0, option, false);
        //        webgui.remove(prenameinp);
        //        prenameinp = webgui.addInputString("New Preset Name", &onMessage, 400, 100, "title", "control", presetnames[preindex]);
    }
    else
    {
        ((MenuPara *)Menus[menuState])->checkControl(OPTION, id, 0, option, false);
    }

    //   FDBG("option select");

    refresh = true;
}

void onButtonClick(int button, int id)
{
    FDBG(String(id) + F(" button was clicked") + " " + String(button));
    if (id == predelsave)
    {
        if (button == 1)
        {
            String name = presetnames[preindex];
            DBG(name + " will be deleted");
            if (SD.exists(name.c_str()))
            {
                SD.remove(name.c_str());
                //    if(debug==1)DBG("ini deleted");
            }
        }
        if (button == 0)
        {
            String pname = presave + ".PRS";
            DBG("Saving " + pname);
            if (SD.exists(pname.c_str()))
            {
                SD.remove(pname.c_str());
                //    if(debug==1)DBG("ini deleted");
            }
            File frec = SD.open(pname.c_str(), FILE_WRITE);
            if (frec)
            {
                //
                DBG("saving preset data ");
                ((MenuSynth *)Menus[SYNTHSETTINGS])->save(frec);
                frec.close();
            }

            File dir = SD.open("/", 0);
            Menus[PRESETS]->numitems = 1;
            px = 1;
            while (true)
            {
                File entry = dir.openNextFile();
                if (!entry)
                {
                    // no more files

                    DBG("**nomorefiles**");
                    break;
                }
                String nf = entry.name();
                String uf = entry.name();

                uf.toUpperCase();

                DBG(nf);
                if (uf.endsWith(".PRS") && px < 99)
                {
                    presetnames[px++] = nf;
                    //
                    DBG(nf);
                    Menus[PRESETS]->insertItem(nf.substring(0, nf.length() - 4));
                }
            }
        }
    }

    refresh = true;
}
int backState = 0;
extern void websetup(void);
void onButtonRelease(int button, int id)
{
    int inc = button == 0 ? 1 : -1;
    MenuSynth *mSynth = (MenuSynth *)MenuPara::SynthMenu;
    //    FDBG(String(id) + F(" button was released") + " " + String(button) + " @ " + SN(Menus[menuState]->backstate));
    if (menuState == MAPPINGS)
    {
        for (int i = 0; i < 20; i++)
        {
            // transId[i - st] = webgui.addButtons(midiNamesFlat[transpose[i] % 12], &onButtonRelease, x + 25, ycp + 200, "f", "smallhide");
            // menuId[i - st] = webgui.addButtons(cc->getName(), &onButtonRelease, x + 40, ycp + 200, "f", "smallhide");
            if (transId[i] == id)
            {
                //                FDBG(xtransId[i]);
                webgui.remove(transId[i]);
                if (showflat)
                    transId[i] = webgui.addOptions("Trans", 13, midiNamesOptF, &onOptionSelect, 20 + xtransId[i], 144 + 200, transpose[i] + 1, "f");
                else
                    transId[i] = webgui.addOptions("Trans", 13, midiNamesOptS, &onOptionSelect, 20 + xtransId[i], 144 + 200, transpose[i] + 1, "f");
                mstate = TRANS;
                return;
            }
            if (chordselId[i] == id)
            {
                //               FDBG(__CALLER__ + " " + SN(xtransId[i]) + SN(replchord[i])+SN(53));
                webgui.remove(chordselId[i]);
                chordselId[i] = webgui.addOptions("Chord", 53, chordopt, &onOptionSelect, xtransId[i] + 20, 144 + 200, replchord[i] + 1, "f");
                mstate = CHORDS;
                return;
            }
        }
        //      FDBG(actkeyidx);
        if (actkeyidx >= 12 && id == iab1)
        {
            actkeyidx -= 12;
            refresh = true;
        }
        if (actkeyidx+12 < maxrepl && id == iab2)
        {
            actkeyidx += 12;
            refresh = true;
        } //        DBG("buttn released" + String(__LINE__));
    }
    if (id == imbb && menuState == SYNTHSETTINGS && ((MenuSynth *)Menus[menuState])->actSubmenu != submod)
    {
        //        FDBG("submenu " + SN(((MenuSynth *)Menus[menuState])->actSubmenu));
        onBackButton(Menus[menuState]);
        return;
    }
    if (id == imbb)
    {
        int nmenuState = Menus[menuState]->backstate;
        //        FDBG("new state: " + SN(Menus[menuState]->backstate));
        menuState = nmenuState;
        Menus[menuState]->show(true, __CALLER__);
        newmapmode = false;
        return;
    }
    if (id == home)
    {
        menuState = MAIN;
        Menus[menuState]->show(true, __CALLER__);
        return;
    }
    if (menuState == SETTINGS)
    {
        for (int i = 0; i < MAXPAT; i++)
        {
            if (actbeatID[i] == id)
            {
#define pattrig (triggerNote[mettrigger] - 1)
                if (!metison)
                    actpattern = i;
                //             FDBG("new " + SN(i));
                if (patvoicelow[actpattern] < MAXVOI - 4)
                    startvoice = patvoicelow[actpattern];
                update_pat(false);
                if (editMode||beatCount[i]==0)
                    return;
                if (metison && mettrigger == acttrigger[i])
                {
                    metison = false;
                    metTimer.end();
                    return;
                }
 //               FDBG(SN(i)+SN(beatCount[i]));
                int ap = i;
                for (int j = i; j >= 0;j--)
                {
                    if (acttrigger[j]<128)
                    {
                        i = j;
                        break;
                    }
                }
 //               FDBG(SN(i) + SN(beatCount[i]));
                {
                    if (actbeatID[pattrig] != -1)
                    {
                        webgui.remove(actbeatID[pattrig]);
                        actbeatID[pattrig] = -1;
                    }
                    if (actbeatID[i] != -1)
                    {
                        webgui.remove(actbeatID[i]);
                        actbeatID[i] = -1;
                    }
                    if (acttrigger[i]<128)
                        mettrigger = acttrigger[i];
                    //                   FDBG(mettrigger);
                        actpattern = ap;
                        patternc = ap * maxticks;
                        if(ap!=i)
                            oldtrigger = mettrigger;
                        if (!metison)
                        {
                            metison = true;
                            metTimer.begin(playPattern, mettime / 48);
                            playSeq = true;
                        }
                }
                return;
            }
        }
        for (int i = 0; i < 10; i++)
        {
            if (movenext[i] == id)
            {
                FDBG("Button " + SN(i));
                if (i == 0)
                {
                    if (editMode && startpattern > -1 && lastpattern > startpattern)
                        clearPat(startpattern, lastpattern);
                }
                else if (i == 9)
                {
                    if (editMode && startpattern > -1 && lastpattern > startpattern)
                    {
                        int np = (lastpattern + 1 - startpattern);
                        //                    FDBG(np);
                        String pcol = coloring[lastColor % 4];
                        //    FDBG(patfiles[actpattern] + " LC " + SN(actpattern) + SN(lastColor) + SN(pcol));
                        lastColor++;
                        for (int a = startpattern, b = np; a <= lastpattern; a++, b--)
                        {
                            beatCount[a] = b;
                            patcolors[a] = pcol;
                            //                       FDBG(SN(np) + " " + SN(a) + " set beats " + SN(beatCount[a]));
                        }
                    }
                }
                else if (i == 3)
                {
                    metison = false;
                    metTimer.end();
                    transport = STOPPED;
                    loadMIDI(lastMidiFile, "n");
                }
                else if (i < 3)
                {
                    if (i == 1)
                        actpattern--;
                    if (i == 2)
                        actpattern++;
                    if (actpattern < 0)
                    {
                        actpattern = 0;
                    }
                    if (actpattern > MAXPAT - 1)
                    {
                        actpattern = MAXPAT - 1;
                    }
                    if (patvoicelow[actpattern] < MAXVOI - 4)
                        startvoice = patvoicelow[actpattern];
                    startOver = false;
                }
                //              FDBG("actpat " + SN(actpattern));
                else if (i == 4 || i == 5)
                {
                    startOver = true;
                    if (i == 4)
                        startvoice--;
                    if (i == 5)
                    {
                        if (startvoice != -1)
                            startvoice++;
                    }
                    if (startvoice < -1)
                        startvoice = -1;
                    if (startvoice > MAXVOI - 4)
                        startvoice = MAXVOI - 4;
                }
                //               FDBG("startvoice " + SN(startvoice));
                //               FDBG("startpattern " + SN(startpattern) + " " + SN(i));
                else if (i == 6)
                    startpattern = actpattern;
                else if (i == 7 && startpattern != -1)
                {
                    lastpattern = actpattern - 1;
                    {
                        for (int a = startpattern * maxticks, b = 0; a <= lastpattern * maxticks; a++, b++)
                        {
                            for (int v = 0; v < MAXVOI; v++)
                                velpattern[a][v] = seqpattern[a][v];
                        }
                        //                       FDBG(SN(editMode) + SN(lastpattern));
                    }
                }
                else if (i == 8)
                    saveDrum("TMS");

                update_pat(true);
                return;
            }
        }
    }
    if (id == backmetro)
        metisback = !metisback;
    if (id == metro)
    {
        metison = !metison;
        if (metison)
        {
            clickc = 0;

            metTimer.end();
            //           if (metison)
            {
                if (triggerNote[mettrigger] > 0)
                    actpattern = triggerNote[mettrigger] - 1;
                patternc = actpattern * maxticks;
                //               FDBG(SN(mettrigger) + SN(triggerNote[mettrigger]-1) + SN(actpattern) + SN(patternc));
                metTimer.begin(playPattern, mettime / 48);
            }
        }
        else
        {
            metTimer.end();
            midiSilence();
            if (menuState == SETTINGS)
                showStatus(actpattern, false);
        }
        return;
    }
    for (int i = 0; i < 20; i++)
    {
        if (menuId[i] == id)
        {
            backState = menuState;
            Menus[menuState]->rot_pos = i + 1;
            FDBG("back state: " + SN(Menus[menuState]->backstate) + " " + SN(i));
            if (menuState == SYNTHSETTINGS)
            {
                //               FDBG(SN(i) + " was selected");
                if (i == 7)
                    i = 1;
                Menus[menuState]->rot_pos = i + 1;
                mSynth->onButton();
            }
            else
                onButton(Menus[menuState]);
            return;
        }
    }
    for (int i = 0; i < 9; i++)
    {
        if (outbuts[i] == id)
        {
            FDBG("button " + SN(i));
            backState = menuState;
            switch (i)
            {
            case 1:
                semiTone--;
                if (semiTone < -12)
                    semiTone = -12;
                break;
            case 0:
                semiTone++;
                if (semiTone > 12)
                    semiTone = 12;
                break;
            case 2:
                octave++;
                if (octave > 6)
                    octave = 6;
                break;
            case 4:
                octave--;
                if (octave < -6)
                    octave = -6;
                break;
            case 8:
                Menus[SETTINGS]->procMode = (Menus[SETTINGS]->procMode + 1) % 4;
                break;
            case 3:
                menuState = SYNTHSETTINGS;
                ((MenuSynth *)Menus[menuState])->setMonitor();
                break;
            }
            Menus[menuState]->backstate = backState;
            Menus[menuState]->show(true, __CALLER__);
            //            FDBG("from " + SN(Menus[menuState]->backstate) + " to " + SN(menuState));
            saveTMS();
            return;
        }
    }
    for (int i = 0; i < 9; i++)
    {
        if (inbuts[i] == id)
        {
            backState = menuState;
            //            FDBG("new " + SN(i));
            switch (i)
            {
            case 0:
                menuState = USB_MIDI_IN;
                break;
            case 1:
                menuState = HOST_MIDI_IN;
                break;
            case 2:
                menuState = MIDI_IN;
                break;
            case 3:
                menuState = SETTINGS;
                break;
            case 4:
                menuState = EXTERNALS;
                break;
            case 5:
                menuState = TOUCH;
                break;
            case 6:
                menuState = MAPPINGS;
                break;
            case 7:
                menuState = SCALES;
                break;
            }
            Menus[menuState]->backstate = backState;
            //            Menus[menuState]->show(true, __CALLER__);
            //           FDBG("from " + SN(Menus[menuState]->backstate) + " to " + SN(menuState));
            return;
        }
    }
    if (id == ipm)
    {
        if (Menus[SETTINGS]->procMode + inc <= 3 && Menus[SETTINGS]->procMode + inc >= 0)
        {
            Menus[SETTINGS]->procMode = Menus[SETTINGS]->procMode + inc;
            saveTMS();
        }
    }
    else if (id == openFile)
    {
        static bool notfile = true;
        FDBG(cliitem);
        if (!addedDisplay)
            webgui.remove(cli);
        else
        {
            webgui.remove(cliitem);
            addedDisplay = false;
            webgui.remove(mapid);
            webgui.remove(impid);
        }
        inimport = false;
        //       FDBG("clear? " + SB(cleared));
        if (Menus[menuState]->isPara && cleared)
        {
            String outmen = "<t1 style=\" text-align : left;\">Options</t1> ";
            outmen += ((MenuPara *)Menus[menuState])->setMenu();
            men = webgui.addStringDisplay("Menu", 200, 90, "f");
            websetMonitor(men, outmen);
            maxg = 0;
            Menus[menuState]->createControl(__CALLER__);
            cleared = false;
        }
        if (menuState == MAIN)
        {
            showstat = webgui.addStringDisplay("Status", 10, 200 + 80, "f");
            webgui.remove(inbuts[3]);
            inbuts[3] = webgui.addButtons("Rhythm/Settings", &onButtonRelease, 10, 150 + 80, "f", "hide");
            websetMonitor(showstat, "Status");
        }
        if (notfile)
        {
            webgui.remove(cli);
            cli = webgui.addInputString("Command ", &onMessage, 50, 135, "title", "control", history);
        }
        notfile = !notfile;
    }
    else if (id == sipm)
    {
        //        FDBG("transport " + SN(button - id * 10));
        transport = button - id * 10;
        STACK;
        switch (transport)
        {
        case RECORDING:
            MidiEvent::starttime = 0;
            digitalWrite(13, HIGH);
            playSeq = true;
            STACK;
            lastEvent = 0;
            if (menuState == SETTINGS)
            {
                mettime = 2000000 * 120.0 / Menu ::BPM;
                //            if (metison)
                clickc = 0;
                metison = true;
                metTimer.end();
                //          if (metison)
                {
                    metTimer.begin(playPattern, mettime / 48);
                    patternc = -1;
                }
            }
            break;
        case STOPPED:
        {
            digitalWrite(13, LOW);
            String notes[16];
            STACK;
            metison = false;
            metTimer.end();
            showStatus(actpattern, false);
            sequences[lastEvent].init(0x90, sequences[lastEvent - 1]._note, 0, sequences[lastEvent - 1]._channel);
            sequences[lastEvent]._time = sequences[lastEvent - 1]._time + 10;
            lastEvent++;
            if (seqOn != "")
            {
                int nn = SplitS(seqOn, ',', notes, 16);
                for (int n = 0; n < nn - 1; n++)
                {
                    //               FDBG(notes[n]);
                    actNoteOff(sequences[0]._channel, notes[n].toInt(), 0, curMMS);
                }
            }
            midiSilence();
            STACK;
        }
        break;
        case PLAYING:
            if (transport == RECORDING)
            {
                digitalWrite(13, LOW);
            }
            //           nextEvent = 0;
            STACK;
            nextEvent = 0;
            SMF.looping(false);
            SMF.restart();
            websetMonitor(sbp, greenled);
            //           FDBG("Play to " + SN(lastEvent));
            //           FDBG(transport);
            STACK;
            break;
        case REPEAT:
            STACK;
            SMF.looping(true);
            SMF.restart();
            nextEvent = 0;
            //            transport = PLAYING;
            websetMonitor(sbp, greenled);
            STACK;
            break;
        case REWIND:
            STACK;
            nextEvent = 0;
            SMF.restart();
            digitalWrite(13, LOW);
            break;
        }
    }

    else if (id == isb)
    {
        semiTone += inc;
        saveTMS();
    }
    else if (id == iob)
    {
        octave += inc;
        saveTMS();
    }
}

void onSlider(float value, int id)
{
    STACK;
    if (menuState == SYNTHSETTINGS)
    {
        ((MenuSynth *)MenuPara::SynthMenu)->checkControl(SLIDER, id, 0, value, false);
        return;
    }
    else
    {
        ((MenuPara *)Menus[menuState])->checkControl(SLIDER, id, 0, value, false);
    }
}

void onSwitches(bool *value, int id)
{
    //   FDBG("New switch status: " + SN(id) + " " + SN(value[0] ? "on" : "off"));
    if (menuState == SETTINGS)
    {
        if (id == actpatid)
        {
            editMode = value[0];
            return;
        }
        //       short ap = (actpattern / (beatlength));
        if (id == ccmetid)
        {
            for (int i = 0; i < maxticks; i++)
            {
                int p = actpattern * maxticks + i;
                //               FDBG("old cc " +SN(i)+ SN(value[i]) + " @" + SN(ccpattern[p]) + SN(ccval[p]));
                if (value[i] && ccpattern[p] == 0)
                {
                    ccpattern[p] = lastcc;
                    ccval[p] = lastccval;
                    //                   FDBG("set cc " + SN(value[i]) + " @" + SN(ccpattern[p]) + SN(ccval[p]));
                }
                else if (!value[i])
                {
                    ccpattern[p] = 0;
                    ccval[p] = 0;
                }
            }
            showCCs();
        }

        if (patvoicelow[actpattern] < MAXVOI - 4)
            startvoice = patvoicelow[actpattern];

        for (int v = 0; v < 4; v++)
        {
            if (id == metid[v])
            {
                for (int i = 11; i > -1; i--)
                {
                    int p = actpattern * maxticks + i;
                    //                  FDBG("set metro " + SN(value[i]) + " @" + SN(p));
                    seqpattern[p][v + startvoice] = (value[i] ? mvelo[v + startvoice] : -1);
                    //                   if (seqpattern[p][v] > 0)
                    //                       seqpattern[p][v] = 0;
                }
                String pas = pat2string(actpattern, v + startvoice);
                String r = showRhythm(pas, actpattern * 10 + v + startvoice);
                //               FDBG("pattern " + SN(i) + "/" + SN(v) + " " + pas + " " + r);
                //              webgui.remove(patidt[v][0]);
                //               int ypos = 450 + (v - startvoice) * 50;
                // patidt[v][0] = webgui.addStringDisplay("Rhythm", 1000, ypos - 10, "f");
                websetMonitor(patidt[v][0], r);
            }
        }
    }
    else if (menuState == SYNTHSETTINGS)
    {
        ((MenuSynth *)MenuPara::SynthMenu)->checkControl(CHECKBOX, id, 0, 0, value[0]);
        return;
    }
    else
    {
        ((MenuPara *)Menus[menuState])->checkControl(CHECKBOX, id, 0, 0, value[0]);
    }
}
