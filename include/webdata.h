#pragma once
#include <NativeEthernet.h>
#define TCPPORT 8124
#include "Webgui.h"
#include "parameter.h"
#include "globals.h"
#include <NativeEthernetUdp.h>
#include "MD_MIDIFile.h"
Webgui webgui;                             // initialize an instance of the class
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back
EthernetUDP Udp;
extern MD_MIDIFile SMF;
// bool sws[] = {false, false, false, false, true};
short cli;
short scaleid, baseid, mapid, cliitem;
short sliderID[45];
short menuId[20];
short transId[20];
short chordselId[20];
short xtransId[20];
short impid = 0;
short posx = 200;
short inc = 250;
short py = 100;
short smp = posx - 100 + inc;
short selitem = 0;
short bgitem = 0;
short showstatid;
short synthstate = 0;
short ledpin = 13, inleds[9], outleds[9], inbuts[9], outbuts[9], movenext[9];
short idt, idtg, sbp, bp, synthmen[3], men, target, gra, cped, chr, ledout, semp, semm, octp, octm, idt1, idt2, showstat, idjx, idjy, cped2, cfsr, csw;
short ipm, isb, iob, iab1, iab2, itb, imb, js2, z1, z2, zone, smen, imbb, sipm, home, file, metro, backmetro;
short delsav = 0, selback = 0, prenameinp = 0, predelsave = 0, mapvers = 0, mapcom = 0;
float val = 0;
unsigned int localPort = 6123;
bool forward = true;
bool inimport = false;
EthernetClient client;
IPAddress server(192, 168, 6, 2);
String mapres[99];
String btns[2] = {"Save", "Delete"};
String btnsi[2] = {"Select", "Back"};
String version = "M";
String comment;
String blueled = "<div class=\"monitor-boolean blue\"><div class=\"notification active\"><div class=\"hover-effect\"></div><div class=\"metal\"><div class=\"light\"><div class=\"glowing\"><div class=\"glowing-blue\"></div></div></div></div></div></div>";
String whiteled = "<div class=\"monitor-boolean white\"><div class=\"notification active\"><div class=\"hover-effect\"></div><div class=\"metal\"><div class=\"light\"><div class=\"glowing\"><div class=\"glowing-white\"></div></div></div></div></div></div>";
String yellowled = "<div class=\"monitor-boolean yellow\"><div class=\"notification active\"><div class=\"hover-effect\"></div><div class=\"metal\"><div class=\"light\"><div class=\"glowing\"><div class=\"glowing-yellow\"></div></div></div></div></div></div>";
String redled = "<div class=\"monitor-boolean red\"><div class=\"notification active\"><div class=\"hover-effect\"></div><div class=\"metal\"><div class=\"light\"><div class=\"glowing\"><div class=\"glowing-yellow\"></div></div></div></div></div></div>";
String greenled = "<div class=\"monitor-boolean green\"><div class=\"notification active\"><div class=\"hover-effect\"></div><div class=\"metal\"><div class=\"light\"><div class=\"glowing\"><div class=\"glowing-yellow\"></div></div></div></div></div></div>";
String offled = "<div class=\"monitor-boolean off\"><div class=\"notification active\"><div class=\"hover-effect\"></div><div class=\"metal\"><div class=\"light\"><div class=\"glowing\"><div class=\"glowing-yellow\"></div></div></div></div></div></div>";

EXTMEM String o;
EXTMEM String out;
// int metopt[maxvoices];
int actpatid, actgrpopt, midiopt;

String presave;
String grid;
String help = "ex[port] [seq|ini|prs|map|<file>] <target>~"
              "he[lp]~"
              "im[port] <file>~"
              "lo[ad] <file>~"
              "ls [<pattern>]~"
              "mo[re] <file>~"
              "mv <file1> <file2>~"
              "pl[ay] file|seq~"
              "pc (program change) <program>~"
              "Pr[ogression] <I..>|<i..>|<I7..>|<i7..><IM7..>~"
              //              "q[uantize] <num> ~"
              "rm <file>~"
              "sa[ve] <file> <comment>~"
              "sh[ow] seq|map|sca ~"
              "tr[anspose] <m-1|m-2|synth|cv1> <0|1>~";
String impFile;
String btns2[] = {"Mode +", "Mode -"};
String btns3[] = {"Semi +", "Semi -"};
String btns4[] = {"Oct +", "Oct -"};
String btns7[] = {"&larr;", "&#127968;", "&#x1F5AB;", "&#x23F2;", "&#x25C4;"};
String btnss[] = {
    "&#x23ee;",
    "&#x25B6;",
    "&#x23F9;",
    "&#x1F534;",
    "&#x1F501;",
    "&#x1F5AB;"};
String moveBtn[9] = {
    "&#x23EE;",
    "&#x23EA;",
    "&#x23E9;",
    "&#x23ED;",
    "&#x2770;",
    "&#x25B2;",
    "&#x25BC;",
    "&#x2771;",
    "&#x27F3;"};

String sw[5] = {"Scaling", "Mapping", "Ratchet", "Synth", "TB"};
String synthitems = "";
