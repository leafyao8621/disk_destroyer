#include "config.h"

const char *DiskDestroyer::Config::ict = "\xFF";
const char *DiskDestroyer::Config::itsg = "\x01\xFF\x01\x00\xFF";
const char *DiskDestroyer::Config::hmg_baseline = "\xFF";
const char *DiskDestroyer::Config::hmg_enhanced = "\x01\xFF\x01\x00\xFF";
const char *DiskDestroyer::Config::guttman =
    "\xFF\xFF\xFF\xFF\xFF"
    "\x01\x55"
    "\x01\xAA"
    "\x03\x92\x49\x24"
    "\x03\x49\x24\x92"
    "\x03\x24\x92\x49"
    "\x01\x00"
    "\x01\x11"
    "\x01\x22"
    "\x01\x33"
    "\x01\x44"
    "\x01\x55"
    "\x01\x66"
    "\x01\x77"
    "\x01\x88"
    "\x01\x99"
    "\x01\xAA"
    "\x01\xBB"
    "\x01\xCC"
    "\x01\xDD"
    "\x01\xEE"
    "\x01\xFF"
    "\x03\x92\x49\x24"
    "\x03\x49\x24\x92"
    "\x03\x24\x92\x49"
    "\x03\x6D\xB6\xDB"
    "\x03\xB6\xDB\x6D"
    "\x03\xDB\x6D\xB6"
    "\xFF\xFF\xFF\xFF";

const char *DiskDestroyer::Config::config[5] = {
    itsg,
    ict,
    hmg_baseline,
    hmg_enhanced,
    guttman
};
