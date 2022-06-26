
void settrill() {
  // Initialise serial and touch sensor

  int ret = trillSensor.setup(Trill::TRILL_SQUARE);
  if (ret != 0) {
    Serial.println("failed to initialise trillSensor");
   DBG("Error code: ");
    Serial.println(ret);
  }// else {
  //  Serial.println("Success initialising trillSensor");
  //}
}

void checkTrill() {

  trillSensor.read();

  if (trillSensor.getNumTouches() > 0 && trillSensor.getNumHorizontalTouches() > 0) {
    //  DBG("V[");
    //  DBG(trillSensor.getNumTouches());
    //  DBG("]: ");

    for (int i = 0; i < trillSensor.getNumTouches() &&0; i++) {
     DBG(trillSensor.touchLocation(i));
     DBG(" = loc ");
     DBG(trillSensor.touchSize(i));
     DBG(" =size ");
    }
    //    Serial.println();
    //    return;
    //   DBG("H[");
    //   DBG(trillSensor.getNumHorizontalTouches());
    //   DBG("]: ");
    for (int i = 0; i < (int)trillSensor.getNumHorizontalTouches(); i++) {
      byte zone = map(trillSensor.touchHorizontalLocation(i), 0, 1792, 0, 5) ;
   //        DBG(trillSensor.touchSize(i));
   //       DBG(" size @ ");
   //         Serial.println(zone);
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
