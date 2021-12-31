#include "usb_names.h"

#define MIDI_NAME   {'T','M',' ','S','i','d','e','k','i','c','k'}
#define MIDI_NAME_LEN  11

#define MANUFACTURER_NAME  {'C','T',' ','D','e','n','k','i',' ','L','L','C'}
#define MANUFACTURER_NAME_LEN 11

struct usb_string_descriptor_struct usb_string_product_name = {
  2 + MIDI_NAME_LEN * 2,
  3,
  MIDI_NAME
};

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
  2 + MANUFACTURER_NAME_LEN * 2,
  3,
  MANUFACTURER_NAME
};
