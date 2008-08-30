#include <usb.h>

// from /usr/src/linux/drivers/usb/input/usbkbd.c
static const unsigned char usb_kbd_keycode[256] = {
   0,  0,  0,  0, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 37, 38,
  50, 49, 24, 25, 16, 19, 31, 20, 22, 47, 17, 45, 21, 44,  2,  3,
   4,  5,  6,  7,  8,  9, 10, 11, 28,  1, 14, 15, 57, 12, 13, 26,
  27, 43, 43, 39, 40, 41, 51, 52, 53, 58, 59, 60, 61, 62, 63, 64,
  65, 66, 67, 68, 87, 88, 99, 70,119,110,102,104,111,107,109,106,
 105,108,103, 69, 98, 55, 74, 78, 96, 79, 80, 81, 75, 76, 77, 71,
  72, 73, 82, 83, 86,127,116,117,183,184,185,186,187,188,189,190,
 191,192,193,194,134,138,130,132,128,129,131,137,133,135,136,113,
 115,114,  0,  0,  0,121,  0, 89, 93,124, 92, 94, 95,  0,  0,  0,
 122,123, 90, 91, 85,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  29, 42, 56,125, 97, 54,100,126,164,166,165,163,161,115,114,113,
 150,158,159,128,136,177,178,176,142,152,173,140
};

// generated structures:

static const unsigned char kbd_lower[127] = {
'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', // 13
'\0', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', // 27
'\n', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', // 41
'\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*', // 55
'\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\n', '\0',
'/', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};
static const unsigned char kbd_upper[127] = {
'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
'\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
'\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
'\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

char code_to_key(bool shifted, unsigned int kcode) {
  //fprintf(stderr, "try %d\n", kcode);
  if(shifted) {
    //fprintf(stderr, "kbd_upper %c\n", kbd_upper[kcode]);
    return kbd_upper[kcode];
  }
  else {
    //fprintf(stderr, "kbd_lower %d '%c'\n", kcode, kbd_lower[kcode]);
    return kbd_lower[kcode];
  }
}

void cleanup(usb_dev_handle *handle) {
  int ret = usb_release_interface(handle, 0);
  usb_close(handle);
}

usb_dev_handle* _find_device (int vendor, int product, int iface) {
  struct usb_bus *bus;
  struct usb_device *device;
  usb_dev_handle *handle;
  int ret;

  usb_init();
  usb_find_busses();
  usb_find_devices();
  // TODO have my own globals for init?
  bus = usb_get_busses();

  for(; bus; bus = bus->next) {
    for(device = bus->devices; device; device = device->next) {
      if(device->descriptor.idVendor == vendor &&
          device->descriptor.idProduct == product) {
        handle = usb_open(device);
        // XXX non-portable, and I guess we don't need to retry
        usb_detach_kernel_driver_np(handle, iface);
        ret = usb_claim_interface(handle, iface);
        if(ret) {
          croak("could not claim device interface %d (%d)", iface, ret);
        }
        // usb_clear_halt(handle, 0x81);
        return(handle);
      }
    }
  }
  croak("failed to find device %x %x", vendor, product);
}

SV*	create (char* class, int vendor_id, int product_id, int iface) {
	SV*        obj_ref = newSViv(0);
	SV*        obj = newSVrv(obj_ref, class); // creates the blessed reference

  struct usb_dev_handle *handle = _find_device(vendor_id, product_id, iface);
  // fprintf(stderr, "got handle %d\n", handle);

	sv_setiv(obj, (IV) handle);
	SvREADONLY_on(obj);
	return(obj_ref);
}

#define PACKET_LEN 8

void _dump_packet(const char* packet) {
  int i;
  fprintf(stderr, "packet: 0x");
  for( i=0; i<PACKET_LEN; fprintf(stderr, "%02x ",packet[i++]) );
  fprintf(stderr, "\n");
}

// XXX hmm, this scheme can't detect a second key while you're holding
// one down.  I guess "don't do that" applies.
void _keycode(SV* obj, int timeout) {
	Inline_Stack_Vars;
  usb_dev_handle* handle = (usb_dev_handle*) SvIV(SvRV(obj));

	Inline_Stack_Reset;

  // XXX right_super is a lot bigger than 255 for some reason?
  unsigned char packet[PACKET_LEN];
  // croak("handle is %d", handle);
  int ret = usb_interrupt_read(handle, 0x81, packet, PACKET_LEN, timeout);
  if(ret > 0 && ! packet[3]) {
    // fprintf(stderr, "read %d bytes\n", ret);
    Inline_Stack_Push(sv_2mortal(newSViv(usb_kbd_keycode[packet[2]])));
    if(packet[0]) {
      packet[0] ^= packet[0] & 0xffffff00; // ugh
      // _dump_packet(packet);
      Inline_Stack_Push(sv_2mortal(newSViv(packet[0])));
    }
  }
  else {
    Inline_Stack_Push(sv_2mortal(newSViv(-1)));
  }
	Inline_Stack_Done;		
}

SV * _char(SV* obj) {
  usb_dev_handle* handle = (usb_dev_handle*) SvIV(SvRV(obj));
  SV * ans;

  char packet[PACKET_LEN];
  int ret = usb_interrupt_read(handle, 0x81, packet, PACKET_LEN, 1000);
  //fprintf(stderr, "read (%d)\n", ret);
  // 0 is the shift code (maybe also something else)
  // 2 is the scan code
  if((ret > 0) && packet[2]) {
    char c = code_to_key((packet[0] == 2), usb_kbd_keycode[packet[2]]);
    if(c) {
      const char str [2] = {c, '\0'};
      ans = newSVpvn( str, 1);
    }
    else {
      ans = newSVpvn( "", 0);
    }
  }
  else {
    ans = newSVpvn( "", 0);
  }

  return ans;
}

void _destroy(SV* obj) {
  usb_dev_handle* handle = (usb_dev_handle*) SvIV(SvRV(obj));
  if(handle)
    cleanup(handle);
}

// vim:ts=2:sw=2:et:sta
