class SubMenu
{
public:
    SubMenu(byte *psubitems, Parameter **pParas, byte id)
    {
        Paras = pParas;
        myID = id;
        subitems = psubitems;
        numsub = 0;
        for (int s = 0; s < 7; s++)
        {
            if (subitems[s] > 0)
                numsub++;
            else
                break;
            //            if (Paras[myID]->name=="VCF")
            //                Serial.println(Paras[myID]->name + " " + SN(subitems[s]));
            Parameter *mPara = Paras[subitems[s]];
//            Serial.println("subparas:" + mPara->name + " " + String(mPara->fvalue) + " " + String(s));
            if (Paras[myID]->name == "ADSR")
            {
                if (mPara->name.startsWith("VCF"))
                    mPara->name.replace("VCF ", "");
                if (mPara->name.startsWith("VCA"))
                    mPara->name.replace("VCA ", "");
            }
            if (Paras[myID]->name == "Wave")
            {
                if (mPara->name.startsWith("VCO"))
                    mPara->name.replace("VCO ", "");
            }
            else
                mPara->name.replace(Paras[myID]->name + " ", "");
            if (mPara->name.endsWith("Shape"))
            {
                hasShape = true;
                shapeix = s;
            }
            //			Serial.println("Shape?" + mPara->name + " " + String(shapeix));
        }
    }
    Parameter *actPara(int s, int submenu)
    {
        DBG(subPara->name + " " + Paras[subitems[s]]->name + " " + SN(subPara->submenu) + " " + SN(Paras[subitems[s]]->submenu) + " " + SN(submenu));
        return (subPara->name != Paras[subitems[s]]->name) ? Paras[subitems[s]] : subPara;
        //       return (subPara);
    }
    void displayValue(int line, Parameter *aPara)
    {
        byte soff = 0;
        String oval = aPara->format();
        //        Serial.println("disp val = " + oval);

        if (Paras[myID]->name == "ADSR")
            soff = 3 * 8;
        else if (numsub < 7)
            soff = 5;
        display.setCursor(6, soff + (line)*8);
        //		print(String("                   "),false);
        print(aPara->name, false, __LINE__);

        display.setCursor(offs - 6 * oval.length(), 8 * (line) + soff);
        print(oval, true, __LINE__);
        display.display();
    }
    void displayMenu(int rot_pos)
    {
        //       printA4(Paras[myID]->name);
        display.clearDisplay();
        rot_pos %= numsub + 1;
        display.setTextSize(1);              // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.cp437(true);                 // Use full 256 char 'Code Page 437' font
        display.setCursor(0, 0);
    //    if (rot_pos == 0)
    //        display.setTextColor(0, 1);
        display.write(17);
        //		if (debug == 1)			Serial.println("myID " + String(myID));
        //		if (debug == 1)			Serial.println(Paras[myID]->name);

#if 1
        byte l = Paras[myID]->name.length();
        byte ml = 21;
        byte pad = (ml - l) / 2;
        byte soff = 0;

        if (Paras[myID]->name == "ADSR")
            pad += 3;
        display.setCursor(pad * 6, 2);
        print(Paras[myID]->name, false, __LINE__);
        //		display.writeFastHLine(0, 7, 128, 1);
        display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
        if (rot_pos == 0)
            subPara = Paras[myID];
        {
            if (Paras[myID]->name == "ADSR")
                soff = 24;
            else if (numsub < 7)
                soff = 5;
            for (int s = 0; s < numsub; s++)
            {
                display.setCursor(6, 8 * (s + 1) + soff);
                Parameter *mPara = Paras[subitems[s]];
                String pn = mPara->name;
  //              FDBG("para " + SP(mPara) + " " + pn);
                //                print(mPara->name, s + 1 == rot_pos, __LINE__);
                print(pn, s + 1 == rot_pos, __LINE__);
                if (s + 1 == rot_pos)
                    subPara = mPara;
                if (mPara->submenu || mPara->name.indexOf("Open") > -1)
                {
                    display.setCursor(120, 8 * (s + 1) + soff);
                    display.write(16);
                }
                else if (mPara->parafunction)
                {
                    display.setCursor(120, 8 * (s + 1) + soff);
                    display.write(15);
                }
                else
                {
                    String oval = "";
                    oval = mPara->format();
                    display.setCursor(offs - 6 * oval.length(), 8 * (s + 1) + soff);
                    print(oval, s + 1 == rot_pos, __LINE__);
                }
            }
            if (Paras[myID]->name == "ADSR")
            {
                Parameter *APara = Paras[subitems[0]];
                Parameter *DPara = Paras[subitems[1]];
                Parameter *SPara = Paras[subitems[2]];
                Parameter *RPara = Paras[subitems[3]];
                float a = APara->fvalue;
                float d = DPara->fvalue;
                float s = SPara->fvalue;
                float r = RPara->fvalue;
                float tl = (64 - 16) / (a + d + r);
                //			Serial.println("tl=" + String(tl));
                //			Serial.println("A=" + String(a) + " D=" + String(d) + " S=" + String(s) + " R=" + String(r));
                //			Serial.println("A=" + String(a*tl) + " D=" + String(d * tl) + " S=" + String(s) + " R=" + String(r));
                display.writeLine(0, 31, int(a * tl), 31 - 24, 1);
                display.writeLine(int(a * tl), 31 - 24, int(a * tl) + int(d * tl), 31 - s * 24, 1);
                display.writeLine(int(a * tl) + int(d * tl), 31 - s * 24, int(a * tl) + int(d * tl) + 16, 31 - s * 24, 1);
                display.writeLine(int(a * tl) + int(d * tl) + 16, 31 - s * 24, int(a * tl) + int(d * tl) + 16 + r * tl, 31, 1);
            }
            else if (hasShape)
            {
                drawShape();
            }
#endif
            if (comment.length() > 0)
            {
                display.setCursor(8, 8 * 7);
                print(comment, false, __LINE__);
            }
        }
        display.display();
    }
    int orp = -1;
    String outmen;
    void showMenu(int rot_pos)
    {
        //       printA4(Paras[myID]->name);
        rot_pos %= numsub + 1;
        static int xpos[7] = {350, 550, 600, 850, 1050, 210, 260};
        static int ypos[7] = {440, 260, 440, 440, 630, 110, 260};
        if (Paras[myID]->name != "Modules" || orp == rot_pos)
            return;
        orp = rot_pos;
        //       FDBG("numsub");
        //       FDBG(numsub);
        for (int s = 0; s < numsub; s++)
        {
            Parameter *mPara = Paras[subitems[s]];
            String item = mPara->name;
            if (mPara->range == 0 && item != "Presets")
            {
                menuId[s] = webgui.addButtons(item, &onButtonRelease, xpos[s], ypos[s], "f", "synth");
                if (s == 1)
                    menuId[numsub] = webgui.addButtons(item, &onButtonRelease, xpos[s] + 250, ypos[s], "f", "synth");
                //               FDBG("showmenu " + item + " " + SN(s));
            }
        }
        menuId[8] = webgui.addButtons("LFO", &onButtonRelease, 300, 630, "f", "synth");
        menuId[9] = webgui.addButtons("LFO", &onButtonRelease, 550, 630, "f", "synth");
        menuId[10] = webgui.addButtons("LFO", &onButtonRelease, 800, 630, "f", "synth");
        menuId[11] = webgui.addButtons("OUT", &onButtonRelease, 1100, 440, "f", "synth");
    }

    byte bot = 63;
    byte top = 63 - 24;
    byte left = 8;
    byte right = 64 + 8;
    byte mid = 32 + 8;
    byte q1 = 16 + 8;
    byte q3 = 48 + 8;
    float f = 44.0 / 7 * 64;
    String comment = "";
    void drawShape(void)
    {
        byte shape = (byte)Paras[subitems[shapeix]]->fvalue;
        //		Serial.println("Shape " + String(shape));
        switch (shape)
        {
        case WAVEFORM_SINE:
            for (int i = left; i < right; i++)
                display.drawPixel(i, 52 - 12 * sin((i - left) * f), 1);
            break;
        case WAVEFORM_SAWTOOTH:
        case 9:
            display.writeLine(left, bot, mid, top, 1);
            display.writeLine(mid, top, mid, bot, 1);
            display.writeLine(mid, bot, right, top, 1);
            display.writeLine(right, top, right, bot, 1);
            break;
        case WAVEFORM_SQUARE:
        case 11:
            display.writeLine(left, bot, left, top, 1);
            display.writeLine(left, top, mid, top, 1);
            display.writeLine(mid, top, mid, bot, 1);
            display.writeLine(mid, bot, right, bot, 1);
            display.writeLine(right, bot, right, top, 1);
            break;
        case WAVEFORM_TRIANGLE:
            display.writeLine(left, bot, q1, top, 1);
            display.writeLine(q1, top, mid, bot, 1);
            display.writeLine(mid, bot, q3, top, 1);
            display.writeLine(q3, top, right, bot, 1);
            break;
        case WAVEFORM_ARBITRARY:

            for (int i = left; i < right; i++)
                display.drawPixel(i, 52 - 3 * myWaveform[(i - left) * 4] / 8192, 1);
            break;
        case WAVEFORM_PULSE:
        case 12:
            display.writeLine(left, bot, left, top, 1);
            display.writeLine(left, top, q1, top, 1);
            display.writeLine(q1, top, q1, bot, 1);
            display.writeLine(q1, bot, right, bot, 1);
            display.writeLine(right, bot, right, top, 1);
            break;
        case WAVEFORM_SAWTOOTH_REVERSE:
        case 10:
            display.writeLine(left, bot, left, top, 1);
            display.writeLine(left, top, mid, bot, 1);
            display.writeLine(mid, bot, mid, top, 1);
            display.writeLine(mid, top, right, bot, 1);
            break;
        case WAVEFORM_TRIANGLE_VARIABLE:
            display.writeLine(left, bot, q1 / 2, top, 1);
            display.writeLine(q1 / 2, top, mid, bot, 1);
            display.writeLine(mid, bot, q3 - q1 / 2, top, 1);
            display.writeLine(q3 - q1 / 2, top, right, bot, 1);
            break;
        case WAVEFORM_SAMPLE_HOLD:
            byte y = 63 - random(24);
            byte y1 = 63 - random(24);
            for (int i = left; i < right; i += 4)
            {
                display.drawLine(i, y, i + 4, y, 1);
                display.drawLine(i + 4, y, i + 4, y1, 1);
                y = y1;
                y1 = 63 - random(24);
            }
            break;
        }
    }
    Parameter **Paras;
    Parameter *subPara = 0;
    byte myID;
    byte *subitems;
    byte numsub = 0;
    bool hasShape = false;
    byte shapeix = 0;
    byte rot_pos = 0;
};
extern float32_t dbBand1[];
