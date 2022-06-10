for (int v = 0; v < NUMVOICES; v++)
{
    vcos1[v]->arbitraryWaveform(myWaveform, 172.0);
    sine21.arbitraryWaveform(myWaveform, 172.0);
    sine2.arbitraryWaveform(myWaveform, 172.0);
    sine1.arbitraryWaveform(myWaveform, 172.0);
    sine16.arbitraryWaveform(myWaveform, 172.0);
}
//float cg = 0.2;
// mixer8.gain(1, cg);
// mixer8.gain(2, cg);
// mixer8.gain(3, cg);
// mixer12.gain(0, cg);
// mixer12.gain(1, cg);
mixer1.gain(0, 0);
mixer1.gain(1, 0);
mixer1.gain(2, 1);
mixer1.gain(3, 1);
mixer3.gain(0, 1);
dc9.amplitude(1);
dc3.amplitude(1);
filter1.octaveControl(2);
//	prename = "PRESET" + String(recentPre) + ".PRS";
display.cp437(true);
//	if (preindex > 0)
//		prename = presetnames[preindex];
alpha4.begin(0x70); // pass in the address

alpha4.writeDigitRaw(3, 0x0);
alpha4.writeDigitRaw(0, 0xFFFF);
alpha4.writeDisplay();
delay(200);
alpha4.writeDigitRaw(0, 0x0);
alpha4.writeDigitRaw(1, 0xFFFF);
alpha4.writeDisplay();
delay(200);
alpha4.writeDigitRaw(1, 0x0);
alpha4.writeDigitRaw(2, 0xFFFF);
alpha4.writeDisplay();
delay(200);
alpha4.writeDigitRaw(2, 0x0);
alpha4.writeDigitRaw(3, 0xFFFF);
alpha4.writeDisplay();
delay(200);
printA4("Hi!");
for (int i = 0; i < 6; i++)
{
    pinMode(leds[i], OUTPUT);

    DBG("led " + String(i));
    digitalWrite(leds[i], HIGH);
    delay(500);
}

DBG("Leds OK");
display.setTextColor(1, 0);
digitalWrite(13, LOW);

AudioInterrupts();
pinMode(buttonPin, INPUT_PULLUP);
pinMode(ledPin, OUTPUT);

// set initial LED state
digitalWrite(ledPin, ledState);