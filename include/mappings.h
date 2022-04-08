

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
        chords[cid] = new Chord(cid, chordIds[i], c, aC->_cindex);
      //      DBG(String(cid) + " " + chords[cid]->name);
    }
    //      if(debug==1)Serial.print(i);
    //      if(debug==1)Serial.print(" ");
    //      if(debug==1)DBG(chords[i]->num);
  }
}
int fx = 4;
int sfx = 84;
int wx = 1;
int px = 1;

String printDirectory(File dir, String pattern, bool collect = false)
{
  String dirout = "";
  pattern.replace("*", "").toUpperCase();
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files

      DBG("**nomorefiles**");
      break;
    }
    String nf = entry.name();
    String uf = entry.name();
    uf.toUpperCase();
    //    FDBG(pattern + " " + uf + " " + SN(uf).indexOf(pattern)));
    if (uf.indexOf(pattern) > -1 && !collect)
      dirout = dirout + nf + " " + SN((unsigned int)entry.size()) + "~";
    else if (uf.indexOf(pattern) > -1)
      dirout = dirout + nf + ",";
  }
  return dirout;
}
void printDirectory(File dir, int numTabs)
{

  FDBG("** read dir **");

  DBG(dir);
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files

      DBG("**nomorefiles**");
      break;
    }
    String nf = entry.name();
    String uf = entry.name();
    uf.toUpperCase();

    DBG(nf);
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
        int ins = Menus[MAPPINGS]->insertItem(fi);

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
    if (uf.endsWith(".PRS") && px < 99)
    {
      presetnames[px++] = nf;

      DBG(nf);
      DBG(Menus[PRESETS]->insertItem(nf.substring(0, nf.length() - 4)));
    }
    entry.close();
  }
  j2 = 0;
  maxsamples = wx;
}


