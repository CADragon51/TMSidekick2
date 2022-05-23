

String readline(File);
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

  for (int i = 0; i < 63; i++)
    chords[chordIds[i]] = new Chord(chordIds[i], i);
  for (int i = 0; i < 63; i++)
  {
    Chord *aC = chords[chordIds[i]];
    for (int c = 1; c < aC->num; c++)
    {
      int cid = aC->inverseID[c];
      if (chords[cid] == nullptr)
      {
        chords[cid] = new Chord(cid, chordIds[i], c, aC->_cindex);
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
#if 1
String dirout = "";
int loadDirectory(File dir, String search, String ldir = "", bool sel = false)
{
  //  FDBG("looking for " + search);
  static int n = 0;
  if (ldir == "")
  {
    n = 1;
  }
  bool isS = search.length() > 0;
  while (true)
  {
    //   FDBG(dir.name());

    File entry = dir.openNextFile();
    //  FDBG(__CALLER__);
    String cd = ldir;
    if (!entry)
    {
      //     Serial.println("**nomorefiles**");
      break;
    }
    String fn = cd.toLowerCase() + "/" + String(entry.name()).toLowerCase() + " ";
    String sr = search;
    if (isS)
    {
 //     if (search.endsWith("*"))
      {
        sr.replace("*", "").toLowerCase();
      }
    }
    int fx = 0;
    if (isS)
      fx = fn.substring(1).indexOf(sr.substring(1));
    //    FDBG(__CALLER__);
   // Serial.println(fn + " " + sr + " " + SN(fx));
    if (fx >= 0 && !entry.isDirectory())
    {
      //    FDBG(sr + " " + SN(sr.length()) + " " + fn);
      String netr = fn;
      if (isS&&search.indexOf(".")==-1)
        netr = fn.substring(sr.length() + 1);
      String size = SN((int)entry.size());
      if (String(entry.name()).endsWith(".mid"))
      {
        int v = 0;
 //       FDBG(ldir + "/" + entry.name());
        int bc = sizeMIDI(ldir + "/" + entry.name(), v);
        size = String(bc) + "/" + String(v);
      }
      dirout += "  " + netr + size + "~";
      if (n < 256)
        res[n] = netr+"#"+size;
      n++;
    }
    if (entry.isDirectory()&&sr.indexOf(".")==-1)
    {
      //      FDBG(__CALLER__);
      cd = ldir + "/" + entry.name();
      cd.toLowerCase();
      if (isS)
      {
        int x1 = cd.indexOf(sr);
        int x2 = sr.indexOf(cd);
        if (x1 == 0 && x2 == -1 && search.endsWith("*"))
        {
          search = cd;
          g_search = search;
        }
        //       FDBG(sr + " " + cd + " " + SN(x1) + SN(x2));
      }
      n = loadDirectory(entry, search, cd, sel);
      //      FDBG(__CALLER__);
    }
    entry.close();
  }
  return (n);
}
#endif
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
      printDirectory(entry, numTabs + 1);
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