#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <endian.h>
#include <stdarg.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <errno.h>

#define SND_RAWMIDI_APPEND       0x0001
#define SND_RAWMIDI_NONBLOCK     0x0002
#define SND_RAWMIDI_SYNC         0x0004

#define SNDRV_RAWMIDI_INFO_OUTPUT		0x00000001
#define SNDRV_RAWMIDI_INFO_INPUT		0x00000002
#define SNDRV_RAWMIDI_INFO_DUPLEX		0x00000004
#define SNDRV_FILE_RAWMIDI          "/dev/midi%i"

/* 
   rawmidi info structure and type definitions
*/
struct sndrv_rawmidi_info {
	unsigned int device;		/* RO/WR (control): device number */
	unsigned int subdevice;		/* RO/WR (control): subdevice number */
	int stream;			/* WR: stream */
	int card;			/* R: card number */
	unsigned int flags;		/* SNDRV_RAWMIDI_INFO_XXXX */
	unsigned char id[64];		/* ID (user selectable) */
	unsigned char name[80];		/* name of device */
	unsigned char subname[32];	/* name of active or selected subdevice */
	unsigned int subdevices_count;
	unsigned int subdevices_avail;
	unsigned char reserved[64];	/* reserved for future use */
};
#define _snd_rawmidi_info sndrv_rawmidi_info
typedef struct _snd_rawmidi_info snd_rawmidi_info_t;
/*---------------------------------------------------*/

/* 
   rawmidi status structure and type definitions
*/
struct sndrv_rawmidi_status {
	int stream;
	struct timespec tstamp;		/* Timestamp */
	size_t avail;			/* available bytes */
	size_t xruns;			/* count of overruns since last status (in bytes) */
	unsigned char reserved[16];	/* reserved for future use */
};
#define _snd_rawmidi_params sndrv_rawmidi_params
typedef struct _snd_rawmidi_params snd_rawmidi_params_t;
/*---------------------------------------------------*/

/* 
   rawmidi params structure and type definitions
 */
struct sndrv_rawmidi_params {
	int stream;
	size_t buffer_size;		/* queue size in bytes */
	size_t avail_min;		/* minimum avail bytes for wakeup */
	unsigned int no_active_sensing: 1; /* do not send active sensing byte in close() */
	unsigned char reserved[16];	/* reserved for future use */
};
#define _snd_rawmidi_status sndrv_rawmidi_status
typedef struct _snd_rawmidi_status snd_rawmidi_status_t;
/*---------------------------------------------------*/

enum {
	SNDRV_RAWMIDI_IOCTL_PVERSION = _IOR('W', 0x00, int),
	SNDRV_RAWMIDI_IOCTL_INFO = _IOR('W', 0x01, struct sndrv_rawmidi_info),
	SNDRV_RAWMIDI_IOCTL_PARAMS = _IOWR('W', 0x10, struct sndrv_rawmidi_params),
	SNDRV_RAWMIDI_IOCTL_STATUS = _IOWR('W', 0x20, struct sndrv_rawmidi_status),
	SNDRV_RAWMIDI_IOCTL_DROP = _IOW('W', 0x30, int),
	SNDRV_RAWMIDI_IOCTL_DRAIN = _IOW('W', 0x31, int),
};

typedef struct {
	int open;
	int fd;
	int card, device, subdevice;
} snd_rawmidi_hw_t;

/** RawMidi stream (direction) */
typedef enum _snd_rawmidi_stream {
	/** Output stream */
	SND_RAWMIDI_STREAM_OUTPUT = 0,
	/** Input stream */
	SND_RAWMIDI_STREAM_INPUT,
	SND_RAWMIDI_STREAM_LAST = SND_RAWMIDI_STREAM_INPUT
} snd_rawmidi_stream_t;

typedef enum _snd_rawmidi_type {
	/** Kernel level RawMidi */
	SND_RAWMIDI_TYPE_HW,
	/** Shared memory client RawMidi (not yet implemented) */
	SND_RAWMIDI_TYPE_SHM,
	/** INET client RawMidi (not yet implemented) */
	SND_RAWMIDI_TYPE_INET,
	/** Virtual (sequencer) RawMidi */
	SND_RAWMIDI_TYPE_VIRTUAL
} snd_rawmidi_type_t;


typedef struct _snd_rawmidi snd_rawmidi_t;

typedef struct {
	int (*close)(snd_rawmidi_t *rawmidi);
	int (*nonblock)(snd_rawmidi_t *rawmidi, int nonblock);
	int (*info)(snd_rawmidi_t *rawmidi, snd_rawmidi_info_t *info);
	int (*params)(snd_rawmidi_t *rawmidi, snd_rawmidi_params_t *params);
	int (*status)(snd_rawmidi_t *rawmidi, snd_rawmidi_status_t *status);
	int (*drop)(snd_rawmidi_t *rawmidi);
	int (*drain)(snd_rawmidi_t *rawmidi);
	ssize_t (*write)(snd_rawmidi_t *rawmidi, const void *buffer, size_t size);
	ssize_t (*read)(snd_rawmidi_t *rawmidi, void *buffer, size_t size);
} snd_rawmidi_ops_t;

struct _snd_rawmidi {
	void *dl_handle;
	char *name;
	snd_rawmidi_type_t type;
	snd_rawmidi_stream_t stream;
	int mode;
	int poll_fd;
	const snd_rawmidi_ops_t *ops;
	void *private_data;
	size_t buffer_size;
	size_t avail_min;
	unsigned int no_active_sensing: 1;
};

/*
  procedure prototypes:
*/

/* open and return a device file descriptor */
static inline int rawmidi_open_device(const char *filename, int fmod);
int rawmidi_hw_print_info(const char *filepath);
int rawmidi_hw_open(snd_rawmidi_t **inputp, snd_rawmidi_t **outputp,
                    const char *filename, const char *name, int mode);
static int rawmidi_hw_params(snd_rawmidi_t *rmidi, snd_rawmidi_params_t * params);
static int rawmidi_hw_status(snd_rawmidi_t *rmidi, snd_rawmidi_status_t * status);
static int rawmidi_hw_drop(snd_rawmidi_t *rmidi);
static int rawmidi_hw_drain(snd_rawmidi_t *rmidi);
static ssize_t rawmidi_hw_write(snd_rawmidi_t *rmidi, const void *buffer, size_t size);
static ssize_t rawmidi_hw_read(snd_rawmidi_t *rmidi, void *buffer, size_t size);
static int rawmidi_hw_close(snd_rawmidi_t *rmidi);
static int rawmidi_hw_nonblock(snd_rawmidi_t *rmidi, int nonblock);
static int rawmidi_hw_info(snd_rawmidi_t *rmidi, snd_rawmidi_info_t * info);
