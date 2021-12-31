void print(String out, bool inv = false)
{
    const char *outstr = out.c_str();
    if (inv)
        display.setTextColor(0, 1);
    else
        display.setTextColor(1, 0);
    for (int16_t i = 0; i < (int)out.length(); i++)
    {
        char c = *outstr++;
        display.write(c);
    }
}
#include "menu.h"
void saveAll(File frec)
{

    frec.write(lastMap);
    frec.write(lastScale);
    frec.write(octave);
    frec.write(semiTone);
    frec.write(s1index);
    frec.write(s2index);
    frec.write(preindex);
    frec.write(actPre);
    frec.write(recentPre);
    /*
	frec.println(filenames[lastMap]);
	frec.println(scfilenames[lastScale]);
	frec.println(wavenames[s1index]);
	frec.println(wavenames[s2index]);
	frec.println(presetnames[preindex]);
	frec.println(geqnames[geqindex]);
	*/
    MenuSet *mes = (MenuSet *)Menus[SETTINGS];
    mes->save(frec);
    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
    mPara->save(frec);
    String geqnames[100];
    for (int e = 0; e < 8; e++)
    {
        MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
        mes->save(frec);
    }
#if 1
    for (int e = 0; e < 3; e++)
    {
        MenuMidiSet *mes = (MenuMidiSet *)Menus[MIDISETTINGS + e];
        mes->save(frec);
    }

    for (int e = 0; e < 6; e++)
    {
        MenuZoneSet *mes = (MenuZoneSet *)Menus[ZONESETTINGS + e];
        mes->save(frec);
    }
#endif
    return;
}

signed char readrec(File frec)
{
    signed char res = 0;
    if (frec.available())
        res = frec.read();
    return (res);
}
float floatrec(File frec)
{
    float res = 0;
    if (frec.available())
        frec.read(&res, 4);
    if (res == 4)
        res = 0;
    return (res);
}
void Restore(File frec)
{
    lastMap = readrec(frec);
    lastScale = readrec(frec);
    octave = readrec(frec);
    semiTone = readrec(frec);
    s1index = readrec(frec);
    s2index = readrec(frec);
    preindex = readrec(frec);
    actPre = readrec(frec);
    recentPre = readrec(frec);

    //	Serial.println("Restored oct " + String(octave) + " semi " + String(semiTone));
    MenuSet *mes = (MenuSet *)Menus[SETTINGS];
    mes->restore(frec);
    MenuSynth *mPara = (MenuSynth *)(Menus[SYNTHSETTINGS]);
    Serial.println("synth restore");
    mPara->restore(frec);

    for (int e = 0; e < 8; e++)
    {
        MenuExtSet *mes = (MenuExtSet *)Menus[EXTSETTINGS + e];
        mes->restore(frec);
    }
#if 1
    for (int e = 0; e < 3; e++)
    {
        MenuMidiSet *mes = (MenuMidiSet *)Menus[MIDISETTINGS + e];
        mes->restore(frec);
    }

    for (int e = 0; e < 6; e++)
    {
        MenuZoneSet *mes = (MenuZoneSet *)Menus[ZONESETTINGS + e];
        mes->restore(frec);
    }
#endif
    return;
}
void saveTMS(void)
{

    if (SD.exists("TMSIDEK.INI"))
    {
        SD.remove("TMSIDEK.INI");
        //    if(debug==1)Serial.println("ini deleted");
    }
    File frec = SD.open("TMSIDEK.INI", FILE_WRITE);
    if (frec)
    {
        if (debug == 2)
            Serial.println("ini created");
        saveAll(frec);
        frec.close();
        if (debug == 1)
            Serial.println("TBSidekick saved");
    }
}
float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int imap(int x, int in_min, int in_max, int out_min, int out_max)
{
    if (x < in_min)
        x = in_min;
    if (x > in_max)
        x = in_max;
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void printA4(String text)
{
    alpha4.clear();
    for (int i = 0; i < 4 && i < (int)text.length(); i++)
        alpha4.writeDigitAscii(i, text.c_str()[i]);
    alpha4.writeDisplay();
}