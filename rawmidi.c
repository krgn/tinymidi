#include "kuri.h"

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


int rawmidi_hw_open() {
  snd_ctl_t *ctl;
  snd_rawmidi_hw_t *hw = NULL;
  snd_rawmidi_info_t info;

  char filename[sizeof(SNDRV_FILE_RAWMIDI) + 20];
  int fmode; 
  
  if (!inputp)
    fmode = O_WRONLY;
  else if (!outputp)
    fmode = O_RDONLY;
  else
    fmode = O_RDWR;

  fd = rawmidi_open_device(filename, fmode);
  if (fd < 0) {
    return -1;
  }

  memset(&info, 0, sizeof(info));
  ioctl(fd, SNDRV_RAWMIDI_IOCTL_INFO, &info);

  hw = calloc(1, sizeof(snd_rawmidi_hw_t));
  hw->card = card;
  hw->device = device;
  hw->subdevice = subdevice;
  hw->fd = fd;

  if (inputp) {
    rmidi = calloc(1, sizeof(snd_rawmidi_t));
    if (rmidi == NULL)
      goto _nomem;
    if (name)
      rmidi->name = strdup(name);
    rmidi->type = SND_RAWMIDI_TYPE_HW;
    rmidi->stream = SND_RAWMIDI_STREAM_INPUT;
    rmidi->mode = mode;
    rmidi->poll_fd = fd;
    rmidi->ops = &snd_rawmidi_hw_ops;
    rmidi->private_data = hw;
    hw->open++;
    *inputp = rmidi;
  }

  if (outputp) {
    rmidi = calloc(1, sizeof(snd_rawmidi_t));
    if (rmidi == NULL)
      goto _nomem;
    if (name)
      rmidi->name = strdup(name);
    rmidi->type = SND_RAWMIDI_TYPE_HW;
    rmidi->stream = SND_RAWMIDI_STREAM_OUTPUT;
    rmidi->mode = mode;
    rmidi->poll_fd = fd;
    rmidi->ops = &snd_rawmidi_hw_ops;
    rmidi->private_data = hw;
    hw->open++;
    *outputp = rmidi;
  }
  return 0;
}
