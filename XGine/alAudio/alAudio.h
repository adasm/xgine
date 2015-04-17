#pragma once
#include "..\XGine.h"

#include "al.h"
#include "alc.h"
#include "alut.h"

#define SOUND_NONE	(-1)
#define LOOPING		0x1
#define RELATIVEPOS 0x2

typedef i32 intID;

struct XGINE_API Sound 
{
	ALenum	format;
	ALuint	buffer;
	i16		*samples;
	i32		sampleRate;
	i32		size;
};

struct XGINE_API SoundSrc 
{
	ALuint	source;
	intID	sound;
};

struct XGINE_API alAudio 
{
	alAudio();
	~alAudio();

	void	clear();

	intID	addSound(const c8 *fileName, u32 flags = 0);
	void	deleteSound(intID sound);

	intID	addSoundSource(intID sound, u32 flags = 0);
	void	deleteSoundSource(intID source);

	void	play(intID source);
	void	stop(intID source);
	void	pause(intID source);
	u32		isPlaying(intID source);

	void	setListenerOrientation(Vec3 &position, Vec3 &zDir);

	void	setSourceGain(intID source, f32 gain);
	void	setSourcePosition(intID source, Vec3 &position);
	void	setSourceAttenuation(intID source, f32 rollOff, f32 refDistance);

protected:
	intID insertSound(Sound &sound);
	intID insertSoundSource(SoundSrc &source);


	ALCcontext	*ctx;
	ALCdevice	*dev;

	vector<Sound>		sounds;
	vector<SoundSrc>	soundSources;
};
