#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoiseWhite noise1;                 //xy=1436,1514
AudioSynthWaveformModulated sine2;           //xy=1446,1311
AudioSynthWaveformModulated sine1;           //xy=1452,1378
AudioSynthMVWaveformModulated waveformMod11; //xy=1672,1581
AudioSynthMVWaveformModulated waveformMod9;  //xy=1681,1445
AudioSynthMVWaveformModulated waveformMod7;  //xy=1686,1335
AudioSynthMVWaveformModulated waveformMod5;  //xy=1687,1155
AudioSynthMVWaveformModulated waveformMod3;  //xy=1693,1033
AudioSynthMVWaveformModulated waveformMod1;  //xy=1694,895
AudioSynthWaveformDc dc3;                    //xy=1728,1236
AudioPlaySdWav playSdWav2;                   //xy=1750,681
AudioPlaySdWav playSdWav1;                   //xy=1753,556
AudioEffectEnvelope envelope11;              //xy=1909,1777
AudioEffectEnvelope envelope9;               //xy=1910,1618
AudioMixer4 mixer23;                         //xy=1910,1700
AudioMixer4 mixer20;                         //xy=1915,1541
AudioEffectEnvelope envelope7;               //xy=1920,1446
AudioEffectEnvelope envelope5;               //xy=1925,1312
AudioMixer4 mixer17;                         //xy=1928,1383
AudioSynthWaveformModulated sine21;          //xy=1930,1245
AudioMixer4 mixer13;                         //xy=1935,1171
AudioEffectEnvelope envelope2;               //xy=1937,930
AudioEffectEnvelope envelope3;               //xy=1941,1112
AudioMixer4 mixer9;                          //xy=1943,1032
AudioMixer4 mixer2;                          //xy=1947,853
AudioSynthWaveformDc dc1;                    //xy=1955,636
AudioMixer4 mixer5;                          //xy=1962,687
AudioSynthWaveformDc dc2;                    //xy=1969,748
AudioMixer4 mixer4;                          //xy=1970,568
AudioMixer4 mixer25;                         //xy=2079,1733
AudioMixer4 mixer27;                         //xy=2080,1630
AudioMixer4 mixer19;                         //xy=2095,934
AudioMixer4 mixer15;                         //xy=2096,1293
AudioMixer4 mixer26;                         //xy=2096,1461
AudioMixer4 mixer22;                         //xy=2115,1191
AudioEffectMultiply multiply1;               //xy=2163,550
AudioEffectMultiply multiply2;               //xy=2176,719
AudioFilterStateVariable filter6;            //xy=2196,1792
AudioFilterStateVariable filter4;            //xy=2207,1530
AudioFilterStateVariable filter5;            //xy=2209,1713
AudioFilterStateVariable filter2;            //xy=2216,1080
AudioFilterStateVariable filter1;            //xy=2230,891
AudioFilterStateVariable filter3;            //xy=2235,1276
AudioSynthWaveformDc dc9;                    //xy=2293,1410
AudioSynthWaveformModulated sine16;          //xy=2297,1371
AudioMixer4 mixer18;                         //xy=2368,1512
AudioMixer4 mixer21;                         //xy=2369,1671
AudioMixer4 mixer24;                         //xy=2369,1787
AudioMixer4 mixer14;                         //xy=2387,1260
AudioMixer4 mixer10;                         //xy=2404,1072
AudioMixer4 mixer1;                          //xy=2435,640
AudioMixer4 mixer6;                          //xy=2479.7143020629883,899.2857513427734
AudioMixer4 mixer16;                         //xy=2501,1384
AudioEffectEnvelope envelope8;               //xy=2513,1507
AudioEffectEnvelope envelope10;              //xy=2519,1693
AudioEffectEnvelope envelope12;              //xy=2522,1787
AudioEffectEnvelope envelope4;               //xy=2556,1146
AudioEffectEnvelope envelope6;               //xy=2557,1263
AudioEffectEnvelope envelope1;               //xy=2568,1049
AudioEffectMultiply multiply3;               //xy=2751,1114
AudioEffectMultiply multiply4;               //xy=2753,1202
AudioEffectMultiply multiply7;               //xy=2757,1464
AudioEffectMultiply multiply8;               //xy=2762,1557
AudioEffectMultiply multiply5;               //xy=2766,1283
AudioEffectMultiply multiply6;               //xy=2772,1361
AudioMixer4 mixer8;                          //xy=2967.4285278320312,1215.4285736083984
AudioMixer4 mixer12;                         //xy=2979.285614013672,1376.2855892181396
AudioMixer4 mixer3;                          //xy=3151,1126
//AudioOutputSPDIF3 spdif3_1;
AudioOutputI2S2 i2s2_1;                      // xy=3334.00008392334,1128.428451538086
AudioOutputUSB usb1;                         //xy=3345.142723083496,1194.4286613464355
AudioConnection patchCord1(noise1, 0, mixer2, 3);
AudioConnection patchCord2(noise1, 0, mixer9, 3);
AudioConnection patchCord3(noise1, 0, mixer13, 3);
AudioConnection patchCord4(noise1, 0, mixer17, 3);
AudioConnection patchCord5(noise1, 0, mixer20, 3);
AudioConnection patchCord6(noise1, 0, mixer23, 3);
AudioConnection patchCord7(sine2, 0, waveformMod1, 0);
AudioConnection patchCord8(sine2, 0, waveformMod3, 0);
AudioConnection patchCord9(sine2, 0, waveformMod5, 0);
AudioConnection patchCord10(sine2, 0, waveformMod7, 0);
AudioConnection patchCord11(sine2, 0, waveformMod9, 0);
AudioConnection patchCord12(sine2, 0, waveformMod11, 0);
AudioConnection patchCord13(sine1, 0, waveformMod1, 1);
AudioConnection patchCord14(sine1, 0, waveformMod3, 1);
AudioConnection patchCord15(sine1, 0, waveformMod5, 1);
AudioConnection patchCord16(sine1, 0, waveformMod7, 1);
AudioConnection patchCord17(sine1, 0, waveformMod9, 1);
AudioConnection patchCord18(sine1, 0, waveformMod11, 1);
AudioConnection patchCord19(waveformMod11, 0, mixer23, 0);
AudioConnection patchCord20(waveformMod9, 0, mixer20, 0);
AudioConnection patchCord21(waveformMod7, 0, mixer17, 0);
AudioConnection patchCord22(waveformMod5, 0, mixer13, 0);
AudioConnection patchCord23(waveformMod3, 0, mixer9, 0);
AudioConnection patchCord24(waveformMod3, 0, mixer9, 0);
AudioConnection patchCord25(waveformMod1, 0, mixer2, 0);
AudioConnection patchCord26(dc3, envelope2);
AudioConnection patchCord27(dc3, envelope3);
AudioConnection patchCord28(dc3, envelope5);
AudioConnection patchCord29(dc3, envelope7);
AudioConnection patchCord30(dc3, envelope9);
AudioConnection patchCord31(dc3, envelope11);
AudioConnection patchCord32(playSdWav2, 0, mixer5, 0);
AudioConnection patchCord33(playSdWav2, 1, mixer5, 1);
AudioConnection patchCord34(playSdWav1, 0, mixer4, 0);
AudioConnection patchCord35(playSdWav1, 1, mixer4, 1);
AudioConnection patchCord36(envelope11, 0, mixer25, 1);
AudioConnection patchCord37(envelope9, 0, mixer27, 1);
AudioConnection patchCord38(mixer23, 0, filter6, 0);
AudioConnection patchCord39(mixer23, 0, mixer24, 0);
AudioConnection patchCord40(mixer20, 0, filter5, 0);
AudioConnection patchCord41(mixer20, 0, mixer21, 0);
AudioConnection patchCord42(envelope7, 0, mixer26, 1);
AudioConnection patchCord43(envelope5, 0, mixer15, 1);
AudioConnection patchCord44(mixer17, 0, filter4, 0);
AudioConnection patchCord45(mixer17, 0, mixer18, 0);
AudioConnection patchCord46(sine21, 0, mixer22, 1);
AudioConnection patchCord47(sine21, 0, mixer15, 0);
AudioConnection patchCord48(sine21, 0, mixer19, 1);
AudioConnection patchCord49(sine21, 0, mixer25, 0);
AudioConnection patchCord50(sine21, 0, mixer26, 0);
AudioConnection patchCord51(sine21, 0, mixer27, 0);
AudioConnection patchCord52(mixer13, 0, filter3, 0);
AudioConnection patchCord53(mixer13, 0, mixer14, 0);
AudioConnection patchCord54(envelope2, 0, mixer19, 0);
AudioConnection patchCord55(envelope3, 0, mixer22, 0);
AudioConnection patchCord56(mixer9, 0, filter2, 0);
AudioConnection patchCord57(mixer9, 0, mixer10, 0);
AudioConnection patchCord58(mixer2, 0, filter1, 0);
AudioConnection patchCord59(mixer2, 0, mixer6, 0);
AudioConnection patchCord60(dc1, 0, multiply1, 1);
AudioConnection patchCord61(mixer5, 0, mixer1, 3);
AudioConnection patchCord62(mixer5, 0, multiply2, 0);
AudioConnection patchCord63(dc2, 0, multiply2, 1);
AudioConnection patchCord64(mixer4, 0, mixer1, 2);
AudioConnection patchCord65(mixer4, 0, multiply1, 0);
AudioConnection patchCord66(mixer25, 0, filter6, 1);
AudioConnection patchCord67(mixer27, 0, filter5, 1);
AudioConnection patchCord68(mixer19, 0, filter1, 1);
AudioConnection patchCord69(mixer15, 0, filter3, 1);
AudioConnection patchCord70(mixer26, 0, filter4, 1);
AudioConnection patchCord71(mixer22, 0, filter2, 1);
AudioConnection patchCord72(multiply1, 0, mixer1, 0);
AudioConnection patchCord73(multiply2, 0, mixer1, 1);
AudioConnection patchCord74(filter6, 0, mixer24, 1);
AudioConnection patchCord75(filter6, 1, mixer24, 2);
AudioConnection patchCord76(filter6, 2, mixer24, 3);
AudioConnection patchCord77(filter4, 0, mixer18, 1);
AudioConnection patchCord78(filter4, 1, mixer18, 2);
AudioConnection patchCord79(filter4, 2, mixer18, 3);
AudioConnection patchCord80(filter5, 0, mixer21, 1);
AudioConnection patchCord81(filter5, 1, mixer21, 2);
AudioConnection patchCord82(filter5, 2, mixer21, 3);
AudioConnection patchCord83(filter2, 0, mixer10, 1);
AudioConnection patchCord84(filter2, 1, mixer10, 2);
AudioConnection patchCord85(filter2, 2, mixer10, 3);
AudioConnection patchCord86(filter1, 0, mixer6, 1);
AudioConnection patchCord87(filter1, 1, mixer6, 2);
AudioConnection patchCord88(filter1, 2, mixer6, 3);
AudioConnection patchCord89(filter3, 0, mixer14, 1);
AudioConnection patchCord90(filter3, 1, mixer14, 2);
AudioConnection patchCord91(filter3, 2, mixer14, 3);
AudioConnection patchCord92(dc9, 0, mixer16, 1);
AudioConnection patchCord93(sine16, 0, mixer16, 0);
AudioConnection patchCord94(sine16, 0, mixer16, 0);
AudioConnection patchCord95(mixer18, envelope8);
AudioConnection patchCord96(mixer21, envelope10);
AudioConnection patchCord97(mixer24, envelope12);
AudioConnection patchCord98(mixer14, envelope6);
AudioConnection patchCord99(mixer10, envelope4);
AudioConnection patchCord100(mixer1, 0, mixer3, 0);
AudioConnection patchCord101(mixer6, 0, mixer3, 2);
AudioConnection patchCord102(mixer6, envelope1);
AudioConnection patchCord103(mixer16, 0, multiply3, 1);
AudioConnection patchCord104(mixer16, 0, multiply4, 1);
AudioConnection patchCord105(mixer16, 0, multiply5, 1);
AudioConnection patchCord106(mixer16, 0, multiply6, 1);
AudioConnection patchCord107(mixer16, 0, multiply7, 1);
AudioConnection patchCord108(mixer16, 0, multiply8, 1);
AudioConnection patchCord109(envelope8, 0, multiply6, 0);
AudioConnection patchCord110(envelope10, 0, multiply7, 0);
AudioConnection patchCord111(envelope12, 0, multiply8, 0);
AudioConnection patchCord112(envelope4, 0, multiply4, 0);
AudioConnection patchCord113(envelope6, 0, multiply5, 0);
AudioConnection patchCord114(envelope1, 0, multiply3, 0);
AudioConnection patchCord115(multiply3, 0, mixer8, 0);
AudioConnection patchCord116(multiply4, 0, mixer8, 1);
AudioConnection patchCord117(multiply7, 0, mixer12, 0);
AudioConnection patchCord118(multiply8, 0, mixer12, 1);
AudioConnection patchCord119(multiply5, 0, mixer8, 2);
AudioConnection patchCord120(multiply6, 0, mixer8, 3);
AudioConnection patchCord121(mixer8, 0, mixer3, 1);
AudioConnection patchCord122(mixer12, 0, mixer3, 3);
AudioConnection patchCord123(mixer3, 0, i2s2_1, 0);
AudioConnection patchCord124(mixer3, 0, i2s2_1, 1);
AudioConnection patchCord125(mixer3, 0, usb1, 0);
AudioConnection patchCord126(mixer3, 0, usb1, 1);
// GUItool: end automatically generated code
