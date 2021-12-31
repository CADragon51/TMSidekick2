#pragma once
int imap(int x, int in_min, int in_max, int out_min, int out_max);
float fmap(float x, float in_min, float in_max, float out_min, float out_max);
extern AudioPlaySdWav playSdWav1;
extern AudioPlaySdWav playSdWav2;
int v = 0;
int v2note[NUMVOICES] = {-1, -1, -1, -1, -1, -1};
float lastfreq = 0;
MenuPara *MenuPara::SynthMenu;
void synthnoteOn(byte sourceNote,byte polye)
{
	for (v = 0; v < NUMVOICES && v < polye; v++)
	{
		if (v2note[v] == sourceNote)
		{
			vcaenv[v]->noteOff();
			vcfenv[v]->noteOff();
			v2note[v] = -1;
			vtmillis[v] = 0;
			if (debug == 1)
				Serial.printf("**** removed:@ %f %d v=%d %f****\n", note_frequency[sourceNote], sourceNote, v, Parameter::aDPara->fvalue);
			break;
		}
	}
	for (v = 0; v < NUMVOICES && v < polye; v++)
	{
		if (v2note[v] == -1 || poly == 0)
		{
			if (debug == 1)
				Serial.printf("**** NoteOn:@ %f %d v=%d %f %d %d****\n", note_frequency[sourceNote], sourceNote, v, lastfreq, onoff, vcaenv[v]->isActive());
			float freq = note_frequency[sourceNote];
			vtmillis[v] = 0;
			if (onoff == 0 || lastfreq == 0 || (!poly && !vcaenv[v]->isActive()))
			{
				vcos1[v]->frequency(freq);
				vcaenv[v]->noteOff();
				vcfenv[v]->noteOff();
				vcaenv[v]->noteOn();
				vcfenv[v]->noteOn();
				filters[v]->frequency(freq);
			}
			else
			{
				vcos1[v]->frequency(lastfreq, freq);
				filters[v]->frequency(freq);
				if (poly)
				{
					vcaenv[v]->noteOn();
					vcfenv[v]->noteOn();
				}
			}
			lastfreq = freq;
			v2note[v] = sourceNote;
			onoff++;
			oled.setCursor(110, 0);
			oled.clearToEOL();
			oled.print(onoff);
			break;
		}
	}
}
void synthnoteOff(byte sourceNote,byte polye)
{
	for (v = 0; v < NUMVOICES && v < polye; v++)
	{
		if (v2note[v] == sourceNote)
		{
			if (debug == 1)
				Serial.printf("**** NoteOff: v= %d****\n", v);
			vcaenv[v]->noteOff();
			vcfenv[v]->noteOff();
			onoff--;
			//						vcos1[v]->frequency(0);
			oled.setCursor(110, 0);
			oled.clearToEOL();
			oled.print(onoff);
			v2note[v] = -1;
			vtmillis[v] = 0;
			break;
		}
	}
}
void playFile(String filename, bool isExtsw)
{
	File myFile = SD.open(filename.c_str());
	if (myFile)
	{
		myFile.close();
		if (debug == 1)
			Serial.println(filename + " playing...");
		if (isExtsw)
			playSdWav1.play(filename.c_str());
		else
			playSdWav2.play(filename.c_str());
		//fclick1.setOn(true);
	}
}
class MenuTargetSet : public MenuPara
{
public:
	MenuTargetSet(int type, int Parent_ID) : MenuPara(TARGETS, "Target", Parent_ID)
	{
		parent = Parent_ID;
		//      if(debug==1)Serial.println(items[0] + " parent " + Menus[parent]->items[0] );
		Paras[nump++] = new Parameter("USB MIDI", &UMO, 0, 1, selected);
		Paras[nump++] = new Parameter("MIDI 1", &MO1, 0, 1, selected);
		Paras[nump++] = new Parameter("MIDI 2", &MO2, 0, 1, selected);
		Paras[nump++] = new Parameter("Audio", &SynthPara, 0, POLYMODE, synthParas);
		Paras[nump++] = new Parameter("Synth", &SYNTH, 0, 1, selected);
		Paras[nump++] = new Parameter("CV 1", &CV1, 0, 1, selected);
		Paras[nump++] = new Parameter("CV 2", &CV2, 0, 1, selected);
		Paras[nump++] = new Parameter("Gate", &Gate, 0, 1, selected);
		Paras[nump++] = new Parameter("Strum", &Strum, 0, 1, selected);
		Paras[nump++] = new Parameter("Divide", &ratdiv, 2, 16);
		Paras[nump++] = new Parameter("BPM", &BPM, 240);
		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//        if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
		items[nump++] = "Back";
		numitems = nump;
		//     if(debug==1)Serial.println(String(nump - 1) + " " + items[nump - 1] + "/" + String(nump));
		me = type;
	}

	void save(File frec)
	{
		frec.write(UMO);
		frec.write(MO1);
		frec.write(MO2);
		frec.write(CV1);
		frec.write(CV2);
		frec.write(Gate);
		frec.write(SynthPara);
		frec.write(Strum);
		frec.write(SYNTH);

		if (SynthPara > 0)
		{
			if (AudioTG[SynthPara] != 0 && AudioTG[SynthPara] != me)
			{
				MenuPara *mp = (MenuPara *)Menus[AudioTG[SynthPara]];
				mp->SynthPara = 0;
			}
			AudioTG[SynthPara] = me;
		}
		if (CV1 > 0)
		{
			if (CV1TG != 0 && CV1TG != me)
			{
				MenuPara *mp = (MenuPara *)Menus[CV1TG];
				mp->CV1 = 0;
			}
			CV1TG = me;
		}
		if (CV2 > 0)
		{
			if (debug == 1)
				Serial.println("Set CV2 " + String(CV2TG));
			if (CV2TG != 0 && CV2TG != me)
			{
				MenuPara *mp = (MenuPara *)Menus[CV2TG];
				mp->CV2 = 0;
			}
			CV2TG = me;
		}
		if (Strum > 0)
		{
			if (StrumTG != 0 && StrumTG != me)
			{
				MenuPara *mp = (MenuPara *)Menus[StrumTG];
				mp->Strum = 0;
			}
			StrumTG = me;
		}
	}
	void restore(File frec) override
	{

		UMO = readrec(frec);
		MO1 = readrec(frec);
		MO2 = readrec(frec);

		CV1 = readrec(frec);
		CV2 = readrec(frec);
		Gate = readrec(frec);
		SynthPara = readrec(frec);
		Strum = readrec(frec);

		SYNTH = readrec(frec);
	}
	int t = 0;
	void action(bool on, signed char eType, signed char sourceCH, signed char sourceNote, signed char sourceVelocity, signed char sourceCC, int rawPitch, int rawVelocity, float mapvalue = -1)
	{
		if (eType == 4)
			return;
		byte polye = 1;

		if (poly)
			polye = NUMVOICES;
		if (eType == 7 && sourceNote != 36)
		{
			if (debug == 1)
				Serial.println("Note:" + String(sourceNote) + " event: " + String(rawPitch) + " Audio ? " + String(rawVelocity) + " val=" + String(rawVelocity));
		}
		//	byte channel = sourceCH;
		//	byte note = sourceNote;
		if (!Menus[SETTINGS]->byPass)
			sourceNote += octave * 12 + semiTone;
		myTimer.end();
		if (SynthPara > 0)
		{
			if (on || (SynthPara != TRIGGER_1 && SynthPara != TRIGGER_2))
			{
				SynthMenu->getPara(SynthPara)->setSynthVal();
				if (Menu::isshowing == SynthMenu->myID)
				{
					SynthMenu->show(true, 137);
				}
			}
		}
		if (on && CV1)
		{
		}
		byte ch = sourceCH;
		switch (eType)
		{
		case 6:
			if (SYNTH)
			{
				for (v = 0; v < NUMVOICES; v++)
				{
					if (debug == 1)
						Serial.printf("**** NoteOff: v= %d****\n", v);
					vcaenv[v]->noteOff();
					vcfenv[v]->noteOff();
					v2note[v] = -1;
				}
			}

		case 0:
			if(debug==1)Serial.printf("\n**** NoteOff: UMO==%d,MO1==%d,MO2==%d ****", UMO,MO1,MO2);
			if (UMO)
				usbMIDI.sendNoteOff(sourceNote, 0, ch, cable);
			if (MO1)
				MIDI1.sendNoteOff(sourceNote, 0, ch);
			if (MO2)
				MIDI2.sendNoteOff(sourceNote, 0, ch);
			if (Gate)
				digitalWrite(11, LOW);
			if (onoff > 0)
				onoff--;
			oled.setCursor(110, 0);
			oled.clearToEOL();
			oled.print(onoff);
			if (SYNTH)
			{
				synthnoteOff(sourceNote, polye);
			}
			break;
		case 1:
			if(debug==1)Serial.println(String(sourceNote)+" = "+String( note_frequency[sourceNote]));
			//if(debug==1)Serial.printf("\n**** NoteOn: UMO==%d,MO1==%d,MO2==%d %d vel=%d****", UMO, MO1, MO2, sourceNote, sourceVelocity);
			if (UMO)
				usbMIDI.sendNoteOn(sourceNote, sourceVelocity, ch, cable);
			if (MO1)
				MIDI1.sendNoteOn(sourceNote, sourceVelocity, ch);
			if (MO2)
				MIDI2.sendNoteOn(sourceNote, sourceVelocity, ch);
			if (Gate)
				digitalWrite(11, HIGH);
			if (SYNTH)
			{
				//fclick1.setOn(true);
				synthnoteOn(sourceNote, polye);
			}

			if (CV1)
			{
				if (debug == 1)
					Serial.println("before CV1 out");

				float volt = sourceNote / 12.0;
				int sout = volt * 4096 / (7 * 0.965);
				Wire.beginTransmission(MCP4725_ADDR);
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();
				if (debug == 1)
					Serial.println("CV1 Out" + String(sout));
			}
			if (CV2)
			{
				Wire.beginTransmission(MCP4725_ADDR + 1);
				float volt = sourceVelocity * 32;
				int sout = volt;
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();
				if (debug == 1)
					Serial.println("event 1 CV2 Out " + String(sout));
			}
			break;
		case 2:
			//			if(debug==1)Serial.println(String(myID) + " CC:" + String(sourceCC));
			if (sourceCC)
			{
				if (UMO)
					usbMIDI.sendControlChange(sourceCC, sourceVelocity, ch, cable);
				if (MO1)
					MIDI1.sendControlChange(sourceCC, sourceVelocity, ch);
				if (MO2)
					MIDI2.sendControlChange(sourceCC, sourceVelocity, ch);
				if (Menu::triggerCC == sourceCC)
					trigger = sourceVelocity > 0;
			}
		case 7:
			if (CV1)
			{
				if (debug == 1)
					Serial.println("before CV1 out");

				float volt = sourceNote / 12.0;
				int sout = volt * 4096 / (7 * 0.965);
				if (debug == 1)
					Serial.println(sout);
				Wire.beginTransmission(MCP4725_ADDR);
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();
			}
			if (CV2)
			{
				if (debug == 1)
					Serial.print(String(this->parent) + "event 2 before CV2 out ");
				Wire.beginTransmission(MCP4725_ADDR + 1);
				float volt = rawVelocity * 4;
				int sout = volt;
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();
				if (debug == 1)
					Serial.println(sout);
			}
			break;
		case 3:
			if (UMO)
				usbMIDI.sendPitchBend(sourceVelocity, ch, cable);
			if (MO1)
				MIDI1.sendPitchBend(sourceVelocity, ch);
			if (MO2)
				MIDI2.sendPitchBend(sourceVelocity, ch);
			break;
		}
		//		if(debug==1)Serial.println("note: " + String(sourceNote) + " vel " + String(sourceVelocity));
	}
	int me;
	signed char UMO = 1;
	signed char MO1 = 0;
	signed char MO2 = 0;
	signed char Gate = 0;
	signed char SYNTH = 0;
};
const String paranamecali[] = {"dummy", "Select from/to", "Set min/max", "And confirm", "Range from ", "To", "Value"};

class MenuCalibrateSet : public Menu
{
public:
	MenuCalibrateSet(int type, int Parent_ID) : Menu(type, "Calibrate", Parent_ID)
	{
		parent = Parent_ID;
		//      if(debug==1)Serial.println(items[0] + " parent " + Menus[parent]->items[0] );
		int i = 1;
		for (; i < 8; i++)
		{
			items[i] = paranamecali[i];
		}
		numitems = i;
		me = type;
	}
	void showVal(float rawvalue, short *emin, short *emax, short from, short to)
	{
		first = true;

		if (inSel)
		{
			if (rot_pos == 4)
				*emin = rawvalue;

			if (rot_pos == 5)
				*emax = rawvalue;
		}
		oled.setCursor(88, 4);
		oled.clearToEOL();
		oled.setInvertMode(inSel && rot_pos == 4);
		oled.print(*emin);
		oled.setCursor(88, 5);
		oled.clearToEOL();
		oled.setInvertMode(inSel && rot_pos == 5);
		oled.print(*emax);
		oled.setCursor(40, 6);
		oled.clearToEOL();
		int mapvalue = fmap(rawvalue, *emin, *emax, from, to);
		oled.setInvertMode(false);
		oled.print(rawvalue);
		oled.print("->");
		oled.print(mapvalue);
		first = false;
	}
	int me;
	bool first = true;
};
