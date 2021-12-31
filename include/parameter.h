#pragma once
class Parameter
{
public:
    Parameter(String pname, signed char *pvalue, signed char pstart, signed char pend, const String *pform = nullptr, int pnum = -1)
    {
        value = pvalue;
        vstart = pstart;
        vend = pend;
        name = pname;
        isFloat = false;
        range = pend - pstart;
        form = pform;
        paranum = pnum;
    }
    Parameter(String pname, float pend, int pnum, String pdim, bool pisFloat, bool pnoForm = false)
    {
        fvstart = 0;
        fvend = pend;
        name = pname;
        isFloat = pisFloat;
        range = pend;
        dim = pdim;
        paranum = pnum;
        fvalue = 0;
        noForm = false;
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
        //       if(debug==1)Serial.println(name + " " + String((int)parafunction));
    }
    void setRange(float vs, float ve)
    {
        fvend = ve;
        fvstart = vs;
        range = fvend - fvstart;
    }
    void setForm(const String *pform = nullptr)
    {
        form = pform;
    }

    String format()
    {
        if (range == 0 || noForm)
            return "";
        char buff[20];
        String out;
        if (!isFloat && value != nullptr)
        {
            int ival = *value;
            out = String(ival);
            if (ival >= 0)
            {
                if (form != nullptr)
                {
                    out = "  " + form[ival];
                }
            }
            else
                sprintf(buff, "%3d", ival);
        }
        else
        {
            if (tvalue != nullptr)
                fvalue = *tvalue;

            if (fvalue > fvend)
                fvalue = 0;
            if (fvalue >= 0)
            {
                //				if(debug==1)Serial.println(name + " " + String((int)fvalue) + " " + String((int)form));
                if (form != nullptr)
                {
                    out = "  " + form[(int)fvalue];
                    return out;
                }
                if (dim == "dB")
                {
                    float fv = 20 * log10f(fvalue);
                    return (String(fv) + dim);
                }
            }
            if (range < 100)
                sprintf(buff, " %6.2f", fvalue);
            else
                sprintf(buff, " %6d", (int)fvalue);
            out = String(buff) + dim;
        }
        return out;
    }
    void setSynthVal(void)
    {
        static float oldsa = 0;
        AudioNoInterrupts();
        //        if (debug == 1)            Serial.println(name + " " + String(paranum) + " " + String(fvalue));
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
                vcos1[v]->amplitude(fvalue, 0);
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
                vcaenv[v]->attack(fvalue);
                vcaenv[v]->hold(0);
                break;
            case VCA_DECAY:
                vcaenv[v]->decay(fvalue);
                break;
            case VCA_SUSTAIN:
                vcaenv[v]->sustain(fvalue);
                //                vcaenv[v]->sustain(0);
                break;
            case VCA_RELEASE:
                vcaenv[v]->release(fvalue);
                break;
            case VCO_MIX:
                vcomix[v]->gain(0, 1);
                vcomix[v]->gain(1, fvalue);
                vcos1[v]->amplitude(fvalue, 1);
                break;
            case VCO_SUB_MIX:
                if (debug == 1)
                    Serial.println(name + " " + String(paranum) + " " + String(fvalue));
                vcomix[v]->gain(2, fvalue);
                vcos1[v]->amplitude(fvalue, 2);
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
                vcfenv[v]->attack(fvalue);
                vcfenv[v]->hold(0);
                break;
            case VCF_DECAY:
                vcfenv[v]->decay(fvalue);
                break;
            case VCF_SUSTAIN:
                vcfenv[v]->sustain(fvalue);
                break;
            case VCF_RELEASE:
                vcfenv[v]->release(fvalue);
                break;
            case VCO_FMRANGE:
                vcos1[v]->frequencyModulation(fvalue);
                //               vcos2[v]->frequencyModulation(fvalue);
                break;
            case PORTAMENTOTIME:
                vcos1[v]->setPortamento(fvalue);
                break;
            case PORTAMENTOTYPE:
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
        case PW_RATE:
            sine1.frequency(fvalue);
            break;
        case PW_DEPTH:
            sine1.amplitude(fvalue);
            break;
        case PW_SHAPE:
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
        case VCA:
            mixer3.gain(2, 1 - fvalue);
            break;
        case TRIGGER_1:
            //           if(debug==1)
            Serial.println(wavenames[s1index]);
            playFile(wavenames[s1index], false);
            break;
        case TRIGGER_2:
            //           if(debug==1)
            Serial.println(wavenames[s2index]);
            playFile(wavenames[s2index], true);
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
            noise1.amplitude(fvalue);
        }
        AudioInterrupts();
    }
    signed char *value = nullptr;
    signed char vstart;
    signed char vend;
    float fvalue = 0;
    float *tvalue = nullptr;
    float fvstart;
    float fvend;
    String name;
    byte parent = 0;
    bool isFloat = false;
    bool noForm = false;
    float range;
    String dim;
    const String *form = nullptr;
    int paranum = -1;
    static Parameter *aHIPara;
    static Parameter *aLOPara;
    static Parameter *aFPara;
    static Parameter *aDPara;

    byte submenu = 0;
    CALLBACK parafunction = 0;
};
