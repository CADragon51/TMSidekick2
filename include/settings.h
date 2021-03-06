#pragma once
signed char readrec(File frec);
float floatrec(File frec);
int intrec(File frec);
String beats[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
class MenuSet : public MenuPara
{
public:
	MenuSet(void) : MenuPara(SETTINGS, "Rhythm/Settings", 0)
	{
		Paras[nump++] = new Parameter("ProcMode", &procMode, 0, 3,pmode,-1,4);
//		Paras[nump++] = new Parameter("Hi-Res", &hiRes, 0, 1, selected,-1,2);
		Paras[nump++] = new Parameter("BPM", &BPM, 240);
		Paras[nump++] = new Parameter("Send AllNoteOff", &noPanic, 0, 1, selected, -1, 2);
		Paras[nump++] = new Parameter("Use Velocity", &useVel, 0, 1, selected, 2);
		Paras[nump++] = new Parameter("After Touch", &sendAfter, 0, 1, selected, -1, 2);
		//		Paras[nump++] = new Parameter("Contrast", &contrast, 0, 3);
		//		Paras[nump++] = new Parameter("Strum", &strum, 0, 50);
		Paras[nump++] = new Parameter("Semitone", &semiTone, -12, 12);
		Paras[nump++] = new Parameter("Octave", &octave, -5, 5);
		Paras[nump++] = new Parameter("Ratchet", &isRat, 0, 1, selected, -1, 2);
		Paras[nump++] = new Parameter("Use BPM", &useBPM, 0, 1, selected, -1, 2);
		Paras[nump++] = new Parameter("Division", &ratdiv, 1, 16);
		Paras[nump++] = new Parameter("Beatlength", &beatlength, 0, 7, beats, -1, 8);
		DBG(Paras[nump - 1]->name + " " + SN(Paras[nump - 1]->name ? "a" : "d"));
		Paras[nump++] = new Parameter("Trigger CC", &triggerCC, 0, 16);
		//		Paras[nump++] = new Parameter("Show ID in Map", &mapID, 0, 1, selected, -1, 2);
		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//        if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
		numitems = nump;
	}
	void save(File frec)
	{
		frec.write(contrast);
		frec.write(triggerCC);
		frec.write(procMode);
		frec.write(sendAfter);
		frec.write(noPanic);
		frec.write(strum);
		frec.write(useVel);
		frec.write(isRat);
		frec.write(useBPM);
		frec.write(ratdiv);
		numq = ratdiv;
		unsigned char bbpm = BPM;
		frec.write(bbpm);
	}
	virtual void restore(File frec) override
	{
		contrast = readrec(frec);
		triggerCC = readrec(frec);
		procMode = readrec(frec);
		//		if(debug==1)Serial.println("ByPass:" + String(byPass));
		sendAfter = readrec(frec);
		noPanic = readrec(frec);
		strum = readrec(frec);

		useVel = readrec(frec);
		isRat= readrec(frec);
		useBPM= readrec(frec);
		ratdiv= readrec(frec);
		unsigned char  bbpm = readrec(frec);
		BPM = bbpm;
//		Serial.println("BPM: " + String(BPM));
		//	printA4(String(BPM));
		//		setContrast();
	}
};