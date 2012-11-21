#define SNDRV_RAWMIDI_INFO_OUTPUT		0x00000001
#define SNDRV_RAWMIDI_INFO_INPUT		0x00000002
#define SNDRV_RAWMIDI_INFO_DUPLEX		0x00000004

#define SNDRV_FILE_RAWMIDI          "/dev/midiC%iD%i"

typedef struct _snd_ctl snd_ctl_t;

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

struct sndrv_rawmidi_status {
	int stream;
	struct timespec tstamp;		/* Timestamp */
	size_t avail;			/* available bytes */
	size_t xruns;			/* count of overruns since last status (in bytes) */
	unsigned char reserved[16];	/* reserved for future use */
};

struct sndrv_rawmidi_params {
	int stream;
	size_t buffer_size;		/* queue size in bytes */
	size_t avail_min;		/* minimum avail bytes for wakeup */
	unsigned int no_active_sensing: 1; /* do not send active sensing byte in close() */
	unsigned char reserved[16];	/* reserved for future use */
};

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


#define _snd_rawmidi_info sndrv_rawmidi_info
#define _snd_rawmidi_params sndrv_rawmidi_params
#define _snd_rawmidi_status sndrv_rawmidi_status


/* open a device file descriptor */
static inline int rawmidi_open_device(const char *filename, int fmod);

