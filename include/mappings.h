

String readline(File);
void createmap()
{
  for (int i = 0; i < 2048; i++)
  {
    cname[i] = "";
    replchord[i] = i;
    transpose[i] = 0;
    invID[i] = 0;
    scalechords[i] = 0;
    scaleNames[i] = "";
    //    invert[i][0] =0;
  }
  for (int i = 0; i < 128; i++)
  {
    baseNoteID[i] = 0;
    midiNamesLong[i] = midiNamesFlat[i % 12] + String((int)(i / 12));
    onnotes[i] = 0;
    offnotes[i] = 0;
  }
  // invert[1241] = A_1241;
#include "assigns.h"
  sfx = 2;
  for (int i = 0; i < 2048 && sfx < 100; i++)
  {
    if (scaleNames[i] != "")
    {
      scFP[sfx++] = i;
      //     Serial.println(String(sfx-1) + "=" + scFP[sfx - 1]);
    }
  }

  for (int i = 0; i < 52; i++)
    chords[chordIds[i]] = new Chord(chordIds[i]);
  for (int i = 0; i < 52; i++)
  {
    Chord *aC = chords[chordIds[i]];
    for (int c = 1; c < aC->num; c++)
    {
      int cid = aC->inverseID[c];
      if (chords[cid] == nullptr)
        chords[cid] = new Chord(cid, chordIds[i], c);
    }
    //      if(debug==1)Serial.print(i);
    //      if(debug==1)Serial.print(" ");
    //      if(debug==1)Serial.println(chords[i]->num);
  }
}
int fx = 2;
int sfx = 2;
int wx = 1;
int px = 1;
int eqx = 1;
void decodeSF(String filename)
{
  File myFile = SD.open(filename.c_str());
  String fncom = readline(myFile);
  // read from the file until there's nothing else in it:
  while (myFile.available())
  {
    String fn = readline(myFile);
    if (fn.length() < 3)
      continue;
    fn = fn + ",";
    fn.replace(" ", ",");
    nx = SplitS(fn.replace(",,", ","), ',', (String *)res, 13);
    if (debug == 1)
    {
      Serial.print(nx);
      Serial.print(" found:");
      Serial.println(fn);
    }
    if (nx > 12)
      nx = 12;
    if (nx < 2)
      continue;
    byte nl[12];
    scid = 0;
    for (int i = 0; i < nx; i++)
    {
      nl[i] = res[i].toInt();
      int n = nl[i] % 12;
      scid |= (1 << n);
    }
    scid = scid >> 1;
    Serial.println("scaleNames[" + String(scid) + "]=\"" + fncom + "\";" + " //" + fn);
  }
  myFile.close();
}
void printDirectory(File dir, int numTabs)
{
  if (debug == 1)
    Serial.println("** read dir **");
  if (debug == 1)
    Serial.println(dir);
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      if (debug == 1)
        Serial.println("**nomorefiles**");
      break;
    }
    String nf = entry.name();
    nf = nf.toUpperCase();
    if (debug == 1)
      Serial.println(nf);
    if (nf.startsWith("MAP") && nf.endsWith(".TXT") && fx < 99)
    {
      filenames[fx++] = nf;
      if (debug == 1)
        Serial.println(nf);
      File myFile = SD.open(nf.c_str());
      if (myFile)
      {
        String fi = readline(myFile);
        if (debug == 1)
          Serial.println(fi);
        Menus[MAPPINGS]->insertItem(fi);
      }
      // close the file:
      myFile.close();
    }
#if 0
    if (nf.startsWith("SCA") && nf.endsWith(".SCL") && sfx < 99)
    {
      scfilenames[sfx++] = nf;
      if (debug == 1)
        Serial.println(nf);
      File myFile = SD.open(nf.c_str());
      if (myFile)
      {
        String fi = readline(myFile).substring(0, 18);
        if (fi.charAt(fi.length() - 1 == ' '))
        {
          fi = fi.substring(0, fi.length() - 1);
        }
        if (debug == 1)
          Serial.println(fi + " " + String((int)myFile.size()));
        Menus[SCALES]->insertItem(fi);
      }
      // close the file:
      myFile.close();
      decodeSF(nf);
    }
#endif
    if (nf.endsWith(".WAV") && wx < 99)
    {
      wavenames[wx++] = nf;
      if (debug == 1)
        Serial.println(nf);
      Menus[FSR]->insertItem(nf.substring(0, nf.length() - 4));
      Menus[SWITCH]->insertItem(nf.substring(0, nf.length() - 4));
    }
    if (nf.endsWith(".PRS") && px < 99)
    {
      presetnames[px++] = nf;
      if (debug == 1)
        Serial.println(nf);
      Menus[PRESETS]->insertItem(nf.substring(0, nf.length() - 4));
    }
    entry.close();
  }
  for (int i = 2; i < sfx; i++)
  {
    Menus[SCALES]->insertItem(scaleNames[scFP[i]]);
  }
  Menus[MAPPINGS]->insertItem("Back");
  Menus[SCALES]->insertItem("Back");
  Menus[FSR]->insertItem("Back");
  Menus[SWITCH]->insertItem("Back");
  Menus[PRESETS]->insertItem("Back");
}

signed char SplitS(String what, char where, String *res, signed char nmax)
{
  int n = 0;
  int p[50];
  p[0] = 0;
  //  if(debug==1)Serial.println(what);
  for (int i = 0; i < (int)what.length(); i++)
  {
    p[n] = what.indexOf(where, i);
    if (p[n] < 0)
      break;
    i = p[n];
    n++;
  }
  res[0] = what.substring(0, p[0]);
  for (int i = 1; i < n && i < nmax; i++)
  {
    res[i] = what.substring(p[i - 1] + 1, p[i]);
  }
  return n;
}
