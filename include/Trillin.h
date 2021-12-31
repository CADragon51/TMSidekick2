
void settrill() {
  // Initialise serial and touch sensor

  int ret = trillSensor.setup(Trill::TRILL_SQUARE);
  if (ret != 0) {
    Serial.println("failed to initialise trillSensor");
    Serial.print("Error code: ");
    Serial.println(ret);
  }// else {
  //  Serial.println("Success initialising trillSensor");
  //}
}

void checkTrill() {
  trillSensor.read();

  if (trillSensor.getNumTouches() > 0 && trillSensor.getNumHorizontalTouches() > 0) {
    //   if(debug==1)Serial.print("V[");
    //   if(debug==1)Serial.print(trillSensor.getNumTouches());
    //   if(debug==1)Serial.print("]: ");

    for (int i = 0; i < trillSensor.getNumTouches() &&0; i++) {
      if(debug==1)Serial.print(trillSensor.touchLocation(i));
      if(debug==1)Serial.print(" = loc ");
      if(debug==1)Serial.print(trillSensor.touchSize(i));
      if(debug==1)Serial.print(" =size ");
    }
    //    if(debug==1)Serial.println();
    //    return;
    //    if(debug==1)Serial.print("H[");
    //    if(debug==1)Serial.print(trillSensor.getNumHorizontalTouches());
    //    if(debug==1)Serial.print("]: ");
    for (int i = 0; i < (int)trillSensor.getNumHorizontalTouches(); i++) {
      byte zone = map(trillSensor.touchHorizontalLocation(i), 0, 1792, 0, 5) ;
   //         if(debug==1)Serial.print(trillSensor.touchSize(i));
   //        if(debug==1)Serial.print(" size @ ");
   //         if(debug==1)Serial.println(zone);
      MenuZoneSet* mzs = (MenuZoneSet * )Menus[ZONESETTINGS + zone];
      mzs->touchSize = imap(trillSensor.touchSize(i), 0, 4096, 0, 127);
      if (mzs->touchSize > 5) {
        mzs->rawvalue = trillSensor.touchLocation(i);
        mzs->eventtype = 7;
        mzs->action(true);

      }
    }
    touchActive = true;
  }
  else if (touchActive) {
    touchActive = false;
    for (int i = 0; i < 6; i++) {
      byte zone = i;
      MenuZoneSet* mzs = (MenuZoneSet*)Menus[ZONESETTINGS + zone];
      if (mzs->state) {
        mzs->action(false);
      }
    }
  }

}
