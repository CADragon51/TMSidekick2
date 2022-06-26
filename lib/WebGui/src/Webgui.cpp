#include <NativeEthernet.h>
#include "Webgui.h"
#define DBG(x) Serial.println(x)
#define SN(x) String(x)
extern int colidx;
extern int fq;
extern int eq;
extern int commas[20];
extern int commidx;
extern int bitidx;
extern char bitstream[1000000];
EXTMEM action_generic_t act[1000];
EXTMEM String lastout[1000];
// define GUISTACK Serial.println(__LINE__)
#define GUISTACK
Webgui::Webgui()
{
	numactions = 0;
	idcounter = 1;
	_lastConnectionAttemptTime = 0;
	_connectconsecutivefails = 0;
	_ethernetclient = NULL;
}

int Webgui::_getId()
{
	return idcounter++;
}

bool Webgui::_connect()
{
	return this->connect(_ethernetclient, _server);
}

void Webgui::_print(String str)
{
	_ethernetclient->print(str);
}

void Webgui::_println(String str)
{
	_ethernetclient->println(str);
}

void Webgui::_println()
{
	_ethernetclient->println();
}

bool Webgui::connect(EthernetClient *client, IPAddress host)
{
	_ethernetclient = client;
	_server = host;
	if (millis() - _lastConnectionAttemptTime < 5000)
	{
		Serial.println(F("Waiting some time before connecting to webgui server."));
		delay(1000);
		return false;
	}
	_lastConnectionAttemptTime = millis();
	int status = (_ethernetclient)->connect(_server, TCPPORT);
	if (status)
	{
		_ethernetclient->println(); // send some data in order to avoid arduino gets crazy
		_connectconsecutivefails = 0;
		return true;
	}
	Serial.println(F("It could not connect to webgui server."));
	return false;
}
// EXTMEM String myStream;

bool Webgui::connected()
{
	return _ethernetclient->connected() ? true : false;
}
void Webgui::update()
{
	if (!this->connected())
	{
		if (!this->_connect())
		{
			return;
		}
	}
	while (_ethernetclient->available())
	{
		char c = _ethernetclient->read();
		if (c == ':')
			colidx = bitidx;
		if (fq == 0 && c == '`')
			fq = bitidx;
		else if (fq > 0 && c == '`')
			eq = bitidx;
		if (c == ',')
			commas[commidx++] = bitidx;
		if (c != '\n')
			bitstream[bitidx++] = c;
		//		Serial.print(c);
	}

	if (bitidx > 0)
	{
		if (bitidx > 1000)
		{
			Serial.print(bitidx);
			Serial.print(" ");
			Serial.print(commidx);
			Serial.print(" ");
			Serial.print(colidx);
			Serial.print(" ");
			Serial.print(fq);
			Serial.print(" ");
			Serial.print(eq);
			Serial.print(" ");
			for (int i = 0; i < commidx; i++)
			{
				Serial.print(commas[i]);
				Serial.print(" ");
			}
			Serial.println();
		}
		if (eq + fq == 0 || eq * fq > 0)
		{
			bitstream[bitidx++] = 0;
			bitstream[colidx] = 0;
			//			Serial.println();
			//			Serial.println(bitstream);
			if (!strncmp(bitstream, "ACTION", 6))
				this->_analyzeStream();
			colidx = 0;
			fq = 0;
			eq = 0;
			commidx = 0;
			bitidx = 0;
		}
	}
}
EXTMEM String value;
void Webgui::_analyzeStream()
{
	int lc = colidx + 1;
	String para[20];
	for (int i = 0; i < commidx; i++)
	{
		bitstream[commas[i]] = 0;

		//		Serial.println(SN(i) + " " + String(bitstream + lc));
		para[i] = String(bitstream + lc);
		lc = commas[i] + 1;
	}

	int sid = para[0].toInt();
	//Serial.println("sid " + SN(sid));
	if (bitidx < 100)
	{
		if (eq)
			bitstream[eq] = 0;
		if (fq)
			value = String(bitstream + fq + 1);
		else
			value = String(bitstream + commas[0] + 1);
		//		Serial.println("val " + value);
	}
	else
		value = "###";
	//	Serial.println(value);
	this->_callAction(sid);
}

void Webgui::_callAction(int id)
{
	int valueInt = value.toInt();
	float valueFloat = value.toFloat();

	{
		for (int i = 0; i < numactions; ++i)
		{
			if (id == actionsList[i])
			{
				if (act[id].type == INTEGER)
				{
					((CallbackTypeInt)act[id].fnAction)(valueInt, id);
					return;
				}
				else if (act[id].type == FLOAT)
				{
					((CallbackTypeFloat)act[id].fnAction)(valueFloat, id);
					return;
				}
				else if (act[id].type == STRING)
				{
					((CallbackTypeCharp)act[id].fnAction)(value, id);
					return;
				}
				if (act[id].type == BOOLEANS)
				{
					String res[30];
					int numpipes = SplitS(value, '|', (String *)res, 30);
					bool valueBoolp[30];
					for (int boolindex = 0; boolindex < numpipes; boolindex++)
					{
						valueBoolp[boolindex] = false;
						if (res[boolindex] == "on")
							valueBoolp[boolindex] = true;
					}
					((CallbackTypeBoolp)act[id].fnAction)(valueBoolp, id);
					return;
				}
			}
		}
	}
}

void Webgui::_addAction(int id, unsigned char type, void *fnAction)
{
	if (numactions >= MAXACTIONS)
	{
		Serial.println(F("There are too many actions. It could not save more actions"));
		return;
	}

	act[id].id = id;
	act[id].type = type;
	act[id].fnAction = fnAction;
	actionsList[numactions % MAXACTIONS] = id;
	numactions++;
}

int Webgui::addButtons(String name, unsigned char numbuttons, String *buttons, CallbackTypeInt fnMouseUp /*click*/, int x, int y, String title, String classname)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	if (!numbuttons)
	{
		Serial.println("id error @" + String(__LINE__));
		return -1;
	}
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_CONTROL:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_print("`,buttons,");
	for (unsigned char i = 0; i < numbuttons; i++)
	{
		_print("`");
		_print(buttons[i]);
		_print("`");
		if (i != numbuttons - 1)
		{
			_print(",");
		}
	}
	_println();
	_addAction(id, INTEGER, (void *)fnMouseUp);

	if (id < 0)
		DBG(name + " " + SN(id));
	return id;
}
int Webgui::addButtons(String name, CallbackTypeInt fnMouseUp /*click*/, int x, int y, String title, String classname)
{
	String sid;
	int id;
	String bname = name + "," + String(x) + "," + String(y) + "," + title + "," + classname;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_CONTROL:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(bname);
	_print("`,buttons,");
	_print("`");
	_print(name);
	_print("`");
	_println();
	_addAction(id, INTEGER, (void *)fnMouseUp);

	if (id < 0)
		Serial.println("id error @" + String(__LINE__));

	return id;
}
int Webgui::addSwitches(String name, unsigned char numswitches, bool *switches, CallbackTypeBoolp fnAction, int x, int y, String title, String classname)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	if (!numswitches)
	{
		Serial.println("id error @" + String(__LINE__));
		return -1;
	}
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_CONTROL:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_print("`,switches,");
	for (unsigned char i = 0; i < numswitches; i++)
	{
		if (switches[i])
		{
			_print("on");
			//			Serial.print("on");
		}
		else
		{
			_print("off");
			//			Serial.print("off");
		}
		if (i != numswitches - 1)
		{
			_print(",");
			//			Serial.print(",");
		}
	}
	_println();
	//	Serial.println(numswitches);

	_addAction(id, BOOLEANS, (void *)fnAction);
	if (id < 0)
		Serial.println(name + " " + SN(id));
	return id;
}
int Webgui::addOptions(String name, int numoptions, String *options, CallbackTypeInt fnAction, int x, int y, int sel, String title, String classname)
{
	String sid;
	int id;
	bool deb = false;
	if (name == "instr")
		deb = true;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname + "," + String(sel);
	if (!numoptions)
	{
		Serial.println("id error @" + String(__LINE__) + " " + name + " " + SN(numoptions));
		return -1;
	}
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_CONTROL:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	if (!deb)
		_print("`,options,");
	else
		_print("`,options127,");
	//	if (deb)
	//		Serial.print("`,options,");

	for (unsigned char i = 0; i < numoptions && !deb; i++)
	{
		_print("`");
		_print(options[i]);
		_print("`");
		if (i != numoptions - 1)
		{
			_print(",");
		}
	}
	_println();
	_addAction(id, INTEGER, (void *)fnAction);
	return id;
}
int Webgui::addInputAnalog(String name, float minvalue, float maxvalue, float defaultvalue, CallbackTypeFloat fnAction, int x, int y, String title, String classname)
{
	String sid;
	int id;
	if (defaultvalue > maxvalue || defaultvalue < minvalue)
	{
		defaultvalue = minvalue;
		Serial.println("id error @" + String(__LINE__) + " " + name);
		//		return -1;
	}
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_CONTROL:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_print("`,analog,");
	_print(SN(minvalue));
	_print(",");
	_print(SN(maxvalue));
	_print(",");
	_println(SN(defaultvalue));
	_addAction(id, FLOAT, (void *)fnAction);
	if (id < 0)
		DBG(name + " " + SN(id));
	return id;
}
int Webgui::addInputString(String name, CallbackTypeCharp fnAction, int x, int y, String title, String classname, String value)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname + "," + value;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_CONTROL:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_println("`,string");
	_addAction(id, STRING, (void *)fnAction);
	if (id < 0)
		Serial.println("id error @" + String(__LINE__));

	return id;
}
int Webgui::addLED(String name, int x, int y, String title, String classname)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_MONITOR:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_println("`,boolean");
	if (id < 0)
		Serial.println("id error @" + String(__LINE__));

	return id;
}
int Webgui::addNeedleIndicator(String name, float minvalue, float maxvalue, int x, int y, String title, String classname)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_MONITOR:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_print("`,analog,");
	_print(SN(minvalue));
	_print(",");
	_println(SN(maxvalue));
	if (id < 0)
		Serial.println("id error @" + String(__LINE__));

	return id;
}
int Webgui::addNumericDisplay(String name, int x, int y, String title, String classname)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_MONITOR:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_println("`,digital");
	if (id < 0)
		Serial.println("id error @" + String(__LINE__));

	return id;
}
int Webgui::addStringDisplay(String name, int x, int y, String title, String classname)
{
	String sid;
	int id;
	name += "," + String(x) + "," + String(y) + "," + title + "," + classname;
	CHECKCONNECTION(-1);
	GUISTACK;
	_print("ADD_MONITOR:");
	id = this->_getId();
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(name);
	_println("`,string");
	if (id < 0)
		Serial.println("id error @" + String(__LINE__));

	return id;
}

void Webgui::setMonitor(int id, float value)
{
	CHECKCONNECTION();
	GUISTACK;
	_print("SET_MONITOR:");
	String sid;
	sid = _idtostr(id);
	_print(sid);
	_print(",");
	_println(String(value));
}

void Webgui::setMonitor(int id, String value)
{
	// if (lastout[id] == value)
	// {
	// 	Serial.println(id);
	// 	return;
	// }
	// lastout[id] = value;
	if (id < 0 || id > idcounter)
	{
		Serial.println("invalid ID " + String(id));
		return;
	}
	CHECKCONNECTION();
	GUISTACK;
	_print("SET_MONITOR:");
	String sid;
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(value);
	_println("`");
}
void Webgui::setMonitorT(int id, String &value, byte type)
{
	// if (lastout[id] == value)
	// {
	// 	Serial.println(id);
	// 	return;
	// }
	// lastout[id] = value;
	if (id < 0 || id > idcounter)
	{
		Serial.println("invalid ID " + String(id));
		return;
	}
	CHECKCONNECTION();
	GUISTACK;
	if (type == 0)
		_print("SET_MONITOR:");
	else
		_print("SET_MONITOR" + String(type) + ":");
	String sid;
	sid = _idtostr(id);
	_print(sid);
	_print(",`");
	_print(value);
	_println("`");
}

// void Webgui::setMonitor(int id, bool value)
// {
// 	CHECKCONNECTION();
// 	GUISTACK;
// 	_print("SET_MONITOR:");
// 	String sid;
// 	sid = _idtostr(id);
// 	_print(sid);
// 	_print(",");
// 	if (value)
// 	{
// 		_println("on");
// 	}
// 	else
// 	{
// 		_println("off");
// 	}
// }
void Webgui::remove(int id)
{
	CHECKCONNECTION();
	if (id < 0 || id > idcounter)
		return;
	_print("REMOVE:");
	String sid;
	sid = _idtostr(id);
	if (id > -1)
		_println(sid);
	// delete from action lists
	int i, j;
	for (i = 0; i < numactions; i++)
	{
		if (act[actionsList[i]].id == id)
		{
			for (j = i; j < numactions - 1; j++)
			{ // shift all the pointers to left
				actionsList[j] = actionsList[j + 1];
			}
			actionsList[numactions - 1] = -1; // delete last, that is repeated
			numactions--;
		}
	}
	for (i = 0; i < nummousedownactions; i++)
	{
		if (mousedownActionsList[i]->id == id)
		{
			free(mousedownActionsList[i]);
			for (j = i; j < nummousedownactions - 1; j++)
			{ // shift all the pointers to left
				mousedownActionsList[j] = mousedownActionsList[j + 1];
			}
			mousedownActionsList[nummousedownactions - 1] = NULL; // delete last, that is repeated
			nummousedownactions--;
		}
	}
}
void Webgui::reset()
{
	CHECKCONNECTION();
	_println("RESET");
	numactions = 0;
	//	idcounter = 1;
}
