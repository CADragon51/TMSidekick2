#ifndef BUTTON_H
#define BUTTON_H
#include <Audio.h>
#include "menu.h"

extern void onButton(Menu *menu);
extern void onBackButton(Menu *menu);
extern void onShow(void);
void saveTMS(void);
extern void inversion(bool emit);
byte wave = 1;
extern bool trigger;
class Button
{
public:
	Button(byte pin, byte pout, signed char *pvar, bool pinc, int plimit, elapsedMillis *led1, signed char cp)
	{
		inp = pin;
		outp = pout;
		var = pvar;
		inc = pinc;
		limit = plimit;
		pinMode(inp, INPUT_PULLUP);
		pinMode(outp, OUTPUT);
		if (debug == 1)
			Serial.print(" Pin Status ");
		if (debug == 1)
			Serial.print(inp);
		if (debug == 1)
			Serial.print(" = ");
		if (debug == 1)
			Serial.println(digitalRead(inp));
		*var = 0;
		led = led1;
		cb = cp;
		test = false;
		mbuttonState = HIGH;
	}
	Button(int pin, signed char *pvar)
	{
		inp = pin;
		var = pvar;
		pinMode(inp, INPUT_PULLUP);
		if (debug == 1)
			Serial.print(" Pin Status ");
		if (debug == 1)
			Serial.print(inp);
		if (debug == 1)
			Serial.print(" ");
		if (debug == 1)
			Serial.println(digitalRead(inp));
		if (var)
			*var = 0;
		outp = inp - 1;
		if (pin == 5)
			outp = 13;
		pinMode(outp, OUTPUT);
		test = true;
	}
	Button(int pin)
	{
		inp = pin;
		var = 0;
		pinMode(inp, INPUT_PULLUP);
		if (debug == 1)
			Serial.print(" SW Pin Status ");
		if (debug == 1)
			Serial.print(inp);
		if (debug == 1)
			Serial.print(" ");
		if (debug == 1)
			Serial.println(digitalRead(inp));
		test = true;
	}

	void loop()
	{
		if (test)
		{
			check();
			return;
		}

		//      if(debug==1)Serial.println("loop");

		int mreading = digitalRead(inp);

		// check to see if you just pressed the button
		// (i.e. the input went from LOW to HIGH), and you've waited long enough
		// since the last press to ignore any noise:

		// If the switch changed, due to noise or pressing:
		if (mreading != mlastButtonState)
		{
			// reset the debouncing timer
			mlastDebounceTime = millis();
		}
		if (*var == 0)
		{
			digitalWrite(outp, LOW);
			digitalWrite(Buttons[cb]->outp, LOW);
		}

		if (inc && *var > 0)
		{
			int to = 1000 / *var;
			if ((int)(*led) >= to)
			{
				*led = *led - to;
				digitalWrite(Buttons[cb]->outp, LOW);
				flash();
			}
		}
		if (!inc && *var < 0)
		{
			int to = -1000 / *var;
			if ((int)(*led) >= to)
			{
				//				if(debug==1)Serial.println("Time:" + String(to));
				*led = *led - to;
				digitalWrite(Buttons[cb]->outp, LOW);
				flash();
			}
		}

		if ((millis() - mlastDebounceTime) > mdebounceDelay)
		{
			// whatever the reading is at, it's been there for longer than the debounce
			// delay, so take it as the actual current state:

			// if the button state has changed:
			if (mreading != mbuttonState)
			{
				if (debug == 1)
				{
					Serial.print(" Pin Status ");
					Serial.print(inp);
					Serial.print(" = ");
					Serial.print(mreading);
					Serial.print(" == ");
					Serial.println(mbuttonState);
				}
				mbuttonState = mreading;

				// only toggle the LED if the new button state is HIGH
				if (mbuttonState == HIGH)
				{
					if (inc)
					{
						(*var)++;
						if (*var > limit)
							*var = limit;
					}
					else
					{
						(*var)--;
						if (*var < limit)
							*var = limit;
					}
					Serial.println("button " + String(inc ? "+" : "-") + " " + String(limit) + " " + String(*var) + " @ " + String(inp));

					if (*var)
						*led = abs(1000 / *var);
					if (abs(limit) == 12)
						printA4("S" + String(*var));
					else
						printA4("o" + String(*var));

					saveTMS();
					//            if (*var != 0)
					//            {
					//              digitalWrite(outp, HIGH);
					//            }
					//            else
					//              digitalWrite(outp, LOW);
				}
			}
		}
		mlastButtonState = mreading;
	}
	int sout = 4064;
	int vout[5] = {0, 1280, 2560, 3840, 4095};
	bool check(bool nocall = false)
	{
		int mreading = 1 - digitalRead(inp);
		hasChanged = false;
		if (mreading != mlastButtonState)
		{
			mlastDebounceTime = millis();
		}
		if ((millis() - mlastDebounceTime) > mdebounceDelay)
		{
			if (mreading != mbuttonState)
			{
				mbuttonState = mreading;
				if (mbuttonState == HIGH)
				{
					if (debug == 1)
						Serial.println("button click @" + String(inp));

					if (inp == 3) //||(inp==4&&Menus[menuState]->isRat))
					{
						hasChanged = true;
						trigger = true;
						return (true);
					}
					if (inp == 4)
					{
//						Serial.println("cancel button click now ");
						if (!nocall && menuState != NEWSCALE)
							onBackButton(Menus[menuState]);
						if (menuState == NEWSCALE)
							inversion(true);
						//						Serial.println("cancel button click ");
						if (menuState == MAIN)
						{
							for (int i = 0; i < 128; i++)
							{
								if (onnotes[i] > offnotes[i])
									Serial.println("On " + midiNamesLong[i] + "=" + String(onnotes[i]) + " Off " + String(offnotes[i]));
								onnotes[i] = 0;
								offnotes[i] = 0;
							}
						}
						trigger = true;
						return true;
					}
					else if (inp == 29)
					{
						*var = 1 - *var;
						flash();
						saveTMS();
						onShow();
					}
					else
					{
//						if (debug == 1)
							Serial.println("button click "+String(inp));
						onButton(Menus[menuState]);
					}
				}

				else if (inp == 3 || inp == 4)
				{
					hasChanged = true;
					trigger = false;
					return (false);
				}
			}
		}
		mlastButtonState = mreading;
		return (mreading);
	}
	bool flash()
	{
		digitalWrite(outp, output);
		output = !output;
		return (!output);
	}
	// Variables will change:
	boolean output = HIGH;
	int mledState = HIGH;		 // the current state of the output pin
	int mbuttonState = HIGH;	 // the current reading from the input pin
	int mlastButtonState = HIGH; // the previous reading from the input pin
	elapsedMillis *led;
	// the following variables are unsigned longs because the time, measured in
	// milliseconds, will quickly become a bigger number than can be stored in an int.
	unsigned long mlastDebounceTime = 0; // the last time the output pin was toggled
	unsigned long mdebounceDelay = 50;	 // the debounce time; increase if the output flickers
	signed char *var = 0;
	bool inc = true;
	int limit = 0;
	byte inp = 0;
	byte outp = 0;
	byte cb = 0;
	bool test = false;
	bool hasChanged = false;
};
#endif
