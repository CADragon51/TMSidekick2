#pragma once
int imap(int x, int in_min, int in_max, int out_min, int out_max);
float fmap(float x, float in_min, float in_max, float out_min, float out_max);
signed char readrec(File frec);
#include "SSD1309AsciiSpi1.h"
extern SSD1309AsciiSpi1 oled;
class MenuExtSet : public MenuPara
{
public:
	MenuExtSet(int type, MenuTargetSet *ptargets, MenuCalibrateSet *pcalibrate, signed char pPin) : MenuPara(EXTSETTINGS + type, paraname[type], 0)
	{
		byte extt = EXTSETTINGS + type;
#if 1
		Paras[nump++] = new Parameter("Target", ptargets->me);
		paraID[nump - 1] = ptargets->me;
		if (extt != HIRES)
		{
			Paras[nump++] = new Parameter("Calibrate", pcalibrate->me);
			paraID[nump - 1] = pcalibrate->me;
			Paras[nump++] = new Parameter("CC", &extCC, 0, 127);
			Paras[nump++] = new Parameter("Channel", &extCh, 0, 15);
			Paras[nump++] = new Parameter("Ping", &extPing, 0, 1, selected);
			Paras[nump++] = new Parameter("From Note", &extNote, 0, 127, midiNamesLong);
			Paras[nump++] = new Parameter("to", &toextNote, 0, 127, midiNamesLong);
			Paras[nump++] = new Parameter("Velocity", &velocity, 0, 14);
		}
		else
		{
			Paras[nump++] = new Parameter("CC", &extCC, 0, 127);
			Paras[nump++] = new Parameter("Channel", &extCh, 0, 15);
			myEnc = new Encoder(25, 24);
			emin = 0;
			emax = 10000;
		}

		for (int i = 1; i < nump; i++)
		{
			items[i] = Paras[i]->name;
			//       if(debug==1)Serial.println(items[0] + " " + Paras[i]->name);
		}
		if (extt != HIRES)
			items[nump++] = "Back";
		numitems = nump;

		me = type;
		parent = EXTERNALS;
		targets = ptargets;
		aPin = pPin;
		if (extt != HIRES)
		{
			calibrate = pcalibrate;

			ping = Ping(aPin);
			if (aPin > 3)
			{
				Analog = new ResponsiveAnalogRead(aPin, true);
				//			ping = Ping(aPin);
			}
			else
			{
				myButton = new Button(3);
				//			if(debug==1)Serial.println(digitalRead(3));
			}
			calibrate->items[0] = "Cal." + items[0];
		}
		targets->items[0] = items[0] + "->";
		rawvalue = 0;
#endif
	}
	int vel[15] = {0, 36, 63, 82, 95, 104, 111, 116, 119, 121, 123, 124, 125, 126, 127};
	int oldraw = -999;
	void probe(void)
	{
		eventtype = 4;
		bool on = true;
		short mapvalue = 0;
		byte notefrom = extNote;
		byte noteto = toextNote;
		//		if(debug==1)Serial.println("Probe " + String(aPin) + " Analog " + String((int)Analog) + " Ping " + String((int)extPing) + " Switch " + String((int)myButton));
		if (myEnc != nullptr)
		{
			rawvalue = myEnc->read();
			if (oldraw == rawvalue)
			{
				rawdir = 0;
				return;
			}
			int delta=(rawvalue-oldraw);
//			Serial.println(delta);
			rawdir = delta >0 ? 1 : delta<0?-1:0;
			if (rawvalue > 9999)
				rawvalue = 9999;
			if (rawvalue < 0)
				rawvalue = 0;
			myEnc->write(rawvalue);
			oldraw = rawvalue;
			//			Serial.println(rawvalue);
			targets->action(on, 2, 0, 0, 0, extCC, rawvalue, rawvalue, mapvalue);
			eventtype = 4;
		}
		if (myButton)
		{
			bool res = myButton->check();
			if (myButton->hasChanged)
			{
				if (res)
				{
					eventtype = 1;
					rawvalue = 1;
				}
				else
				{
					eventtype = 0;
					on = false;
					rawvalue = 0;
				}
			}
		}

		if (Analog)
		{
			if (aPin == FSR_PIN)
			{

				int fsrADC = analogRead(aPin);
				if (fsrADC != 0) // If the analog reading is non-zero
				{
					float fsrV = fsrADC * VCC / 1023.0;
					float fsrR = R_DIV * (VCC / fsrV - 1.0);
					if (fsrR > 10000)
						return;
					if (debug == 1)
						Serial.println("Resistance: " + String(fsrR) + " ohms");
					float force;
					float fsrG = 1.0 / fsrR; // Calculate conductance
					// Break parabolic curve down into two linear slopes:
					if (fsrR <= 600)
						force = (fsrG - 0.00075) / 0.00000032639;
					else
						force = fsrG / 0.000000642857;
					if (debug == 1)
						Serial.println("Force: " + String(force) + " g");
					rawvalue = force;
					eventtype = 7;
					if (isshowing == calibrate->myID)
					{
						calibrate->showVal(rawvalue, &emin, &emax, 0, 127);
					}
					else
					{
						if (rawvalue < emin)
							emin = rawvalue;
						if (rawvalue > emax)
							emax = rawvalue;
						mapvalue = map(rawvalue, emin, emax, 0, 127);
					}
					float fx = fmap(force, 0, 5000, 0, 1);
					if (targets->SynthPara == 25)
						dc1.amplitude(fx);
					else
						dc2.amplitude(fx);
					hit = true;
					on = true;
				}
				else if (hit)
				{
					hit = false;
					eventtype = 0;
					on = false;
					if (targets->SynthPara == 25)
						dc1.amplitude(0);
					else
						dc2.amplitude(0);
				}
			}
		}
		if (extPing != 27)
		{
			if (Analog)
			{
				Analog->update();
				if (isshowing == calibrate->myID)
				{
					calibrate->showVal(rawvalue, &emin, &emax, 0, 127);
				}
				if (Analog->hasChanged())
				{
					rawvalue = Analog->getValue();
					eventtype = 2;
					byte sNote = imap(rawvalue, emin, emax, notefrom, noteto);
					//					if (aPin == A15)
					//							if(debug==1)Serial.println(String(myID) + " Probe " + String(extNote) + " Analog " +  String(rawvalue));
					//					if(debug==1)Serial.println(targets->SynthPara);
					targets->action(on, eventtype, extCh, sNote, vel[velocity], extCC, rawvalue, rawvalue, mapvalue);
					eventtype = 4;
				}
			}
		}
#if 1
		else
		{

			int _pin = aPin;
			pinMode(_pin, OUTPUT);
			digitalWrite(_pin, LOW);
			delayMicroseconds(2);
			digitalWrite(_pin, HIGH);
			delayMicroseconds(5);
			digitalWrite(_pin, LOW);
			pinMode(_pin, INPUT);
			int _duration = pulseIn(_pin, HIGH, 10000);
			if (_duration != -1)
				rawvalue = (float)_duration / (29) / 2;
			else
				rawvalue = -1;
			if (rawvalue < 1)
				return;
			if (isshowing == calibrate->myID)
			{

				calibrate->showVal(rawvalue, &emin, &emax, 0, 127);
				//				if(debug==1)Serial.println("sel? " + String(calibrate->inSel)+ " @ "+String(calibrate->rot_pos));
			}
			if (rawvalue > 2 && rawvalue < 50)
			{
				if (rawvalue != pcm && onote != 0)
				{
					eventtype = 0;
					onote = 0;
				}
				//					int note = fmap(cm, 0, 50, 1, 127);
				//					usbMIDI.sendNoteOn(note, 64, _channel, cable);
				//					onote = note;
				pcm = rawvalue;
				eventtype = 6;
			}
			else if (onote > 0)
			{
				//					usbMIDI.sendNoteOff(onote, 0, _channel, cable);
				eventtype = 0;
				onote = 0;
				on = false;
			}
		}
#endif
#if 1
		if (targets->SynthPara)
		{
			byte pn = targets->SynthPara;
			Parameter *aPara = MenuPara::SynthMenu->getPara(pn);
			if (aPara == nullptr)
				return;
			//			if(debug==1)Serial.println(String(pn) + " " +aPara->name + " " +String(aPara->isFloat?1:0));
			if (aPara->isFloat)
			{
				//				if(debug==1)Serial.println(String((int)aPara->fvalue) + " " + items[0]);
				//				if(debug==1)Serial.println("audio " + String(eventtype) + " " + String(pn) + " " + mPara->Paras[pn]->name);// +" " + ((MenuPara*)Menus[SYNTHSETTINGS])->Paras[targets->SynthPara]->name);
				float mapvalue = fmap(rawvalue, emin, emax, aPara->fvstart, aPara->fvend);
				aPara->fvalue = mapvalue;
				//			if(debug==1)Serial.printf("values %f %d %d %f %f %f\n", rawvalue, emin, emax, aPara->fvstart, aPara->fvend, mapvalue);
				//			if(debug==1)Serial.println("audio "+ String(mapvalue) + " " + mPara->Paras[pn]->name);
				//			MenuPara::setSynthVal(targets->SynthPara);
				targets->action(on, eventtype, extCh, 0, 0, extCC, rawvalue, mapvalue);
				return;
			}
		}
#endif
		if (eventtype != 4)
		{
			//			if(debug==1)Serial.println("Probe " + String(aPin) + " Analog " + String((int)Analog) + " Ping " + String(extNote) + " Switch " + String(rawvalue));
			targets->action(on, eventtype, extCh, extNote, vel[velocity], extCC, rawvalue, mapvalue);
		}
	}
	void save(File frec)
	{
		frec.write(extCC);
		frec.write(extCh);
		frec.write(xextOct);
		frec.write(extNote);
		frec.write(velocity);
		frec.write(extPing);
		targets->save(frec);
		frec.write(emin & 0xff);
		frec.write(emin >> 8);
		frec.write(emax & 0xff);
		frec.write(emax >> 8);
		frec.write(xtoextOct);
		frec.write(toextNote);
		//		if(debug==1)Serial.println(String(emin & 0xff) + " " + String(emin >> 8) + " -> " + String(emin));
		//		if(debug==1)Serial.println(String(emax & 0xff) + " " + String(emax >> 8) + " -> " + String(emax));
	}
	virtual void restore(File frec) override
	{
		extCC = readrec(frec);
		extCh = readrec(frec);
		xextOct = readrec(frec);
		extNote = readrec(frec);
		velocity = readrec(frec);
		extPing = readrec(frec);
#if 1
		targets->restore(frec);
		byte em1 = readrec(frec);
		byte em2 = readrec(frec);
		byte em3 = readrec(frec);
		byte em4 = readrec(frec);
		emin = em2 * 256 + em1;
		emax = em4 * 256 + em3;
		xtoextOct = readrec(frec);
		toextNote = readrec(frec);
#endif
		//		if(debug==1)Serial.println(String(em1) + " " + String(em2) + " -> " + String(emin));
		//		if(debug==1)Serial.println(String(em3) + " " + String(em4) + " -> " + String(emax));
	}

	int me;
	signed char velocity = 0;
	signed char extCC = 0;
	signed char extCh = 0;
	signed char xextOct = 3;
	signed char extNote = 0;
	signed char xtoextOct = 3;
	signed char toextNote = 0;
	signed char extPing = 0;
	signed char aPin = 0;
	MenuTargetSet *targets = 0;
	MenuCalibrateSet *calibrate = 0;
	Button *myButton = 0;
	Ping ping = 0;
	float cm = 0;
	float pcm = 0;
	int onote = 0;
	bool hit = false;
	const float VCC = 3.3;		// Measured voltage of Ardunio 5V line
	const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor
};

#include "synthesizer.h"