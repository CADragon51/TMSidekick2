#ifndef MENU_H
#define MENU_H
#include <RotaryEncoder.h>
#include <ResponsiveAnalogRead.h>
#include "Button.h"
#include <Ping.h>
#include <chord.h>

void saveTMS(void);
int nextm = -1;
class Targets;
extern Adafruit_SSD1306 display;
extern float fmap(float, float, float, float, float);
const String paranamemidi[] = {"USB MIDI IN", "HOST MIDI IN", "MIDI IN", "USB MIDI OUT", "MIDI OUT 1", "MIDI OUT 2"};
const String paraname[] = {"Pedal 1", "Pedal 2", "FSR", "Switch", "Joystick X", "Joystick Y", "Pot1", "Pot2", "HiRes Encoder"};
void playFile(String, bool);
typedef void (*CALLBACK)(signed char type);
extern String readline(File);
#include "parameter.h"
#define PIN_IN1 5
#define PIN_IN2 6
void printA4(String);
enum class Direction
{
	NOROTATION = 0,
	CLOCKWISE = 1,
	COUNTERCLOCKWISE = -1
};
extern void save(void);

extern signed char SplitS(String what, char where, String *res, signed char nmax);

int sout = 4000;

extern Parameter *Paras[];
int dpos;
int r;
byte fnote = 0;
byte ln[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short mln[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short mlninv[12][5];
// short actinv[12];
byte mlnchord[12][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
int bnx = 0;
int mapnx = 0;
extern int sfx;
String res[24];
int nx = 0;

class Menu
{
public:
	Menu(signed char pmyID, String pname, String pitems = "", signed char pparent = MAIN)
	{
		//		if(debug==1)Serial.println("menu " + String(pparent));
		//		if(debug==1)Serial.println(items[0] + "***" + String(pmyID));
		setup(pname);
		numitems = 0;
		if (pitems.length() > 0)
		{
			if (pmyID != MAIN)
				numitems = Split(pitems + ",");
			else
				numitems = Split(pitems + ",");
		}
		else
		{
			items[numitems++] = pname;
			//			items[numitems++] = "Back";
		}

		parent = pparent;
		myID = pmyID;
		for (int i = 0; i < 50; i++)
			paraID[i] = -1;
	}
	void setup(String pname)
	{
		encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
		paraencoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
		items[0] = pname;
		rot_pos = 1;
		encoder->setPosition(1);
	}
	void mainSel(int pos)
	{
		int para = rot_pos;
		if (debug == 1)
		{
			Serial.print(String(pos) + " para =");
			Serial.print(String(para) + " MSel =" + String(paraID[para]));
		}
		//    if(debug==1)Serial.println(paraID[para]);
		menuState = paraID[para];
		//		if(debug==1)Serial.print(" 162 now :");
		//		if(debug==1)Serial.println(menuState);

		if (Menus[menuState])
		{
			//        Menus[menuState]->display();
		}
	}
	bool selMenu(void)
	{
		int para = rot_pos;
		//		if(debug==1)Serial.print("+++++ ");
		//		if(debug==1)Serial.print(items[0]);
		//		if(debug==1)Serial.print(" = ");
		if (debug == 1)
			Serial.print(items[para] + " ");
		if (debug == 1)
			Serial.println(paraID[para]);
		inSel = !inSel;
		if (items[para] == "Back" || rot_pos == 0)
		{
			menuState = parent;
			para_change = false;
			//			if(debug==1)Serial.println("par " + String(parent));
			saveTMS();

			return true;
		}

		if (paraID[para] > -1)
		{
			menuState = paraID[para];
			if (debug == 1)
				Serial.print(" 189 now :" + String(para));
			//			if(debug==1)Serial.println(menuState);

			para_change = false;
			//			if(debug==1)Serial.println("par " + String(parent));
			saveTMS();

			return true;
		}
		else
		{
			if (items[para] == "Samples 1")
			{
				menuState = FSR;

				para_change = false;
				saveTMS();

				return true;
			}
			if (items[para] == "Find")
			{
				menuState = NEWSCALE;

				para_change = false;
				saveTMS();

				return true;
			}
			if (items[para] == "New Mapping")
			{
				menuState = NEWMAP;
				mapnx = 0;
				para_change = false;
				saveTMS();

				return true;
			}
			if (items[para] == "Samples 2")
			{
				menuState = SWITCH;
				para_change = false;
				saveTMS();

				return true;
			}
			if (items[para] == "Target")
			{
				menuState = paraID[para];
				//				if(debug==1)Serial.print("217 now :");
				//				if(debug==1)Serial.println(menuState);
				para_change = false;
				saveTMS();

				return true;
			}
		}
		return false;
	}
	int Split(String ins)
	{
		int n = 0;
		int p[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		for (int i = 0; i < (int)ins.length(); i++)
		{
			p[n] = ins.indexOf(',', i);
			if (p[n] < 0)
				break;
			i = p[n];
			n++;
		}
		items[1] = ins.substring(0, p[0]);
		//		if(debug==1)Serial.println(items[1]);
		for (int i = 1; i < n; i++)
		{
			String it = ins.substring(p[i - 1] + 1, p[i]);
			//			if(debug==1)Serial.println(it);
			//        items[i + 1] = ins.substring(p[i - 1] + 1, p[i]);
			items[i + 1] = it;
		}
		return n + 1;
	}
	void mapit(void)
	{
		mapit(lastMap);
	}
	void scaleit(void)
	{
		scaleit(lastScale);
	}
	static short maparr[100];
	static byte mnx;
	void mapit(int para)
	{

		if (para < numitems - 1)
		{
			//			if(debug==1)Serial.println(items[para]);
			File myFile = SD.open(filenames[para].c_str());
			if (myFile)
			{
				if (debug == 1)
					Serial.println(filenames[para]);
				for (int i = 0; i < 2048; i++)
				{
					replchord[i] = i;
					transpose[i] = 0;
				}
				String fn = readline(myFile);
				lastMap = para;
				// read from the file until there's nothing else in it:
				mnx = 0;
				while (myFile.available())
				{
					fn = readline(myFile) + "=";
					String res[2];
					signed char n = SplitS(fn, '=', (String *)res, 2);
					if (n == 2)
					{
						int ix = res[1].indexOf("+");
						signed char n2 = 0;
						if (ix > -1)
							n2 = res[1].substring(ix).toInt();
						replchord[res[0].toInt()] = res[1].toInt();
						transpose[res[0].toInt()] = n2;
						maparr[mnx] = res[0].toInt();
						//					if (debug == 1)
						//						if (debug == 1)
						Serial.println(String(mnx) + " " + String(maparr[mnx]) + "=" + res[1] + " + " + String(n2));
						mnx++;
					}
					else
					{
						if (debug == 1)
						{

							Serial.print(n);
							Serial.print(" found:");
							Serial.println(fn);
						}
					}
				}
				// close the file:
				myFile.close();
				displayMenu();
			}
		}
	}
	void scaleit(int para)
	{
		if (para < numitems - 1)
		{
			//			if (debug == 1)
			//			Serial.println(items[para] + "->" + scFP[para]);
			lastScale = para;
			nx = 1;
			scid = scFP[lastScale] << 1;
			//			scid = scid << 1;
			for (int i = 0; i < 12; i++)
			{
				int test = 1 << i;
				if ((scid & test) > 0)
					ln[nx++] = i;
				//				Serial.println(String(i) + " " + String(test) + " " + String(scid & test) + " " + String(nx));
			}
			scid = scid >> 1;

			int o = 0;
			for (int i = 0, j = 0; i < 128 && o < 11; i++)
			{
				byte n = i % 12;
				if (n == 1 || n == 3 || n == 6 || n == 8 || n == 10)
					basescale[i] = -1;
				else
				{
					basescale[i] = ln[j] + o * 12;
					if (debug == 1)
						Serial.print(String(j) + ": " + String(i) + " ->" + String(ln[j]) + " " + String(o));
					if (debug == 1)
						Serial.println(" " + midiNamesFlat[n] + String(i / 12) + " ->" + midiNamesFlat[ln[j]] + " " + String(o));
					j++;
					if (j >= nx)
					{
						j = 0;
						o++;
					}
				}
			}
			displayMenu();
		}
	}
	void selNewMenu(void)
	{
		int rp = rot_pos;
		//		rot_pos--;
		if (debug == 1)
			Serial.println("myID " + String(myID) + " " + String(rot_pos));
		switch (myID)
		{
		case FSR:
			if (s1index != rot_pos)
			{
				s1index = rot_pos;
				saveTMS();
				displayMenu();
			}
			else
			{
				playFile(wavenames[s1index], false);
			}
			break;
		case SWITCH:
			if (s2index != rot_pos)
			{
				s2index = rot_pos;
				saveTMS();
				displayMenu();
			}
			else
			{
				playFile(wavenames[s2index], false);
			}
			break;
		case PRESETS:
			preindex = rot_pos;
			//           saveTMS();
			displayMenu();
			break;
		case MAPPINGS:
			if (rot_pos > 1)
			{
				if (lastMap != rot_pos)
					mapit(rot_pos);
				menuState = SHOWMAP;
				Menus[menuState]->items[0] = items[rot_pos];
				Menus[menuState]->show(true, 369);
			}
			else
				selMenu();
			break;
		case SCALES:
			if (rot_pos > 1)
			{
				if (lastScale != rot_pos)
					scaleit(rot_pos);
				menuState = SHOWSCALE;
				Menus[menuState]->items[0] = items[rot_pos];
				Menus[menuState]->show(true, 369);
			}
			else
				selMenu();
			break;
		case MAIN:
			mainSel(1);
		case EXTERNALS:
			mainSel(rot_pos);
		case TOUCH:
			mainSel(rot_pos);
		case MIDISET:
			mainSel(rot_pos);
		case NEWMAP:
			if (rot_pos == 0 && !para_change)
				selMenu();
			else
			{
				if (psel == 0)
				{
					para_change = true;
					psel = 1;
					rot_pos = 0;
				}
				else if (psel == 1)
				{
					para_change = true;
					psel = 2;
					last_Sel = rot_pos;
					for (int ix = 0; ix < 51; ix++)
					{
						if (replchord[inx] == chordIds[ix])
						{
							rot_pos = ix;
							break;
						}
					}
				}
				else if (psel == 2)
				{
					para_change = true;
					psel = 3;
					rot_pos = transpose[inx];
				}
				else if (psel == 3)
				{
					psel = 0;
					para_change = false;
					rot_pos = last_Sel;
				}
				encoder->setPosition(rot_pos);
				Serial.println("myID " + String(myID) + " " + String(para_change ? "selected" : "not selected") + " psel " + String(psel));
			}
			break;
		default:
			selMenu();
		}
		rot_pos = rp;
	}
	virtual void restore(File frec) {}
	virtual void onButton(bool cancel)
	{
		para_change = false;
		//		if (debug == 1)
		Serial.println("ID " + String(myID) + " items " + items[rot_pos] + "@" + String(rot_pos) + " " + items[0]);
		dpos = 0;
		r = 1;
		if (myID == NEWSCALE || myID == SHOWSCALE || myID == SHOWMAP)
		{
			Serial.println("back -> parent " + String(parent));
			menuState = parent;
			Menus[menuState]->show(true, 391);
			if (myID == NEWSCALE)
			{
				bnx = 0;
				ln[0] = 0;
#if 0
				for (int i = 0; i < 12 && invert[scid] != nullptr; i++)
				{
					Serial.println(String(invert[scid][i] / 10) + " " + String(invert[scid][i] % 10));
					invID[invert[scid][i] / 10] = invert[scid][i] % 10;
				}
#endif
			}
			return;
		}
		if (myID == PRESETS && items[rot_pos] != "Back")
		{
			Serial.println("Open " + items[rot_pos] + ".PRS");
			if (SD.exists(presetnames[rot_pos].c_str()))
			{
				File frec = SD.open(presetnames[preindex].c_str(), FILE_READ);
				if (frec)
				{
					if (debug == 1)
						Serial.println("loading preset data ");
					AudioNoInterrupts();
					Menus[SYNTHSETTINGS]->restore(frec);
					AudioInterrupts();
					if (debug == 1)
						Serial.println("data loaded");
					frec.close();
				}
			}
		}
		if (myID == NEWMAP)
		{
			if (rot_pos == 0)
			{
				mapnx = 0;
			}
#if 0
			else
			{
				String out = "";
				for (int i = 0; i < mapnx; i++)
				{
					int sn = ln[i] - ln[0];
					out = out + String(sn % 12) + " ";
				}
				Serial.print(String(bnx) + " scale :" + out);
				Serial.println("\nEnter scale name:");
				while (Serial.available() == 0)
				{
					if (Buttons[6]->check(true))
						return;
				}
				String comment = Serial.readString();
				comment = comment.substring(0, comment.length() - 1);
				String fn = "SCALE" + String(sfx) + ".scl";
				//

				Serial.println("Saving in " + fn);
				Serial.println("comment <" + comment + "> " + String(comment.length()));
				Serial.println("scale <" + out + ">");
				if (comment.length() > 2)
				{
					File frec = SD.open(fn.c_str(), FILE_WRITE);
					frec.println(comment);
					frec.println(out);
					frec.close();
					scfilenames[sfx++] = fn;
					byte r = Menus[SCALES]->insertItem(comment, true);
					lastScale = r;
					scaleit(r);
				}
				for (int i = 0; i < bnx; i++)
				{
					ln[i] = 255;
				}
				bnx = 0;
			}
#endif
		}
		if (myID == NEWSCALE)
		{
			if (rot_pos == 0)
			{
				for (int i = 0; i < bnx; i++)
				{
					ln[i] = 255;
				}
				bnx = 0;
			}
#if 0
			else
			{
				String out = "";
				for (int i = 0; i < bnx; i++)
				{
					int sn = ln[i] - ln[0];
					out = out + String(sn % 12) + " ";
				}
				Serial.print(String(bnx) + " scale :" + out);
				Serial.println("\nEnter scale name:");
				while (Serial.available() == 0)
				{
					if (Buttons[6]->check(true))
						return;
				}
				String comment = Serial.readString();
				comment = comment.substring(0, comment.length() - 1);
				String fn = "SCALE" + String(sfx) + ".scl";
				//

				Serial.println("Saving in " + fn);
				Serial.println("comment <" + comment + "> " + String(comment.length()));
				Serial.println("scale <" + out + ">");
				if (comment.length() > 2)
				{
					File frec = SD.open(fn.c_str(), FILE_WRITE);
					frec.println(comment);
					frec.println(out);
					frec.close();
					scfilenames[sfx++] = fn;
					byte r = Menus[SCALES]->insertItem(comment, true);
					lastScale = r;
					scaleit(r);
				}
				for (int i = 0; i < bnx; i++)
				{
					ln[i] = 255;
				}
				bnx = 0;
			}
#endif
		}

		if (items[rot_pos] == "Back" || rot_pos == 0)
		{
			//       para_change = false;
			//			if (debug == 1)
			Serial.println("back -> parent " + String(parent));
			//       if(debug==1)Serial.println(mstate[ parent]);
			//			if(debug==1)Serial.println("Back");
			//			if(debug==1)Serial.println(parent);
			//			setoled();
			setContrast();
			menuState = parent;
			//		if(debug==1)Serial.print("318 now :");
			//			if(debug==1)Serial.println(menuState);
			if (Menus[SETTINGS]->hiRes > 0)
			{

				Menus[HIRES]->myEnc->write(map(Menus[menuState]->rot_pos, 0, 24, 0, 10000));
			}
			saveTMS();
		}
		else if (myID == MAIN)
			mainSel(1);
		else
			selNewMenu();
		//		if(debug==1)Serial.print("326 now :");
		//		if(debug==1)Serial.println(menuState);
		if (menuState > 90)
			menuState = 0;
		//		setoled();
		setContrast();
		displayMenu();
		//     if(debug==1)Serial.println(" pc " + String(para_change) + " ps " + String(para_sel) + " selected " + String(pstart + rot_pos - 1) + " " + String(pend));
	}
	void setmark(int ml, int i)
	{
		if ((myID == SCALES && lastScale == ml) ||
			(myID == MAPPINGS && lastMap == ml) ||
			(myID == FSR && s1index == ml) ||
			(myID == SWITCH && s2index == ml) ||
			(myID == PRESETS && preindex == ml))
		{
			oled.setCursor(0, i);
			oled.print("*");
		}

	} //     if(debug==1)Serial.println(" pc " + String(para_change) + " ps " + String(para_sel) + " selected " + String(pstart + rot_pos - 1) + " " + String(pend));

	virtual void displayMenu(void)
	{
		//      if(debug==1)Serial.println(" pc " + String(para_change) + " ps " + String(para_sel) + " selected " + String(pstart + rot_pos - 1) + " " + String(pend));
		if (myID == VERSION)
		{
			display.clearDisplay();
			display.drawBitmap(0, 0, tmslogo, 128, 64, 1);
			display.display();
			delay(2000); // Pause for 2 seconds
		}

		oled.clear();
		int ml = line1;
		oled.setInvertMode(rot_pos == 0);
		//          oled.setunderlineMode(true);
		oled.setCursor(0, 0);
		if (myID != MAIN)
			oled.write(17);
		oled.print("                   ");
		oled.setCursor(8 + (16 - items[0].length()) * 4, 0);
		oled.print(items[0]);
		for (int i = 1; i < 8 && ml < numitems; i++, ml++)
		{
			oled.setInvertMode(ml == rot_pos);
			setmark(ml, i);
			if (items[ml] == "Back")
			{
				oled.setCursor(0, i);
				oled.write(17);
			}
			oled.setCursor(8, i);
			oled.print(items[ml]);
			if (myID == EXTERNALS && items[ml] != "Back")
			{
				oled.setCursor(100, i);
				oled.printf("%4d", (int)Menus[EXTSETTINGS + ml - 1]->rawvalue);
			}
			//			if (ml == rot_pos)
			//				if(debug==1)Serial.println(String(i) + " ml " + String(ml) + " numitems " + String(numitems) + " " + items[ml]);
		}
		isshowing = myID;
		//		if(debug==1)Serial.println("showing " + items[ml-1]);
	}
	void rotpos(void)
	{
		if (myID == SYNTHSETTINGS || myID == NEWMAP)
			return;
		if (numitems < 9)
		{
			if (rot_pos >= numitems)
				rot_pos = 1;
			if (rot_pos < 0)
				rot_pos = 0;
		}
		else
		{
			if (rot_pos > line1 + 6 && rot_dir > 0)
			{
				if (rot_pos >= numitems)
				{
					rot_pos = numitems - 1;
				}
				else
					line1++;
				//          if(debug==1)Serial.print(" pos:");
				//          if(debug==1)Serial.println(rot_pos);
			}
			if (rot_pos == line1 - 1 && rot_dir < 0 && rot_pos > 0)
			{
				line1--;
			}
			if (rot_pos < 0)
			{
				rot_pos = 0;
			}
		}
		//		if (debug == 1)
		//			Serial.print("line 1" + String(rot_pos));
	}
	void showscale(String item, byte *res, byte nx, bool newsc = false, bool refresh = false)
	{
		static short oldrot = -1, oldnx = -1, oldscb = -1;
		if (nx == oldnx && oldrot == rot_pos && oldscb == scalebase && !refresh)
		{
			display.display();
			return;
		}
		oldnx = nx;
		oldrot = rot_pos;
		oldscb = scalebase;
		display.clearDisplay();
		numitems = nx;
		display.setCursor(6, 1);
		display.write(17);
		display.setCursor(6, 5);
		byte l = item.length();
		byte ml = 21;
		byte pad = (ml - l) / 2;

		display.setCursor(pad * 6, 2);
		display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
		print(item, rot_pos == 0);
#if 0
		if (newsc)
		{
			display.setCursor(120, 8);
			if (rot_pos == 1)
				display.setTextColor(0, 1);
			else
				display.setTextColor(1, 0);
			display.write(15);
			if (rot_pos == 1)
				display.setTextColor(1, 0);
			else
				display.setTextColor(0, 1);
		}
#endif
		byte ypos = 20;
		display.drawFastVLine(0, ypos, 24, 1);
		display.drawFastVLine(126, ypos, 24, 1);
		display.drawFastVLine(125, ypos, 24, 1);
		byte off =25;
		static byte yposold[12] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};
		byte oldyp = 255;
		//		Serial.println(String(res[0]) + " " + String(nx) + " " + String(rot_pos));
		int lp = rot_pos > 1 ? rot_pos - 2 : 0;
		if (nx == 0)
			lp = 0;
		display.setCursor(98, 0);

		if (scaleNames[scid] != "")
		{
			item = scaleNames[scid] + " " + midiNamesFlat[scalebase];
			display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 0);
			l = item.length();
			ml = 21;
			pad = (ml - l) / 2;

			display.setCursor(pad * 6, 2);
			//		display.writeFastHLine(0, 7, 128, 1);
			display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
			print(item, rot_pos == 0);
			scend = true;
		}
		else
		{
			display.setCursor(98, 0);
			print(String(scid), false);
		}
		int nl = 0;
		firstnote = lp;
		for (int i = lp; i < nx && i < lp + 7; i++)
		{
			int n = (res[i]+scalebase-res[0]) % 12;
			int nn = n;
			if(n<nl)
				n +=  12;
			nl = n;
			int x = (i - lp) * 15 + off;
			xpos[i] = x;
			int yn = yposold[nn] + 7 *( n>11?1:0);
			if (!n)
				display.drawFastHLine(x - 2, ypos + 30, 10, 1);
			bool isflat = false;
			if (nn == 1 || nn == 3 || nn == 6 || nn == 8 || nn == 10)
			{
//				off += 5;
//				x += 5;
				isflat = true;
				display.drawBitmap(x-8, ypos + 30 - yn * 3 - 8, flat, 7, 13, 1);
				oldyp = yn;
			}
			else
			{
				if (oldyp == yn)
				{
//					off += 5;
//					x += 5;
					display.drawBitmap(x-6, ypos + 30 - yn * 3 - 6, natural, 5, 13, 1);
				}
			}
			display.fillRoundRect(x, ypos + 30 - yn * 3 - 2, 6, 5, 2, 1);
			//			if (nn < 10)
			display.setCursor(x-(isflat?4:0), ypos + 30 + 5);
			print( midiNamesFlat[nn],false);
			//			else
			//			{
			//				display.drawChar(9 + x, ypos + 30 + 5, '1', 1, 0, 1);
			//				display.drawChar(14 + x, ypos + 30 + 5, nn % 10 + '0', 1, 0, 1);
			//			}
		}

		for (int i = 1; i < 6; i++)
		{
			display.drawFastHLine(0, ypos + 30 - i * 6, 127, 1);
		}
		display.drawBitmap(1, ypos - 5, clef, 16, 36, 1);

		display.display();
	}
	void showmap(String item, short *res, byte mnx, bool newmap = false, bool refresh = false)
	{
		static short orp = -5, lastnum = 12;
		//		static int old_pos = 0;
		//, oldrep = -1, oldtrans = -1;
		//		static int xpos[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		display.display();
		rot_test();
		if (rot_pos < 0)
		{
			rot_pos = 0;
			encoder->setPosition(rot_pos);
		}
		if (rot_pos > lastnum)
		{
			rot_pos = lastnum;
			encoder->setPosition(rot_pos);
		}
		if (orp == rot_pos)
			return;
		//		int pos = scalechords[scid][scalebase][0];
		//		if (rot_pos < 1 && !para_change)
		//			return;
		//		if (pos == old_pos)
		//			return;
		//		old_pos = pos;
		static int rpos = 0;
		String basename = "C";
		//		if (rpos >= 12)
		//			rpos = 11;

		//		if (para_change)
		//			rpos = last_Sel - 1;
		display.clearDisplay();
		display.setCursor(6, 1);
		display.write(17);
		item = scaleNames[scid];
		byte l = item.length();
		byte ml = 21;
		byte pad = (ml - l) / 2;
		numitems = lastnum;
#if 0
		if (newmap && mnx > rpos && psel > 0)
		{
			if (psel == 1)
			{
				if (rot_pos > mlnchord[rpos][0])
				{
					rot_pos = 0;
					encoder->setPosition(rot_pos);
				}
				if (rot_pos < 0)
				{
					rot_pos = mlnchord[rpos][0];
					encoder->setPosition(rot_pos);
				}
				res[rpos] = mlninv[rpos][rot_pos];
				basename = midiNamesFlat[mlnchord[rpos][rot_pos + 1] % 12];
			}
			if (psel == 2)
			{
				if (rot_pos > 50)
				{
					rot_pos = 0;
					encoder->setPosition(rot_pos);
				}
				if (rot_pos < 0)
				{
					rot_pos = 50;
					encoder->setPosition(rot_pos);
				}
				replchord[inx] = chordIds[rot_pos];
			}
			if (psel == 3)
			{
				if (rot_pos > 11)
				{
					rot_pos = 0;
					encoder->setPosition(rot_pos);
				}
				if (rot_pos < 0)
				{
					rot_pos = 11;
					encoder->setPosition(rot_pos);
				}
				transpose[inx] = rot_pos;
			}
		}
#endif
		display.setCursor(pad * 6, 2);
		//		display.writeFastHLine(0, 7, 128, 1);
		display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
		print(item, rot_pos == 0);
		print(String("  "), false);
		print(String(rot_pos), false);
		print(String(" "), false);
		//		print(String(rpos) + " ", false);
		//		print(String(psel) + " ", false);
		//		print(String(rot_pos) + " ", false);
		//		print(String(mlnchord[rpos][0]));
		display.drawFastVLine(2, 20, 24, 1);
		display.drawFastVLine(124, 20, 24, 1);
		byte numshown = 5;
		byte off = 10;
		//		static byte ypos[12] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};
		//		byte sp = 0;
		//		int beg = rot_pos <3 ? 0 : rot_pos-2;
		//		if (mnx == 1)
		//			beg = 0;
		int r = rot_pos;
		if (r <= rpos || r > rpos + numshown)
		{
			if (r <= rpos)
				rpos--;
			else
				rpos++;
			if (rpos < 0)
				rpos = 0;
		}
		print(String(rpos), false);
		for (int m = rpos; m < lastnum && m < rpos + numshown; m++)
		//		for (int m = 0; m < 6; m++)
		{
			int key = scalechords[scid][scalebase][m];
			int chid = (key % 1000000) / 1000;
			if (chid == 0)
			{
				lastnum = m;
				break;
			}
			Chord *nC = chords[chid];
			byte inv = key % 10;
			//			if (inv != 0&&m==1)
			//			{
			//				Serial.println(String(chid) + " " + String(chords[chid]->note[inv]) + " " + String((key % 1000) / 10));
			//			}
			int nb = ((chords[chid]->note[inv]) + (key % 1000) / 10) % 12;
			//			continue;
			Chord *iC = chords[nC->inverseID[inv]];

			if (replchord[res[m]] != res[m] && 0)
			{
				display.setCursor(off + 13, 56);
				if (transpose[res[m]] > 0)
				{
					if (mapID)
					{
						display.setCursor(off + 13, 56);
						print(String(replchord[res[m]]) + "+" + String(transpose[res[m]]), psel == 2 && m == rpos);
					}
					else
						print(midiNamesFlat[transpose[res[m]]] + cname[replchord[res[m]]], psel == 2 && m == rpos);
				}
				else
				{
					if (mapID)
						print(String(replchord[res[m]]), psel == 1 && m == rpos);
					else
						print("C" + cname[replchord[res[m]]], psel == 1 && m == rpos);
				}
			}

			int je = 1;
			//			if(newmap)
			//				je = 1;
			for (int j = 0; j < je; j++)
			{
				//				short ix = replchord[res[m]];
				if (j == 1)
				{
					nb = transpose[res[m]];
				}

				byte num = iC->num;
				off += 17;
				for (int i = 0; i < num; i++)
				{
					byte n = iC->note[i] + nb;
					if (n == 1 || n == 3 || n == 6 || n == 8 || n == 10)
					{
						off += 5;
						break;
					}
				}
				iC->dodisplay(off, nb, m == r - 1);
			}
			if (mapID)
			{
				display.setCursor(off + 8, 12 + 44 * (m % 2));
				print(String(chid), para_sel && m == r);
			}
			else
			{
				String na = midiNamesFlat[nb] + iC->name.substring(0, 4);
				int oo = na.length() * 2;
				display.setCursor(off - oo, 12 + 44 * (m % 2));
				print(na, para_sel && m == r);
			}
		}
		for (int i = 1; i < 6; i++)
		{
			display.drawFastHLine(2, 50 - i * 6, 122, 1);
		}
		display.drawBitmap(1, 15, clef, 16, 36, 1);
		display.display();
	}
	virtual void show(bool force, int caller)
	{
		static int oldv[6] = {-1, -1, -1, -1, -1, -1};
		static int olde[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
		static int moldv[6] = {-1, -1, -1};
		//		if(debug==1)Serial.println("424 show");
		if (!force)
			rot_test();
		if (myID == NEWSCALE)
			showscale("Find", ln, bnx, true);
		if (myID == NEWMAP)
			showmap("Show Chords", mln, mapnx, true);

		if (myID == SHOWSCALE)
		{
			rot_test();
			if (force)
				rot_pos = 0;
			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && menuState == oldmenuState)
				return;
#if 0
			byte nl[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			scid = 0;
			nx = 1;
			scid = scFP[lastScale]<<1;
			//			scid = scid << 1;
			for (int i = 0; i < 12; i++)
			{
				int test = 1 << i;
				if ((scid & test) > 0)
					nl[nx++] = i;
				//				Serial.println(String(i) + " " + String(test) + " " + String(scid & test) + " " + String(nx));
			}
			scid = scid >> 1;
#endif
			showscale(scaleNames[scFP[lastScale]], ln, nx, true);
#if 0
			if (nx > 1)
			{
				for (int i = 0; i < nx && 0; i++)
				{
					nl[i] = res[i].toInt();
					int n = nl[i] % 12;
					scid |= (1 << n);
				}
				scid = scid >> 1;
				if (scaleNames[scid] != "")
					items[0] = scaleNames[scid];

				showscale(items[0], nl, nx, true);
				//				showscale(594, true);
			}
#endif
			return;
		}
		if (myID == SHOWMAP)
		{
			rot_test();
			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && menuState == oldmenuState)
				return;
			//			oldmenuState = menuState;
			showmap(items[0], maparr, mnx, false);
			return;
		}
		if (!force)
		{
			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && menuState == oldmenuState)
			{
				if (myID == EXTERNALS)
				{
					int ml = line1;
					for (int i = 1; i < 8 && ml < numitems; i++, ml++)
					{
						if (items[ml] != "Back")
						{
							int val = Menus[EXTSETTINGS + ml - 1]->rawvalue;
							if (val != olde[ml - 1])
							{
								oled.setInvertMode(ml == rot_pos);
								oled.setCursor(100, i);
								oled.printf("%4d", val);
								if (ml == rot_pos)
								{
									Serial.println(val);
								}
								olde[ml - 1] = val;
							}
						}
					}
				}
				if (myID == TOUCH)
				{
					int ml = line1;
					for (int i = 1; i < 8 && ml < numitems; i++, ml++)
					{
						if (items[ml] != "Back")
						{
							if (oldv[ml - 1] != (int)Menus[ZONESETTINGS + ml - 1]->rawvalue)
							{
								oldv[ml - 1] = (int)Menus[ZONESETTINGS + ml - 1]->rawvalue;
								oled.setInvertMode(ml == rot_pos);
								oled.setCursor(64, i);
								oled.clearToEOL();
								oled.printf("%4d/%4d", (int)Menus[ZONESETTINGS + ml - 1]->rawvalue, (int)Menus[ZONESETTINGS + ml - 1]->touchSize);
							}
						}
					}
				}
				if (myID == MIDISET)
				{
					for (int i = 0; i < 3; i++)
					{
						int mm = Menus[MIDISETTINGS + i]->sourceNote;
						//							Serial.println(String(i)+" "+String(mm));
						if (moldv[i] != mm)
						{
							oled.setInvertMode(i + 1 == rot_pos);
							oled.setCursor(96, i + 1);
							oled.clearToEOL();
							oled.printf("%4d", mm);
							moldv[i] = mm;
						}
					}
					return;
				}

				return;
			}

			oldmenuState = menuState;
		}
		//     else
		//      if(debug==1)Serial.println("Show it to me");
		displayMenu();
	}
	int hiresval = 0;
	void rot_test()
	{
		encoder->tick();
		int newPos = encoder->getPosition();
		rot_dir = (int)encoder->getDirection();
		//		   if(debug==1)Serial.println(rot_dir);
		//		    if(debug==1)Serial.println(rot_pos);
		if (Menus[SETTINGS]->hiRes && !rot_dir)
		{
			int val = Menus[HIRES]->rawvalue;
			rot_dir = Menus[HIRES]->rawdir;
			newPos = map(val, 1, 10000, 0, 24);
			if (newPos == rot_pos)
				rot_dir = 0;
			if (rot_dir && debug == 1)
			{
				Serial.print("pos:");
				Serial.print(rot_pos);
				Serial.print(" new:");
				Serial.print(newPos);
				Serial.print(" dir:");
				Serial.println(rot_dir);
			}
		}
		if (rot_pos != newPos && rot_dir)
		{
			rot_pos = newPos;
			rotpos();
			encoder->setPosition(rot_pos);
			//			if(debug==1)Serial.print("pos:");
			//			if(debug==1)Serial.print(rot_pos);
			//			if(debug==1)Serial.print(" dir:");
			//			if(debug==1)Serial.println(rot_dir);
		}
	}
	byte insertItem(String pitem, bool newins = false)
	{
		byte ret = numitems;
		if (numitems < 99)
		{
#if 0
			if(debug==1)Serial.print(numitems);
			if(debug==1)Serial.print(" ");
			if(debug==1)Serial.print(items[numitems]);
#endif
			if (!newins)
				items[numitems++] = pitem;
			else
			{
				numitems--;
				String old = items[numitems];
				items[numitems++] = pitem;
				items[numitems++] = old;
				ret = numitems - 1;
			}
#if 0
			if(debug==1)Serial.print("->");
			if(debug==1)Serial.print(items[numitems - 1]);
			if(debug==1)Serial.print(" ");
			if(debug==1)Serial.println(numitems);
#endif
		}
		return ret;
	}

	void setState(signed char first, int num)
	{
		//     if(debug==1)Serial.println(Menus[parent]->items[0] + "****" + items[0]);
		//     if(debug==1)Serial.println(parent + "****" + items[0]);
		int s = 1;
		for (int i = s; i < num + s; i++)
		{
			paraID[i] = first + i - s;
			if (debug == 1)
				Serial.println(String(i) + " " + items[0] + "->" + Menus[paraID[i]]->items[0]);
		}
	}
	void setParent(signed char pparent)
	{
		parent = pparent;
	}
	void setValue(char *pvalue)
	{
		value = pvalue;
	}
	static void setContrast()
	{
		static u_int8_t con[] = {0, 0x60, 0xA0, 0xFF};
		oled.ssd1306WriteCmd(SSD1306_SETCONTRAST); // 0x81
		oled.ssd1306WriteCmd(con[contrast]);
	}

	char *value;
	RotaryEncoder *encoder;
	RotaryEncoder *paraencoder;
	Encoder *myEnc = nullptr;

	int pvalue = 0;
	int oldvalue = -1;
	int rot_pos = 0;
	int pararot_pos = 0;
	int rot_dir = 0;
	int pararot_dir = 0;
	int last_Sel = -1;
	int nump = 0;
	bool para_change = false;
	bool para_sel = false;
	int psel = 0;
	int inx = 0;
	String items[100];
	int numitems = 0;
	int line1 = 1;
	static signed char contrast;
	static signed char actSubmenu;
	static signed char triggerCC;
	static signed char hiRes;
	static signed char isRat;
	static signed char byPass;
	static signed char sendAfter;
	static signed char strum;
	static signed char noPanic;
	static signed char useBPM;
	static signed char ratdiv;
	static float BPM;
	signed char sourceNote = 0;
	short paraID[50];
	short parent;
	signed char myID;
	String name;
	bool inSel = false;
	float rawvalue;
	int rawdir = 0;
	float oldfvalue = -1;
	static short isshowing;
	short emin = 1023;
	short emax = 0;
	ResponsiveAnalogRead *Analog = 0;
	int touchSize = 0;
};

class MenuPara : public Menu
{
public:
	MenuPara(signed char myID, String pname, signed char parent) : Menu(myID, pname, "", parent)
	{
		//      if(debug==1)Serial.println("menuPara " + String(parent));

		nump = 1;
		for (int p = 0; p < 100; p++)
			Paras[p] = 0;
	}

	void displayMenu(void) override
	{
		//		if(debug==1)Serial.println("Menu display");
		Menu::displayMenu();

		//		nump = numitems - 1;
		int ml = line1;
		//		if(debug==1)Serial.println(String(ml) + " / " + String(numitems));

		for (int i = 1; i < 8 && ml < numitems - 1; i++, ml++)
		{
			//				if(debug==1)Serial.println(i);
			if (Paras[ml] && Paras[ml]->name != "Back")
			{
				String oval = Paras[ml]->format();
				oled.setInvertMode(ml == rot_pos);
				oled.setCursor(124 - 6 * oval.length(), i);
				oled.clearToEOL();
				oled.print(oval);
				//					if(debug==1)Serial.println(Paras[ml]->name + " " + oval);
				//					if(debug==1)Serial.println(i);
			}
		}
	}
	void show(bool force, int caller)
	{
		nump = numitems - 1;

		static char oldval = -1;
		//		static float foldval = -1;
		static float foldval = 0;
		if (para_sel && actPara)
		{

			if (!actPara->isFloat)
			{
				pararot_test();
				pvalue = pararot_pos;
				if (oldval == pvalue)
					return;

				oldval = pvalue;
				//			if(debug==1)Serial.println(actPara->name + "=>" + pvalue + " from " + String(caller));
				if (actPara->value)
				{
					*actPara->value = pararot_pos;
					//					if (debug == 1)						Serial.println(*actPara->value);
				}
				if (actPara->name == "Contrast")
				{
					contrast = pvalue;
					setContrast();
				}
				printA4(String(pvalue));
			}
			else
			{
				float val = 0;
				if (Menus[SETTINGS]->hiRes > 0)
				{
					float tv = *actPara->tvalue;
					int setv = (int)fmap(tv, actPara->fvstart, actPara->fvend, 0, 10000);
					//					Serial.println("set enc "+String(tv)+"->"+String(setv));
					Menus[HIRES]->myEnc->write(setv);
					val = Menus[HIRES]->rawvalue;
					val = fmap(val, 0, 10000, actPara->fvstart, actPara->fvend);
				}
				else
				{
					val = Menus[ZONESETTINGS]->rawvalue;
					val = fmap(val, 0, 1792, actPara->fvstart, actPara->fvend);
				}
				if (val == foldval)
					return;
				//				actPara->fvalue = fmap(val, 0, 1792, actPara->fvstart, actPara->fvend);
				*actPara->tvalue = val;
				actPara->fvalue = val;
				foldval = val;
				printA4(String(val));
			}

			int i = rot_pos - line1 + 1;
			oled.setInvertMode(false);
			oled.setCursor(8, i);
			oled.print(items[rot_pos]);
			String oval = actPara->format();
			oled.setCursor(46, i);
			oled.clearToEOL();
			oled.setCursor(124 - 6 * oval.length(), i);
			oled.setInvertMode(true);
			oled.print(oval);
			//      if(debug==1)Serial.println(actPara->name + "=>" + oval + " from " + String(124 - 6 * oval.length()));
			return;
		}
		//		if(debug==1)Serial.println("651 show");
		Menu::show(force, 653);
	}

	void
	onButton(bool cancel) override
	{
		if (debug == 1)
			Serial.println("Para click " + String(rot_pos) + " " + String(para_sel ? "sel" : "unsel"));
		if (myID == NEWMAP && rot_pos > 0)
		{
			para_sel = !para_sel;
			return;
		}
		if (rot_pos - 1 < nump)
		{

			para_change = true;
			para_sel = !para_sel;
			last_Sel = rot_pos;

			if (!Paras[rot_pos])
			{
				Menu::onButton(false);
				return;
			}

			actPara = Paras[rot_pos];
			actPnum = rot_pos;
			if (actPara->value || actPara->isFloat)
			{
				if (!actPara->isFloat)
				{
					if (!cancel)
					{
						Serial.println(actPara->name + " old " + String(oldvalue) + " " + String(ratdiv));
						pvalue = *actPara->value;
						if (oldvalue == -1)
							oldvalue = pvalue;
						Serial.println(actPara->name + " old " + String(oldvalue) + " " + String(ratdiv));
					}
					else
					{
						Serial.println(actPara->name + " next old " + String(oldvalue));
						*actPara->value = oldvalue;
						para_sel = false;
						oldvalue = -1;
					}
				}
				else
				{
					if (!cancel)
					{
						Serial.println(actPara->name + " old " + String(oldfvalue));
						if (oldfvalue == -1)
							oldfvalue = *actPara->tvalue;
						Serial.println(actPara->name + " old " + String(oldfvalue));
					}
					else
					{
						Serial.println(actPara->name + " next old " + String(oldfvalue));
						*actPara->tvalue = oldfvalue;
						printA4(oldfvalue);
						para_sel = false;
						oldfvalue = -1;
					}
				}
			}
			if (para_sel && rot_pos > 0 && actPara->range == 0)
			{
				menuState = actPara->parent;
				//				if(debug==1)Serial.print("670 now :");
				//				if(debug==1)Serial.println(menuState);

				//				if (Menus[menuState])
				//					Menus[menuState]->items[0] = items[0] + "->";
				nextm = menuState;
				para_change = false;
				para_sel = false;

				return;
			}
			//			if(debug==1)Serial.println(items[rot_pos] + " target " + String(actPara->parent) + " me " + String(myID));

			if (para_sel && actPara->value && !actPara->isFloat)
			{
				pararot_pos = pvalue;
				paraencoder->setPosition(pararot_pos);
				int tv = *actPara->value;
				int setv = map(tv, actPara->vstart, actPara->vend, 0, 10000);
				Menus[HIRES]->myEnc->write(setv);
			}
			else
				displayMenu();
		}

		Menu::onButton(false);

		return;
	}
	void pararot_test()
	{
		paraencoder->tick();
		int newPos = paraencoder->getPosition();
		pararot_dir = (int)paraencoder->getDirection();
		if (Menus[SETTINGS]->hiRes && !pararot_dir)
		{
			int val = Menus[HIRES]->rawvalue;
			pararot_dir = Menus[HIRES]->rawdir;
			newPos = map(val, 1, 10000, actPara->vstart, actPara->vend);
			if (newPos == rot_pos)
				pararot_dir = 0;
			if (pararot_dir && debug == 1)
			{
				Serial.print("pos:");
				Serial.print(pararot_pos);
				Serial.print(" new:");
				Serial.print(newPos);
				Serial.print(" dir:");
				Serial.println(pararot_dir);
			}
		}
		if (pararot_pos != newPos && pararot_dir)
		{
			pararot_pos = newPos;

			if (actPara && !actPara->isFloat)
			{
				if (pararot_pos > actPara->vend)
					pararot_pos = actPara->vstart;
				else if (pararot_pos < actPara->vstart)
					pararot_pos = actPara->vend;
			}
			else if (actPara)
			{
				//        if(debug==1)Serial.println(pararot_dir);
				if (pararot_pos > actPara->fvend)
					pararot_pos = actPara->fvstart;
				else if (pararot_pos < actPara->fvstart)
					pararot_pos = actPara->fvend;
			}

			paraencoder->setPosition(pararot_pos);
			//        if(debug==1)Serial.print("pos:");
			//        if(debug==1)Serial.print(pararot_pos);
			//        if(debug==1)Serial.print(" dir:");
			//        if(debug==1)Serial.println(pararot_dir);
		}
	}

	Parameter *getPara(int p)
	{
		if (p > 0 && p < NUMSYNTH)
			return Paras[p];
		return nullptr;
	}

	signed char SynthPara = 0;
	signed char Strum = 0;
	signed char CV1 = 0;
	signed char CV2 = 0;
	static int AudioTG[54];
	static int CV1TG;
	static int CV2TG;
	static int StrumTG;
	Parameter *Paras[100];
	Parameter *actPara = 0;
	signed char actPnum = 0;
	// int nump = 1;
	signed char eventtype = 0; // 0: off,1: on; 2: CC event;3: pitch,4: no event, 5: analog change, 6: distance change, 7: touch event
	int SourceVelocity = 0;
	//   Parameter* pparas[NUMSYNTH + 1];
	static MenuPara *SynthMenu;
};
class MenuTouch : public Menu
{
public:
	MenuTouch(void) : Menu(TOUCH, "Touch Zone")
	{
		nump = 1;
		for (int i = nump; i < nump + 6; i++)
		{
			items[i] = "Zone " + String(i);
		}
		numitems = 7;
	}
};

#include "settings.h"
#include "targets.h"
#include "externals.h"
#include "midisource.h"
#include "zones.h"

#endif
