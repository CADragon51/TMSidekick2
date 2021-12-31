#pragma once
class SSD1309AsciiSpi1 : public SSD1306Ascii {
  public:
    /**
       @brief Initialize the display controller.

       @param[in] dev A device initialization structure.
       @param[in] cs The display controller chip select pin.
       @param[in] dc The display controller data/command pin.
    */
    void begin(const DevType* dev, uint8_t cs, uint8_t dc) {
      m_cs = cs;
      m_dc = dc;
      pinMode(m_cs, OUTPUT);
      pinMode(m_dc, OUTPUT);
      SPI1.begin();
      init(dev);
    }
    /**
       @brief Initialize the display controller.

       @param[in] dev A device initialization structure.
       @param[in] cs The display controller chip select pin.
       @param[in] dc The display controller cdata/command pin.
       @param[in] rst The display controller reset pin.
    */
    void begin(const DevType* dev, uint8_t cs, uint8_t dc, uint8_t rst) {
      oledReset(rst);
      begin(dev, cs, dc);
    }

  protected:
    void writeDisplay(uint8_t b, uint8_t mode) {
//      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(m_dc, mode != SSD1306_MODE_CMD);
      SPI1.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
      digitalWrite(m_cs, LOW);
      SPI1.transfer(b);
      digitalWrite(m_cs, HIGH);
      SPI1.endTransaction();
//      digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW

    }

    int8_t m_cs;
    int8_t m_dc;
};
