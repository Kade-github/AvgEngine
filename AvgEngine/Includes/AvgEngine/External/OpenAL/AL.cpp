#include "AL.h"

#include <AvgEngine/External/minivorbis.h>

ALCdevice* AvgEngine::External::OpenAL::AL::device = nullptr;
ALCcontext* AvgEngine::External::OpenAL::AL::context = nullptr;

void AvgEngine::External::OpenAL::AL::LoadOgg(const std::string& path, AvgEngine::Audio::Channel* channel)
{
	FILE* file = fopen(path.c_str(), "rb");
	if (!file)
	{
		AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to open file: " + path);
		return;
	}

	int channels, sampleRate;
	short* output = NULL;
	int samples = 0;

	OggVorbis_File vorbis;

	if (ov_open_callbacks(file, &vorbis, NULL, 0, OV_CALLBACKS_DEFAULT) != 0)
	{
		AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to open OGG file: " + path);
		return;
	}

	vorbis_info* info = ov_info(&vorbis, -1);
	channels = info->channels;
	sampleRate = info->rate;
	samples = ov_pcm_total(&vorbis, -1);

	output = (short*)std::malloc(samples * channels * 2);

	while (1)
	{
		int current_section;
		long ret = ov_read(&vorbis, (char*)output, sizeof(output), 0, 2, 1, &current_section);
		if (ret == 0)
			break;
		else if (ret < 0)
		{
			AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to read OGG file: " + path);
			return;
		}
	}

	ov_clear(&vorbis);

	AvgEngine::Logging::writeLog("[OpenAL] [Info] Loaded OGG file: " + path + " (" + std::to_string(samples) + " samples, " + std::to_string(channels) + " channels, " + std::to_string(sampleRate) + " Hz)");

	channel->LoadData((const char*)output, samples * channels * 2, sampleRate, channels, 16);
	free(output);
}
