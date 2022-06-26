#include "webdata.h"

#include "webcb.h"
EXTMEM String zoneDisp;
EXTMEM char zoneMem[5000];
void websetup()
{
    STACK;

    int actsb = ((MenuSynth *)MenuPara::SynthMenu)->actSubmenu;
    SubMenu *sb = ((MenuSynth *)MenuPara::SynthMenu)->submenus[actsb];
    String name = sb->Paras[sb->myID]->name;
    if (menuState == MAIN)
        sbase = 280;
    else
        sbase = 470;
    tbase = sbase + 15;
    fbase = tbase + 40;
    xbase = fbase + 20;
    base5 = xbase + 80;
    base6 = base5 + 20;
    base7 = base6 + 30;
    base8 = base7 + 50;
    base9 = base8 + 50;
    base10 = base9 + 50;
    maxg = 0;
    //   DBG("Submenu " + name + " my ID " + SN(sb->myID));
    int myID = sb->myID;
    lastXPos = 0;
    //   DBG(F("Connected to webgui server !"));
    webgui.reset(); // clear the board
                    //  bgitem = webgui.addStringDisplay("_bg_", 0, 0, false, "nomonitor");
    for (int p = 0; p < MAXPAT; p++)
    {
        actbeatID[p] = -1;
    }

    STACK;
    selitem = webgui.addStringDisplay("<t3>TMS</t3>", 0, 0, "f", "nomonitor");
    if (selitem < 0)
        FDBG("selitem error");
    //   DBG("selitem " + SN(selitem));
    imbb = webgui.addButtons(btns7[0], &onButtonRelease, 0, mbase, "f");
    home = webgui.addButtons(btns7[1], &onButtonRelease, 50, mbase, "f");
    metro = webgui.addButtons(btns7[3], &onButtonRelease, 100, mbase, "f");
    //   backmetro = webgui.addButtons(btns7[4], &onButtonRelease, 100, mbase + 50, "f");
    openFile = webgui.addButtons(btns7[5], &onButtonRelease, 0, mbase + 50, "f");
    STACK;
    if (menuState == SYNTHSETTINGS)
    {
        static int sframe = 0;
        STACK;
        webgui.remove(sframe);
        sframe = webgui.addStringDisplay("Menu", 200, 90, "f");
        String outmen = "";
        //        FDBG("submenu " + SN(actsb));
        if (actsb == submod)
            outmen = "_synth_";
        if (actsb == subVCO)
            outmen = "_vco_";
        if (actsb == subVCA || actsb == subVCF)
            outmen = "_vcfa_";
        if (actsb == subSample || actsb == subKeys)
            outmen = "_sample_";
        if (actsb == subFADSR || actsb == subAADSR)
            outmen = "_adsr_";
        webgui.setMonitor(sframe, outmen);
        smen = webgui.addStringDisplay("paras", 450, mbase, "f");
        sb->orp = -1;
        STACK;
        ((MenuSynth *)MenuPara::SynthMenu)->createControl(myID);
        STACK;
        return;
    }
    else if (Menus[menuState]->isPara)
    {
        STACK;
        MenuTargetSet *ts = ((MenuPara *)Menus[menuState])->targets;
        if (ts)
        {
            //           DBG("Target " + SN(ts->me));
            target = webgui.addStringDisplay("Menu", 200, 90, "f");
            String outmen = " <style>  table td  {  font-size : 16px; } </style> ";
            outmen += "<t1 style=\" text-align : left;\">Targets</t1> ";
            outmen += ts->setMenu();
            webgui.setMonitor(target, outmen);
            ts->createControl(__CALLER__);
            lastXPos = 250;
        }
        men = webgui.addStringDisplay("Menu", 200 + lastXPos, 90, "f");
    }
    webgui.setMonitor(men, "Hello, me TMS");
    if (selitem < 0)
        FDBG("selitem error " + __CALLER__);
    if (menuState == SHOWSCALE || menuState == SCALES)
    {
        //       for (int i = 0; i < 84; i++)
        //           FDBG(SN(i) + scales[i]);
        scaleid = webgui.addOptions("Scale", 84, (String *)scales, &onOptionSelect, 210, 120, scidtoix(g_scid), "title");
        baseid = webgui.addOptions("Basenote", 12, midiNamesFlat, &onOptionSelect, 400, 120, scalebase, "title");
        idt = webgui.addStringDisplay("Scales", 0, 200, "f");
        STACK;
        webgui.setMonitor(idt, noteLine(true));
        inleds[3] = webgui.addStringDisplay("MIDI IN", 250, tbase - 50, "title", "monframe");
        inleds[7] = webgui.addStringDisplay("scaled", 550, tbase - 50, "title", "monframe");
    }
    //   webgui.setMonitor(bgitem, "Hello _bg_");
    else if (menuState == MAPPINGS || menuState == SHOWMAP)
    {
        int posx = 0;
        int py = 200;
        if (lastMap > Menus[MAPPINGS]->numitems)
            lastMap = Menus[MAPPINGS]->numitems - 1;
        webgui.remove(idt);
        webgui.remove(idtg);
        webgui.remove(iab1);
        webgui.remove(iab2);
        idt = webgui.addStringDisplay("Maps", posx, py, "f");
        idtg = webgui.addStringDisplay("Maps", posx, py, "f");
        iab1 = webgui.addButtons("&#x25C0;", &onButtonRelease, posx - 20, py + 70, "f", "hide");
        iab2 = webgui.addButtons("&#x25B6;", &onButtonRelease, posx + 1170, py + 70, "f", "hide");
        baseid = webgui.addOptions("Basenote", 12, midiNamesFlat, &onOptionSelect, 10, tbase - 50, scalebase, "title");
        inleds[3] = webgui.addStringDisplay("MIDI IN", 250, tbase - 50, "title", "monframe");
        inleds[6] = webgui.addStringDisplay("mapped", 550, tbase - 50, "title", "monframe");
    }
    else if (menuState == MAIN)
    {
        webgui.setMonitor(selitem, "_main_<t3>The MIDI Sidekick/t3>");
        men = webgui.addStringDisplay("Menu", 200 + lastXPos, mbase, "f");
        if (!inimport)
        {
            webgui.remove(showstat);
            webgui.remove(inbuts[3]);
            showstat = webgui.addStringDisplay("Status", 10, 200 + 80, "f");
            inbuts[3] = webgui.addButtons("Rhythm/Settings", &onButtonRelease, 10, 150 + 80, "f", "hide");
            webgui.setMonitor(showstat, "Status");
        }
    }
    if (menuState == MAIN || menuState == MAPPINGS)
    {
        ledout = webgui.addStringDisplay("LED", 250, mbase, "title");
        //       ipm = webgui.addButtons("Process Mode", 2, btns2, &onButtonRelease, xbase, mbase, "title");
        sipm = webgui.addButtons("MIDI", 5, btnss, &onButtonRelease, 950, mbase, "title");
        if (menuState == MAIN)
            sbp = webgui.addStringDisplay("Monitor", 850, mbase, "title");
        outbuts[8] = webgui.addButtons("Mode", &onButtonRelease, 350, mbase, "f", "hide");
        bp = webgui.addStringDisplay("Mode", 350, ledbase, "f");
        //       isb = webgui.addButtons("Semitone", 2, btns3, &onButtonRelease, 620, mbase, "title");
        outbuts[0] = webgui.addButtons("Semitone +", &onButtonRelease, 550, mbase, "f", "hide");
        outbuts[1] = webgui.addButtons("Semitone -", &onButtonRelease, 450, mbase, "f", "hide");
        semp = webgui.addStringDisplay("Semitone +", 550, ledbase, "hide");
        semm = webgui.addStringDisplay("Semitone -", 450, ledbase, "hide");
        //       iob = webgui.addButtons("Octave", 2, btns4, &onButtonRelease, 820, mbase, "title");
        //       iob = webgui.addButtons("Octave", 2, btns4, &onButtonRelease, 820, mbase, "hide");
        outbuts[2] = webgui.addButtons("Octave +", &onButtonRelease, 750, mbase, "f", "hide");
        outbuts[4] = webgui.addButtons("Octave -", &onButtonRelease, 650, mbase, "f", "hide");
        octp = webgui.addStringDisplay("Octave +", 750, ledbase, "hide");
        octm = webgui.addStringDisplay("Octave -", 650, ledbase, "hide");
        //   webgui.addSwitches(createTable(sw, 5), 5, sws, &onSwitches, 430, 350);
    }
    else if (menuState == TOUCH || (menuState >= EXTSETTINGS && menuState <= TARGETS))
        webgui.setMonitor(selitem, "<t3>" + Menus[menuState]->items[0] + "</t3>_target_");
    else if (menuState == EXTERNALS)
        webgui.setMonitor(selitem, "<t3>" + Menus[menuState]->items[0] + "</t3>_extern_");
    else if (menuState == SETTINGS)
    {
        if (selitem == -1)
            selitem = webgui.addStringDisplay("<t3>TMS</t3>", 0, 0, "f", "nomonitor");
        STACK;
        webgui.setMonitor(selitem, "<t3>" + Menus[menuState]->items[0] + "</t3>_set_");
        actpatid = webgui.addSwitches("edit", 1, actpat, &onSwitches, 0, 450, "t", "nomonitor");
        //       FDBG("actpatopt " + SN(actpatopt) + " " + SN(actpattern));
        //        midiopt = webgui.addOptions("Note", 127, midiNamesLong, &onOptionSelect, 0, 650, triggerNote[actpattern], "title");
        actgrpopt = webgui.addOptions("Goto", 4, pcount, &onOptionSelect, 0, 480, 0, "title");
        inleds[3] = webgui.addStringDisplay("MIDI IN", 0, 350, "title", "monframe");
        sipm = webgui.addButtons("MIDI", 5, btnss, &onButtonRelease, 0, 250, "title");
        movenext[0] = webgui.addButtons(moveBtn[0], &onButtonRelease, 40, 600, "f");  //  set Block
        movenext[1] = webgui.addButtons(moveBtn[1], &onButtonRelease, 100, 500, "f"); // next
        movenext[2] = webgui.addButtons(moveBtn[2], &onButtonRelease, 100, 550, "f"); // prev
        movenext[3] = webgui.addButtons(btns7[6], &onButtonRelease, 0, 200, "f");     // insert
        movenext[4] = webgui.addButtons(moveBtn[5], &onButtonRelease, 100, 450, "f"); // voice up
        movenext[5] = webgui.addButtons(moveBtn[6], &onButtonRelease, 100, 600, "f"); // voice dwn
        movenext[6] = webgui.addButtons(moveBtn[4], &onButtonRelease, 0, 550, "f");   // mark left
        movenext[7] = webgui.addButtons(moveBtn[7], &onButtonRelease, 30, 550, "f");  // mark right
        movenext[8] = webgui.addButtons(btns7[2], &onButtonRelease, 0, 600, "f");     // save
        movenext[9] = webgui.addButtons(moveBtn[9], &onButtonRelease, 60, 550, "f");  // delete
        patternidt = webgui.addStringDisplay("Pattern #", 400, 380, "f");
        ccpatternidt = webgui.addStringDisplay("Pattern #", 200, 400, "f");
        ccvpatternidt = webgui.addStringDisplay("Pattern #", 400, 400, "f");
        patternidt2 = webgui.addStringDisplay("Pattern #", 35, 725, "f");
        ccidt = webgui.addStringDisplay("", 600, 380, "f");
        keyidt = webgui.addStringDisplay("", 25, 730, "f");
        beatidt = webgui.addStringDisplay("", 25, 750, "f");
        nltidt = webgui.addStringDisplay("", 30, 675, "f");
        for (int v = 0; v < 4; v++)
        {
            int ypos = 450 + (v)*50;
            voiceidt[v] = webgui.addStringDisplay("", 200, ypos, "f");
            for (int i = 0; i < 2; i++)
            {
                patidt[v][i] = webgui.addStringDisplay("Rhythm", 1000 + i * 150, ypos - 10, "f");
                //                FDBG(SN(v) + SN(i) + SN(patidt[v][i]));
            } //       FDBG("ap " + SN(ap) + " " + SN(beatlength * numq) + " " + SN(beatlength) + " " + SN(numq));
        }
        STACK;
        update_pat(true);
    }
    else
        webgui.setMonitor(selitem, "<t3>" + Menus[menuState]->items[0] + "</t3>");
    //  DBG("menustate " + SN(menuState));
    if (menuState == MAIN || menuState == TOUCH || (menuState >= EXTSETTINGS && menuState <= TARGETS))
    {
        webgui.remove(idt);
        idt = webgui.addStringDisplay("connect", 200, 300, "f");
        for (int l = 0; l < 3; l++)
        {
            inleds[l] = webgui.addStringDisplay(paranamemidi[l], 250, xbase + 20 + l * mbase, "hide");
            inbuts[l] = webgui.addButtons(paranamemidi[l], &onButtonRelease, 250, xbase - 20 + l * mbase, "f", "hide");
        }
        for (int l = 0; l < 3; l++)
        {
            centerx[l] = 850;
            centery[l] = fbase + l * mbase + (menuState == MAIN ? 30 : 30);
        }
        //        inleds[3] = webgui.addStringDisplay("MIDI IN", 450, tbase, "title", "monframe");
        //        outleds[8] = webgui.addStringDisplay("MIDI OUT", 700, tbase, "title", "monframe");
        //        menuId[m] = webgui.addButtons(items[m], &onButtonRelease, 250, 110 + m * 20, "f", "hide");
        for (int l = 0; l < 4; l++)
        {
            centerx[l + 3] = 1050;
            centery[l + 3] = sbase + l * mbase - 20;
            if (l == 1 || l == 2)
            {
                centery[l + 3] += 12;
            }
            else
            {
                centery[l + 3] += 30;
            }
        }
        String s = showConnections();
        webgui.setMonitor(idt, s);
        for (int l = 0; l < 3; l++)
        {
            outleds[l] = webgui.addStringDisplay(paranamemidi[l + 3], 850, fbase + l * mbase, "title");
        }
        inleds[3] = webgui.addStringDisplay("MIDI IN", 250, tbase - 50, "title", "monframe");
        outleds[8] = webgui.addStringDisplay("MIDI OUT", 850, tbase - 50, "title", "monframe");
        //        menuId[m] = webgui.addButtons(items[m], &onButtonRelease, 250, 110 + m * 20, "f", "hide");
        int yo = sbase - 5;
        for (int l = 0; l < 4; l++)
        {
            if (l == 1 || l == 2)
            {
                outleds[l + 3] = webgui.addNumericDisplay(Menus[TARGETS]->items[5 + l], 1050 + 12, yo + l * mbase - 10, "title", "monframe");
            }
            else if (l > 0)
                outleds[l + 3] = webgui.addStringDisplay(Menus[TARGETS]->items[5 + l], 1050, yo + l * mbase - 10, "title");
            else
            {
                outleds[l + 3] = webgui.addStringDisplay(Menus[TARGETS]->items[5 + l], 1050, yo + l * 100 + 20, "hide");
                outbuts[l + 3] = webgui.addButtons(Menus[TARGETS]->items[5 + l], &onButtonRelease, 1020, yo + l * mbase - 15, "f", "hide");
            }
        }
        inleds[4] = webgui.addNumericDisplay("c", 380, tbase, "f", "monframe");
        inbuts[4] = webgui.addButtons("Controls", &onButtonRelease, 350, tbase - 50, "f", "hide");
        inleds[5] = webgui.addNumericDisplay("z", 450, tbase, "f", "monframe");
        inbuts[5] = webgui.addButtons("Touch", &onButtonRelease, 420, tbase - 50, "f", "hide");

        inleds[6] = webgui.addStringDisplay("mapped", 550, tbase, "title", "monframe");
        inbuts[6] = webgui.addButtons("Mapping", &onButtonRelease, 550, tbase - 50, "f", "hide");
        inleds[7] = webgui.addStringDisplay("scaled", 650, tbase, "title", "monframe");
        inbuts[7] = webgui.addButtons("Scaling", &onButtonRelease, 650, tbase - 50, "f", "hide");
    }
    if (menuState == EXTERNALS)
    {
        int off = 200;
        int offb = 240;

        int y = 120;

        for (int i = JOY_X, m = 0, j = 0; i <= EXT_SWITCH; i++, m++, j++)
        {
            String name = Menus[EXTERNALS]->items[i + 1 - EXTSETTINGS];
//            FDBG(name);
            if (i == EXT_PED_1)
            {
                j = 0;
                y = 350;
            }
            if (i == EXT_SWITCH)
            {
                y = 280;
                extId[i] = webgui.addStringDisplay(name, offb + 200 * j - 50, y, "f", "hide");
//                FDBG(SN(i) + SN(extId[i]));
                offb -= 50;
            }
            else if (i == HIRES)
            {
                extId[i] = webgui.addNeedleIndicator(name, 0, 10000, off + 200 * j, y, "f");
 //               FDBG(SN(i) + SN(extId[i]));
            }
            else
                extId[i] = webgui.addNeedleIndicator(name, 0, 1024, off + 200 * j, y, "f");
            menuId[m] = webgui.addButtons(name, &onButtonRelease, offb + 200 * j, y - 30, "f", "hide");
        }
    }
    if (menuState == TOUCH)
    {
        int off = 720;
        int y = 100;
        zone = webgui.addStringDisplay("Zone ", off, y, "f");
//        z1 = webgui.addStringDisplay("lp", 195, y - 15, "f");
//        z2 = webgui.addStringDisplay("lp", 440, y - 15, "f");
        //       String lp = "<img src = \"img/touch.png\" alt = \"touch\">";
        //       String ld = "<img src = \"img/disp.png\" alt = \"touch\">";
        //       webgui.setMonitor(z1, lp);
        //       webgui.setMonitor(z2, ld);
    }
    Menus[menuState]->createControl(__CALLER__);
    if (menuState != MAIN && menuState != EXTERNALS)
        Menus[menuState]->setMenu();
 //   FDBG("setmenu");
}
void webloop()
{
    static int mstate = -1;
    static int oldsb = -1;
    static String ooled = "";

    String outmen = "<style>table td  {  font-size : 16px; } </style> ";
    if (!client.connected())
    {
        FDBG(server);
        webgui.connect(&client, server);
        websetup();
    }
    if (!client.connected())
        return;
    //
    //    osb = -1;
    webgui.update();
    int actsb = ((MenuSynth *)MenuPara::SynthMenu)->actSubmenu;
    if (refresh || menuState != mstate)
    {
        websetup();
        oldsb = -1;
    }
    refresh = false;
    if (menuState == SYNTHSETTINGS)
    {
        if (oldsb != actsb)
        {
            //           FDBG("actsb " + SN(actsb) + SN(oldsb));
            websetup();
            SubMenu *sb = ((MenuSynth *)MenuPara::SynthMenu)->submenus[actsb];
            String name = sb->Paras[sb->myID]->name;
            if (name != "Presets")
                ((MenuSynth *)MenuPara::SynthMenu)->setMonitor();
            else
                webgui.remove(men);
            webgui.setMonitor(selitem, "<t3>" + name + "</t3>");
        }
        oldsb = actsb;
    }
    else if (menuState == MAIN || menuState == TOUCH || (menuState >= EXTSETTINGS && menuState <= TARGETS))
    {
        for (int l = 0; l < 3; l++)
            webgui.setMonitor(inleds[l], incoming[l] ? redled : offled);
        for (int l = 0; l < 7; l++)
        {
            bool state = outgoing[l] ? HIGH : LOW;
            if (l != 4 && l != 5)
                webgui.setMonitor(outleds[l], state ? redled : offled);
            else
                webgui.setMonitor(outleds[l], outgoing[l]);
        }
        webgui.setMonitor(inleds[3], MIDIinData);
        webgui.setMonitor(outleds[8], outData);
        isMap = curMMS->sourceCH == curMMS->outCH && (Menus[SETTINGS]->procMode == 2 || Menus[SETTINGS]->procMode == 0);
        scaled = (scFP[SCALES] != 2047 && Menus[SETTINGS]->procMode == 1) && !curMMS->isTB;

        //        if (isMap)
        if (scaled)
            webgui.setMonitor(inleds[7], outData);
        else
            webgui.setMonitor(inleds[6], outData);
    }

    if (menuState == SHOWSCALE || menuState == SCALES)
    {
        if (mstate != menuState || refresh)
            webgui.setMonitor(idt, noteLine(true));
        STACK;
        webgui.setMonitor(inleds[7], outData);
        webgui.setMonitor(inleds[3], MIDIinData);
        refresh = false;
    }
    else if (menuState == MAPPINGS || menuState == SHOWMAP)
    {
        webgui.setMonitor(inleds[6], outData);
        webgui.setMonitor(inleds[3], MIDIinData);
        if (mstate != menuState || refresh)
        {
            //           FSTACK;
            String nl = noteLine(false);
            //            FDBG(nl);
            webgui.setMonitor(idt, nl);
            webgui.setMonitor(idtg, grid);
            refresh = false;
            //          FSTACK;
        }
        webgui.setMonitor(bp, digitalRead(28) ? greenled : offled);
        webgui.setMonitor(ledout, digitalRead(13) ? redled : offled);
        webgui.setMonitor(semm, digitalRead(32) ? whiteled : offled);
        webgui.setMonitor(semp, digitalRead(30) ? yellowled : offled);
        webgui.setMonitor(octm, digitalRead(36) ? blueled : offled);
        webgui.setMonitor(octp, digitalRead(34) ? redled : offled);
    }

    else if (menuState == MAIN)
    {
        webgui.setMonitor(bp, digitalRead(28) ? greenled : offled);
        webgui.setMonitor(ledout, digitalRead(13) ? redled : offled);
        webgui.setMonitor(semm, digitalRead(32) ? whiteled : offled);
        webgui.setMonitor(semp, digitalRead(30) ? yellowled : offled);
        webgui.setMonitor(octm, digitalRead(36) ? blueled : offled);
        webgui.setMonitor(octp, digitalRead(34) ? redled : offled);
        String items[4] = {"Mode", "Octave", "Semitone", "Version"};
        String val[4] = {pmode[Menus[SETTINGS]->procMode], String(octave), SN(semiTone), "1.99"};
        String outmen = "";
        outmen = " <style>  table td  {  font-size : 16px;   } </style> ";
        //        outmen += "<t1 style=\" text-align : center;\"> Settings</t1> ";
        outmen += "<style>.alnright {text-align : right; }</style> ";
        outmen += "<style>.alnleft {text-align : left; }</style> ";
        outmen += "<table>";
        for (int i = 0; i < 4; i++)
        {
            outmen += "<tr>";
            outmen += "<td> " + items[i] + "</td>";
            outmen += "<td class=\'alnleft\'> " + val[i] + "</td>";
            outmen += "</tr>";
        }
        for (int i = 2; i < 6; i += 2)
        {
            outmen += "<tr>";
            outmen += "<td> " + Menus[VERSION]->items[i] + "</td>";
            outmen += "<td class=\'alnleft\'> " + Menus[VERSION]->items[i + 1] + "</td>";
            outmen += "</tr>";
        }
        outmen + "</table";
        webgui.setMonitor(showstat, outmen);
        webgui.setMonitor(sbp, offled);
        if (transport == RECORDING)
            webgui.setMonitor(sbp, redled);
    }
    else if (menuState == EXTERNALS)
    {
        Menus[EXTERNALS]->displayMenu(__CALLER__);
    }
    else if (menuState == SETTINGS)
    {
        webgui.setMonitor(inleds[3], midiNamesLong[mettrigger]);
        if (metison)
        {
            showStatus(patcnt, true);
        }
    }
    else if (menuState == TOUCH)
    {
        zoneDisp.setBuffer(zoneMem, 5000);
        zoneDisp = "<svg width=\"400px\" height=\"300px\"  style=\"  \"_defs_ ";
        for (int z = 0; z < 6; z++)
        {
            int h = Menus[ZONESETTINGS + z]->rawvalue / 7;
            int y = 250 - h;
            int x = z * 60 + 10;
            zoneDisp += " _r1_ height=\"" + String(h) + "\" x=\"" + String(x) + "\" y=\"" + String(y) + "\" />";
        }
        zoneDisp += showLine(10, 260, 366, 260, 3);

        zoneDisp += "</svg>";
        webgui.setMonitor(zone, zoneDisp);
 //       Serial.print("freeram = ");
 //       Serial.println(freeram());
    }

    if (menuState != SYNTHSETTINGS)
    {
        if (menuState != SCALES && menuState != MAPPINGS)
        {
            if (Menus[menuState]->isPara)
            {
                outmen += "<t1 style=\" text-align : left;\">Options</t1> ";
                outmen += ((MenuPara *)Menus[menuState])->setMenu();
            }
            else
            {
                if (menuState == MAIN)
                {
                    String name = "TM Sidekick";
                    //                  outmen += "<t1 style=\" text-align : left;\">" + Menus[menuState]->items[0] + "</t1> ";
                    webgui.setMonitor(selitem, "<t5>" + name + "</t5>_main_");
                    //                   Menus[menuState]->setMenu();
                }
                //               Menus[menuState]->setMenu();
            }
            webgui.setMonitor(men, outmen);
        }
        else
            webgui.remove(men);
    }

    mstate = menuState;
 //   if (menuState == TOUCH)
 //       FDBG(millis());
}
