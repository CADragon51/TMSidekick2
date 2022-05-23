#ifndef MENU_H
#define MENU_H
#include <RotaryEncoder.h>
#include <ResponsiveAnalogRead.h>
#include "Button.h"
#include <Ping.h>
#include <NativeEthernet.h>
#include <chord.h>
#include "Webgui.h"
extern Webgui webgui; // initialize an instance of the class
extern EthernetClient client;
extern IPAddress server;
extern int scidtoix(int sc);
void onOptionSelect(int option, int id);
extern Webgui webgui; // initialize an instance of the class
extern void onSlider(float value, int id);
extern void onSwitches(bool *value, int id);

extern void onButtonPress(int button, int id);

extern void midiSilence(void);
extern void onButtonRelease(int button, int id);

extern void saveTMS(void);
extern short menuId[20];
int nextm = -1;
class Targets;
extern Adafruit_SSD1306 display;
extern float fmap(float, float, float, float, float);
const String paranamemidi[] = {"USB MIDI IN", "HOST MIDI IN", "MIDI IN", "USB MIDI OUT", "MIDI OUT 1", "MIDI OUT 2"};
const String paraname[] = {"Joystick X", "Joystick Y", "Pot1", "Pot2", "Pedal 1", "Pedal 2", "FSR", "Hi-Res", "Switch"};
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

extern int SplitS(String what, char where, String *res, int nmax);

int sout = 4000;

extern Parameter *Paras[];
int dpos;
int r;
byte fnote = 0;
byte g_ln[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short mln[12] = {72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72};
// short mlninv[12][5];
//  short actinv[12];
// byte mlnchord[12][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
int bnx = 0;
int mapnx = 0;
extern int sfx;
int g_nx = 0;

class Menu
{
public:
	Menu(signed char pmyID, String pname, String pitems = "", signed char pparent = MAIN)
	{
		//		DBG("menu " + String(pparent));
		//		DBG(items[0] + "***" + String(pmyID));
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
		}

		parent = pparent;
		myID = pmyID;
		for (int i = 0; i < 50; i++)
			paraID[i] = -1;
		isPara = false;
	}
	void setup(String pname)
	{
		encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
		paraencoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
		items[0] = pname;
		rot_pos = 1;
		//		encoder->setPosition(1, __CALLER__);
	}
	void mainSel(int pos)
	{
		int para = rot_pos;
#if 0
		{
			FDBG(String(pos) + " para =");
			FDBG(String(para) + " MSel =" + String(paraID[para]));
		}
#endif
		//    DBG(paraID[para]);
		menuState = paraID[para];
		//		DBG(" 162 now :");
		//		DBG(menuState);

		if (Menus[menuState])
		{
			Menus[menuState]->backstate = myID;
		}
	}
	bool selMenu(void)
	{
		int para = rot_pos;
		//		DBG("+++++ ");
		//		DBG(items[0]);
		//		DBG(" = ");
		//
		{
			DBG("Sel " + items[para] + " ");
			DBG(paraID[para]);
			DBG("maxrepl " + String(maxrepl));
		}
		STACK;
		inSel = !inSel;
		if (rot_pos == 0)
		{
			menuState = parent;
			para_change = false;
			//			DBG("par " + String(parent));
			saveTMS();

			return true;
		}
		STACK;
		if (paraID[para] > -1)
		{
			menuState = paraID[para];
			//			DBG(paraID[para]);

			DBG(" 189 now :" + String(para));
			//			DBG(menuState);

			para_change = false;
			//			DBG("par " + String(parent));
			if (items[para] == "Find Scale")
			{
				menuState = NEWSCALE;
				para_change = false;
//				maxrepl = 0;
//				actkeyidx = 0;
				return true;
			}
			if (items[para] == "Edit Map")
			{
				//				DBG("maxrepl " + String(maxrepl));
				menuState = NEWMAP;
				para_change = false;
				//				saveTMS();
				return true;
			}
			if (items[para] == "Show Map")
			{
				menuState = SHOWMAP;
				actkeyidx = 0;
				para_change = false;
				return true;
			}
			STACK;
			if (items[para] == "Target")
			{
				menuState = paraID[para];
				//				DBG("217 now :");
				//				DBG(menuState);
				para_change = false;
			}
			saveTMS();
			return true;
		}
		else
		{
			DBG(paraID[para]);
		}
		STACK;
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
		//		DBG(items[1]);
		for (int i = 1; i < n; i++)
		{
			String it = ins.substring(p[i - 1] + 1, p[i]);
			//			DBG(it);
			//        items[i + 1] = ins.substring(p[i - 1] + 1, p[i]);
			items[i + 1] = it;
		}
		return n + 1;
	}
	void mapit(String from)
	{
		mapit(lastMap, from);
	}
	void mapit(void)
	{
		maxrepl = 11;
		actkeyidx = 0;
		for (int i = 0; i < maxrepl; i++)
		{
			keychord[i] = chordIds[i] * 100;
			replchord[i] = chordIds[i];
			transpose[0] = 0;
		}
		lastMap = 3;
		DBG("mapped " + SN(numitems));
	}
	void mapit(int para, String from)
	{
		FDBG("mapit " + String(para) + " ::::: " + items[para] + " " + SN(numitems) + " " + from);
		refresh = true;
		if (para == 3)
		{
			mapit();
			return;
		}
		if (para < numitems)
		{
//			loadMap(filenames[para]);
//			displayMenu(__CALLER__);
		}
	}
	void scaleit(int _scid)
	{
		static int oldscid =-100;
		static byte sb = 199;
		if (_scid == oldscid && scalebase == sb)
			return;
		oldscid = _scid;
		sb = scalebase;
		_scid = _scid << 1;
		g_nx = 1;
//		FDBG("scaleit" + SN(oldscid)+SN(scalebase));
		refresh = true;
		inscale[sb] = sb;
		//		FDBG("In Scale: " + midiNamesFlat[(0) % 12] + " " + midiNamesFlat[(sb) % 12]);
		for (int i = 1; i < 12; i++)
		{
			int test = 1 << i;
			inscale[(i + sb) % 12] = -1;
			//			anx[i] = g_nx - 1;
			if ((_scid & test) > 0)
			{
				g_ln[g_nx++] = (i) % 12;
				inscale[(i + sb) % 12] = (i + sb) % 12;
				//				anx[i] = g_nx - 1;
			}
		}
		//		for (int i = 0; i < 12; i++)
		//			FDBG("In Scale: " + midiNamesFlat[i] + " " + SN(inscale[i]));

		for (int i = 0; i < 12; i++)
		{
			int delta = 99;
			int di = 0;
			if (inscale[i] != -1)
				continue;
			for (int j = 0; j < 12; j++)
			{
				if (inscale[j] == j)
				{
					int d = abs(i - j);
					if (d < delta)
					{
						di = j;
						delta = d;
					}
				}
			}
			inscale[i] = (di) % 12;
			//				FDBG("Scale, " + midiNamesFlat[(i) % 12] + "," + SN(delta) + "," + midiNamesFlat[inscale[i]]);
		}
		_scid = oldscid;
		DBG("g_scid ");
		DBG(_scid);
		g_scid = _scid;
		if (_scid == 2047)
		{
			Menus[MAPPINGS]->mapit();
			return;
		}
#if 0
		int o = 0;
		for (int i = 0, j = 0; i < 128 && o < 11 && g_nx < 10; i++)
		{
			byte n = i % 12;
			if (n == 1 || n == 3 || n == 6 || n == 8 || n == 10)
				basescale[i] = basescale[i - 1];
			else
			{
				basescale[i] = g_ln[j] + scalebase + o * 12;

				DBG(String(j) + ": " + String(i) + " ->" + String(basescale[i]));

				DBG(" " + midiNamesFlat[n] + String(i / 12) + " ->" + midiNamesFlat[g_ln[j]] + " " + String(o));
				j++;
				if (j >= g_nx)
				{
					j = 0;
					o++;
				}
			}
		}
		//		displayMenu(__CALLER__);
#endif
	}
	void selNewMenu(void)
	{
		//
		DBG("myID " + String(myID) + " " + String(rot_pos));
		STACK;
		switch (myID)
		{

		case PRESETS:
			preindex = rot_pos;
			//           saveTMS();
			displayMenu(__CALLER__);
			break;
		case MAPPINGS:
			if (rot_pos > 2)
			{
				//				DBG("myID " + String(lastMap) + " " + String(rot_pos));

				if (lastMap != rot_pos)
				{
					mapit(rot_pos, __CALLER__);
				}
				menuState = SHOWMAP;
				//				DBG("mapit " + String(menuState));
				Menus[menuState]->items[0] = items[rot_pos];
				Menus[menuState]->show(true, __CALLER__);
			}
			else
				selMenu();
			break;
		case SCALES:
			if (rot_pos > 2)
			{
				//				if (lastScale != rot_pos)
				//					scaleit(rot_pos);
				menuState = SHOWSCALE;
				g_scid = scFP[rot_pos];
				Menus[menuState]->items[0] = items[rot_pos];
				Menus[menuState]->show(true, __CALLER__);
				refresh = true;
			}
			else
				selMenu();
			break;
		case MAIN:
			mainSel(1);
			break;
		case EXTERNALS:
		case TOUCH:
		case MIDISET:
			mainSel(rot_pos);
			break;
		case NEWMAP:
			if (rot_pos == 0 && !para_change)
				selMenu();
			else
			{
				if (psel == 0)
				{
					para_change = true;
					psel = 1;
					last_Sel = rot_pos;
					//					DBG(String(replchord[actkeyidx]) + " ixchord " + String(chordIds[0]) + " actkeyidx " + String(actkeyidx));
					Chord *cc = chords[replchord[actkeyidx]];
					//					DBG("cc " + cc->name);
					rot_pos = cc->_cindex + 1;
					//					DBG("ix " + String(rot_pos) + " " + String(replchord[actkeyidx]) + " ixchord " +
					//								   String(chordIds[cc->_cindex - 1]) + " " + String(chordIds[cc->_cindex]) + " " + String(chordIds[cc->_cindex + 1]) + " " + String(actkeyidx));
					//					DBG("ix " + String(cc->_cindex) + " " + String(replchord[actkeyidx]) + " ixchord " +
					//								   String(chordName[chordIds[cc->_cindex - 1]]) + " " + String(chordName[chordIds[cc->_cindex]]) + " " + String(chordName[chordIds[cc->_cindex + 1]]) + " " + String(actkeyidx));
				}
				else if (psel == 1)
				{
					para_change = true;
					psel = 2;
					rot_pos = transpose[actkeyidx] + 1;
					//					DBG("myID " + String(myID) + " " + String(para_change ? "selected" : "not selected") + " psel " + String(psel) + " actkeyidx " + String(actkeyidx));
				}
				else if (psel == 2)
				{
					para_change = false;
					psel = 0;
					rot_pos = actkeyidx + 1;
				}

				//				encoder->setPosition(rot_pos, _CALLER_);
				//				DBG("myID " + String(rot_pos) + " " + String(para_change ? "selected" : "not selected") + " psel " + String(psel) + " actkeyidx " + String(actkeyidx));
			}
			break;
		default:
			selMenu();
		}
		//		rot_pos = rp;
	}
	virtual void restore(File frec) {}
	virtual void onButton(bool cancel)
	{
		para_change = false;
		//
		FDBG("ID " + String(myID) + " items " + items[rot_pos] + "@" + String(rot_pos) + " " + items[0]);
		dpos = 0;
		r = 1;
		if (myID == NEWSCALE || myID == SHOWSCALE || myID == SHOWMAP)
		{
			//			DBG("back -> parent " + String(parent));
			menuState = parent;
			Menus[menuState]->show(true, __CALLER__);
			if (myID == NEWSCALE)
			{
				bnx = 0;
				g_ln[0] = 0;
			}
			return;
		}
		if (myID == PRESETS)
		{
			DBG("Open " + items[rot_pos] + ".PRS");
			if (SD.exists(presetnames[rot_pos].c_str()))
			{
				File frec = SD.open(presetnames[preindex].c_str(), FILE_READ);
				if (frec)
				{
					//
					DBG("loading preset data ");
					AudioNoInterrupts();
					Menus[SYNTHSETTINGS]->restore(frec);
					AudioInterrupts();
					//
					DBG("data loaded");
					frec.close();
				}
			}
		}
		STACK;
		if (rot_pos == 0)
		{
			//       para_change = false;
			//
			//			DBG("back -> parent " + String(parent));
			//       DBG(mstate[ parent]);
			//			DBG("Back to");
			//			DBG(parent);
			//			setoled();
			// setContrast();
			menuState = parent;
			rot_pos = Menus[parent]->rot_pos;
			//		DBG("318 now :");
			//			DBG(menuState);
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
		STACK;
		//		DBG("326 now :");
		//		DBG(menuState);
		if (menuState > 90)
			menuState = 0;
		//		setoled();
		//		setContrast();
		STACK;
		displayMenu(__CALLER__);
		//     DBG(" pc " + String(para_change) + " ps " + String(para_sel) + " selected " + String(pstart + rot_pos - 1) + " " + String(pend));
	}
	void setmark(int ml, int i)
	{
		if ((myID == SCALES && scidtoix(g_scid) + 3 == ml) ||
			(myID == MAPPINGS && lastMap == ml) ||
			(myID == PRESETS && preindex == ml))
		{
			display.setCursor(0, i * 8);
			display.write(3);
			//			print("*", false, __CALLER__);
		}

	} //     DBG(" pc " + String(para_change) + " ps " + String(para_sel) + " selected " + String(pstart + rot_pos - 1) + " " + String(pend));
	bool printmenu = false;
	virtual void displayMenu(String line)
	{
		//      DBG(" pc " + String(para_change) + " ps " + String(para_sel) + " selected " + String(pstart + rot_pos - 1) + " " + String(pend));
		//		if (isshowing == 2)
		//			return;

		display.clearDisplay();
		int ml = line1;
		display.setCursor(0, 0);
		setInvertMode(rot_pos == 0);
		if (myID != MAIN)
			display.write(17);
		clearToEOL();
		display.setCursor(8 + (16 - items[0].length()) * 4, 0);
		display.print(items[0]);

		for (int i = 1; i < 8 && ml < numitems; i++, ml++)
		{
			//			setInvertMode(ml == rot_pos);
			setmark(ml, i);
			display.setCursor(8, i * 8);
			//			DBG("setcursor " + String(8 ) + "," + String(i));
			setInvertMode(ml == rot_pos);
			display.print(items[ml]);
			if (myID == EXTERNALS)
			{
				display.setCursor(100, i * 8);
				setInvertMode(ml == rot_pos);
				display.printf("%4d", (int)Menus[EXTSETTINGS + ml - 1]->rawvalue);
			}
			//			if (ml == rot_pos)
			//				DBG(String(i) + " ml " + String(ml) + " numitems " + String(numitems) + " " + items[ml]);
		}
		isshowing = myID;
		display.display();
		//		DBG("showing " + items[ml-1]);
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
				//          DBG(" pos:");
				//          DBG(rot_pos);
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
		//
		//			DBG("line 1" + String(rot_pos));
	}
	void showscale(String item, byte *res, byte nx, bool newsc = false, bool refresh = false)
	{
#if 0
		static short oldrot = -1, oldnx = -1, oldscb = -1, oldscid = -1;

		if (nx == oldnx && oldrot == rot_pos && oldscb == scalebase && oldscid == g_scid && !refresh)
		{
			display.display();
			return;
		}
		oldnx = nx;
		oldrot = rot_pos;
		oldscb = scalebase;
		oldscid = g_scid;
#endif
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
//		print(item, rot_pos == 0);
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
		byte off = 25;
		static byte yposold[12] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};
		byte oldyp = 255;
		//		DBG(String(res[0]) + " " + String(nx) + " " + String(rot_pos));
		int lp = rot_pos > 1 ? rot_pos - 2 : 0;
		if (nx == 0)
			lp = 0;
		display.setCursor(98, 0);

		item = scaleNames[g_scid] + " " + midiNamesFlat[scalebase];
		display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 0);
		l = item.length();
		ml = 21;
		pad = (ml - l) / 2;

		display.setCursor(pad * 6, 2);
		//		display.writeFastHLine(0, 7, 128, 1);
		display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
		print(item, rot_pos == 0, __CALLER__);
		scend = true;

		int nl = 0;
		firstnote = lp;
		for (int i = lp; i < nx && i < lp + 7; i++)
		{
			int n = (res[i] + scalebase - res[0]) % 12;
			int nn = n;
			if (n < nl)
				n += 12;
			nl = n;
			int x = (i - lp) * 15 + off;
			xpos[i] = x;
			int yn = yposold[nn] + 7 * (n > 11 ? 1 : 0);
			if (!n)
				display.drawFastHLine(x - 2, ypos + 30, 10, 1);
			bool isflat = false;
			if (nn == 1 || nn == 3 || nn == 6 || nn == 8 || nn == 10)
			{
				//				off += 5;
				//				x += 5;
				isflat = true;
				display.drawBitmap(x - 8, ypos + 30 - yn * 3 - 8, flat, 7, 13, 1);
				oldyp = yn;
			}
			else
			{
				if (oldyp == yn)
				{
					//					off += 5;
					//					x += 5;
					display.drawBitmap(x - 6, ypos + 30 - yn * 3 - 6, natural, 5, 13, 1);
				}
			}
			display.fillRoundRect(x, ypos + 30 - yn * 3 - 2, 6, 5, 2, 1);
			//			if (nn < 10)
			display.setCursor(x - (isflat ? 4 : 0), ypos + 30 + 5);
			print(midiNamesFlat[nn], false, __CALLER__);
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
	void showmap(bool setit = false)
	{
		display.clearDisplay();
		display.setCursor(6, 1);
		display.write(17);
		numitems = maxrepl + 1;
		display.drawFastVLine(2, 20, 24, 1);
		display.drawFastVLine(124, 20, 24, 1);
		// int mscid = 2047;
		// int mscalebase = 0;
		// if (lastMap > 3)
		// {
		// 	mscid = filenames[lastMap].substring(1).toInt();
		// 	mscalebase = mscid % 100;
		// 	mscid = mscid / 100;
		// }
		byte off = 0;
		short llimits[3] = {0, 1, 1};
		int ulimits[3] = {maxrepl + 1, 53, 12};
		if (setit)
		{
			rot_pos = actkeyidx + 1;
			psel = 0;
		}
		if (rot_pos < llimits[psel])
		{
			rot_pos = ulimits[psel];
			encoder->setPosition(rot_pos, __CALLER__);
		}
		if (rot_pos > ulimits[psel])
		{
			rot_pos = llimits[psel];
			encoder->setPosition(rot_pos, __CALLER__);
		}
		String item = scaleNames[g_scid] + " " + midiNamesFlat[scalebase];
		//		String item = scaleNames[g_scid] + " " + String(maxrepl) + " " + String(actkeyidx);
		byte l = item.length();
		byte ml = 21;
		byte pad = (ml - l) / 2;

		if (maxrepl > 0 && psel == 0 && rot_pos > 0 && !setit)
		{
			if (rot_pos <= maxrepl)
				actkeyidx = rot_pos - 1;
		}
		if (actkeyidx < maxrepl && maxrepl > 0)
		{
			int ki = keychord[actkeyidx];

			Chord *inC = chords[ki / 100];
			Chord *iC = inC;
			int nb = ki % 100;
			if (rot_pos > 0 && rot_pos < 53 && psel == 1)
			{
				replchord[actkeyidx] = chordIds[rot_pos - 1];
			}
			if (rot_pos > 0 && rot_pos < 13 && psel == 2)
			{
				STACK;
				transpose[actkeyidx] = rot_pos - 1;
			}

			if (inC != nullptr)
			{

				int je = 2;
				//			if(newmap)
				//				je = 1;
				for (int j = 0; j < je; j++)
				{
					//				short ix = replchord[res[m]];
					if (j > 0)
					{
						iC = chords[replchord[actkeyidx]];
						nb += transpose[actkeyidx];
					}
					byte num = iC->num;
					off += 34;
					//					DBG(String(actkeyidx) + " / " + String(maxrepl) + " " + String(nb) + " " + String(num));

					for (int i = 0; i < num; i++)
					{
						byte n = (iC->note[i] + nb) % 12;
						//						DBG(String(i) + " / " + String(n) + " " + String(nb) + " " + String(num));
						if (n == 1 || n == 3 || n == 6 || n == 8 || n == 10)
						{
							off += 5;
							break;
						}
					}

					iC->dodisplay(off, nb, psel > 0 && j == 1);
#if 1
					if (mapID)
					{
						display.setCursor(off + 8, 12 + 44);
						print(String(iC->_scid), psel > 0 && j == 1, __CALLER__);
					}
					else
					{
						String na = midiNamesFlat[(nb) % 12] + iC->name;
						int oo = na.length() * 2;
						display.setCursor(off - oo - 15, 12 + 44);
						if (!j)
							print(String(actkeyidx + 1), false, __CALLER__);
						display.setCursor(off - oo, 12 + 44);
						print(na, psel > 0 && j == 1, __CALLER__);
					}
#endif
				}
			}
		}
		display.setCursor(pad * 6, 2);
		display.drawRect(pad * 6 - 2, 0, l * 6 + 3, 11, 1);
		print(item, rot_pos == 0, __CALLER__);

		for (int i = 1; i < 6; i++)
		{
			display.drawFastHLine(2, 50 - i * 6, 122, 1);
		}
		display.drawBitmap(1, 15, clef, 16, 36, 1);
		display.display();
	}

	virtual void show(bool force, String caller)
	{
		static int oldv[6] = {-1, -1, -1, -1, -1, -1};
		static int olde[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
		static int moldv[6] = {-1, -1, -1};
		//	if(myID!=0)
		//				DBG("show "+String(myID));
		if (!force)
			rot_test();
		if (myID == NEWSCALE)
			showscale("Find", g_ln, bnx, true);
		if (myID == NEWMAP)
			showmap();

		if (myID == SHOWSCALE)
		{
			rot_test();
			//			DBG("show " + String(myID));
			if (force)
				rot_pos = 0;
			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && menuState == oldmenuState)
				return;
			int nscid = g_scid << 1;
			//			DBG(lastScale);
			g_nx = 1;
			//			g_scid = g_scid << 1;
			for (int i = 0; i < 12; i++)
			{
				int test = 1 << i;
				if ((nscid & test) > 0)
					g_ln[g_nx++] = i;
				//				DBG(String(i) + " " + String(test) + " " + String(nscid & test) + " " + String(nx));
			}
			//			DBG(scaleNames[scFP[lastScale]]);
			//			printA4(SN(g_scid));
			showscale(scaleNames[g_scid], g_ln, g_nx, true);
#if 0
			if (nx > 1)
			{
				for (int i = 0; i < nx && 0; i++)
				{
					nl[i] = res[i].toInt();
					int n = nl[i] % 12;
					g_scid |= (1 << n);
				}
				g_scid = g_scid >> 1;
				if (scaleNames[g_scid] != "")
					items[0] = scaleNames[g_scid];

				showscale(items[0], nl, nx, true);
				//				showscale(594, true);
			}
#endif
			return;
		}
		if (myID == SHOWMAP)
		{
			rot_test();
			//			if (rot_dir == (int)RotaryEncoder::Direction::NOROTATION && menuState == oldmenuState)
			//				return;
			//			oldmenuState = menuState;
			showmap();
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
						int val = Menus[EXTSETTINGS + ml - 1]->rawvalue;
						//					FDBG(Menus[EXTSETTINGS + ml - 1]->name);
						if (val != olde[ml - 1])
						{
							display.setCursor(100, i * 8);
							setInvertMode(ml == rot_pos);
							display.printf("%4d", val);
							if (ml == rot_pos && 0)
							{
								DBG(val);
							}
							olde[ml - 1] = val;
						}
					}
				}
				if (myID == TOUCH)
				{
					int ml = line1;
					for (int i = 1; i < 8 && ml < numitems; i++, ml++)
					{
						if (oldv[ml - 1] != (int)Menus[ZONESETTINGS + ml - 1]->rawvalue)
						{
							oldv[ml - 1] = (int)Menus[ZONESETTINGS + ml - 1]->rawvalue;
							//								DBG("Touch " + String(ml) + " " + String(oldv[ml - 1]));
							display.setCursor(64, i * 8);
							setInvertMode(ml == rot_pos);
							clearToEOL();
							display.printf("%4d/%4d", (int)Menus[ZONESETTINGS + ml - 1]->rawvalue, (int)Menus[ZONESETTINGS + ml - 1]->touchSize);
						}
					}
					display.display();
				}
				if (myID == MIDISET)
				{
					for (int i = 0; i < 3; i++)
					{
						int mm = Menus[MIDISETTINGS + i]->sourceNote;
						//							DBG(String(i)+" "+String(mm));
						if (moldv[i] != mm)
						{
							display.setCursor(96, 8 * (i + 1));
							setInvertMode(i + 1 == rot_pos);
							clearToEOL();
							display.printf("%4d", mm);
							moldv[i] = mm;
						}
					}
					display.display();
					return;
				}
				nodisplay = true;
				return;
			}
		}
		//		DBG("Show it to me");

		oldmenuState = menuState;
		displayMenu(caller);
		nodisplay = false;
	}
	int hiresval = 0;
	void rot_test()
	{
		encoder->tick();
		int newPos = encoder->getPosition(__CALLER__);
		rot_dir = (int)encoder->getDirection();
		if (rot_pos < 0)
		{
			DBG("get pos");
			DBG(newPos);
			rot_pos = newPos;
		}
		if (Menus[SETTINGS]->hiRes && !rot_dir)
		{
			int val = Menus[HIRES]->rawvalue;
			rot_dir = Menus[HIRES]->rawdir;
			newPos = map(val, 1, 10000, 0, 24);
			if (newPos == rot_pos)
				rot_dir = 0;
			if (rot_dir && 0)
			{
				DBG("pos:");
				DBG(rot_pos);
				DBG(" new:");
				DBG(newPos);
				DBG(" dir:");
				DBG(rot_dir);
			}
		}
		if (rot_pos != newPos && rot_dir)
		{
			rot_pos = newPos;
			rotpos();
			encoder->setPosition(rot_pos, __CALLER__);
			//			DBG("pos:");
			//			DBG(rot_pos);
			//			DBG(" dir:");
			//			DBG(rot_dir);
		}
	}
	byte insertItem(String pitem)
	{
		byte ret = numitems;
		if (numitems < 99)
		{
			items[numitems++] = pitem;
		}
		return ret;
	}

	void setState(signed char first, int num)
	{
		//     DBG(Menus[parent]->items[0] + "****" + items[0]);
		//     DBG(parent + "****" + items[0]);
		int s = 1;
		for (int i = s; i < num + s; i++)
		{
			paraID[i] = first + i - s;

			DBG(String(i) + " " + items[0] + "->" + Menus[paraID[i]]->items[0]);
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
	String setMenu(void)
	{
		FDBG(__CALLER__ + " " + SN(myID));
		//		if (myID == omyId && !force)
		//			return "";
		// omyId = myID;
		String outmen = "";
		bool link = true;
		for (int m = 1; m < numitems; m++)
		{
			//			FDBG(items[m] + " " + SN(paraID[m])+" "+SN(myID)+" "+SN(SYNTHSETTINGS));

			if (paraID[m] == -1)
			{
				outmen = outmen + "<div><span>" + items[m] + "</span></div>";
				link = false;
			}
		}
		if (link || myID == MAIN)
			setLinkMenu();
		return outmen;
	}
	void setLinkMenu(void)
	{
		//		static int omyId = -1;
		//		FDBG(SN(myID) + " " + SN(omyId));
		//		if (myID == omyId && !force)
		//			return "";
		//		omyId = myID;
		for (int i = 0; i < 20; i++)
		{
			if (menuId[i] > 0)
				webgui.remove(i);
			menuId[i] = -1;
		}
		for (int m = 1, k = 1; m < numitems; m++)
		{
			if (paraID[m] > -1)
			{
				if (myID == TOUCH)
				{
					menuId[m] = webgui.addButtons(items[m], &onButtonRelease, 665 + (k++) * 60, 365, "f", "hide");
				}
				//				else
				//					menuId[m] = webgui.addButtons(items[m], &onButtonRelease, 200, 110 + (k++) * 25, "f", "hide");
			}
		}
	}
	virtual void createControl(String) {}
	virtual void checkControl(int type, int id, int select = 0, float value = 0, bool swval = false) {}

	char *value;
	static RotaryEncoder *encoder;
	static RotaryEncoder *paraencoder;
	static Encoder *myEnc;
	signed char basescale[128];
	int pvalue = 0;
	int oldvalue = -1;
	int rot_pos = 0;
	int pararot_pos = 0;
	int rot_dir = 0;
	int pararot_dir = 0;
	int last_Sel = -1;
	int backstate = 0;
	int nump = 0;
	bool para_change = false;
	bool para_sel = false;
	int psel = 0;
	String items[100];
	int numitems = 0;
	int line1 = 1;
	static signed char contrast;
	static signed char actSubmenu;
	static signed char triggerCC;
	static signed char hiRes;
	static signed char isRat;
	static signed char procMode;
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
	bool isPara = false;
	ResponsiveAnalogRead *Analog = 0;
	int touchSize = 0;
};

#include "paramenu.h"
#endif