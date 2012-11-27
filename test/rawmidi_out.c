#include "../include/rawmidi.h"

int main() {
  int err, i;
  unsigned char note_on[3];
  unsigned char note_off[3];
  snd_rawmidi_t *output;

  note_on[0] = 0x90;
  note_on[1] = 60;
  note_on[2] = 100;

  note_off[0] = 0x90;
  note_off[1] = 60;
  note_off[2] = 0;

  if ( err = rawmidi_hw_open(0, &output, "/dev/midi2", "UM-1", 0) < 0 ) {
    printf("can't open midi output for writing\n");
  }

  i = 0;
  while( i < 10 ) {
    rawmidi_hw_write(output, note_on, 3);

    sleep(3);

    rawmidi_hw_write(output, note_off, 3);

    sleep(3);
    
    i++;
  }

  return 0;
}
