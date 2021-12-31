#pragma once
signed char readrec(File frec);
float floatrec(File frec);
class MenuSet : public MenuPara
{
public:
	MenuSet(void) : MenuPara(SETTINGS, "Setting", 0)
	{
		Paras[nump++] = new Parameter("ByPass", &byPass, 0, 3);
		Paras[nump++] = new Parameter("HiRes Enc.", &hiRes, 0, 1, selected);
		Paras[nump++] = new Parameter("Ratchet", &isRat, 0, 1, selected);
		Paras[nump++] = new Parameter("Use BPM", &useBPM, 0, 1, selected);
		Paras[nump++] = new Parameter("Division", &ratdiv, 2, 16);
		Paras[nump++] = new Parameter("Trigger CC", &triggerCC, 0,16);
		Paras[nump++] = new Parameter("BPM", &BPM, 250.0);
		Paras[nump++] = new Parameter("Send AllNoteOff", &noPanic, 0, 1, selected);
		Paras[nump++] = new Parameter("After Touch", &sendAfter, 0, 1, selected);
		Paras[nump++] = new Parameter("Contrast", &contrast, 0, 3);
		Paras[nump++] = new Parameter("Strum", &strum, 0, 50);
		Paras[nump++] = new Parameter("Semitone", &semiTone, -12, 12);
		Paras[nump++] = new Parameter("Octave", &octave, -5, 5);
		Paras[nump++] = new Parameter("Show ID in Map", &mapID, 0, 1, selected);
		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//        if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
		items[nump++] = "Back";
		numitems = nump;
	}
	void save(File frec)
	{
		frec.write(contrast);
		frec.write(triggerCC);
		frec.write(byPass);
		frec.write(sendAfter);
		frec.write(noPanic);
		frec.write(strum);
		frec.write(hiRes);
		frec.write(isRat);
		frec.write(useBPM);
		frec.write(ratdiv);
		frec.write((char *)&BPM, 4);
	
	}
	virtual void restore(File frec) override
	{
		contrast = readrec(frec);
		triggerCC = readrec(frec);
		byPass = readrec(frec);
		//		if(debug==1)Serial.println("ByPass:" + String(byPass));
		sendAfter = readrec(frec);
		noPanic = readrec(frec);
		strum = readrec(frec);

		hiRes= readrec(frec);
		isRat= readrec(frec);
		useBPM= readrec(frec);
		ratdiv= readrec(frec);
		BPM=floatrec(frec);
		Serial.println("BPM: "+String(BPM));
	//	printA4(String(BPM));
		setContrast();
	}
};