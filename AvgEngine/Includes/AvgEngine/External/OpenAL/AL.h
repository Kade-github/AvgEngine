#ifndef _AVGENGINE_OPENAL_AL_H_
#define _AVGENGINE_OPENAL_AL_H_
#include <AL/al.h>
#include <AL/alc.h>
#include <string>

#include "../../Audio/Channel.h"

#include <AvgEngine/Utils/Logging.h>

namespace AvgEngine::External::OpenAL
{
	class AL
	{
	public:
		static ALCdevice* device;
		static ALCcontext* context;

		// device list
		static std::vector<std::string> listDevices()
		{
			const ALCchar* devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
			const ALCchar* defaultDevice = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

			std::vector<std::string> deviceList;

			AvgEngine::Logging::writeLog("[OpenAL] [Info] Devices:");
			while (*devices)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Info] " + std::string(devices));
				deviceList.push_back(devices);
				devices += strlen(devices) + 1;
			}
			AvgEngine::Logging::writeLog("[OpenAL] [Info] Default device: " + std::string(defaultDevice));

			return deviceList;
		}

		static void switchDevice(const std::string& deviceName)
		{
			if (device)
			{
				alcCloseDevice(device);
				device = alcOpenDevice(deviceName.c_str());
				if (!device)
				{
					AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to open device: " + deviceName);
					getError();
					return;
				}
			}
		}

		static void close()
		{
			if (device)
			{
				alcCloseDevice(device);
				device = nullptr;
			}
		}

		static void init()
		{
			const ALCchar* defaultDeviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);

			device = alcOpenDevice(defaultDeviceName);
			if (!device)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to open default device.");
				getError();
				return;
			}
		}

		static void getError()
		{
			ALenum error = alGetError();
			if (error != AL_NO_ERROR)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Error: " + std::to_string(error));
			}
		}


		// context

		static void createContext()
		{
			if (context)
			{
				alcDestroyContext(context);
			}

			context = alcCreateContext(device, nullptr);
			if (!context)
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to create context.");
				getError();
				return;
			}

			if (!alcMakeContextCurrent(context))
			{
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to make context current.");
				getError();
				return;
			}
		}

		// channels

		static AvgEngine::Audio::Channel* createChannel()
		{
			return new AvgEngine::Audio::Channel();
		}

		static void LoadOgg(const std::string& path, AvgEngine::Audio::Channel* channel);
	};
}

#endif