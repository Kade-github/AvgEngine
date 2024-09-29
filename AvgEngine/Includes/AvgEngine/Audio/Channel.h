/* 
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>

#include <AL/al.h>
#include <AvgEngine/Utils/Logging.h>

namespace AvgEngine::Audio
{
	class Channel
	{
	public:
		bool hasEnded = false;
		ALuint id;
		ALuint buffer;
		bool autoFree = false;
		char* data;
		bool isPlaying;
		std::string name;
		std::string path;

		float rate = 1;

		float volume = 1;

		int length = 0;
		int sampleRate = 0;
		float lengthSeconds = 0;

		Channel()
		{
			alGenSources(1, &id);
			alGenBuffers(1, &buffer);

			alSourcef(id, AL_PITCH, 1);
			alSourcef(id, AL_GAIN, 1);
			alSource3f(id, AL_POSITION, 0, 0, 0);
			alSource3f(id, AL_VELOCITY, 0, 0, 0);
			alSourcei(id, AL_LOOPING, AL_FALSE);

			data = NULL;
			isPlaying = false;
		}

		void Update()
		{
			if (id == -1)
				return;
			ALint state;
			alGetSourcei(id, AL_SOURCE_STATE, &state);
			if (state == AL_STOPPED)
			{
				isPlaying = false;
				hasEnded = true;
			}
			else
				hasEnded = false;
		}

		void LoadData(const char* _data, int _length, int _rate, int _channels, int _format)
		{
			if (id == -1)
				return;
			data = (char*)std::malloc(_length);
			memcpy(data, _data, _length);
			length = _length;
			ALenum format = _channels == 2 ? (_format == 16 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8) : (_format == 16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8);
			alBufferData(buffer, format, data, length, _rate);
			if (alGetError() != AL_NO_ERROR)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to load buffer data.");
				return;
			}
			alSourcei(id, AL_BUFFER, buffer);
			if (alGetError() != AL_NO_ERROR)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to set buffer data.");
				return;
			}

			sampleRate = _rate;
			lengthSeconds = (float)length / (float)_rate;

			AvgEngine::Logging::writeLog("[OpenAL] [Info] Loaded buffer data." + std::to_string(length) + " bytes, " + std::to_string(_rate) + " Hz, " + std::to_string(_channels) + " channels, " + std::to_string(_format) + " bits.");
		}

		void Repeat(bool once)
		{
			if (id == -1)
				return;
			alSourcei(id, AL_LOOPING, once ? AL_FALSE : AL_TRUE);
		}

		/// <summary>
		/// Free up the channel
		/// </summary>
		void Free()
		{
			id = -1;

			if (data) 
				std::free(data);
		}

		/// <summary>
		/// Set the channel in a playing state
		/// </summary>
		void Play()
		{
			
			alSourcef(id, AL_GAIN, volume);
			alSourcef(id, AL_PITCH, rate);
			alSourcePlay(id);
			if (alGetError() != AL_NO_ERROR)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to play source.");
				return;
			}

			isPlaying = true;
		}

		/// <summary>
		/// Stop the channel
		/// </summary>
		void Stop()
		{
			if (id == -1)
				return;
			
			alSourceStop(id);

			isPlaying = false;
		}

		/// <summary>
		/// Get the current position of the channel in seconds
		/// </summary>
		/// <returns>Seconds</returns>
		float GetPos()
		{
			if (id == -1)
				return 0;
			ALint byteOffset;
			alGetSourcei(id, AL_BYTE_OFFSET, &byteOffset);
			return byteOffset / (float)length;
		}

		/// <summary>
		/// Set the current position of the channel
		/// </summary>
		/// <param name="s">Seconds</param>
		void SetPos(float s)
		{
			if (id == -1)
				return;

			alSourcef(id, AL_SEC_OFFSET, s);
		}

		/// <summary>
		/// Set the current channels time rate
		/// </summary>
		/// <param name="_rate">The rate to be set</param>
		void RateChange(float _rate)
		{
			if (id == -1)
				return;
			if (_rate < 0)
				_rate = 0.1;
			rate = _rate;
		}

		/// <summary>
		/// Set the current volume of the current channel
		/// </summary>
		/// <param name="vol">The volume to set</param>
		void SetVolume(float vol)
		{
			if (id == -1)
				return;
			volume = vol;

			alSourcef(id, AL_GAIN, volume);
		}

		bool operator==(const Channel& other) {
			return (name == other.name) || (id == other.id);
		}
	};
}

#endif // !CHANNEL_H