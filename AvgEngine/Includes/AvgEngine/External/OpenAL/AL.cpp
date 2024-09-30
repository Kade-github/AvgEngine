#include "AL.h"

#include <AvgEngine/External/minivorbis.h>

ALCdevice* AvgEngine::External::OpenAL::AL::device = nullptr;
ALCcontext* AvgEngine::External::OpenAL::AL::context = nullptr;

void AvgEngine::External::OpenAL::AL::LoadOgg(const std::string& path, AvgEngine::Audio::Channel* channel)
{
    // Open the Ogg file
    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
		AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to open OGG file: " + path);
        return;
    }

    vorbis_info* vi;
    vorbis_comment* vc;
    OggVorbis_File oggFile;

    if (ov_open(file, &oggFile, NULL, 0) < 0) {
		AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to open OGG file: " + path);
        fclose(file);
        return;
    }

    vi = ov_info(&oggFile, -1);
    int channels = vi->channels;
    int sampleRate = vi->rate;

    // Read the audio data
    int totalSize = 0;
    int current_section;
    size_t bufferSize = 4096; // Start with 4 KB
    char* bufferData = (char*)malloc(bufferSize);
	if (!bufferData) {
		AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to allocate memory for OGG file: " + path);
		ov_clear(&oggFile);
		fclose(file);
		return;
	}
    int bytesRead;

    // Read the audio data
    while ((bytesRead = ov_read(&oggFile, bufferData + totalSize, bufferSize - totalSize, 0, 2, 1, &current_section)) > 0) {
        totalSize += bytesRead;

        // If the buffer is full, resize it
        if (totalSize >= bufferSize) {
            bufferSize *= 2; // Double the size
            bufferData = (char*)realloc(bufferData, bufferSize);
            if (!bufferData) {
				AvgEngine::Logging::writeLog("[OpenAL] [Error] Failed to allocate memory for OGG file: " + path);
                ov_clear(&oggFile);
                fclose(file);
                return;
            }
        }
    }

	AvgEngine::Logging::writeLog("[OpenAL] [Info] Loaded OGG file: " + path + " - Channels: " + std::to_string(channels) + " - Sample rate: " + std::to_string(sampleRate) + " - Size: " + std::to_string(totalSize));

	channel->LoadData((const char*)bufferData, totalSize, sampleRate, channels, 16);

	ov_clear(&oggFile);
	fclose(file);
}
