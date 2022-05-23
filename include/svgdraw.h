#pragma once
int left = 20;
int offset = 12;
int top = 50;
int len = 620;
int idc = 100;
#include "parameter.h"
#include "globals.h"
#include "webdata.h"
String style = "<style> .small { font: italic 11px sans-serif; color:#ffffff; }  .flat { font-size: 26px ;font-family: 'Music'; color:#ffffff;} .note { font-size: 26px;font-family: 'Music';color:#ffffff; }"
               " .clef { font-size : 40px;font-family: 'Music'; color:#ffffff;} .thin{ stroke : rgb(0xff, 0xff, 0xff); stroke-width : 1px;} </style>";

String header = " <svg height=\"300\" width=\"650\">";
String defs = " _defs_ ";
//"<defs> < g transform = \"translate(x,y)\" /><path id = \"mysine\" d = \"M10 80  C 40 10, 65 10, 95 80 S 150 150, 180 80 \" stroke = \"#ffffff\" fill = \"transparent\" /></g></defs> ";
//+ style;
String group = "<g>";
String endgroup = "</g>";
String svgend = "</svg>";
// String transform = "<g     transform=\"";
byte notey[24] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7, 8, 8, 9, 9, 10, 11, 11, 12, 12, 13, 13};
byte noteyS[24] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 9, 10, 10, 11, 12, 12, 13, 13};
byte noteflat[12] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
byte lastflat = 0;
Parameter **sParas = MenuPara::SynthMenu->Paras;
String P[12];

String showADSR(int w = 0)
{
    String P[10];
    float fv[9] = {0, 0, 0, 0.5, 0, 0, 0.8, 0};
    for (int i = 1; i < 9; i++)
    {
        byte sp = id2synth[i];
        //           Parameter *para = sParas[sp];
        fv[i] = ((MenuSynth *)MenuPara::SynthMenu)->getValue(sp);
        //       Serial.println(String(i) + " " + String(sp) + " " + String(fv[i])); // + " " + para->name);
    }
    float f = 0.7;
    int shift = 120;
    if (w > 0)
    {
        shift = 0;
        f = 0.5;
    }
    int top = 40 * f;
    int left = 20 * f;
    int bottom = 220 * f;
    int off = 360 * f + left;
    int right = 360 * f;
    //  float mag =1;
    float t1 = 100 * f;
    float t2 = 100 * f;
    float fa = fv[1] * t1;
    float fd = fa + fv[2] * t1;
    float fs = (bottom - top) * fv[3];
    float fr = 290 * f + 70 * (1 - fv[4]) * f;
    float aa = fv[1 + 4] * t2;
    float ad = aa + fv[2 + 4] * t2;
    float as = (bottom - top) * fv[3 + 4];
    float ar = 670 * f + 70 * (1 - fv[4 + 4]) * f; // Serial.println(String(fv[3]) + " " + String(t1) + " " + String(ar));
    String men = " <svg height=\"230\" width=\"785\">"
                 "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>";

    if (w == 0 || w == 1)
    {
        P[0] = " " + String(left + shift) + "," + String(bottom);
        P[1] = " " + String(left + fa + shift) + "," + String(top);
        P[2] = " " + String(left + fd + shift) + "," + String(bottom - fs);
        P[3] = " " + String(fr + shift) + "," + String(bottom - fs);
        P[4] = " " + String(right + shift) + "," + String(bottom);
        String FPos = P[0] + P[1] + P[2] + P[3] + P[4];
        men += "<polyline points=\"" + FPos + " \" class=\"poly\"/>";
    }
    if (w == 0 || w == 2)
    {
        shift = 2 * shift;
        P[5] = " " + String(off + shift) + "," + String(bottom);
        P[6] = " " + String(off + aa + shift) + "," + String(top);
        P[7] = " " + String(off + ad + shift) + "," + String(bottom - as);
        P[8] = " " + String(ar + shift) + "," + String(bottom - as);
        P[9] = " " + String(off + right + shift) + "," + String(bottom);
        String APos = P[5] + P[6] + P[7] + P[8] + P[9];
        men += "<polyline points=\"" + APos + " \" class=\"poly\"/>";
    }

    men += svgend;
    //    Serial.println(men);
    return men;
}

String showLine(float x1, float y1, float x2, float y2, int width = 1)
{
    String x1P = "M " + String(x1) + " " + String(y1);
    String x2P = " l " + String(x2 - x1) + " " + String(y2 - y1);
    String ret = "<path d=\"" + x1P + x2P + "\" stroke-width=\"" + String(width) + " \" fill = \"none\" stroke=\"#ffffff\" /> ";
    //    DBG(ret);
    String swnw = "stroke-width=\"1 \" fill = \"none\" stroke=\"#ffffff\"";
    return ret.replace(swnw, "_w_");
}

String barNote(float x, float y, bool center, String color = "#000000")
{
    String out = "";
    String mid = showLine(x, y - 4, x + 18, y - 4);
    String bottom = showLine(x, y + 2, x + 18, y + 2);
    if (center)
        out += mid;
    else
        out += bottom;
    return out;
}
String showNote(byte n, float basex, float basey)
{
    int ypos = notey[n % 24];
    int shiftx = 0;
    if (!showflat)
        ypos = noteyS[n % 24];
    bool isflat = noteflat[n % 12];
    static float ox = 0;
    String out = "";
    //   Serial.println(midiNames[n % 12] + " @ " + String(ypos) + " " + SN(lastflat) + " " + basex);
    if (lastflat == ypos && ox == basex)
    {
        shiftx = -10;
        //        Serial.println(midiNames[n % 12] + " @ " + String(ypos) + " " + SN(lastflat) + " " + basex);
    }
    if (isflat)
    {
        if (showflat)
            out = showText("b", shiftx + basex - 12, basey - ypos * 6 - 3, "", "white");
        else
        {
            out = showText("#", shiftx + basex - 12, basey - ypos * 6 - 3, "", "white");
        }
    }
    out += showText("w", shiftx + basex, basey - ypos * 6 - 3, "", "white");

    if (ypos == 0 || ypos > 11)
    {
        out += barNote(shiftx + basex - 4, basey - ypos * 6 + 1, ypos != 13);
    }
    lastflat = ypos;
    ox = basex;
    return out;
}

String notew = "fill =\"#ffffff\"  class =\"note\"  >w</text>";
String noteb = "fill =\"#ffffff\"  class =\"flat\"  >b</text>";
String ctext = "fill =\"#ffffff\"  class =\"small\" >";
String tx = "<text X=\"";
String ty = "\"Y =\"";
String ta = " text-anchor = \"middle\" ";

String showText(String text, int x, int y, String tclass = "", String fill = "#ffffff")
{
    String ret = "_x_" + String(x) + "_y_" + String(y) + "\" ";
    if (text == "w")
        ret += "_n_";
    else if (text == "b")
        ret += "_f_";
    else if (text == "#")
        ret += "_s_";
    else if (text == "&amp;")
        ret += "fill=\"" + fill + "\" class=\"clef\" >&amp;</text>";
    else if (tclass.indexOf("small") > -1)
        ret += "_c_" + text + "</text>";
    else if (tclass.indexOf("tiny") > -1)
        ret += "_t_" + text + "</text>";
    else if (tclass.indexOf("teeny") > -1)
        ret += "_u_" + text + "</text>";
    else
        ret += " text-anchor = \" middle \"  fill =\"" + fill + "\"   >" + text + "</text>";
    return ret;
}
int second = 0;

EXTMEM String ltot[] = {"3t", "3s", "s", "3e", "s.", "e", "3q", "3q", "e.", "e.", "q", "q"};
EXTMEM String erg[maxticks * 4];
String pat2string(int actpat, int v)
{
    int pat = actpat;
    String pzero = "";
    int f1 = 48;
    STACK;
    if (v > -1)
    {
        int pos = (actpattern / 4 + actpat) * 4 * maxticks;
        //       FDBG("pos " + SN(pos) + " " + SN(actpattern) + " "+ SN(actpat));
        for (int i = 0; i < maxticks * beatlength; i++)
        {
            short val = seqpattern[i + pos][v][actgrp];
            if (val > 0)
            {
                val = 1;
                if (i == 0)
                    f1 = 0;
                else if (f1 == 48)
                    f1 = i;
                //               FDBG("f1 " + SN(f1));
            }
            else
                val = 0;
            pzero += SN(val);
        }
 //       FDBG("show metro " + SN(pos) + " " + SN(v) + " " + pzero+" "+SN(f1)+SN(actgrp));
    }
    else
    {
        for (int i = 0; i < maxticks; i++)
        {
            bool pt = (pat & (1 << i)) > 0;
            if (i == 0 && pt)
                f1 = 0;
            if (pt)
            {
                pzero += "1";
                if (f1 == 48 && i)
                    f1 = i;
            }
            else
                pzero += "0";
        }
        //       FDBG(pzero);
    }
    int p = 0;

    p = SplitS(pzero, '1', erg, maxticks * 4);
//    FDBG(pzero);
//    FDBG(p);
    String rs = "";
    STACK;
    for (int i = 1; i < p; i++)
    {
        byte nl = erg[i].length();
        //       FDBG(SN(i) + " notelen " + SN(nl));
        if (nl < 12)
        {
            rs += ltot[nl];
            //          FDBG(rs + " notelen " + SN(nl));
        }
        else if (nl <= 17)
        {
            rs += "q.";
        }
        else if (nl <= 23)
        {
            rs += "h";
        }
        else if (nl < 47)
        {
            rs += "h.";
        }
        else
        {
            rs += "w";
        }
        //        FDBG(rs + " notelen " + SN(nl));
    }
    if (f1 > 12)
    {
        if (f1 == 24)
            f1 = 13;
        if (f1 == 36)
            f1 = 14;
        else
            f1 = 15;
    }
    if(rs.length()>0)
    rs.replace("3s3s3s", "3sss").replace("3e3e3e", "3eee").replace("3ssse", "3sss e");
    static String rests[16] = {"", ".", "..", "&#119103;",
                               "&#119103;.", "&#119103;..", "&#119102;", "&#119102;.",
                               "&#119102;..", "&#119102;&#119103;", "&#119102;&#119103;.", "%&#119103;..",
                               "&#119101;", "&#119100;", "&#119100;.", "&#119099;"};
    //    FDBG(pzero+" rest " + SN(f1));
    if (f1 > 0)
        rs = rests[f1] + "&nbsp;" + rs;
//    FDBG("pat 2 string "+SN(pat) + " " + pzero + " " + SN(f1) + " " + rs);
    return rs;
}
byte checkKey(int p)
{
    return acttrigger[p][actgrp] < 128 ? acttrigger[p][actgrp] : 0;
}
#define patstate(x) beatCount[x][actgrp] == 0 ? "&#x25AF;" : "&#x25AE;"
#define actstate(x) beatCount[x][actgrp] == 0 ? "&#x25C7;" : "&#x25C6;"
//#define patkey(x) checkkey(x) ? "&#x251C;" : "&#x250B;"
#define patkey(x) "&#x251C;"

void showStatus(int pos)
{
    String outblocks = "<span style =\"color:beige;font-size:20px;\">";
    String inblocks = "<text style =\"color:red;font-size:20px;\">";
    String markblocks = "<text style =\"color:#FFD700;font-size:20px;\">";
    String open[2] = {"&#x25AF;", "&#x25AE;"};
    String key[2] = {"&#x2503;", "&#x2503;"};
    String act[2] = {"&#x25C7;", "&#x25C6;"};
    int lastbeat = 0;
    bool isOver = false;
    for (int i = 0; i < MAXPAT; i++)
    {
        bool isBeat = (i % beatlength) == 0;
        if (isBeat)
            lastbeat = i;
        int b = beatCount[lastbeat][actgrp] == 0 ? 0 : 1;
        int k = checkKey(lastbeat) > 0 ? 1 : 0;
        if (patfiles[i][actgrp] != "")
        {
            outblocks += "</text><text style =\"color:" +
                         patcolors[i][actgrp] + ";font-size:20px;\">";
            //            FDBG(patcolors[i] + " " + patfiles[i] + " " + SN(i) + SN(i - lastbeat * 4) + SN(beatCount[i]));
            isOver = true;
        }
        else if (!isOver)
        {
            isOver = false;
            outblocks += "</text><text style =\"color:beige;font-size:20px;\">";
        }
        if (i >= 0 && i < pos)
        {
            if (isBeat)
            {
                if (actpat[0])
                    inblocks += key[k];
                else
                    markblocks += key[k];
            }
            if (actpat[0])
                inblocks += open[b];
            else
                markblocks += open[b];
            continue;
        }
        if (isBeat && (i < startpattern || i > pos))
        {

            outblocks += key[0];
        }
        if (i == pos)
        {
            if (isBeat)
            {
                if (actpat[0])
                    inblocks += key[k];
                else
                    markblocks += key[k];
            }
            if (actpat[0])
            {
                inblocks += act[b] + "</text>";
                outblocks += inblocks;
            }
            else
            {
                markblocks += act[b] + "</text>";
                outblocks += markblocks;
            }

            continue;
        }

        outblocks += open[b];
    }
    outblocks += "</span>";
    //    FDBG(outblocks);
    webgui.setMonitor(patternidt2, outblocks);
}
String showRhythm(String what, int id)
{

    String mettext = "<svg width=\"60mm\" height=\"80mm\" id=\"" + String(id) + "\">" +
                     "<text><tspan style=\" font-size:16px;" +
                     "font-family:'Metric';stroke-width:0.5px;fill:#f6ffd5\" x=\"0\" y=\"30\">" +
                     what + "</tspan></text></svg>";
    //   FDBG(mettext);
    return mettext;
}
void showCCs(void)
{
    String header = " <svg height=\"40\" width=\"1000\">";
    String nl = header;
    for (int i = actpattern * maxticks, j = 0; i < (actpattern + 1) * maxticks; i++, j++)
    {
        if (ccpattern[i][actgrp] < 1)
            continue;
        String out = String(ccpattern[i][actgrp]) + "/" + String(ccval[i][actgrp]);
        //       FDBG(SN(i) + " " + mid);
        nl += showText(out, 80 + j * 25.5, 15, "tiny");
    }
    //    FDBG(header + nl + svgend);
    webgui.setMonitor(ccidt, nl + svgend);
    //    FDBG(nl + svgend);
}
void showKeys(void)
{
    String header = " <svg height=\"50\" width=\"1600\">";
    String nl, nlb, nlt;
    int startpat = actpattern / 64;
    for (int i = startpat, f = 0; i < MAXPAT; i += beatlength, f++)
    {
        byte mid = checkKey(i);
        int j = i - startpat * 64;
  //      FDBG("showkeys "+SN(i)+SN(mid)+SN(mettrigger));
        //        if (i == 0)
        //            FDBG(SN(i) + " " + SN(mid) + " " + SN(triggerNote[mid]));
        if (patfiles[f * 4][actgrp] != "")
            nlt += showText(patfiles[f * 4][actgrp], 12 + f * 90, 10 + (f % 2) * 15, "teeny");
        if (mid > 0)
        {
            if (mid != mettrigger)
                nl += showText(midiNamesLong[mid], 15 + j * 22.5, 20, "");
            else
            {
                nl += showText(midiNamesLong[mid], 15 + j * 22.5, 20, "", "red");
                //                FDBG(nl);
            }
        }
        nlb += showText(String(i / 4 + zerobase), 15 + j * 22.5, 20, "");
    }
    //    FDBG(header + nl + svgend);
    webgui.setMonitor(nltidt, header + nlt + svgend);
    webgui.setMonitor(keyidt, header + nl + svgend);
    webgui.setMonitor(beatidt, header + nlb + svgend);
}
String showGrid(int which)
{
    String nl = showText(SN(g_scid * 2 + 1) + " " + scaleNames[g_scid] + " " + midiNamesFlat[scalebase] + " Scale", 300, 20);
    int _top = top;
    int yc = 106;
    int yt = 61;
    len = 620;
    if (which == 2)
    {
        String mn = actMap;
        nl = showText(mn + " Mapping", 300, 40, "");
        len = 1150;
    }
    _top += offset - 1;

    for (int i = 0; i < 5; i++)
    {
        float yp = _top + i * offset;
        nl += showLine(left, yp, len, yp);
    }
    nl += showLine(20, yt, 20, yt + 48);
    nl += showLine(len, yt, len, yt + 48, 4);
    //   Serial.println(nl);
    //   nl += showclef(yc);
    //    nl += showtext("&#x1D11E;", 40, yc, "#ffffff", "class=\"huge\"");
    nl += showText("&amp;", 30, yc);
    // nl += "<text < tspan x = \"93.083664\" y = \"140.41434\" >&amp;</tspan></text> ";

    return nl;
}
String noteLine(bool scale = true)
{
    String header = " <svg height=\"200\" width=\"1200\">";
    String nl = header;
    STACK;

    if (scale)
    {
        nl += showGrid(1);
        //   Serial.println(nl);
        Menus[SCALES]->scaleit(g_scid);
 //       FDBG("Scale " + SN(g_scid) + SN(scalebase));
        //       if (refresh)
        //           Serial.println("refreshing");

        for (int i = 0; i < g_nx; i++)
        {
            int no = g_ln[i] + scalebase;
  //          FDBG(SN(i) + SN(no)+SN(g_ln[i]));
            nl += showLine(left + 66 + i * 40, 60, left + 66 + i * 40, 110);
            nl += showNote(no, left + 50 + i * 40, 124);
            if (showflat)
                nl += showText(midiNamesFlat[no % 12], left + 50 + i * 40 - noteflat[no % 12] * 8, 145);
            else
                nl += showText(midiNamesSharp[no % 12], left + 50 + i * 40 - noteflat[no % 12] * 8, 145);
        }
    }
    else
    {
        STACK;
        int numc = 12;
        grid = nl + showGrid(2);
        int x = left + 55;
        int st = actkeyidx / numc;
        st *= numc;
        for (int i = 0; i < 20; i++)
        {
            webgui.remove(chordselId[i]);
            webgui.remove(transId[i]);
        }

        short pos = -1;
        //       FDBG(SN(st));
        for (int l = st; l < maxrepl && l < st + numc; l++)
        {
            int i = l;
            int k = keychord[i] / 100;
            if (iswhole[scalebase][i] < 0 && progressmode && g_scid == 2047)
                continue;
            pos++;
            int b = (keychord[i] % 100 + scalebase) % 12;
            Chord *cc = chords[k];
            int ycp = 118;
            int t = 0;
            int rc = replchord[pos];
            int tr = (scalebase + transpose[pos] + b) % 12;
 //           if (transpose[pos]<0)
 //               FDBG(SN(pos) + " " + SN(transpose[pos]) + " " + SN(tr) + SN(scalebase));
            //            FDBG(SN(scalebase) + " " + SN(i) + "? whole " + SN(mapwhole[scalebase][i]) + " -> " + SN(pos) + " " + isFlat[tr]);
            //            if (progressmode && (scalebase != 5 && scalebase != 0))
            //                tr -= isFlat[tr];
            //            else if (progressmode)
            //                tr += isFlat[tr];

            if (cc != nullptr)
            {
                nl += cc->showChord(x, ycp, b - t, false, false);
            }
            cc = chords[rc];
            if (cc != nullptr)
            {
                nl += cc->showChord(x + 40, ycp, tr, false, true);
                //               short l = midiNamesFlat[transpose[i] % 12].length();
                //              short xp = l == 1 ? 10 : 0;
                xtransId[i - st] = x;
                if (showflat)
                    transId[i - st] = webgui.addButtons(midiNamesFlat[tr], &onButtonRelease, x + 20, ycp + 230, "f", "smallhide");
                else
                    transId[i - st] = webgui.addButtons(midiNamesSharp[tr], &onButtonRelease, x + 20, ycp + 230, "f", "smallhide");
                chordselId[i - st] = webgui.addButtons(cc->getName(), &onButtonRelease, x + 40, ycp + 230, "f", "smallhide");
                //                FDBG("menuu " + SN(i - st));
            }
            x += 60;

            nl += showLine(x, 140 + 62, x, 250);
            x += 28;
        }
    }
    STACK;
    nl += svgend;
    grid += svgend;
    //    Serial.println(nl);
    //   Serial.println("show");
    String swnw = "stroke-width=\"1 \" fill = \"none\" stroke=\"#ffffff\"";
    return nl.replace(swnw, "_w_");
}
extern int centerx[10], centery[10];

extern short xbase;
String showConnections(void)
{
    String header = " <svg height=\"600\" width=\"1000\"> _def2_";
    String nl = header;
    int k = 0;
    for (int i = USB_MIDI_IN; i <= MIDI_IN; i++)
    {
        MenuMidiSet *ms = ((MenuMidiSet *)Menus[i]);
        MenuTargetSet *ts = ms->targets;
        k = i - USB_MIDI_IN;
        for (int t = 2; t < ts->nump; t++)
        {
            Parameter *mpara = ts->Paras[t];
            if (SB(*mpara->value) == "f")
                continue;
            //            FDBG(SN(t) + ": center " + SN(centerx[t - 3]) + "," + SN(centery[t - 3] - 270) + " <- " + SN(xbase + k * 100 - 260));
            short alpha = 0;
            if (centery[t - 2] - 270 == xbase + k * 100 - 260)
                alpha = 4;
            nl += showLine(75, xbase + k * 100 - 260 - alpha, centerx[t - 2] - 200, centery[t - 2] - 270 + alpha, 4).replace("path ", "path _blur_");
            //            DBG(SN(k) + " " + ms->items[0] + "->" + SN(t) + " " + mpara->name + " = " + SB(*mpara->value));
        }
    }
    nl += svgend;
    return nl;
}
String lowp = "<path style=\"fill:none;stroke:#ffffff;stroke-width:2px;\" d=\"m 16.178244,71.551319 45.751484,-0.741122 c 14.94349,-0.150156 29.942094,26.907563 40.318492,56.324953 2.35996,5.66496 3.40941,13.02681 8.00651,13.34013 l 29.35141,0.0449\"/>";
String bandp = "<path style=\"fill:none;stroke:#ffffff;stroke-width:2px;\"d=\"m 19.087042,223.13999 h 9.725705 c 23.734618,-0.58737 36.478951,-31.91839 46.34013,-58.89282 2.071384,-6.393 2.656095,-9.43415 5.721004,-9.51065 4.540544,0.13778 5.557076,8.71974 7.437305,13.90017 5.856311,24.67856 28.666084,54.50715 45.481984,55.23488 l 8.5815,0.36582\"/>";
String hip = "<path style=\"fill:none;stroke:#ffffff;stroke-width:2px; \" d=\"m 83.070363,76.209161 29.829427,-5.4e-5 c 4.12239,-0.198289 6.92242,-7.309269 8.53861,-11.984668 7.10281,-20.547172 21.66276,-58.005247 43.59187,-57.32891 l 41.33508,-0.185262\"/>";
String expo = "<path  style=\"fill:none;stroke:#ffffff;stroke-width:2px\" d=\" M 11.621978, 84.23875 C 76.201161, 79.533011 138.5956, 65.669047 189.92466, 5.4201668 v 0 \" />";
String showShape(int sp)
{
    String fx;
    int t = ((MenuSynth *)MenuPara::SynthMenu)->getType(sp, fx);
    int s = ((MenuSynth *)MenuPara::SynthMenu)->getValue(sp);
    DBG("show shape" + SN(t) + " " + fx);
    int top = 20;
    int left = 10;
    int bottom = 150;
    String APos = "";
    String width = "width=\"230\" >";
    String nl = " <svg height=\"150\"" + width;
    //    String lpass ="<path style =\"fill:none;stroke:#000000;stroke-width:2px;\" d=\"m 26.496645,63.523893 28.882149,-0.490158 c 9.433576,-0.09931 18.901944,17.79596 25.452393,37.251855 1.489799,3.74665 2.152307,8.61559 5.054375,8.82281 l 18.529058,0.0297\"  />"; //   DBG(" function " + SN(s));
    if (t <= 3)
    {
        //        Serial.println(SN(t) + "?" + fx);
        if (fx == "LP")
        {
            nl = "<svg height = \"150\" " + width + " <g transform=\" translate(0,-30) \" >";
            nl += lowp;
        }
        else if (fx == "BP")
        {
            nl = "<svg height = \"150\" " + width + " <g transform=\" translate(20,-100) \" >";
            nl += bandp;
        }
        else if (fx == "HP")
        {
            nl = "<svg height = \"150\" " + width + " <g transform=\" translate(-30,50) \" >";
            nl += hip;
        }
        else if (fx == "EXP")
        {
            nl = "<svg height = \"150\" " + width + " <g transform=\" translate(0,+20) \" >";
            nl += expo;
        }
        else if (fx == "LIN")
        {
            nl += showLine(10, 104, 200, 25, 1);
        }
        else
            nl += showLine(10, 75, 200, 75, 1);
        nl += "</g>";
        nl += svgend;
        DBG(nl);
        return (nl);
    }
    switch (s)
    {
    case WAVEFORM_SINE:
        nl = "<svg height = \"150\" " + width + " <g transform=\" translate(20, 0) \" >";
        nl += "<path id = \"mysine\" d = \"M10 80  C 40 10, 65 10, 95 80 S 150 150, 180 80 \" stroke = \"#ffffff\" fill = \"transparent\" stroke-width=\"2\"/>";
        nl += "</g>";
        break;
    case WAVEFORM_SAWTOOTH:
    case 9:
    {
        P[0] = " " + String(left) + "," + String(bottom);
        P[1] = " " + String(left + 100) + "," + String(top);
        P[2] = " " + String(left + 100) + "," + String(bottom);
        P[3] = " " + String(left + 200) + "," + String(top);
        P[4] = " " + String(left + 200) + "," + String(bottom);
        APos = P[0] + P[1] + P[2] + P[3] + P[4];

        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    case WAVEFORM_SQUARE:
    case 11:
    {
        P[0] = " " + String(left) + "," + String(bottom);
        P[1] = " " + String(left) + "," + String(top);
        P[2] = " " + String(left + 100) + "," + String(top);
        P[3] = " " + String(left + 100) + "," + String(bottom);
        P[4] = " " + String(left + 200) + "," + String(bottom);
        P[5] = " " + String(left + 200) + "," + String(top);
        APos = P[0] + P[1] + P[2] + P[3] + P[4] + P[5];

        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    case WAVEFORM_TRIANGLE:
    {
        P[0] = " " + String(left) + "," + String(bottom);
        P[1] = " " + String(left + 50) + "," + String(top);
        P[2] = " " + String(left + 100) + "," + String(bottom);
        P[3] = " " + String(left + 150) + "," + String(top);
        P[4] = " " + String(left + 200) + "," + String(bottom);
        APos = P[0] + P[1] + P[2] + P[3] + P[4];

        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    case WAVEFORM_ARBITRARY:
        //       nl = < svg height = \"150\" " + width;
        for (int i = 0; i < 256; i++)
        {
            float y = left + i * 0.8;
            P[0] = " " + String(y) + "," + String(75 - myWaveform[i] / 512);
            APos += P[0];
        }
        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
        break;
    case WAVEFORM_PULSE:
    case 12:
    {
        P[0] = " " + String(left) + "," + String(bottom);
        P[1] = " " + String(left) + "," + String(top);
        P[2] = " " + String(left + 75) + "," + String(top);
        P[3] = " " + String(left + 75) + "," + String(bottom);
        P[4] = " " + String(left + 200) + "," + String(bottom);
        P[5] = " " + String(left + 200) + "," + String(top);
        APos = P[0] + P[1] + P[2] + P[3] + P[4] + P[5];

        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    case WAVEFORM_SAWTOOTH_REVERSE:
    case 10:
    {
        P[0] = " " + String(left) + "," + String(bottom);
        P[1] = " " + String(left) + "," + String(top);
        P[2] = " " + String(left + 100) + "," + String(bottom);
        P[3] = " " + String(left + 100) + "," + String(top);
        P[4] = " " + String(left + 200) + "," + String(bottom);
        APos = P[0] + P[1] + P[2] + P[3] + P[4];

        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    case WAVEFORM_TRIANGLE_VARIABLE:
    {
        P[0] = " " + String(left) + "," + String(bottom);
        P[1] = " " + String(left + 20) + "," + String(top);
        P[2] = " " + String(left + 100) + "," + String(bottom);
        P[3] = " " + String(left + 130) + "," + String(top);
        P[4] = " " + String(left + 200) + "," + String(bottom);
        APos = P[0] + P[1] + P[2] + P[3] + P[4];

        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    case WAVEFORM_SAMPLE_HOLD:
    {
        byte y = bottom - random(130);
        byte y1 = bottom - random(130);
        for (int i = 0; i < 8; i++)
        {
            P[0] = " " + String(left + i * 25) + "," + String(y);
            P[1] = " " + String(left + i * 25) + "," + String(y1);
            P[2] = " " + String(left + (1 + i) * 25) + "," + String(y1);
            y = y1;
            y1 = bottom - random(130);
            APos += P[0] + P[1] + P[2];
        }
        nl += "<style>.poly{fill:none;stroke:#ffffff;stroke-width:2;}</style>"
              "<polyline points=\"" +
              APos + " \" class=\"poly\"/>";
    }
    break;
    }
    nl += svgend;
    //   Serial.println(nl);
    return (nl);
}