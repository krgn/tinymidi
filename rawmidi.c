#include "include/rawmidi.h"

static const snd_rawmidi_ops_t snd_rawmidi_hw_ops = {
	.close = rawmidi_hw_close,
	.nonblock = rawmidi_hw_nonblock,
	.info = rawmidi_hw_info,
	.params = rawmidi_hw_params,
	.status = rawmidi_hw_status,
	.drop = rawmidi_hw_drop,
	.drain = rawmidi_hw_drain,
	.write = rawmidi_hw_write,
	.read = rawmidi_hw_read,
};

int rawmidi_hw_close(snd_rawmidi_t *rmidi)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	int err = 0;

	hw->open--;
	if (hw->open)
		return 0;
	if (close(hw->fd)) {
		err = -errno;
		printf("close failed\n");
	}
	free(hw);
	return err;
}

int rawmidi_hw_nonblock(snd_rawmidi_t *rmidi, int nonblock)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	long flags;

	if ((flags = fcntl(hw->fd, F_GETFL)) < 0) {
		printf("F_GETFL failed");
		return -errno;
	}
	if (nonblock)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;
	if (fcntl(hw->fd, F_SETFL, flags) < 0) {
		printf("F_SETFL for O_NONBLOCK failed");
		return -errno;
	}
	return 0;
}

int rawmidi_hw_info(snd_rawmidi_t *rmidi, snd_rawmidi_info_t * info)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	info->stream = rmidi->stream;
	if (ioctl(hw->fd, SNDRV_RAWMIDI_IOCTL_INFO, info) < 0) {
		printf("SNDRV_RAWMIDI_IOCTL_INFO failed");
		return -errno;
	}
	return 0;
}

/* opens a device file and returns file descriptor */
int rawmidi_open_device(const char *filename, int fmode)
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

int rawmidi_hw_open(snd_rawmidi_t **inputp, snd_rawmidi_t **outputp,
                    const char *filename, const char *name, int mode)
{
	int fd, ver, ret;
	snd_rawmidi_t *rmidi;
	snd_rawmidi_hw_t *hw = NULL;
	int fmode;

  if (inputp)
		*inputp = NULL;
	if (outputp)
		*outputp = NULL;

	if (!inputp)
		fmode = O_WRONLY;
	else if (!outputp)
		fmode = O_RDONLY;
	else
		fmode = O_RDWR;

	if (mode & SND_RAWMIDI_APPEND) {
		assert(outputp);
		fmode |= O_APPEND;
	}

	if (mode & SND_RAWMIDI_NONBLOCK) {
		fmode |= O_NONBLOCK;
	}
	
	if (mode & SND_RAWMIDI_SYNC) {
		fmode |= O_SYNC;
	}

	assert(!(mode & ~(SND_RAWMIDI_APPEND|SND_RAWMIDI_NONBLOCK|SND_RAWMIDI_SYNC)));

	fd = rawmidi_open_device(filename, fmode);
  if( fd < 0 )
    return -errno;

  hw = calloc(1, sizeof(snd_rawmidi_hw_t));
  if (hw == NULL)
    return -errno;
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

_nomem:
	close(fd);
	free(hw);
	if (inputp)
		free(*inputp);
	if (outputp)
		free(*outputp);
	return -ENOMEM;
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


int rawmidi_hw_params(snd_rawmidi_t *rmidi, snd_rawmidi_params_t * params)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	params->stream = rmidi->stream;
	if (ioctl(hw->fd, SNDRV_RAWMIDI_IOCTL_PARAMS, params) < 0) {
		printf("SNDRV_RAWMIDI_IOCTL_PARAMS failed");
		return -errno;
	}
	return 0;
}

int rawmidi_hw_status(snd_rawmidi_t *rmidi, snd_rawmidi_status_t * status)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	status->stream = rmidi->stream;
	if (ioctl(hw->fd, SNDRV_RAWMIDI_IOCTL_STATUS, status) < 0) {
		printf("SNDRV_RAWMIDI_IOCTL_STATUS failed");
		return -errno;
	}
	return 0;
}

int rawmidi_hw_drop(snd_rawmidi_t *rmidi)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	int str = rmidi->stream;
	if (ioctl(hw->fd, SNDRV_RAWMIDI_IOCTL_DROP, &str) < 0) {
		printf("SNDRV_RAWMIDI_IOCTL_DROP failed");
		return -errno;
	}
	return 0;
}

int rawmidi_hw_drain(snd_rawmidi_t *rmidi)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	int str = rmidi->stream;
	if (ioctl(hw->fd, SNDRV_RAWMIDI_IOCTL_DRAIN, &str) < 0) {
		printf("SNDRV_RAWMIDI_IOCTL_DRAIN failed");
		return -errno;
	}
	return 0;
}

ssize_t rawmidi_hw_write(snd_rawmidi_t *rmidi, const void *buffer, size_t size)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	ssize_t result;
	result = write(hw->fd, buffer, size);
	if (result < 0)
		return -errno;
	return result;
}

ssize_t rawmidi_hw_read(snd_rawmidi_t *rmidi, void *buffer, size_t size)
{
	snd_rawmidi_hw_t *hw = rmidi->private_data;
	ssize_t result;
	result = read(hw->fd, buffer, size);
	if (result < 0)
		return -errno;
	return result;
}
 

