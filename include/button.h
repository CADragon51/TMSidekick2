#ifndef BUTTON_H
#define BUTTON_H
#include <Audio.h>
#include "menu.h"

extern void onButton(Menu *menu);
extern void onBackButton(Menu *menu);
extern void onShow(void);
extern void dumpDisplay(void);
void saveTMS(void);
extern bool inversion(bool emit);
byte wave = 1;
extern bool trigger;
class Button
{
public:
	Button(byte pin, byte pout, signed char *pvar, int pinc, int plimit, elapsedMillis &led1, signed char cp)
	{
		inp = pin;
		outp = pout;
		var = pvar;
		inc = pinc;
		limit = plimit;

		pinMode(inp, INPUT_PULLUP);
		pinMode(outp, OUTPUT);
		if(0)
		{
			Serial.print(" Pin Status ");
			Serial.print(inp);
			Serial.print(" = ");
			FDBG(digitalRead(inp));
		}
		*var = 0;
		led = led1;
		led = 1000;
		cb = cp;
		test = false;
		mbuttonState = HIGH;
	}
	Button(int pin, signed char *pvar)
	{
		inp = pin;
		var = pvar;
		pinMode(inp, INPUT_PULLUP);
		if (0)
		{
			Serial.print(" Pin Status ");
			Serial.print(inp);
			Serial.print(" ");
			FDBG(digitalRead(inp));
		}
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
		if (0)
		{
			Serial.print(" SW Pin Status ");
			Serial.print(inp);
			Serial.print(" ");
			FDBG(digitalRead(inp));
		}
		test = true;
	}

	void loop()
	{
		if (test)
		{
			checkButton();
			return;
		}

		//      if(debug==1)DBG("loop");

		int mreading = digitalRead(inp);
//		if (inp == 29)
//			printA4(SN(mreading));
		// check to see if you just pressed the button
		// (i.e. the input went from LOW to HIGH), and you've waited long enough
		// since the last press to ignore any noise:

		// If the switch changed, due to noise or pressing:
		if (mreading != mlastButtonState)
		{
			// reset the debouncing timer
			mlastDebounceTime = millis();
		}
//		if(inp==29)
//		FDBG(SN(inp)+SN(inc) + SN(*var));
		if (*var == 0)
		{
			digitalWrite(outp, LOW);
			digitalWrite(Buttons[cb]->outp, LOW);
			ledstate[outp] = false;
			ledstate[Buttons[cb]->outp] = false;
		}
		if (inc == 2)
		{
			if (*var > 0)
			{
				int to = 1000 / *var;
//				FDBG(to);
//				FDBG("flash " + SN((int)(led)));
				if ((int)(led) >= to)
				{
					led = led - to;
//					FDBG("flash "+SN((int)(led)));
					//					digitalWrite(outp, LOW);
					flash();
				}
			}
		}
		if (inc == 1 && *var > 0)
		{
			int to = 1000 / *var;
			if ((int)(led) >= to)
			{
				led = led - to;
				digitalWrite(Buttons[cb]->outp, LOW);
				ledstate[Buttons[cb]->outp] = false;
				flash();
			}
		}
		if (inc == -1 && *var < 0)
		{
			int to = -1000 / *var;
			if ((int)(led) >= to)
			{
				//				if(debug==1)DBG("Time:" + String(to));
				led = led - to;
				digitalWrite(Buttons[cb]->outp, LOW);
				ledstate[Buttons[cb]->outp] = false;
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
				if (0)
				{
					DBG(" Pin Status ");
					DBG(inp);
					DBG(" = ");
					DBG(mreading);
					DBG(" == ");
					DBG(mbuttonState);
				}
				mbuttonState = mreading;

				// only toggle the LED if the new button state is HIGH
				if (mbuttonState == HIGH)
				{
					if (inc == 2)
					{
						if (*var >= limit)
							*var = 0;
						else
						{
							(*var)++;
						}
					}
					else if (inc == 1)
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

					DBG("button " + String(inc) + " " + String(limit) + " " + String(*var) + " @ " + String(inp));

					if (*var)
						led = abs(1000 / *var);
					if (abs(limit) == 12)
						printA4("S" + String(*var));
					else if (abs(limit) == 5)
						printA4("O" + String(*var));
					else
						printA4("PM" + String(*var));

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
	bool checkButton(bool nocall = false);

	bool flash()
	{
		digitalWrite(outp, output);
		ledstate[outp] = output;
		output = !output;
		return (!output);
	}
	// Variables will change:
	boolean output = HIGH;
	int mledState = HIGH;		 // the current state of the output pin
	int mbuttonState = HIGH;	 // the current reading from the input pin
	int mlastButtonState = HIGH; // the previous reading from the input pin
	elapsedMillis led;
	// the following variables are unsigned longs because the time, measured in
	// milliseconds, will quickly become a bigger number than can be stored in an int.
	unsigned long mlastDebounceTime = 0; // the last time the output pin was toggled
	unsigned long mdebounceDelay = 50;	 // the debounce time; increase if the output flickers
	signed char *var = 0;
	int inc = 1;
	int limit = 0;
	byte inp = 0;
	byte outp = 0;
	byte cb = 0;
	bool test = false;
	bool hasChanged = false;
};
#endif
