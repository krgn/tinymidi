#include "../include/rawmidi.h"

int main() {
  return rawmidi_hw_print_info("/dev/midi2");
}
