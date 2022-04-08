extern void midiSilence(void);

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
        digitalWrite(13, LOW);
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
        int argc = SplitS(value.replace("  ", " "), ' ', argv, 20);
        String starts = argv[0].substring(0, 2);
        FDBG("starts with " + starts);
        webgui.remove(cliitem);
        if (starts == "x")
            webgui.remove(cli);
        if (starts == "tr" && argc == 3)
        {
            int what = 0;
            argv[1].toLowerCase();
            if (argv[1] == "m-1")
                what = 1;
            if (argv[1] == "m-2")
                what = 2;
            if (argv[1] == "synth")
                what = 3;
            if (argv[1] == "cv1")
                what = 4;
            transposeit[what] = argv[2].toInt();
            saveTMS();
            return;
        }
        if (starts == "q")
        {
            int f = 4;
            if (argc > 1)
                f = argv[1].toInt();
            if (!f)
                qfactor = f;
            {
                if (argv[1].endsWith("T"))
                    qfactor = 3000 / f;
                else
                    qfactor = 2000 / f;
            }
            DBG(qfactor);
        }
        if (starts == "rm" && argc > 1)
        {
            String pname = argv[1];
            FDBG("deleting " + pname);
            if (SD.exists(pname.c_str()))
            {
                SD.remove(pname.c_str());
                //    if(debug==1)DBG("ini deleted");
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
        if (starts == "sc" && argc > 1)
        {
            g_scid = argv[1].toInt();
            g_scid = (g_scid - 1) / 2;
            Menus[SCALES]->scaleit(g_scid);
            FDBG(SN(g_scid) + " " + scaleNames[g_scid]);
            saveTMS();
        }
        if (starts == "sa" && argc > 1)
        {
            String pname = argv[1];
            FDBG("Saving " + pname);
            if (argv[1].indexOf(".mid") > -1 && lastEvent > 0)
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
                    for (int s = 0; s < lastEvent; s++)
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
                comment = value.replace(argv[1], "").replace("save ", "").replace("sav ", "").replace("sa ", "");
                if(progressmode)
                    comment = "Progression: " + comment;
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
        if (starts == "sh" && argc > 1 && argv[1].indexOf("seq") > -1)
        {
            String event = "";
            int lastnotetime = 0;
            //           int onnotes[128];
            //           onnotes[0] = 0;
            for (int s = 0; s < lastEvent; s++)
            {
                String pre = SN(s) + " " + SN(sequences[s]._time) + " " + SP(sequences[s]._event) + " " + SN(sequences[s]._note) + " ";
                if (sequences[s]._event == 0x90)
                {
                    event += pre + SN(sequences[s]._length) + " " + SN(sequences[s]._time - lastnotetime) + "~";
                    lastnotetime = sequences[s]._time;
                }
                else
                {
                    event += pre + SN(sequences[s]._velocity) + "~";
                }
            }
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + event + "</textarea>";
            cliitem = webgui.addStringDisplay("cli", 50, 160, "f", "nomonitor");
            webgui.setMonitor(cliitem, out);
        }
        if (starts == "sh" && argc > 1 && argv[1].indexOf("cal") > -1)
        {
            String cal = "";

            for (int s = JOY_X; s < EXT_SWITCH; s++)
            {
                MenuExtSet *em = (MenuExtSet *)Menus[s];
                cal += em->name + " " + SN(em->emin) + " " + SN(em->emax) + " " + SN(em->rawvalue) + "~";
                FDBG(em->name + " " + SN(em->emin) + " " + SN(em->emax) + " " + SN(em->rawvalue));
            }
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=30>" + cal + "</textarea>";
            cliitem = webgui.addStringDisplay("cli", 50, 160, "f", "nomonitor");
            webgui.setMonitor(cliitem, out);
        }
        if (starts == "ex" && argc >= 2)
        {
            int l = 0;
            if (argv[1].endsWith("seq") && argc >= 3)
            {
                createMidiFile();
                int delta = 0;
                int lasttime = 0;
                //            FDBG("midi " + SN(midiptr));
                for (int s = 0; s < lastEvent; s++)
                {
                    delta = sequences[s]._time - lasttime;
                    //                FDBG(" " + SB(sequences[s]._event) + " delta " + SN(delta) + " " + SN(midiptr-22));
                    writeToFile(sequences[s]._event + sequences[s]._channel, sequences[s]._note, sequences[s]._velocity, delta);
                    lasttime = sequences[s]._time;
                }
                setTrackLength(midiptr - 14);
                FDBG(" Length " + SN(midiptr - 14));
                l = midiptr;
                Serial.printf("\n%04x ", 0);
                for (int i = 0; i < midiptr; i++)
                {
                    Serial.printf("%02x ", midifile[i]);
                    if (i % 16 == 15)
                        Serial.printf("\n%04x ", i + 1);
                }
                Serial.println();
                encode_base64(midifile, l, midibase64);
                o = (char *)midibase64;
                out = "<textarea class=\"scrollabletextbox\" id=\"export-data\" rows=10 cols=40 name=\"" + argv[2] + "\">" + o + "</textarea><input type=\"button\" id=\"dwn-btn\" value=\"export\" onclick=\"onclickbtn()\"/>";
                cliitem = webgui.addStringDisplay("cli", 50, 160, "f", "nomonitor");
                webgui.setMonitor(cliitem, out);
            }
            else
            {
                File myFile = SD.open(argv[1].c_str());
                l = myFile.size();
                //               FDBG(argv[1] + SP(myFile));
                if (myFile)
                {
                    myFile.readBytes((char *)midifile, l);
                    // close the file:
                    myFile.close();
                }
                else
                    return;
            }
            Serial.printf("\n%04x ", 0);
            for (int i = 0; i < l && 0; i++)
            {
                Serial.printf("%02x ", midifile[i]);
                if (i % 16 == 15)
                    Serial.printf("\n%04x ", i + 1);
            }
            encode_base64(midifile, l, midibase64);
            //            o = (char *)midibase64;
            //            FDBG(l);
            //            FDBG(o);
            //            out = "<textarea class=\"scrollabletextbox\" id=\"export-data\" rows=10 cols=40 name=\"" + argv[1] + "\">" + o.substring(0,5000) + "</textarea><input type=\"button\" id=\"dwn-btn\" value=\"export\" onclick=\"onclickbtn()\"/>";
            //            cliitem = webgui.addStringDisplay("cli", 50, 160, "f", "nomonitor");
            //            webgui.setMonitor(cliitem, out);
        }
        if (starts == "im" && argc > 1)
        {
            o = "";
            webgui.remove(inbuts[3]);
            webgui.remove(impid);
            webgui.remove(showstat);
            impid = webgui.addInputString("import", &onMessage, 0, 200, "f", "scroll");
            impFile = argv[1];
            inimport = true;
            digitalWrite(13, HIGH);
        }
        if (starts == "pr" && argc > 1)
        {
            static String progression = "I,II,III,IV,V,VI,VII,"
                                        "IM7,IIM7,IIIM7,IVM7,VM7,VIM7,VIIM7,"
                                        "i,ii,iii,iv,v,vi,vii,"
                                        "i7,ii7,iii7,iv7,v7,vi7,vii7,"
                                        "I7,II7,III7,IV7,V7,VI7,VII7";
            static int np = 0;
            static String res[70];
            static int p2k[7] = {1, 3, 5, 6, 8, 10, 11};
            static int g2k[10] = {72, 1096, 68, 516, 584, 9, 11};
            if (!np)
                np = SplitS(progression, ',', res, 70);
            progressmode = true;
            for (int i = 0; i < maxrepl; i++)
            {
                keychord[i] = chordIds[i] * 100;
                replchord[i] = chordIds[i];
                transpose[i] = 0;
            }
            for (int i = 1; i < argc && i - 1 < 6; i++)
            {
                //                FDBG(SN(i) + ":" + argv[i]+" "+SN(np));
                FDBG(argv[i]);
                for (int p = 0; p < np; p++)
                {

                    if (argv[i] == res[p])
                    {
                        int k = p2k[p % 7] - 1;
 //                       int ik = i - 1;
 //                       FDBG(SN(p / 7) + " " + SN(p % 7) + " " + res[p] + " " + SN(k) + " " + SN(ik) + " " + SN(keychord[ik]));
                        replchord[i - 1] = g2k[p / 7];
                        transpose[i - 1] = k;
                    }
                }
            }
  //          for (int i = 0; i < maxrepl; i++)
  //              FDBG(SN(i) + " " + SN(keychord[i]) + " " + SN(replchord[i]) + " " + SN(transpose[i]));
            webgui.setMonitor(idt, noteline(false));
        }
        if (starts == "pc" && argc > 1)
        {
            actProgramChange(curMMS->outCH, argv[1].toInt(), curMMS);
        }
        if (starts == "lo" && argc > 1)
        {
            if (argv[1].indexOf("*") > -1)
            {
                String dir = printDirectory(SD.open("/"), argv[1], true);
                //                FDBG(dir);
                int ni = SplitS("select," + dir, ',', mapres, 99);
                //               FDBG(SN(ni) + " " + res[0]);
                mapid = webgui.addOptions("files", ni, mapres, &onOptionSelect, 50, 154, 0, "f");
                return;
            }
            if (argv[1].toLowerCase().endsWith(".mid"))
            {
                int err = SMF.load(argv[1].c_str());
                if (err != MD_MIDIFile::E_OK)
                {
                    Serial.print("SMF load Error ");
                    Serial.println(err);
                }
                FDBG("Loaded " + argv[1]);
                playSeq = false;
                seqFile = argv[1];
                saveTMS();
                playSeq = false;
                SMF.restart();
                transport = PLAYING;
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
            File myFile = SD.open(argv[1].toUpperCase().c_str());
            String more = "";
            FDBG(argv[1] + SP(myFile));
            if (myFile)
            {
                FDBG(argv[1]);
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
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + more + "</textarea>";
            cliitem = webgui.addStringDisplay("cli", 50, 260, "f", "nomonitor");
            webgui.setMonitor(cliitem, out);
        }
        if (starts == "he")
        {
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + help + "</textarea>";
            cliitem = webgui.addStringDisplay("cli", 50, 260, "f", "nomonitor");
            webgui.setMonitor(cliitem, out);
        }
        if (starts == "ls")
        {
            if (argc == 1)
                argv[1] = "*.*";
            String dir = printDirectory(SD.open("/"), argv[1]);
            //           FDBG(dir);
            String out = "<textarea class=\"scrollabletextbox\" name=\"note\" rows=10 cols=40>" + dir + "</textarea>";
            //           FDBG(out);
            cliitem = webgui.addStringDisplay("cli", 50, 260, "f", "nomonitor");
            webgui.setMonitor(cliitem, out);
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
    //   FDBG("option " + SN(id) + " @ " + SN(menuState));
    if (menuState == MAPPINGS)
    {
        for (int i = 0; i < 20; i++)
        {
            int tdx = i + actkeyidx;
            if (transId[i] == id && mstate == TRANS)
            {
                if (option)
                    transpose[tdx] = option - 1;
                webgui.remove(transId[i]);
                mstate = NONE;
                refresh = true;
                return;
            }
            if (menuId[i] == id)
            {
                webgui.remove(menuId[i]);
                if (option)
                    replchord[tdx] = chordIds[option + 10];
                mstate = NONE;
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
        FDBG(String(id) + " Basenote selected: " + option);
        saveTMS();
    }
    if (id == mapid)
    {
        //       replchord[actkeyidx] = chordIds[option];

        FDBG(String(id) + "selected: " + SN(option) + " " + mapres[option]);
        String opt = mapres[option];
        opt.toLowerCase();
        if (opt.indexOf(".map") > -1)
        {
            loadMap(mapres[option]);
        }
        else if (opt.indexOf(".prs") > -1)
        {
            loadPRS(mapres[option]);
        }
        if (opt.indexOf(".mid"))
        {
            int err = SMF.load(mapres[option].c_str());
            if (err != MD_MIDIFile::E_OK)
            {
                Serial.print("SMF load Error ");
                Serial.println(err);
            }
            FDBG("Loaded " + opt);
            seqFile = mapres[option];
            saveTMS();
            playSeq = false;
            SMF.restart();
        }

        webgui.remove(mapid);
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
        ((MenuSynth *)MenuPara::SynthMenu)->checkcontrol(OPTION, id, 0, option, false);
        //        webgui.remove(prenameinp);
        //        prenameinp = webgui.addInputString("New Preset Name", &onMessage, 400, 100, "title", "control", presetnames[preindex]);
    }
    else
    {
        ((MenuPara *)Menus[menuState])->checkcontrol(OPTION, id, 0, option, false);
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
    FDBG(String(id) + F(" button was released") + " " + String(button) + " @ " + SN(Menus[menuState]->backstate));
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
                transId[i] = webgui.addOptions("Trans", 13, midiNamesOpt, &onOptionSelect, 20 + xtransId[i], 144 + 200, transpose[i] + 1, "f");
                mstate = TRANS;
                return;
            }
            if (menuId[i] == id)
            {
                //                FDBG(xtransId[i]);
                webgui.remove(menuId[i]);
                menuId[i] = webgui.addOptions("Chord", j3, chordopt, &onOptionSelect, 20 + xtransId[i], 144 + 200, replchord[i] + 1, "f");
                mstate = TRANS;
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
        FDBG("new state: " + SN(Menus[menuState]->backstate));
        menuState = nmenuState;
        Menus[menuState]->show(true, __CALLER__);
        return;
    }
    if (id == home)
    {
        menuState = MAIN;
        Menus[menuState]->show(true, __CALLER__);
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
                FDBG(SN(i) + " was selected");
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
            FDBG("from " + SN(Menus[menuState]->backstate) + " to " + SN(menuState));
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
        webgui.remove(cli);
        webgui.remove(cliitem);
        webgui.remove(mapid);
        webgui.remove(impid);
        inimport = false;
        if (menuState == MAIN)
        {
            showstat = webgui.addStringDisplay("Status", 10, 200 + 80, "f");
            inbuts[3] = webgui.addButtons("Info/Settings", &onButtonRelease, 60, 150 + 80, "f", "hide");
            webgui.setMonitor(showstat, "Status");
        }
        if (notfile)
            cli = webgui.addInputString("Command ", &onMessage, 50, 121, "title", "control", "help");
        notfile = !notfile;
    }
    else if (id == sipm)
    {
        FDBG("transport " + SN(button));
        transport = button;
        switch (button)
        {
        case RECORDING:
            lastEvent = 0;
            MidiEvent::starttime = 0;
            digitalWrite(13, HIGH);
            ledstate[13] = 1;
            playSeq = true;
            break;
        case STOPPED:
        {
            digitalWrite(13, LOW);
            ledstate[13] = 0;
            String notes[16];
            int nn = SplitS(seqOn, ',', notes, 16);
            for (int n = 0; n < nn - 1; n++)
            {
                //               FDBG(notes[n]);
                actNoteOff(sequences[0]._channel, notes[n].toInt(), 0, curMMS);
            }
            midiSilence();
        }
        break;
        case PLAYING:
            if (transport == RECORDING)
            {
                digitalWrite(13, LOW);
                ledstate[13] = 0;
            }
            //           nextEvent = 0;
            SMF.looping(false);
            SMF.restart();
            webgui.setMonitor(sbp, greenled);
            //           FDBG("Play to " + SN(lastEvent));
            //           FDBG(transport);
            break;
        case REPEAT:
            SMF.looping(true);
            SMF.restart();
            //            transport = PLAYING;
            webgui.setMonitor(sbp, greenled);
            break;
        case REWIND:
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
    if (menuState == SYNTHSETTINGS)
    {
        ((MenuSynth *)MenuPara::SynthMenu)->checkcontrol(SLIDER, id, 0, value, false);
        return;
    }
    else
    {
        ((MenuPara *)Menus[menuState])->checkcontrol(SLIDER, id, 0, value, false);
    }
}

void onSwitches(bool *value, int id)
{
    DBG("New switch status: " + SN(id) + " " + SN(value[0] ? "on" : "off"));

    if (menuState == SYNTHSETTINGS)
    {
        ((MenuSynth *)MenuPara::SynthMenu)->checkcontrol(CHECKBOX, id, 0, 0, value[0]);
        return;
    }
    else
    {
        ((MenuPara *)Menus[menuState])->checkcontrol(CHECKBOX, id, 0, 0, value[0]);
    }
}
