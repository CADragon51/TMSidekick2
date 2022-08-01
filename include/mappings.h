

String readline(File);
#include <Regexp.h>
MatchState regexstate;
void createmap()
{
  for (int i = 0; i < 2048 && 0; i++)
  {
    chordName[i] = "";
    scaleNames[i] = "";
  }
  for (int i = 0; i < 128; i++)
  {
    baseNoteID[i] = 0;
    midiNamesLong[i] = midiNamesFlat[i % 12] + String((int)(i / 12));
    onnotes[i] = 0;
  }
  for (int i = 0; i < 2048; i++)
    chords[i] = 0;
  for (int i = 0; i < 63; i++)
  {
    chords[chordIds[i]] = new Chord(chordIds[i], i);
    //    chords[chordIds[i]*2]=new Chord(2*chordIds[i], i);
    //    Serial.println(SN(chordIds[i]));
  }
  for (int i = 0; i < 63; i++)
  {
    Chord *aC = chords[chordIds[i]];
    for (int c = 1; c < aC->num; c++)
    {
      int cid = aC->inverseID[c];
      if (chords[cid] == nullptr)
      {
        chords[cid] = new Chord(cid, chordIds[i], c, aC->_cindex);
        //       Serial.println(SN(cid));
        //        if (72 == chordIds[i])
        //          FDBG(SN(c) + " " + SN(chordIds[i]) + " <" + (chords[chordIds[i]]->nn) + "> == " + chords[cid]->name + " " + SN(cid) + " <" + (chords[cid]->nn) + ">");
      } //      DBG(String(cid) + " " + chords[cid]->name);
    }
    //      if(debug==1)Serial.print(i);
    //      if(debug==1)Serial.print(" ");
    //      if(debug==1)DBG(chords[i]->num);
  }
}
int fx = 4;
int sfx = 84;
int wx = 1;
int pwx = 0;
int px = 1;
void rmDirectory(File dir, String pattern)
{
  pattern.replace("*", "").toUpperCase();
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files

      DBG("**nomorefiles**");
      return;
    }
    String nf = entry.name();
    String uf = entry.name();
    uf.toUpperCase();
    //    FDBG(pattern + " " + uf + " " + SN(uf).indexOf(pattern)));
    if (uf.endsWith(pattern))
      SD.remove(nf.c_str());
    entry.close();
  }
}
EXTMEM String dirout = "";
void listDirectory(File dir, String search)
{
  bool isS = search.length() > 1;
//  FDBG(currentDirectory);
  dirout.setBuffer(dirbuff, 100000);
  String ldir = dir.name();
  if (isS)
  {
    search.replace(".", "[.]");
    search.replace("*", ".*");
    int x = search.lastIndexOf("/");
    if (x > 0)
    {
      ldir = search.substring(0, x);
      search = search.substring(x + 1);
      dir = SD.open(ldir.c_str());
    }
//    FDBG("looking for " + search + " in " + ldir);
  }
  dirout = "&#x1f5c2;" + ldir+"~";
  while (true)
  {
    //   FDBG(dir.name());

    File entry = dir.openNextFile();
    String fn = entry.name();
    fn.toLowerCase();
    //    FDBG(dir + " " + fn);
    if (!entry)
    {
      //     Serial.println("**nomorefiles**");
      break;
    }
    bool match = true;
    if (isS)
    {
      regexstate.Target((char *)fn.c_str()); // set its address
      char result = regexstate.Match(search.c_str());
      match = (result == REGEXP_MATCHED);
//      FDBG(fn + " " + search + " " + SB(match));
    }
    if (!match)
      continue;
    if (!entry.isDirectory())
    {
      //    FDBG(sr + " " + SN(sr.length()) + " " + fn);
      String size = SN((int)entry.size());
      if (fn.endsWith(".mid"))
      {
        int v = 0;
        //       FDBG(ldir + "/" + entry.name());
        int bc = sizeMIDI(ldir + "/" + entry.name(), v);
        size = String(bc) + "/" + String(v);
      }
      dirout += "&emsp;&#9507;" + fn + " " + size + "~";
    }
    else
    {
      //      FDBG(__CALLER__);
      dirout += "&emsp;&#9507;&#x1F4C1;" + fn + "~";
    }
    entry.close();
  }

  dirout+="~";
}
int loadDirectory(File dir, String search, int stn = 0)
{
  bool isS = search.length() > 1;
  String ldir = dir.name();
  FDBG("looking in " + ldir);
  if (isS)
  {
    search.replace(".", "[.]");
    search.replace("*", ".*");
    int x = search.lastIndexOf("/");
    if (x > 0)
    {
      ldir = search.substring(0, x);
      search = search.substring(x + 1);
      dir = SD.open(ldir.c_str());
    }
//    FDBG("looking for " + search + " in " + ldir);

  }
  res[0] = ldir;
  dirres[0] = "_&#x1f5c2;" + ldir;
  res[1] = "..";
  dirres[1] = "..";
  //  FDBG("looking for " + search);
  int n = 2;
  String ename;
  while (true)
  {
    //   FDBG(dir.name());

    File entry = dir.openNextFile();
    if (!entry)
    {
      //     Serial.println("**nomorefiles**");
      break;
    }
 //   FDBG(entry.name());
    ename = String(entry.name()).toLowerCase();
    //    String fn = "/" + ename + " ";
    //   if (stn > 0)
    {
      bool match = true;
      if (isS)
      {
        //     match = compare(ename, stc);
        regexstate.Target((char *)ename.c_str()); // set its address
        char result = regexstate.Match(search.c_str());
        match = (result == REGEXP_MATCHED);
      }
      if (match && !entry.isDirectory())
      {
        //        Serial.println(ename + " " + search + " " + SB(match));
        String size = SN((int)entry.size());
        if (ename.endsWith(".mid"))
        {
          int v = 0;
          FDBG(currentDirectory + "/" + ename);
          int bc = sizeMIDI(currentDirectory + "/" + ename, v);
          size = String(bc) + "|" + String(v);
        }

        if (n - 2 < stn)
        {
          //          FDBG("skip " + SN(n) + ename);
          n++;
          continue;
        }
        if (n - 2 - stn < 20)
        {
          res[n - stn].setBuffer(resbuf[n - stn], 1000);
          dirres[n - stn].setBuffer(dirresbuf[n - stn], 1000);
          res[n - stn] = ename;
  //        FDBG(SN(n) + dirres[n]);
          dirres[n - stn] = ename + " " + size;
          n++;
        }
        else
        {
 //         res[n - stn] = String(stn) + "&larr;";
          dirres[n - stn] = stn - 20 > 0 ? String(stn - 20) + "&larr;" : " ";
          n++;
 //         res[n - stn] = String(n - 3) + "&rarr;";
          dirres[n - stn] = String(n - 3) + "&rarr;";
          n++;
          return n - stn;
        }
      }
    }
    if (entry.isDirectory())
    {
      //      dirout += "  " + fn + "~";
      bool match = true;
      // fn = ename;
      if (isS)
      {
        regexstate.Target((char *)ename.c_str()); // set its address
        char result = regexstate.Match(search.c_str());
        match = (result == REGEXP_MATCHED);
      }
      //     FDBG(ename + " " + search + " " + SB(match));
      if (match)
      {
        res[n].setBuffer(resbuf[n - stn], 1000);
        dirres[n].setBuffer(dirresbuf[n - stn], 1000);
        res[n] = ename;
        ename.replace("__", "&emsp;").replace("_", " ");
        int xat = ename.indexOf("@");
        int xat1 = ename.indexOf("@ ");
        if (xat == -1)
          dirres[n] = "&#x1F4C1;" + ename;
        else
        {
          //          FDBG(fn + " " + SN(xat1));
          if (xat1 > -1)
          {
            dirres[n] = "_" + ename.substring(xat1 + 2);
          }
          else
            dirres[n] = "&#x1F4C1;" + ename.substring(xat + 1);
        }
        n++;
      }
      entry.close();
    }
  }
  return (n - stn);
}

#if 1
void printDirectory(File dir, int numTabs)
{

  // FDBG("** read dir **");

  // DBG(dir);
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files

      DBG("**nomorefiles**");
      break;
    }
    if (entry.isDirectory())
    {
      //  Serial.println("/");
      //     printDirectory(entry, numTabs + 1);
    }
    String nf = entry.name();
    String uf = entry.name();
    uf.toUpperCase();

    // FDBG(nf);
    //    FDBG(uf);
    if (uf.endsWith(".MAP") && fx < 99)
    {

      File myFile = SD.open(nf.c_str());
      if (myFile)
      {
        //       SD.remove(nf.c_str());
        DBG(nf + " " + SN(fx));
        filenames[fx] = nf;
        //.replace(".map", "").replace(".MAP", "");
        String fi = readline(myFile);
        Menus[MAPPINGS]->insertItem(fi);

        DBG(fi + " " + String(ins) + " " + String(fx) + filenames[fx] + " " + Menus[MAPPINGS]->items[ins]);
        fx++;
      }
      // close the file:
      myFile.close();
    }

    if (uf.endsWith(".WAV") && wx < 99)
    {
      wavenames[wx++] = nf;
    }
    if (uf.endsWith(".PAT") && pwx < 99 && uf.substring(0, 1) == "P")
    {
      //      FDBG(nf);
      File myFile = SD.open(nf.c_str());
      //      File myFile = SD.open(argv[1].c_str());
      String more = "";
      //      FDBG(nf + " " + SP(myFile));
      if (myFile)
      {
        STACK;
        pwx = 0;
        patnames[pwx] = 0;
        patvals[pwx] = "PATTERN";
        pwx++;
        patnames[pwx] = 0;
        patvals[pwx] = " ";
        pwx++;
        STACK;
        while (myFile.available() && pwx < 100)
        {
          String fi = readline(myFile);
          //         FDBG(fi + " " + SN(pwx));
          if (fi.length() > 0 && fi.toInt() < 4096)
          {
            patnames[pwx] = fi.toInt();
            //            int x = fi.indexOf(" ");
            patvals[pwx] = pat2string(fi.toInt(), -1);
            //             FDBG(fi + " " + patvals[pwx] + " " + SN(patnames[pwx]));
            pwx++;
          }
        }
        myFile.close();
      }
      // close the file:
    }
    if (uf.endsWith(".PRS") && px < 99)
    {
      presetnames[px++] = nf;

      //      DBG(nf);
      //      DBG(Menus[PRESETS]->insertItem(nf.substring(0, nf.length() - 4)));
    }
    entry.close();
  }

  maxsamples = wx;
}
#endif