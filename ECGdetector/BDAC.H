
#define BEAT_SAMPLE_RATE	100
#define BEAT_MS_PER_SAMPLE	( (double) 1000/ (double) BEAT_SAMPLE_RATE)

#define BEAT_MS10		((int) (10/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS20		((int) (20/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS40		((int) (40/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS50		((int) (50/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS60		((int) (60/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS70		((int) (70/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS80		((int) (80/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS90		((int) (90/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS100	((int) (100/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS110	((int) (110/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS130	((int) (130/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS140	((int) (140/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS150	((int) (150/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS250	((int) (250/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS280	((int) (280/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS300	((int) (300/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS350	((int) (350/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS400	((int) (400/BEAT_MS_PER_SAMPLE + 0.5))
#define BEAT_MS1000	BEAT_SAMPLE_RATE

#define BEATLGTH	BEAT_MS1000
#define MAXTYPES 8
#define FIDMARK BEAT_MS400


void ResetBDAC(void);

int peakfinder(short data, bool init, short refractory );
int BeatDetectAndClassify(int ecgSample);

void ResetNoiseCheck();

