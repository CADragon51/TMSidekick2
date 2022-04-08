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

String barnote(float x, float y, bool center, String color = "#000000")
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
String shownote(byte n, float basex, float basey)
{
    int ypos = notey[n % 24];
    bool isflat = noteflat[n % 12];
    String out = "";
    //    Serial.println(String(n % 24) + " @ " + String(ypos));
    if (isflat)
    {
        lastflat = ypos;
        out = showtext("b", basex - 12, basey - ypos * 6 - 3, "");
    }
    out += showtext("w", basex, basey - ypos * 6 - 3, "");

    if (ypos == 0 || ypos > 11)
    {
        out += barnote(basex - 4, basey - ypos * 6 + 1, ypos != 13);
    }
    return out;
}

// String showclef(float ptop)
// {
//     String my = String(ptop);
//     String ret = group +
//                  "<path"
//                  " style=\"fill:#063333;fill-opacity:1;stroke-width:0.3\" transform=\" translate(0 -100)\""
//                  " d=\"m 30," +
//                  my + " _clef_ " + endgroup;
//     //    Serial.println(ret);
//     return ret;
// }
// sMessage = sMessage.replaceAll('_ctext_', ctext);
// sMessage = sMessage.replaceAll('_note_', notew);
// sMessage = sMessage.replaceAll('_flat_', noteb);
String notew = "fill =\"#ffffff\"  class =\"note\"  >w</text>";
String noteb = "fill =\"#ffffff\"  class =\"flat\"  >b</text>";
String ctext = "fill =\"#ffffff\"  class =\"small\" >";
String tx = "<text X=\"";
String ty = "\"Y =\"";
String ta = " text-anchor = \"middle\" ";

String showtext(String text, int x, int y, String tclass = "")
{
    String ret = "_x_" + String(x) + "_y_" + String(y) + "\" ";
    if (text == "w")
        ret += "_n_";
    else if (text == "b")
        ret += "_f_";
    else if (text == "&amp;")
        ret += "fill=\"#ffffff\" class=\"clef\" >&amp;</text>";
    else if (tclass.indexOf("small") > -1)
        ret += "_c_" + text + "</text>";
    else
        ret += " text-anchor = \" middle \"  fill =\" #ffffff \"   >" + text + "</text>";
#if 0
    String t1 = " " + tclass + " >" + text + "</text>";
    String t0 = "<text X=\"" + String(x) + "\"Y =\"" + String(y) + "\"";
    String ta = "";
    String tc = "";
    tc = " fill =\"" + color + "\" ";
    if (tclass == "")
        ta = " text-anchor = \"middle\" ";
    ret = t0 + ta + tc + t1;
    //    Serial.println(ret);
    ret.replace(notew, "_n_").replace(noteb, "_f_").replace(ctext, "_c_").replace(tx, "_x_").replace(ty, "_y_");
#endif
    //   FDBG(ret);
    return ret;
}
int second = 0;
String showgrid(int which)
{
    String nl = showtext(SN(g_scid*2+1) + " " + scaleNames[g_scid] + " " + midiNamesFlat[scalebase] + " Scale", 300, 20);
    int _top = top;
    int yc = 106;
    int yt = 61;
    len = 620;
    if (which == 2)
    {
        String mn = actMap;
        nl = showtext(mn + " Mapping", 300, 40, "");
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
    nl += showtext("&amp;", 30, yc);
    // nl += "<text < tspan x = \"93.083664\" y = \"140.41434\" >&amp;</tspan></text> ";

    return nl;
}
String noteline(bool scale = true)
{
    String header = " <svg height=\"200\" width=\"1200\">";
    String nl = header;

    if (scale)
    {
        nl += showgrid(1);
        //   Serial.println(nl);
        Menus[SCALES]->scaleit(g_scid);
        //       if (refresh)
        //           Serial.println("refreshing");

        for (int i = 0; i < nx; i++)
        {
            int no = ln[i] + scalebase;
            nl += showLine(left + 66 + i * 40, 60, left + 66 + i * 40, 110);
            nl += shownote(no, left + 50 + i * 40, 124);
            nl += showtext(midiNamesFlat[no % 12], left + 50 + i * 40 - noteflat[no % 12] * 8, 145);
        }
    }
    else
    {
        int numc = 12;
        grid = nl + showgrid(2);
        int x = left + 55;
        int st = actkeyidx / numc;
        st *= numc;
        for (int i = 0; i < 20; i++)
        {
            webgui.remove(menuId[i]);
            webgui.remove(transId[i]);
        }

         
        short pos = -1;
        #if 0
        for (int s = 0; s < 12; s++)
        {
            String out = "{";
            int pos = 0;
            for (int t = 0; t < 12; t++)
            {
                if (iswhole[s][t] < 0)
                    out = out + ",-1";
                else
                    out = out + "," + SN(pos++);
            }
            FDBG(out + "},");
        }
        #endif
        for (int l = st; l < maxrepl && l < st + numc; l++)
        {
            int i = l;
            int k = keychord[i] / 100;
            if (iswhole[scalebase][i] < 0&&progressmode)
                continue;
            pos++;
            int b = (keychord[i] % 100 + scalebase) % 12;
            Chord *cc = chords[k];
            int ycp = 118;
            int t = 0;
            int rc = replchord[pos];
            int tr = (scalebase + transpose[pos]) % 12;
//            FDBG(SN(scalebase) + " " + SN(i) + "? whole " + SN(mapwhole[scalebase][i]) + " -> " + SN(pos) + " " + isFlat[tr]);
            if (progressmode&&(scalebase != 5 && scalebase != 0))
                tr -= isFlat[tr];
            else if (progressmode)
                tr += isFlat[tr];

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
                transId[i - st] = webgui.addButtons(midiNamesFlat[tr], &onButtonRelease, x + 20, ycp + 220, "f", "smallhide");
                menuId[i - st] = webgui.addButtons(cc->getName(), &onButtonRelease, x + 40, ycp + 220, "f", "smallhide");
                //                FDBG("menuu " + SN(i - st));
            }
            x += 60;

            nl += showLine(x, 140 + 62, x, 250);
            x += 28;
        }
    }
    nl += svgend;
    grid += svgend;
    //    Serial.println(nl);
    //   Serial.println("show");
    String swnw = "stroke-width=\"1 \" fill = \"none\" stroke=\"#ffffff\"";
    return nl.replace(swnw, "_w_");
}
extern int centerx[10], centery[10];

extern short xbase;
String connections(void)
{
    String header = " <svg height=\"600\" width=\"1000\"> _def2_";
    String nl = header;
    int k = 0;
    for (int i = USB_MIDI_IN; i <= MIDI_IN; i++)
    {
        MenuMidiSet *ms = ((MenuMidiSet *)Menus[i]);
        MenuTargetSet *ts = ms->targets;
        k = i - USB_MIDI_IN;
        for (int t = 3; t < ts->nump; t++)
        {
            Parameter *mpara = ts->Paras[t];
            if (SB(*mpara->value) == "f")
                continue;
            //            FDBG(SN(t) + ": center " + SN(centerx[t - 3]) + "," + SN(centery[t - 3] - 270) + " <- " + SN(xbase + k * 100 - 260));
            short alpha = 0;
            if (centery[t - 3] - 270 == xbase + k * 100 - 260)
                alpha = 4;
            nl += showLine(75, xbase + k * 100 - 260 - alpha, centerx[t - 3] - 200, centery[t - 3] - 270 + alpha, 4).replace("path ", "path _blur_");
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