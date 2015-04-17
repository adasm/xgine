#include "alAudio.h"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")
//#pragma comment(lib, "ogg_static.lib")
//#pragma comment(lib, "vorbis_static.lib")
//#pragma comment(lib, "vorbisfile_static.lib")

#include <stdio.h>
#include "codec.h"
#include "vorbisfile.h"

alAudio::alAudio()
{
	dev = alcOpenDevice(NULL);
	ctx = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(ctx);
	//alDistanceModel(AL_INVERSE_DISTANCE);
	//alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	alListenerf(AL_GAIN, 1.0f);
}

alAudio::~alAudio()
{
	clear();
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(dev);
}

void alAudio::clear()
{
	i32 index = sounds.size();
	while (index--)
	{
		deleteSound(index);
	}

	index = soundSources.size();
	while (index--)
	{
		deleteSoundSource(index);
	}
}

intID alAudio::addSound(const c8 *fileName, u32 flags)
{
	Sound sound;
	alGetError();

	/*const c8 *ext = strrchr(fileName, '.') + 1;

	if(_stricmp(ext, "ogg") == 0)
	{
		FILE *file = fopen(fileName, "rb");
		if (file == NULL)
		{
			gEngine.kernel->log->prnEx("Couldn't open \"%s\"", fileName);
			return SOUND_NONE;
		}

		OggVorbis_File vf;
		memset(&vf, 0, sizeof(vf));
		if (ov_open(file, &vf, NULL, 0) < 0)
		{
			fclose(file);
			gEngine.kernel->log->prnEx("\"%s\" is not an ogg file", fileName);
			return SOUND_NONE;
		}

		vorbis_info *vi = ov_info(&vf, -1);

		int nSamples = (u32) ov_pcm_total(&vf, -1);
		int nChannels = vi->channels;
		sound.format = nChannels == 1? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		sound.sampleRate = vi->rate;

		sound.size = nSamples * nChannels;

		sound.samples = new short[sound.size];
		sound.size *= sizeof(short);

		int samplePos = 0;
		while (samplePos < sound.size)
		{
			char *dest = ((char *) sound.samples) + samplePos;
			int bitStream, readBytes = ov_read(&vf, dest, sound.size - samplePos, 0, 2, 1, &bitStream);
			if (readBytes <= 0) break;
			samplePos += readBytes;
		}

		ov_clear(&vf);

	} 
	else 
	{*/
		ALboolean al_bool;
		ALvoid *data;
		alutLoadWAVFile(fileName, &sound.format, &data, &sound.size, &sound.sampleRate, &al_bool);
		sound.samples = (short *) data;
	//}

	alGenBuffers(1, &sound.buffer);
	alBufferData(sound.buffer, sound.format, sound.samples, sound.size, sound.sampleRate);
	if (alGetError() != AL_NO_ERROR)
	{
		alDeleteBuffers(1, &sound.buffer);
		gEngine.kernel->log->prnEx(LT_ERROR, "alAudio", "Audio: Couldn't open \"%s\"", fileName);
		return SOUND_NONE;
	}

	return insertSound(sound);
}

intID alAudio::insertSound(Sound &sound)
{
	for (u32 i = 0; i < sounds.size(); i++)
	{
		if (sounds[i].samples == NULL)
		{
			sounds[i] = sound;
			return i;
		}
	}

	sounds.push_back(sound);
	return (sounds.size()-1);
}

void alAudio::deleteSound(intID sound)
{
	if (sounds[sound].samples)
	{
		alDeleteBuffers(1, &sounds[sound].buffer);
		alutUnloadWAV(sounds[sound].format, sounds[sound].samples, sounds[sound].size, sounds[sound].sampleRate);
		sounds[sound].samples = NULL;
	}
}

intID alAudio::addSoundSource(intID sound, u32 flags)
{
	SoundSrc soundSource;
	soundSource.sound = sound;
	alGenSources(1, &soundSource.source);
	alSourcei(soundSource.source, AL_LOOPING, (flags & LOOPING)? AL_TRUE : AL_FALSE);
	alSourcei(soundSource.source, AL_SOURCE_RELATIVE, (flags & RELATIVEPOS)? AL_TRUE : AL_FALSE);
	alSourcei(soundSource.source, AL_BUFFER, sounds[sound].buffer);
	alSourcef(soundSource.source, AL_MIN_GAIN, 0.0f);
	alSourcef(soundSource.source, AL_MAX_GAIN, 1.0f);
	return insertSoundSource(soundSource);
}

intID alAudio::insertSoundSource(SoundSrc &source)
{
	for (u32 i = 0; i < soundSources.size(); i++)
	{
		if (soundSources[i].sound == SOUND_NONE)
		{
			soundSources[i] = source;
			return i;
		}
	}

	soundSources.push_back(source);
	return (soundSources.size()-1);
}

void alAudio::deleteSoundSource(intID source)
{
	if (soundSources[source].sound != SOUND_NONE){
		alDeleteSources(1, &soundSources[source].source);
		soundSources[source].sound = SOUND_NONE;
	}
}

void alAudio::play(intID source)
{
	alSourcePlay(soundSources[source].source);
}

void alAudio::stop(intID source)
{
	alSourceStop(soundSources[source].source);
}

void alAudio::pause(intID source)
{
	alSourcePause(soundSources[source].source);
}

u32 alAudio::isPlaying(intID source)
{
	ALint state;
	alGetSourcei(soundSources[source].source, AL_SOURCE_STATE, &state);

	return (state == AL_PLAYING);
}

void alAudio::setListenerOrientation(Vec3 &position, Vec3 &zDir)
{
	alListenerfv(AL_POSITION, position);
	f32 orient[] = { zDir.x, zDir.y, zDir.z, 0, -1, 0 };
	alListenerfv(AL_ORIENTATION, orient);
}

void alAudio::setSourceGain(intID source, f32 gain)
{
	alSourcef(soundSources[source].source, AL_GAIN, gain);
}

void alAudio::setSourcePosition(intID source, Vec3 &position)
{
	alSourcefv(soundSources[source].source, AL_POSITION, position);
}

void alAudio::setSourceAttenuation(intID source, f32 rollOff, f32 refDistance)
{
	alSourcef(soundSources[source].source, AL_REFERENCE_DISTANCE, refDistance);
	alSourcef(soundSources[source].source, AL_ROLLOFF_FACTOR, rollOff);
}
