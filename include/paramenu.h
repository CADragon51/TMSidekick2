class MenuTargetSet;
extern short idt;
extern String connections(void);
class MenuPara : public Menu
{
public:
    MenuPara(signed char myID, String pname, signed char parent) : Menu(myID, pname, "", parent)
    {
        //      DBG("menuPara " + String(parent));

        nump = 1;
        for (int p = 0; p < 100; p++)
            Paras[p] = 0;
        isPara = true;
        me = myID;
    }
    void checkcontrol(int type, int id, int select, float value, bool swval);

    String setMenu()
    {
        String outmen = "";
        int widthl = 100;
        //        int widthr = 50;

        if (maxiteml == 0)
            setLength();
        widthl = maxiteml * 5;
        int width = widthl + maxvall * 6;
        if (width < 150)
            width = 150;
        outmen = " <style>  table td  {  font-size : 14px; height:25px; width: " + String(width) + "px; } </style> ";
        outmen += "<style>.alnleft {font-size : 14px;text-align : left;  width: " + String(widthl) + "px ;}</style> ";
        outmen += "<table>";
        for (int m = 1; m < numitems; m++)
        {
            Parameter *mPara = Paras[m];
            if (mPara == nullptr) // || mPara->isAnalog)
                continue;
            if (mPara->range == 0 || mPara->isAnalog)
                continue;
            outmen += "<tr>";
            outmen += "<td> " + items[m] + "</td>";
            outmen += "</tr>";
        }

        outmen + "</table";
        return outmen;
    }
    float getValue(int what)
    {
        if (what < 0 || what > NUMSYNTH)
            return -1;
        return Paras[what]->fvalue;
    }
    int maxiteml = 0;
    int maxvall = 0;
    void setLength(void)
    {
        for (int i = 1; i < numitems; i++)
        {
            int l = items[i].length();
            int lp = 0;
            if (l > maxiteml)
                maxiteml = l;
            if (Paras[i])
            {
                lp = Paras[i]->setLength();
                if (lp > maxvall)
                    maxvall = lp;
            }
        }
        //       DBG(items[0] + " il: " + SN(maxiteml) + " ,vl: " + SN(maxvall));
    }
    void displayMenu(String line) override
    {
        //      FDBG("Menu display " + String(myID) + " " + items[0] + " " + String(isshowing) + " " + String(menuState) + " " + String(line));

        Menu::displayMenu(line);
        //		nump = numitems - 1;
        int ml = line1;
        //		DBG(String(ml) + " / " + String(numitems));

        for (int i = 1; i < 8 && ml < numitems - 1; i++, ml++)
        {
            if (Paras[ml])
            {
                // FDBG(Paras[ml]->name);
                String oval = Paras[ml]->format();
                //				DBG(Paras[ml]->name + " is " + oval);

                display.setCursor(124 - 6 * oval.length(), i * 8);
                clearToEOL();
                print(oval, ml == rot_pos, __LINE__);
                //					DBG(i);
            }
        }
    }
    void show(bool force, String caller)
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
                //			DBG(actPara->name + "=>" + pvalue + " from " + String(caller));
                if (actPara->value)
                {
                    *actPara->value = pararot_pos;
                    //					if (debug == 1)						DBG(*actPara->value);
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
                    //					DBG("set enc "+String(tv)+"->"+String(setv));
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
            display.setCursor(8, i * 8);
            setInvertMode(false);
            display.print(items[rot_pos]);
            String oval = actPara->format();
            display.setCursor(46, i * 8);
            clearToEOL();
            display.setCursor(124 - 6 * oval.length(), i * 8);
            setInvertMode(false);
            display.print(oval);
            DBG(actPara->name + "=>" + oval + " from " + String(124 - 6 * oval.length()));
            display.display();
            return;
        }
        //		DBG("651 show");
        Menu::show(force, __LINE__);
        display.display();
    }

    void onButton(bool cancel) override
    {
        if (debug == 1)
            DBG("Para click " + String(rot_pos) + " " + String(para_sel ? "sel" : "unsel"));
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
                        if (rot_pos == 1)
                        {
                            FDBG(SN(rot_pos) + " " + actPara->name + "  " + synthParas[(*actPara->value)] + " " + name + " " + SN(actPara->source));
                            for (int t = TARGETS; t <= TARGETS + EXT_SWITCH - EXTSETTINGS; t++)
                            {
                                MenuPara *mp = (MenuPara *)Menus[t];
                                FDBG(SN(t) + " " + SN(mp->Paras[rot_pos]->source));
                            }
                        }
                        // pvalue = *actPara->value;
                        // if (oldvalue == -1)
                        //     oldvalue = pvalue;
                        // if (actPara->source >= TARGETS)
                        // {
                        //     MenuPara *oldPara = (MenuPara *)Menus[actPara->source];
                        //     FDBG(oldPara->name);//+ " removed" +  SN(oldPara->Paras[rot_pos]->name);
                        // }
                        // actPara->source = me;
                        // FDBG(actPara->name + "  " + String(*actPara->value) + " " + name + " " + SN(actPara->source));
                        //						DBG(actPara->name + " old " + String(oldvalue) + " " + String(ratdiv));
                    }
                    else
                    {
                        //						DBG(actPara->name + " next old " + String(oldvalue));
                        *actPara->value = oldvalue;
                        para_sel = false;
                        oldvalue = -1;
                    }
                }
                else
                {
                    if (!cancel)
                    {
                        //						DBG(actPara->name + " old " + String(oldfvalue));
                        if (oldfvalue == -1)
                            oldfvalue = *actPara->tvalue;
                        DBG(actPara->name + " new " + String(*actPara->tvalue));
                    }
                    else
                    {
                        //						DBG(actPara->name + " next old " + String(oldfvalue));
                        *actPara->tvalue = oldfvalue;
                        //                       printA4(oldfvalue);
                        para_sel = false;
                        oldfvalue = -1;
                    }
                }
            }
            if (para_sel && rot_pos > 0 && actPara->range == 0)
            {
                menuState = actPara->parent;
                //				DBG("670 now :");
                //				DBG(menuState);

                //				if (Menus[menuState])
                //					Menus[menuState]->items[0] = items[0] + "->";
                nextm = menuState;
                para_change = false;
                para_sel = false;

                return;
            }
            //			DBG(items[rot_pos] + " target " + String(actPara->parent) + " me " + String(myID));

            if (para_sel && actPara->value && !actPara->isFloat)
            {
                pararot_pos = pvalue;
                paraencoder->setPosition(pararot_pos, __CALLER__);
                int tv = *actPara->value;
                int setv = map(tv, actPara->vstart, actPara->vend, 0, 10000);
                Menus[HIRES]->myEnc->write(setv);
            }
            else
                displayMenu(__CALLER__);
        }

        Menu::onButton(false);

        return;
    }
    void pararot_test()
    {
        paraencoder->tick();
        int newPos = paraencoder->getPosition(__CALLER__);
        pararot_dir = (int)paraencoder->getDirection();
        if (Menus[SETTINGS]->hiRes && !pararot_dir)
        {
            int val = Menus[HIRES]->rawvalue;
            pararot_dir = Menus[HIRES]->rawdir;
            newPos = map(val, 1, 10000, actPara->vstart, actPara->vend);
            if (newPos == rot_pos)
                pararot_dir = 0;
            if (pararot_dir)
            {
                DBG("pos:");
                DBG(pararot_pos);
                DBG(" new:");
                DBG(newPos);
                DBG(" dir:");
                DBG(pararot_dir);
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
                //        DBG(pararot_dir);
                if (pararot_pos > actPara->fvend)
                    pararot_pos = actPara->fvstart;
                else if (pararot_pos < actPara->fvstart)
                    pararot_pos = actPara->fvend;
            }

            paraencoder->setPosition(pararot_pos, __CALLER__);
            //        DBG("pos:");
            //        DBG(pararot_pos);
            //        DBG(" dir:");
            //        DBG(pararot_dir);
        }
    }

    Parameter *getPara(int p)
    {
        if (p > -1 && p < NUMSYNTH)
            return Paras[p];
 //       FDBG("error: no para @" + SN(p));
        return nullptr;
    }
    void createControl()
    {
        int base = 300 + lastXPos;
        bool sws[1];
        int posx = base;
        int posy = 90;
        String swn = "";
     //   numswitches = 0;
        //        maxg = 0;
        //        DBG("myID :" + SN(myID));
        if (maxiteml == 0)
            setLength();
        if (maxg > 399)
            maxg = 0;
        int widthl = maxiteml * 6;
        int widthr = maxvall * 6;
        int j = 1;
        posx = base + widthl + widthr;
        //        maxg = 0;
        for (int i = 1; i < nump + 1; i++)
        {
            if (Paras[i] == 0)
                continue;
            if (!Paras[i]->isAnalog)
                continue;
            int ve = Paras[i]->vend;
            if (Paras[i]->isFloat)
                ve = Paras[i]->fvend;
            int vs = Paras[i]->vstart;
            if (Paras[i]->isFloat)
                vs = Paras[i]->fvstart;
            int vv = *Paras[i]->value;
            if (Paras[i]->isFloat)
                vv = Paras[i]->fvalue;
            String name = Paras[i]->name;
            //           DBG(name);
            int ret = webgui.addNumericDisplay(name, posx + j * 80 + 65, posy + 200, "f", "nomonitor");
            Paras[i]->monid = ret;

            webgui.setMonitor(ret, Paras[i]->format());
            guiid[maxg] = webgui.addInputAnalog(name, vs, ve, vv, &onSlider, posx + j * 80, posy, "title", "controlx");
            if (guiid[maxg] == -1)
                guiid[maxg] = webgui.addInputAnalog(name, vs, ve, vs, &onSlider, posx + j * 80, posy, "title", "controlx");
            DBG(Paras[i]->name + " " + SN(posx + maxg * 80) + "," + posy + " " + guiid[maxg]);
            id2para[guiid[maxg] % 400] = i;
            menId[guiid[maxg] % 400] = me;
            maxg++;
            j++;
        }
        j = 1;
        int lg = 0;
        int inpx = 250 + widthl + widthr + lastXPos;
        int inph = 29;
        int inpy = 100;
        if (maxvall > 16)
            inpx -= 50;
        for (int i = 1; i < nump + 1; i++)
        {
            if (Paras[i] == 0 || Paras[i]->isAnalog || Paras[i]->range == 0)
                continue;
            //           DBG(Paras[i]->name + " " + Paras[i]->vend);
            if (Paras[i]->vend == 1)
            {
                sws[0] = *Paras[i]->value > 0;
                DBG(Paras[i]->name + " " + SB(sws[0]) + " " + SP(Paras[i]));
                guiid[maxg] = webgui.addSwitches(Paras[i]->name, 1, sws, &onSwitches, inpx, inpy + j++ * inph, "f", "nomonitor");
                id2para[guiid[maxg]] = i;
                //              DBG(Paras[i]->name + " " + SN(guiid[maxg]) + " " + SN(id2para[guiid[maxg]]));
                menId[guiid[maxg] % 400] = me;
                maxg++;
            }
            else if (Paras[i]->form != 0)
            {
                //               DBG(Paras[i]->name + " " + SN(Paras[i]->vend + 1) + " - " + SN(Paras[i]->form[0]) + " " + SN(*Paras[i]->value));
                int av = *Paras[i]->value;
                int pl = 8 * Paras[i]->mv;
                //                DBG("position " + SN(Paras[i]->mv) + " " + SN(pl) + " " + SN(inpx - pl));
                if (pl > 150)
                    pl -= 40;
                guiid[maxg] = webgui.addOptions(Paras[i]->name, Paras[i]->vend + 1, (String *)Paras[i]->form, &onOptionSelect, inpx - pl, inpy + j++ * inph, av, "f", "nomonitor"); //*Paras[i]->value);
                lg++;
                id2para[guiid[maxg]] = i;
                menId[guiid[maxg]] = me;
                maxg++;
            }
        }
    }
    signed char SynthPara = 0;
    signed char Strum = 0;
    signed char CV1 = 0;
    signed char CV2 = 0;
    Parameter *Paras[200];
    Parameter *actPara = 0;
    signed char actPnum = 0;
    // int nump = 1;
    signed char eventtype = 0; // 0: off,1: on; 2: CC event;3: pitch,4: no event, 5: analog change, 6: distance change, 7: touch event
    int SourceVelocity = 0;
    //   Parameter* pparas[NUMSYNTH + 1];
    static MenuPara *SynthMenu;
    MenuTargetSet *targets = 0;
    int me;
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
