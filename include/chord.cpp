extern byte _channel, cable;
#include "chord.h"
#include "menu.h"
extern MenuMidiSet *curMMS;
void Chord::noteOn(byte base, byte channel, byte velocity)
{
    short del = 0 * Menus[SETTINGS]->strum;
    //       if(debug==1)Serial.println(String(num)+" chord note on start del:" + String(del));
    for (int i = 0; i < num; i++)
    {
        curMMS->eventtype = 1; // 0: note-off,1: Note on event; 2: CC event;3: pitch
        curMMS->sourceCH = channel;
        curMMS->sourceNote = base + note[i];
        curMMS->SourceCC = 0;
        curMMS->SourceVelocity = velocity;
        curMMS->action(true);
        if (del > 0)
            delay(del);
    }
    //      if(debug==1)Serial.println("chord note on end");
    basenote = base;
}
void Chord::noteOff()
{
    //       if(debug==1)Serial.println("chord note off start");
    short del = Menus[SETTINGS]->strum;
    for (int i = 0; i < num; i++)
    {
        curMMS->eventtype = 0; // 0: note-off,1: Note on event; 2: CC event;3: pitch
        curMMS->sourceCH = _channel;
        curMMS->sourceNote = basenote + note[i];
        curMMS->SourceCC = 0;
        curMMS->SourceVelocity = 0;
        curMMS->action(false);
        if (del > 0)
            delay(del);
    }
    curMMS->eventtype = 6; // 0: note-off,1: Note on event; 2: CC event;3: pitch
    curMMS->action(false);

    //     if(debug==1)Serial.println("chord note off end");
}
