#pragma once
class Parameter
{
public:
    Parameter(String pname, signed char *pvalue, signed char pstart, signed char pend, const String *pform = nullptr, int pnum = -1, int size = 0)
    {
        value = pvalue;
        vstart = pstart;
        vend = pend;
        name = pname;
        isFloat = false;
        range = pend - pstart;
        form = pform;
        paranum = pnum;
        isAnalog = false;
        //       DBG(__LINE__);
        if (range > 1 && form == 0)
            isAnalog = true;
         formsize = size;
    }
    Parameter(String pname, byte *pvalue, byte pstart, byte pend, const String *pform = nullptr, int pnum = -1, int size = 0)
    {
        value = (signed char *)pvalue;
        vstart = pstart;
        vend = pend;
        name = pname;
        isFloat = false;
        range = pend - pstart;
        form = pform;
        paranum = pnum;
        isByte = true;
        if (range > 1 && form == 0)
            isAnalog = true;
        formsize = size;
    }
    Parameter(String pname, synPara *sP)
    {
        value = 0;
        vstart = sP->startvalue;
        fvstart = sP->startvalue;
        dim = sP->dimension;
        vend = sP->endvalue;
        fvend = sP->endvalue;
        name = pname;
        isFloat = sP->isFloat;
        range = sP->endvalue - sP->startvalue;
        form = 0;
        paranum = sP->para;
        isAnalog = isFloat;
        formsize = 0;
    }
    Parameter(String pname, float pend, int pnum, String pdim, bool pisFloat)
    {
        fvstart = 0;
        fvend = pend;
        name = pname;
        isFloat = pisFloat;
        range = pend;
        dim = pdim;
        paranum = pnum;
        fvalue = 0;
        isAnalog = isFloat;
        formsize = 0;

        //     if(debug==1)Serial.println(name + " " + String(fvalue) + " " + String(fvend) + " " + String(isFloat ? "f" : "i"));
    }
    Parameter(String pname, float *pvalue, float pend)
    {
        fvstart = 0;
        fvend = pend;
        name = pname;
        isFloat = true;
        range = pend;
        dim = "";
        paranum = 0;
        tvalue = pvalue;
        fvalue = 0;
        isAnalog = true;
        //     if(debug==1)Serial.println(name + " " + String(fvalue) + " " + String(fvend) + " " + String(isFloat ? "f" : "i"));
    }

    Parameter(const String pname, byte pparent, byte psub = 0, CALLBACK pfunction = 0)
    {
        value = 0;
        vstart = 0;
        vend = 0;
        name = pname;
        parent = pparent;
        range = 0;
        isFloat = false;
        if (pfunction == 0)
            submenu = psub;
        else
            vstart = psub;
        parafunction = pfunction;
        isAnalog = false;
        //       if(debug==1)Serial.println(name + " " + String((int)parafunction));
    }
    void setRange(float vs, float ve)
    {
        fvend = ve;
        fvstart = vs;
        range = fvend - fvstart;
    }
    void setForm(const String *pform = nullptr, const short *pindirect = nullptr, int size = 0)
    {
        form = pform;
        indirect = pindirect;
        isAnalog = false;
        formsize = size;
    }
    float getValue()
    {
        if (fvalue < fvstart)
            return fvstart;
        if (fvalue > fvend)
            return fvend;
        return fvalue;
    }
    int checkVal(void)
    {
        int ival = (int)fvalue;
        int lim = fvend + 1;
        if (!isFloat && value != nullptr)
        {
            //           if (*value < vstart || *value > vend)
            //               *value = 0;
            ival = *value;
            lim = vend;
        }
        if (form != nullptr && ival > 0)
        {
            //           if (*value < vstart || *value > vend)
            //               *value = 0;
            if (ival > formsize || lim > formsize)
                FDBG("wrong size " + name + " " + SN(lim) + " " + SN(formsize));
        }
        if (isByte && value != nullptr)
        {
            //           if (*value < vstart || *value > vend)
            //               *value = 0;
            ival = (byte)*value;
        }
        //        if(form!=nullptr)
        //            FDBG(name + " " + SN(lim) + " " + SN(ival));
        if (isFloat || value == nullptr)
        {
            if (fvalue < fvstart)
                fvalue = fvstart;
            if (fvalue > fvend)
                fvalue = fvend;
            if (isnan(fvalue))
                fvalue = 0;
        }
        else if (value != nullptr)
        {
            if (*value > vend)
                *value = vend;
            if (*value < vstart)
                *value = vstart;
        }
        else
        {
            //           FDBG(name + " error "+SP(value)+" "+SP(form));
            return -1;
        }
        return 0;
    }
    int mv = 0;
    int formsize;
    int setLength(void)
    {
        //       DBG(name);
        int maxvall = 0;
        if (form == nullptr)
        {
            if (isFloat)
                maxvall = String(fvend).length();
            else
                maxvall = String(vend).length();
        }
        else
        {
            for (int i = 0; i < vend; i++)
            {
                int lp = form[i].length();
                if (lp > maxvall)
                    maxvall = lp;
            }
        }
        mv = maxvall;
        return maxvall;
    }
    String dumpValue()
    {
        return SP(value) + " " + SN(vstart) + " " + SN(vend) + " " + SN(fvalue) + " " + SN(sc) + " " + SP(tvalue) + " " + SP(form) + " " + SN(fvstart) + " " + SN(fvend) + " " + SB(isAnalog) + " " + name + " " + SN(parent) + " " + SB(isFloat) + " " + SB(isByte) + " " + SN(formsize) + " " + SN(range) + " " + dim;
    }
    String format()
    {
        if (range == 0)
            return "";
        //       FDBG("check "+SN(checkVal()));
        checkVal();

        char buff[20];
        static String out;
        //       if (checkVal() == -1)
        //           return "";
        int ival = (int)fvalue;
        if (!isFloat && value != nullptr)
        {
            //           if (*value < vstart || *value > vend)
            //               *value = 0;
            ival = *value;
        }
        if (isByte && value != nullptr)
        {
            //           if (*value < vstart || *value > vend)
            //               *value = 0;
            ival = (byte)*value;
        }
        // Serial.println(name + " format " + String(fvalue) + " " + String(ival) + " " + SP(form) + " " + SP(indirect));
        out = String(ival);
 //       if (name == "Portam. Type" || name == "Poly")
 //           return out;
        if (!isFloat)
        {
            //				if(debug==1)Serial.println(name + " " + String((int)fvalue) + " " + String((int)form));
            if (form != nullptr)
            {
                if (indirect == nullptr)
                {
                    //                   Serial.println(name + " format " + String(fvalue) + " " + String(ival) + " " + SP(form) + "  " + form[0]);
                    out = "  " + form[ival];
                    return out;
                }
                else
                {
                    out = "  " + form[indirect[ival]];
                    return out;
                }
            }
            sprintf(buff, "%3d", ival);
            out = buff;
            //            Serial.println("formatted value: now " + name + " " + String((int)ival) + " ->" + out);
            return out;
        }
        else
        {
            //           if (dim == "Hz")
            //              DBG(name + " *************************** " + dumpValue());
            if (tvalue != nullptr)
            {
                //              Serial.println(String(*tvalue )+ " " + String(fvstart) + " " + String(fvend));
                //               if (*tvalue < fvstart || *tvalue > fvend)
                //                    *tvalue = 0;
                //                Serial.println(String(*tvalue )+ " " + String(fvstart) + " " + String(fvend));
                fvalue = *tvalue;
            }
            if (fvalue >= 0)
            {
                if (debug == 1)
                    Serial.println(name + " " + String(fvalue) + " " + dim + " " + SN(range) + " " + SP(tvalue));
                if (form != nullptr && indirect == nullptr)
                {
                    out = "  " + form[(int)fvalue];
                    return out.replace(".wav", "");
                }
                if (form != nullptr && indirect != nullptr)
                {
                    out = "  " + form[indirect[(int)fvalue]];
                    return out;
                }
                if (dim == "dB")
                {
                    float fv = 20 * log10f(fvalue);
                    return (String(fv) + " " + dim);
                }
                if (dim == "ms" && range == 1)
                {
                    float fv = aRPara->fvalue * fvalue;
                    return (String(fv, 0) + " " + dim);
                }
            }
            if (range < 100)
                sprintf(buff, " %6.2f", fvalue);
            else
                sprintf(buff, " %6d", (int)fvalue);
            out = String(buff) + " " + dim;
            //           Serial.println("formatted value: " + name + " " + String((int)fvalue) + " " + out);
        }
        return out;
    }
    void setSynthVal(String caller)
    {
        static float oldsa = 0;
        if (isnan(fvalue))
            fvalue = 0;
        float range = aRPara->fvalue;
        //       Serial.println(name + " " + String(paranum) + " " + String(fvalue)+" "+SN(range)+" "+caller);

        AudioNoInterrupts();
        if (debug == 1)
            Serial.println(name + " no interrupt " + String(paranum) + " " + String(fvalue));
        for (int v = 0; v < NUMVOICES; v++)
        {
            switch (paranum)
            {
            case NO_AUDIO:
                break;
            case VCO_SHAPE:
                vcos1[v]->begin((int)fvalue);
                //                vcos2[v]->begin((int)fvalue);
                //                vcos3[v]->begin((int)fvalue);
                break;
            case VCO_FREQ:
            case VCO_RANGE:
            case VCO_DETUNE:
                vcos1[v]->setDetune(fvalue);
                //			if(debug==1)Serial.println("what " + String(what) + "=" + String(vcof * pow(2, vcor)));
                //				vcos1[v]->frequency(vcof * pow(2, vcor));
                //				vcos2[v]->frequency(vcof * pow(2, vcor) + vcod);
                break;
            case VCO_AMP:
                vcos1[v]->amplitude(fvalue / 25, 0);
                //               vcos2[v]->amplitude(fvalue);
                //               vcos3[v]->amplitude(fvalue);
                break;
            case VCF_FREQ:
                filters[v]->frequency(fvalue);
                if (v == 0)
                    //    				if(debug==1)Serial.println(name+" set audio " + String(fvalue));
                    break;
            case VCF_Q:
                filters[v]->resonance(fvalue);
                break;
            case VCA_ATTACK:
                vcaenv[v]->attack(fvalue * range);
                vcaenv[v]->hold(0);
                break;
            case VCA_DECAY:
                vcaenv[v]->decay(fvalue * range);
                break;
            case VCA_SUSTAIN:
                vcaenv[v]->sustain(fvalue);
                //                vcaenv[v]->sustain(0);
                break;
            case VCA_RELEASE:
                vcaenv[v]->release(fvalue * range);
                break;
            case VCO_MIX:
                vcomix[v]->gain(0, 1);
                vcomix[v]->gain(1, fvalue / 50);
                vcos1[v]->amplitude(fvalue / 50, 1);
                break;
            case VCO_SUB_MIX:
                if (debug == 1)
                    Serial.println(name + " " + String(paranum) + " " + String(fvalue));
                vcomix[v]->gain(2, fvalue / 50);
                vcos1[v]->amplitude(fvalue / 50, 2);
                break;
            case VCF_MODE:
                //                filters[v]->moog(fvalue == 4);
                for (int i = 0; i < 4; i++)
                    vcfout[v]->gain(i, 0);
                if (fvalue == 4)
                {
                    vcfout[v]->gain((int)1, 1);
                }
                else
                    vcfout[v]->gain((int)fvalue, 1);
                break;
            case VCF_ATTACK:
                vcfenv[v]->attack(fvalue * range);
                vcfenv[v]->hold(0);
                break;
            case VCF_DECAY:
                vcfenv[v]->decay(fvalue * range);
                break;
            case VCF_SUSTAIN:
                vcfenv[v]->sustain(fvalue);
                break;
            case VCF_RELEASE:
                vcfenv[v]->release(fvalue * range);
                break;
            case VCO_FMRANGE:
                vcos1[v]->frequencyModulation(fvalue);
                //               vcos2[v]->frequencyModulation(fvalue);
                break;
            case PORTAMENTOTIME:
                vcos1[v]->setPortamento(fvalue);
                break;
            case PORTAMENTOTYPE:
 //               Serial.println(name + " set " + String(paranum) + " " + String(fvalue));
                vcos1[v]->setTau(fvalue);
                break;
            }
        }
        switch (paranum)
        {

        case POLYMODE:
            poly = fvalue;
            //               vcos2[v]->frequencyModulation(fvalue);
            break;
        case AM_RATE:
            sine16.frequency(fvalue);
            break;
        case AM_DEPTH:
            sine16.amplitude(fvalue);
            break;
        case AM_SHAPE:
            sine16.begin((int)fvalue);
            break;
        case FM_RATE:
            sine2.frequency(fvalue);
            break;
        case FM_DEPTH:
            sine2.amplitude(fvalue);
            break;
        case FM_SHAPE:
            sine2.begin((int)fvalue);
            break;
        case VCF_RATE:
            sine21.frequency(fvalue);
            break;
        case VCF_DEPTH:
            sine21.amplitude(fvalue);
            break;
        case VCF_SHAPE:
            sine21.begin((int)fvalue);
            break;
        case PM_RATE:
            sine1.frequency(fvalue);
            break;
        case PM_DEPTH:
            sine1.amplitude(fvalue);
            break;
        case PM_SHAPE:
            sine1.begin((int)fvalue);
            break;
        case SAMPLE_AMP:
            dc1.amplitude(fvalue);
            dc2.amplitude(fvalue);
            oldsa = fvalue;
            break;
        case SAMPLE_TIME:
            dc1.amplitude(oldsa, fvalue);
            dc2.amplitude(oldsa, fvalue);
            break;
        case DIRECT:
            mixer1.gain(0, 1 - fvalue);
            mixer1.gain(1, 1 - fvalue);
            mixer1.gain(2, fvalue);
            mixer1.gain(3, fvalue);
            break;
        case VCA_BP:
            mixer3.gain(2, fvalue);
            break;
        case TRIGGER_1:
            //           if(debug==1)
            //           Serial.println(wavenames[s1index]);
            s1index = (int)fvalue;
            //            playFile(wavenames[s1index], false);
            break;
        case TRIGGER_2:
            //           if(debug==1)
            //            Serial.println(wavenames[s2index]);
            //            playFile(wavenames[s2index], true);
            s2index = (int)fvalue;
            //           playFile(wavenames[s2index], true);
            break;
        case VCF_LO:
            if (aHIPara != nullptr)
            {
                aHIPara->setRange(fvalue, fvalue + 3000);
                aFPara->setRange(fvalue, aHIPara->fvalue);
            }
            break;
        case VCF_HI:
            if (aHIPara != nullptr)
            {
                aFPara->setRange(aLOPara->fvalue, fvalue);
            }
            break;

        case NOISE:
            noise1.amplitude(fvalue / 50);
        }
        AudioInterrupts();
        // 			Serial.println(name + " interrupt " + String(paranum) + " " + String(fvalue));
    }
    signed char *value = nullptr;
    short vstart;
    short vend;
    float fvalue = 0;
    short sc;
    float *tvalue = nullptr;
    float fvstart;
    float fvend;
    bool isAnalog;
    String name;
    byte parent = 0;
    bool isByte = false;
    bool isFloat = false;
    float range;
    int monid;
    String dim;
    const String *form = nullptr;
    const short *indirect = nullptr;
    int paranum = -1;
    static Parameter *aHIPara;
    static Parameter *aLOPara;
    static Parameter *aFPara;
    static Parameter *aDPara;
    static Parameter *aRPara;
    int source = 0;
    byte submenu = 0;
    CALLBACK parafunction = 0;
};
