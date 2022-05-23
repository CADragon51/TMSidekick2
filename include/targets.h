#pragma once
int imap(int x, int in_min, int in_max, int out_min, int out_max);
float fmap(float x, float in_min, float in_max, float out_min, float out_max);
extern AudioPlaySdWav playSdWav1;
extern AudioPlaySdWav playSdWav2;
int v = 0;
int v2note[NUMVOICES] = {-1, -1, -1, -1, -1, -1};
float lastfreq = 0;
//#define SN(x) String(x)
//#define SB(x) String(x ? "t" : "f")
#define SP(x) String((int)x, HEX)
MenuPara *MenuPara::SynthMenu;
void synthnoteOn(byte sourceNote, byte polye, float vel)
{
	AudioInterrupts();
	for (v = 0; v < NUMVOICES && v < polye; v++)
	{
		if (v2note[v] == sourceNote)
		{
			vcaenv[v]->noteOff();
			vcfenv[v]->noteOff();
			v2note[v] = -1;
			vtmillis[v] = 0;

			//				DBG("**** removed:@ %f %d v=%d %f****\n", note_frequency[sourceNote], sourceNote, v, Parameter::aDPara->fvalue);
			break;
		}
	}
	for (v = 0; v < NUMVOICES && v < polye; v++)
	{
		if (v2note[v] == -1 || poly == 0)
		{
			//
//			Serial.println("**** NoteOn: " + SN(note_frequency[sourceNote]) + " " + SN(sourceNote) + " " + SN(v) + " " + SN(lastfreq));
			float freq = note_frequency[sourceNote];
			vtmillis[v] = 0;
			vcomix[v]->gain(0, vel);
//			FDBG("Tau "+SN(vcos1[v]->getTau()));
			if (vcos1[v]->getTau() ==0||onoff == 0 || lastfreq == 0 || (!poly && !vcaenv[v]->isActive()))
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
			display.setCursor(110, 0);
			clearToEOL();
			display.print(onoff);
			break;
		}
	}
}
void synthnoteOff(byte sourceNote, byte polye)
{
	for (v = 0; v < NUMVOICES && v < polye; v++)
	{
		if (v2note[v] == sourceNote)
		{

			//				DBG("**** NoteOff: v= %d****\n", v);
			vcaenv[v]->noteOff();
			vcfenv[v]->noteOff();
			onoff--;
			//						vcos1[v]->frequency(0);
			display.setCursor(110, 0);
			clearToEOL();
			display.print(onoff);
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

		FDBG(filename + " playing...");
		if (isExtsw)
			playSdWav1.play(filename.c_str());
		else
			playSdWav2.play(filename.c_str());
		// fclick1.setOn(true);
	}
}
class MenuTargetSet : public MenuPara
{
public:
	MenuTargetSet(int type, int Parent_ID) : MenuPara(TARGETS, "Target", Parent_ID)
	{
		parent = Parent_ID;
		//      if(debug==1)DBG(items[0] + " parent " + Menus[parent]->items[0] );
		Paras[nump++] = new Parameter("Syn. Para", &SynthPara, 0, NUMSYNTH, synthParas);
		//		Paras[nump++] = new Parameter("BPM", &BPM, 0, 1, selected);
		Paras[nump++] = new Parameter("USB MIDI", &UMO, 0, 1, selected);
		Paras[nump++] = new Parameter("MIDI 1", &MO1, 0, 1, selected);
		Paras[nump++] = new Parameter("MIDI 2", &MO2, 0, 1, selected);
		Paras[nump++] = new Parameter("Synthesizer", &SYNTH, 0, 1, selected);
		Paras[nump++] = new Parameter("CV 1", &CV1, 0, 1, selected);
		Paras[nump++] = new Parameter("CV 2", &CV2, 0, 1, selected);
		Paras[nump++] = new Parameter("Gate", &Gate, 0, 1, selected);
		//		Paras[nump++] = new Parameter("Strum", &Strum, 0, 1, selected);
		//		Paras[nump++] = new Parameter("Divide", &ratdiv, 2, 16);
		//		Paras[nump++] = new Parameter("BPM", &BPM, 10,240);
		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//        if(debug==1)DBG(items[0] + " " + Paras[i]->name);
		}
		numitems = nump;
		//     if(debug==1)DBG(String(nump - 1) + " " + items[nump - 1] + "/" + String(nump));
		me = type;
		name = items[0];
	}
	void setName(String pname)
	{
		name = pname;
		//		FDBG("my ID " + SN(me) + " " + name);
	}
	void createControl(String);

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
		frec.write(BPM);
#if 0
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
			
				DBG("Set CV2 " + String(CV2TG));
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
#endif
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
		// 		if (SynthPara > 0 )
		// 		{
		// 			FDBG(synthParas[SynthPara] + " by " + String(me));
		// 				byte pn = SynthPara;
		// 				Parameter *SynthPara = MenuPara::SynthMenu->getPara(pn);
		// 				if (SynthPara == nullptr)
		// 					return;
		// 				SynthPara->source = me;
		// 				FDBG(synthParas[pn] + " sourced by " + SN(me)+" "+name);
		// 		}
		Strum = readrec(frec);
		SYNTH = readrec(frec);
		BPM = readrec(frec);
	}
	int t = 0;
	void action(bool on, signed char eType, signed char sourceCH, signed char sourceNote, int sourceVelocity, signed char sourceCC, String from, int rawVelocity, float mapvalue = -1)
	{
		// if (BPM)
		// {
		// 	//			FDBG(SN(eType) + " " + SN(rawVelocity));
		// 	((MenuPara *)Menus[SETTINGS])->BPM = imap(rawVelocity, 0, 1023, 1, 250);
		// 	printA4(((MenuPara *)Menus[SETTINGS])->BPM);
		// 	return;
		// }
		STACK;
		if (eType == 4)
			return;
		byte polye = 1;
		if (sourceCH > 16)
			return;
		if (poly)
			polye = NUMVOICES;
		signed char sno[6] = {sourceNote, sourceNote, sourceNote, sourceNote, sourceNote, sourceNote};
		if (UMO && transposeit[16] == 127)
			sno[0] = sourceNote + 12 * octave + semiTone;
		if (MO1 && transposeit[17] == 127)
			sno[1] = sourceNote + 12 * octave + semiTone;
		if (MO2 && transposeit[18] == 127)
			sno[2] = sourceNote + 12 * octave + semiTone;
		if (SYNTH && transposeit[19] == 127)
			sno[3] = sourceNote + 12 * octave + semiTone;
		if (CV1 && transposeit[20] == 127)
			sno[4] = sourceNote + 12 * octave + semiTone;
		int trans = transposeit[sourceCH];
		if (trans == 127)
			trans = 12 * octave + semiTone;
		if (UMO && transposeit[16] == 0)
			sno[0] = sourceNote + trans;
		if (MO1 && transposeit[17] == 0)
			sno[1] = sourceNote + trans;
		if (MO2 && transposeit[18] == 0)
			sno[2] = sourceNote + trans;
		if (SYNTH && transposeit[19] == 0)
			sno[3] = sourceNote + trans;
		if (CV1 && transposeit[20] == 0)
			sno[4] = sourceNote + trans;
		STACK;
		if (UMO && transposeit[16] != 0 && transposeit[16] < 120)
			sno[0] = sourceNote + transposeit[16];
		if (MO1 && transposeit[17] != 0 && transposeit[17] < 120)
			sno[1] = sourceNote + transposeit[17];
		if (MO2 && transposeit[18] != 0 && transposeit[18] < 120)
			sno[2] = sourceNote + transposeit[18];
		if (SYNTH && transposeit[19] != 0 && transposeit[19] < 120)
			sno[3] = sourceNote + transposeit[19];
		if (CV1 && transposeit[20] != 0 && transposeit[20] < 120)
			sno[4] = sourceNote + transposeit[20];
		//		if (CV2 && transposeit[5])
		//			sno[5] = sourceNote + 12 * octave + semiTone;
		if (outData == "")
			outData = midiNamesLong[sourceNote];
		STACK;

// FDBG("action " + SN(eType) + " " + SN(SYNTH) + " " + SN(sourceCC) + " " + SN(transposeit[sourceCH - 1]));
#if 0 
		if (eType == 2 && sourceCH > 0)
		{

			FDBG("CC: val " + String(sourceVelocity) + " CC: " + String(sourceCC));
		}

		if (eType != 2)
		{
			//	byte channel = sourceCH;
			//	byte note = sourceNote;
			//			if (!Menus[SETTINGS]->byPass)
			//				sourceNote += octave * 12 + semiTone;
			if (SynthPara > 0)
			{
				if (on)
				{
					SynthMenu->getPara(SynthPara)->setSynthVal(__CALLER__);
					if (Menu::isshowing == SynthMenu->myID)
					{
						SynthMenu->show(true, __LINE__);
					}
				}
			}
			if (on && CV1)
			{
			}
		}
#endif
		byte ch = sourceCH;
		STACK;
		switch (eType)
		{
		case 6:
			if (SYNTH)
			{
				for (v = 0; v < NUMVOICES; v++)
				{

					//						DBG("**** NoteOff: v= %d****\n", v);
					vcaenv[v]->noteOff();
					vcfenv[v]->noteOff();
					v2note[v] = -1;
				}
			}
			break;
		case 0:

			//				DBG("\n**** NoteOff: UMO==%d,MO1==%d,MO2==%d ****", UMO, MO1, MO2);
//			FDBG(String(on ? "On " : "Off ") + String(sourceNote) + " = " + midiNamesLong[sourceNote] + " on ch " + String(ch) + " from " + from);
			if (UMO)
			{
				usbMIDI.sendNoteOff(sno[0], 0, ch, cable);
				outgoing[0] = LOW;
			}
			if (MO1)
			{
				MIDI1.sendNoteOff(sno[1], 0, ch);
				outgoing[1] = LOW;
			}
			if (MO2)
			{
				MIDI2.sendNoteOff(sno[2], 0, ch);
				outgoing[2] = LOW;
			}
			if (Gate)
			{
				digitalWrite(11, LOW);
				outgoing[6] = LOW;
			}
			if (onoff > 0)
				onoff--;
			display.setCursor(110, 0);
			clearToEOL();
			display.print(onoff);
			if (SYNTH)
			{
				synthnoteOff(sno[3], polye);
				outgoing[3] = LOW;
			}
			break;
		case 1:
			//			if(debug==1)
			//			FDBG(SN(millis())+":"+String(sourceNote) + " = " + midiNamesLong[sourceNote] + " on ch " + String(ch) + " from " + from);
			// if(debug==1)DBG("\n**** NoteOn: UMO==%d,MO1==%d,MO2==%d %d vel=%d****", UMO, MO1, MO2, sourceNote, sourceVelocity);
//			FDBG(ch);
			if (UMO)
			{
				usbMIDI.sendNoteOn(sno[0], sourceVelocity, ch, cable);
				outgoing[0] = HIGH;
			}
			if (MO1)
			{
				MIDI1.sendNoteOn(sno[1], sourceVelocity, ch);
				outgoing[1] = HIGH;
			}
			if (MO2)
			{
				MIDI2.sendNoteOn(sno[2], sourceVelocity, ch);
				outgoing[2] = HIGH;
			}
			if (Gate)
			{
				digitalWrite(11, HIGH);
				outgoing[6] = HIGH;
			}
			if (SYNTH)
			{
				// fclick1.setOn(true);
				if (sourceVelocity > 0)
				{
					synthnoteOn(sno[3], polye, sourceVelocity / 127.0);
					outgoing[3] = HIGH;
				}
				else
				{
					synthnoteOff(sno[3], polye);
					outgoing[3] = LOW;
				}
			}
			if (CV1)
			{

				DBG("before CV1 out");

				float volt = sno[4] / 12.0;
				int sout = volt * 4096 / (7 * 0.965);
				Wire.beginTransmission(MCP4725_ADDR);
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();

				DBG("CV1 Out" + String(sout));
				outgoing[4] = sout * 6.176 / 4096;
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

				DBG("event 1 CV2 Out " + String(sout));
				outgoing[5] = sout * 6.176 / 4096;
			}
			break;
		case 2:
			//			if(debug==1)
			if (sourceCC)
			{
				//			if (myID != 35)
				//				DBG(Menus[myID]->items[0] + " CC:" + String(sourceCC));
				static byte oldCC = 0;
				static byte oldVal = 0;
				if (oldCC == sourceCC && oldVal == sourceVelocity)
					return;
				oldCC = sourceCC;
				oldVal = sourceVelocity;
				if (sourceCC < 120)
				{
					lastcc = sourceCC;
					lastccval = sourceVelocity;
				}
				if (ccpatternidt > 0)
				{
					webgui.setMonitor(ccpatternidt, "CC " + String(lastcc));
					webgui.setMonitor(ccvpatternidt, "CC Value " + String(lastccval));
				}
				//				FDBG("CC " + SN(lastcc) +SN(lastccval));
				if (transport == RECORDING && lastEvent < 100000)
				{
					sequences[lastEvent].init(0xB0, sourceCC, sourceVelocity, ch);
					lastEvent++;
				} //	debug = 0;
				if (UMO)
					usbMIDI.sendControlChange(sourceCC, sourceVelocity, ch, cable);
				if (MO1)
					MIDI1.sendControlChange(sourceCC, sourceVelocity, ch);
				if (MO2)
					MIDI2.sendControlChange(sourceCC, sourceVelocity, ch);
				if (SYNTH)
				{
					Parameter *mP = SynthMenu->getPara(sourceCC);
					//					FDBG("CC " + SN(sourceCC));
					if (mP)
					{
						mP->fvalue = fmap(sourceVelocity, 0, 128, mP->fvstart, mP->fvend);
						mP->setSynthVal();
					}
				} //				if (Menu::triggerCC == sourceCC)
				  //					trigger = sourceVelocity > 0;
			}
			break;
		case 7:
			if (CV1)
			{
				//
				DBG("before CV1 out");

				float volt = sno[4] / 12.0;
				int sout = volt * 4096 / (7 * 0.965);
				//
				DBG(sout);
				Wire.beginTransmission(MCP4725_ADDR);
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();
			}
			if (CV2)
			{

				Serial.print(String(this->parent) + "event 2 before CV2 out ");
				Wire.beginTransmission(MCP4725_ADDR + 1);
				float volt = rawVelocity * 4;
				int sout = volt;
				Wire.write(64);				   // cmd to update the DAC
				Wire.write(sout >> 4);		   // the 8 most significant bits...
				Wire.write((sout & 0xf) << 4); // the 4 least significant bits...
				Wire.endTransmission();

				DBG(sout);
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
		//		if(debug==1)DBG("note: " + String(sourceNote) + " vel " + String(sourceVelocity));
	}

	signed char UMO = 1;
	signed char MO1 = 0;
	signed char MO2 = 0;
	signed char Gate = 0;
	signed char BPM = 0;
	signed char SYNTH = 0;
};
const String paranamecali[7] = {"dummy", "Select from/to", "Set min/max", "And confirm", "Range from ", "To", "Value"};

class MenuCalibrateSet : public Menu
{
public:
	MenuCalibrateSet(int type, int Parent_ID) : Menu(type, "Calibrate", Parent_ID)
	{
		parent = Parent_ID;
		//      if(debug==1)DBG(items[0] + " parent " + Menus[parent]->items[0] );
		int i = 1;
		for (; i < 7; i++)
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
		display.setCursor(88, 4 * 8);
		clearToEOL();
		print(*emin, inSel && rot_pos == 4, __LINE__);
		display.setCursor(88, 5 * 8);
		clearToEOL();
		print(*emax, inSel && rot_pos == 5, __LINE__);
		display.setCursor(40, 6 * 8);
		clearToEOL();
		int mapvalue = fmap(rawvalue, *emin, *emax, from, to);
		display.print(rawvalue);
		display.print("->");
		display.print(mapvalue);
		display.setCursor(0, 8 * 7);
		clearToEOL();
		display.display();
		first = false;
	}
	int me;
	bool first = true;
};
