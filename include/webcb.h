extern void midiSilence(void);

void update_pat()
{
    if (menuState != SETTINGS)
        return;
    //   FDBG(__CALLER__);

    webgui.remove(actgrpopt);
    webgui.remove(ccmetid);
    //   FDBG(__CALLER__);
    int end = startvoice + 4;
    //   FDBG("u pat " + SN(start) + SN(end));

    //   FDBG(__CALLER__);
    for (int v = 0; v < MAXVOI; v++)
    {
        //      FDBG(__CALLER__);
        for (int i = 0; i < MAXPAT; i++)
            webgui.remove(patidt[v][i]);
        //       FDBG(__CALLER__);
        webgui.remove(instopt[v]);
        webgui.remove(velopt[v]);
        webgui.remove(metopt[v]);
        webgui.remove(metid[v]);
        webgui.remove(voiceidt[v]);
    }
    //  FDBG(__CALLER__);
    actgrpopt = webgui.addOptions("Group", MAXGRP, counts + zerobase, &onOptionSelect, 0, 480, actgrp, "title");
    //  FDBG(__CALLER__);

    showCCs();
    showKeys();
    showStatus(actpattern);

    int ypos = 400;
    webgui.setMonitor(patternidt, "Group " + String(actgrp + zerobase) + " Pattern #" + String(actpattern + zerobase));
    for (int i = 0; i < maxticks; i++)
        seqswitch[i] = ccpattern[actpattern * maxticks + i][actgrp] > 0;
    // ccpattern[actpattern * maxticks + i] > 0;
    ccmetid = webgui.addSwitches("steps", maxticks, seqswitch, &onSwitches, 680, ypos, "x", "nomonitor");
    webgui.setMonitor(ccpatternidt, "CC " + String(lastcc < 128 ? lastcc : 0));
    webgui.setMonitor(ccvpatternidt, "CC Value " + String(lastccval < 128 ? lastccval : 0));
    //   FDBG(__CALLER__);

    for (int v = startvoice; v < end && v < MAXVOI; v++)
    {
        //       FDBG("update voice " + SN(v));
        int ypos = 450 + (v - startvoice) * 50;

        int inx = minstr[v] - 2;
        if (inx < 1 || inx > 127)
            inx = 1;
        if (pwx < 1 || pwx > 127)
            pwx = 16;
        //        FDBG("instr " + SN(inx) + " " + SN(mvelo[v]));
        voiceidt[v] = webgui.addStringDisplay("", 180, ypos, "f");
        webgui.setMonitor(voiceidt[v], String(v + zerobase));
        instopt[v] = webgui.addOptions("instr", 127, perc, &onOptionSelect, 200, ypos, inx, "f");
        velopt[v] = webgui.addOptions("velo", 128, counts, &onOptionSelect, 400, ypos, mvelo[v], "f");
        metopt[v] = webgui.addOptions("Pattern", pwx, patvals, &onOptionSelect, 500, ypos, lastmet[v], "f", "metro"); //*Paras[i]->value)                                                                                                                                     //  FDBG(SN(pwx) + " " + patvals[0]);
        int lb = actpattern / 4;
        short bc = beatCount[lb * 4][actgrp];
        for (int i = 0; i < MAXPAT; i++)
        {
            webgui.remove(patidt[v][i]);
        } //       FDBG("ap " + SN(ap) + " " + SN(beatlength * numq) + " " + SN(beatlength) + " " + SN(numq));
        for (int i = 0; i < bc; i++)
        {
            patidt[v][i] = webgui.addStringDisplay("Rhythm", 1000 + i * 150, ypos - 10, "f");
        } //       FDBG("ap " + SN(ap) + " " + SN(beatlength * numq) + " " + SN(beatlength) + " " + SN(numq));
          //(SN(actgrp) + SN(bc) + SN(actpattern) + SN(lb));
        if (bc > 0)
        {
            for (int i = 0; i < bc && i < 2; i++)
            {
                String pas = pat2string(i, v);
                String r = showRhythm(pas, i * 10 + v);
                //               FDBG("pattern " + SN(i) + "/" + SN(v) + " " + pas + " " + r);
                webgui.setMonitor(patidt[v][i], r);
            }
        }
        for (int i = 0; i < maxticks; i++)
        {
            seqswitch[i] = seqpattern[actpattern * maxticks + i][v][actgrp] > 0;
        }
        metid[v] = webgui.addSwitches("steps", maxticks, seqswitch, &onSwitches, 680, ypos, "x", "nomonitor");
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
void setDisplay(String out)
{
    webgui.remove(men);
    if (Menus[menuState]->isPara)
    {
        for (int i = 0; i < maxg; i++)
            webgui.remove(guiid[i]);
    }
    if (cliitem > 0)
        webgui.remove(cliitem);
    cliitem = webgui.addStringDisplay("cli", 50, 210, "f", "nomonitor");
    webgui.setMonitor(cliitem, out);
    cleared = true;
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
void clearPat(void)
{
    for (int p = 0; p < 128; p++)
    {
        triggerNote[p] = 255;
        voices[p] = 255;
    }
    for (int g = 0; g < MAXGRP; g++)
    {
        for (int p = 0; p < MAXPAT; p++)
        {
            beatCount[p][g] = 0;
            patfiles[p][g] = "";
            acttrigger[p][g] = 255;
        }
    }
    lastvoice = 0;
    for (int v = 0; v < MAXVOI; v++)
    {
        for (int p = 0; p < 12 * MAXPAT; p++)
        {
            for (int g = 0; g < MAXGRP; g++)
            {
                seqpattern[p][v][g] = -1;
            }
        }
    }
    update_pat();
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

        String event = showDrum();

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
    digitalWrite(13, HIGH);
}
void procVoice(String proc, int from, int to)
{
    if (proc == "cp")
    {
        FDBG("copy voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            seqpattern[i][to][actgrp] = seqpattern[i][from][actgrp];
        }
    }
    if (proc == "mv")
    {
        FDBG("move voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            seqpattern[i][to][actgrp] = seqpattern[i][from][actgrp];
            seqpattern[i][from][actgrp] = -1;
        }
    }
    if (proc == "sw")
    {
        FDBG("swap voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            short sw = seqpattern[i][to][actgrp];
            seqpattern[i][to][actgrp] = seqpattern[i][from][actgrp];
            seqpattern[i][from][actgrp] = sw;
        }
    }
    if (proc == "rm")
    {
        FDBG("delete voice from " + SN(from) + SN(to));
        for (int i = startpattern * maxticks; i < actpattern * maxticks; i++)
        {
            seqpattern[i][from][actgrp] = -1;
        }
    }
    actpattern = startpattern;
    update_pat();
}
String mos = "";
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
        out = "<textarea class=\"scrollabletextbox\" id=\"export-data\" rows=10 cols=40 name=\"" + argv[2] + "\">" + o + "</textarea><input type=\"button\" id=\"dwn-btn\" value=\"export\" onclick=\"onclickbtn('" + argv[2] + "')\"/>";
        setDisplay(out);
        //     FDBG(out);
        return;
    }
    File myFile = SD.open(argv[1].c_str());
    int l = myFile.size();
    //               FDBG(argv[1] + SP(myFile));
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
    //   FDBG(l);
    //            FDBG(o);
    if (l < 100000)
    {
        out = "<textarea class=\"scrollabletextbox\" id=\"export-data\" rows=10 cols=40 name=\"" + argv[1] + "\">" + o.substring(0, 5000) + "</textarea><input type=\"button\" id=\"dwn-btn\" value=\"export\" onclick=\"onclickbtn('" + argv[1] + "')\"/>";
        setDisplay(out);
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
        cliitem = -1;
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
            FDBG(SN(g_scid) + " " + scaleNames[g_scid]);
            saveTMS();
        }
        if (starts == "sa")
        {
            argv[0] = value;
            saveData(argc, argv);
            return;
        }
        if (starts == "vo")
        {
            FDBG(argv[1] + " " + argv[2] + " " + SN(startpattern) + SN(actpattern));
            if (startpattern > -1 && actpattern > startpattern)
                procVoice(argv[1], argv[2].toInt(), argv[3].toInt());
            return;
        }
        if (starts == "ag")
        {
            actgrp = argv[1].toInt();
            update_pat();
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
                webgui.setMonitor(idt, noteLine(false));
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
                FDBG(__CALLER__);
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
                            FDBG("goal " + SN(k) + SN(keychord[0] % 100) + SN(keychord[r] % 100));
                            transpose[r] = (k - (keychord[r] % 100) % 12) % 12;
                        }
                    }
                }
                //               FDBG(__CALLER__);
                //          for (int i = 0; i < maxrepl; i++)
                //              FDBG(SN(i) + " " + SN(keychord[i]) + " " + SN(replchord[i]) + " " + SN(transpose[i]));
                webgui.setMonitor(idt, noteLine(false));
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
                mapid = webgui.addOptions("files", n, res, &onOptionSelect, 50, 160, 0, "f");
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
                    update_pat();
                }
                return;
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
            String out = "<textarea style=\"overflow:scroll;\"class =\"scrollabletextbox\" name=\"note\" rows=10 cols=160>" + more + "</textarea>";
            setDisplay(out);
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
    FDBG("option " + SN(id) + " @ " + SN(menuState));
    if (id == actgrpopt)
    {
        actgrp = option;
        if (patvoicelow[4 * (actpattern / 4)][actgrp] < MAXVOI - 4)
            startvoice = patvoicelow[4 * (actpattern / 4)][actgrp];
        update_pat();
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
                    seqpattern[i + pos][v][actgrp] = 100;
                }
                else
                    seqpattern[i + pos][v][actgrp] = -1;
                //           FDBG(metid[i]);
            }
            update_pat();
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
    if (id == mapid)
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
            loadMIDI(g_search.replace("*", "") + "/" + opt, mos);
        }
        if (opt.indexOf(".drm") > -1)
        {
            loadDrum(file);
            if (menuState == SETTINGS)
            {
                update_pat();
            }
        }
        webgui.remove(mapid);
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
                chordselId[i] = webgui.addOptions("Chord", 53, chordopt, &onOptionSelect, xtransId[i]+20, 144 + 200, replchord[i] + 1, "f");
                mstate = CHORDS;
                return;
            }
        }
        if (actkeyidx > 12 && id == iab1)
        {
            actkeyidx -= 12;
            refresh = true;
        }
        if (actkeyidx < maxrepl && id == iab2)
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
        for (int i = 0; i < 9; i++)
        {
            if (movenext[i] == id)
            {
                //               FDBG("Button " + SN(i));
                int actalign = actpattern / beatlength;

                if (i < 4)
                {
                    if (i == 0)
                        actpattern = (actalign - 1) * beatlength;
                    if (i == 1)
                        actpattern--;
                    if (i == 2)
                        actpattern++;
                    if (i == 3)
                        actpattern = (actalign + 1) * beatlength;
                    if (actpattern < 0)
                        actpattern = 0;
                    if (actpattern > MAXPAT - 1)
                        actpattern = MAXPAT - 1;
                    if (patvoicelow[4 * (actpattern / 4)][actgrp] < MAXVOI - 4)
                        startvoice = patvoicelow[4 * (actpattern / 4)][actgrp];
                    update_pat();
                }
                //              FDBG("actpat " + SN(actpattern));
                if (i == 4 || i == 5)
                {
                    //                    if (patvoice[actpattern][actgrp] < MAXVOI - 4 && startvoice < 0)
                    //                       start = patvoice[actpattern][actgrp];
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
                    update_pat();
                }
                //               FDBG("startvoice " + SN(startvoice));
                //               FDBG("startpattern " + SN(startpattern) + " " + SN(i));
                if (i == 6)
                    startpattern = actpattern;
                if (i == 7 && startpattern != -1 && actpat[0])
                {
                    int np = (actpattern + 1 - startpattern) / 4;
                    //                    FDBG(np);
                    for (int a = startpattern, b = 0; a < actpattern; a++, b++)
                    {
                        beatCount[a][actgrp] = np - b / 4;
                        //                       FDBG(SN(np) + " " + SN(a) + " set beats " + SN(beatCount[a]));
                    }
                    update_pat();
                }
                if (i == 8 || i == 6)
                    update_pat();
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
                metTimer.begin(playPattern, mettime / 48);
                playSeq = true;
                patternc = -1;
            }
        }
        else
        {
            metTimer.end();
            midiSilence();
            showStatus(actpattern);
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
    else if (id == file)
    {
        static bool notfile = true;
        //        FDBG(cliitem);
        if (cliitem == -1)
            webgui.remove(cli);
        else
        {
            webgui.remove(cliitem);
            cliitem = -1;
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
            webgui.setMonitor(men, outmen);
            maxg = 0;
            Menus[menuState]->createControl(__CALLER__);
            cleared = false;
        }
        if (menuState == MAIN)
        {
            showstat = webgui.addStringDisplay("Status", 10, 200 + 80, "f");
            webgui.remove(inbuts[3]);
            inbuts[3] = webgui.addButtons("Rhythm/Settings", &onButtonRelease, 10, 150 + 80, "f", "hide");
            webgui.setMonitor(showstat, "Status");
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
            ledstate[13] = 1;
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
            ledstate[13] = 0;
            String notes[16];
            STACK;
            metison = false;
            metTimer.end();
            showStatus(actpattern);
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
                ledstate[13] = 0;
            }
            //           nextEvent = 0;
            STACK;
            nextEvent = 0;
            SMF.looping(false);
            SMF.restart();
            webgui.setMonitor(sbp, greenled);
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
            webgui.setMonitor(sbp, greenled);
            STACK;
            break;
        case REWIND:
            STACK;
            nextEvent = 0;
            SMF.restart();
            digitalWrite(13, LOW);
            ledstate[13] = 0;
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
            actpat[0] = value[0];
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
                    ccpattern[p][actgrp] = lastcc;
                    ccval[p][actgrp] = lastccval;
                    //                   FDBG("set cc " + SN(value[i]) + " @" + SN(ccpattern[p]) + SN(ccval[p]));
                }
                else if (!value[i])
                {
                    ccpattern[p][actgrp] = 0;
                    ccval[p][actgrp] = 0;
                }
            }
            showCCs();
        }

        if (patvoicelow[4 * (actpattern / 4)][actgrp] < MAXVOI - 4)
            startvoice = patvoicelow[4 * (actpattern / 4)][actgrp];

        for (int v = 0; v < MAXVOI; v++)
        {
            if (id == metid[v])
            {
                for (int i = 11; i > -1; i--)
                {
                    int p = actpattern * maxticks + i;
                    //                  FDBG("set metro " + SN(value[i]) + " @" + SN(p));
                    seqpattern[p][v][actgrp] = (value[i] ? mvelo[v] : -1);
                    //                   if (seqpattern[p][v] > 0)
                    //                       seqpattern[p][v] = 0;
                }
                String pas = pat2string(actpattern, v);
                String r = showRhythm(pas, actpattern * 10 + v);
                //               FDBG("pattern " + SN(i) + "/" + SN(v) + " " + pas + " " + r);
                webgui.remove(patidt[v][0]);
                int ypos = 450 + (v - startvoice) * 50;
                patidt[v][0] = webgui.addStringDisplay("Rhythm", 1000, ypos - 10, "f");
                webgui.setMonitor(patidt[v][0], r);
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
