#ifndef IMPORTS_IOKIT_H
#define IMPORTS_IOKIT_H

int IOBSDNameMatching();
int IOIteratorNext();
int IOMasterPort();
int IOObjectConformsTo();
int IOObjectGetClass();
int IOObjectRelease();
int IOObjectRetain();
int IORegistryEntryCreateCFProperty();
int IORegistryEntryCreateIterator();
int IORegistryEntryGetParentEntry();
int IOServiceGetMatchingServices();

#endif
