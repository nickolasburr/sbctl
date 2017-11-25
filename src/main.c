#include "main.h"

CFStringRef find_serial(int idVendor, int idProduct) {
  CFMutableDictionaryRef matchingDictionary = IOServiceMatching(kIOUSBDeviceClassName);

  CFNumberRef numberRef;
  numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idVendor);
  CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBVendorID), numberRef);
  CFRelease(numberRef);
  numberRef = 0;

  numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &idProduct);
  CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBProductID), numberRef);
  CFRelease(numberRef);
  numberRef = 0;

  io_iterator_t iter = NULL;
  if (IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDictionary, &iter) == KERN_SUCCESS) {
    io_service_t usbDeviceRef;
    if ((usbDeviceRef = IOIteratorNext(iter))) {
      CFMutableDictionaryRef dict = NULL;
      if (IORegistryEntryCreateCFProperties(usbDeviceRef, &dict, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS) {
        CFTypeRef obj = CFDictionaryGetValue(dict, CFSTR(kIOHIDSerialNumberKey));
        if (!obj) {
          obj = CFDictionaryGetValue(dict, CFSTR(kUSBSerialNumberString));
        }
        if (obj) {
          return CFStringCreateCopy(kCFAllocatorDefault, (CFStringRef)obj);
        }
      }
    }
  }
  return NULL;
}

int main(void) {
  CFStringRef obj;
  
  obj = find_serial(0x90c, 0x1000); // serial mode
  if (!obj) {
    obj = find_serial(0x1c34, 0x7241); // keyboard mode
  }

  if (obj) {
    char serial[256];
    if (CFStringGetCString(obj, serial, 256, CFStringGetSystemEncoding())) {
      printf("got serial: %s\n", serial);
    }
  } else {
    printf("No matching USB devices found\n");
  }
  return 0;
}
