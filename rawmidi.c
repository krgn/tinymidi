#include "include/rawmidi.h"

/* opens a device file and returns file descriptor */
static inline int rawmidi_open_device(const char *filename, int fmode)
{
	int fd;

  printf ("[RAWMIDI] opening %s\n", filename );
	fmode |= O_CLOEXEC;
	fd = open(filename, fmode);

	if (fd >= 0) {
    printf ("[RAWMIDI] setting FD_CLOEXEC flag on device file descriptor\n");
		fcntl(fd, F_SETFD, FD_CLOEXEC);
  }
  
	return fd;
}

void rawmidi_hw_open() {
  /* hw = calloc(1, sizeof(snd_rawmidi_hw_t)); */
  /* hw->card = card; */
  /* hw->device = device; */
  /* hw->subdevice = subdevice; */
  /* hw->fd = fd; */

  /* if (inputp) { */
  /*   rmidi = calloc(1, sizeof(snd_rawmidi_t)); */
  /*   if (rmidi == NULL) */
  /*     goto _nomem; */
  /*   if (name) */
  /*     rmidi->name = strdup(name); */
  /*   rmidi->type = SND_RAWMIDI_TYPE_HW; */
  /*   rmidi->stream = SND_RAWMIDI_STREAM_INPUT; */
  /*   rmidi->mode = mode; */
  /*   rmidi->poll_fd = fd; */
  /*   rmidi->ops = &snd_rawmidi_hw_ops; */
  /*   rmidi->private_data = hw; */
  /*   hw->open++; */
  /*   *inputp = rmidi; */
  /* } */

  /* if (outputp) { */
  /*   rmidi = calloc(1, sizeof(snd_rawmidi_t)); */
  /*   if (rmidi == NULL) */
  /*     goto _nomem; */
  /*   if (name) */
  /*     rmidi->name = strdup(name); */
  /*   rmidi->type = SND_RAWMIDI_TYPE_HW; */
  /*   rmidi->stream = SND_RAWMIDI_STREAM_OUTPUT; */
  /*   rmidi->mode = mode; */
  /*   rmidi->poll_fd = fd; */
  /*   rmidi->ops = &snd_rawmidi_hw_ops; */
  /*   rmidi->private_data = hw; */
  /*   hw->open++; */
  /*   *outputp = rmidi; */
  /* } */
}

/* print some info about a device */
int rawmidi_hw_print_info(const char *filepath) {
  snd_rawmidi_info_t info;
  int fd, fmode; 

  fmode = O_RDWR;
  fd = rawmidi_open_device(filepath, fmode);
  if (fd < 0) {
    return -1;
  }

  memset(&info, 0, sizeof(info));
  ioctl(fd, SNDRV_RAWMIDI_IOCTL_INFO, &info);

  printf ("[RAWMIDI] info.device \t%d\n",info.device);
  printf ("[RAWMIDI] info.subdevice \t%d\n", info.subdevice);
  printf ("[RAWMIDI] info.stream \t%d\n", info.stream);
  printf ("[RAWMIDI] info.card \t%d\n", info.card);
  printf ("[RAWMIDI] info.flags \t%d\n", info.flags);
  printf ("[RAWMIDI] info.id \t%s\n", info.id);
  printf ("[RAWMIDI] info.name \t%s\n", info.name);
  printf ("[RAWMIDI] info.subname \t%s\n", info.subname);
  printf ("[RAWMIDI] info.subdevices_count \t%d\n", info.subdevices_count);
  printf ("[RAWMIDI] info.subdevices_avail \t%d\n", info.subdevices_avail);

  return 0;
}

