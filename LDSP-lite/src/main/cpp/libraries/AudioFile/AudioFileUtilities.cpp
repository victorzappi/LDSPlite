// This code is based on the code credited below, but it has been modified
// further by Victor Zappi
 
 /*
 ___  _____ _        _
| __ )| ____| |      / \
|  _ \|  _| | |     / _ \
| |_) | |___| |___ / ___ \
|____/|_____|_____/_/   \_\

The platform for ultra-low latency audio and sensor processing

http://bela.io

A project of the Augmented Instruments Laboratory within the Centre for Digital Music at Queen Mary University of London. http://instrumentslab.org

(c) 2016-2020 Augmented Instruments Laboratory: Andrew McPherson, Astrid Bin, Liam Donovan, Christian Heinrichs, Robert Jack, Giulio Moro, Laurel Pardue, Victor Zappi. All rights reserved.

The Bela software is distributed under the GNU Lesser General Public License (LGPL 3.0), available here: https://www.gnu.org/licenses/lgpl-3.0.txt */

#include "sndfile.h" // to load audio files
#include "AudioFile.h"
#include <unistd.h> //for sync
#include <cstdlib>
#include <iostream>
#include "files_utils.h"
#include "LDSP_log.h"

namespace AudioFileUtilities {

struct VirtualIOContext {
  sf_count_t pos;
  const std::vector<char>* fileData;
};

sf_count_t vf_get_filelen(void* user_data) {
  VirtualIOContext* context = reinterpret_cast<VirtualIOContext*>(user_data);
  return context->fileData->size();
}

sf_count_t vf_seek(sf_count_t offset, int whence, void* user_data) {
  VirtualIOContext* context = reinterpret_cast<VirtualIOContext*>(user_data);
  sf_count_t newPos = context->pos;

  switch (whence) {
    case SEEK_SET: newPos = offset; break;
    case SEEK_CUR: newPos += offset; break;
    case SEEK_END: newPos = context->fileData->size() + offset; break;
    default: return -1;
  }

  if (newPos < 0 || newPos > static_cast<sf_count_t>(context->fileData->size())) {
    return -1;
  }

  context->pos = newPos;
  return context->pos;
}

sf_count_t vf_read(void* ptr, sf_count_t count, void* user_data) {
  VirtualIOContext* context = reinterpret_cast<VirtualIOContext*>(user_data);
  if (context->pos + count > context->fileData->size()) {
    count = context->fileData->size() - context->pos;
  }

  memcpy(ptr, context->fileData->data() + context->pos, count);
  context->pos += count;
  return count;
}

sf_count_t vf_tell(void* user_data) {
  VirtualIOContext* context = reinterpret_cast<VirtualIOContext*>(user_data);
  return context->pos;
}


sf_count_t vf_write(const void* ptr, sf_count_t count, void* user_data) {
  // Writing is not supported, just log the attempt
  LDSP_log("vf_write: called with count %ld, but writing is not supported.\n", count);
  return 0;
}

int getSamples(const std::string& file, float* buf, unsigned int channel, unsigned int startFrame, unsigned int endFrame) {
  std::vector<char> fileData = readFile(file);  // Load the file into memory
  VirtualIOContext context = {0, &fileData};

  SF_VIRTUAL_IO virtualIO;
  virtualIO.get_filelen = vf_get_filelen;
  virtualIO.seek = vf_seek;
  virtualIO.read = vf_read;
  virtualIO.write = nullptr;
  virtualIO.tell = vf_tell;

  SF_INFO sfinfo;
  sfinfo.format = 0;
  SNDFILE* sndfile = sf_open_virtual(&virtualIO, SFM_READ, &sfinfo, &context);
  if (!sndfile) {
    std::cerr << "Couldn't open file from memory: " << sf_strerror(sndfile) << std::endl;
    return 1;
  }

  int numChannelsInFile = sfinfo.channels;
  if (numChannelsInFile < channel + 1) {
    std::cerr << "Error: " << file << " doesn't contain requested channel" << std::endl;
    sf_close(sndfile);
    return 1;
  }

  if (endFrame <= startFrame || endFrame > sfinfo.frames) {
    std::cerr << "Error: " << file << " invalid frame range requested" << std::endl;
    sf_close(sndfile);
    return 1;
  }

  unsigned int frameLen = endFrame - startFrame;

  sf_seek(sndfile, startFrame, SEEK_SET);

  std::vector<float> tempBuf(frameLen * numChannelsInFile);
  int readcount = sf_read_float(sndfile, tempBuf.data(), frameLen * numChannelsInFile);

  // Pad with zeros in case we couldn't read the whole file
  for (int k = readcount; k < frameLen * numChannelsInFile; k++)
    tempBuf[k] = 0;

  // Extract the requested channel
  for (unsigned int n = 0; n < frameLen; n++)
    buf[n] = tempBuf[n * numChannelsInFile + channel];

  sf_close(sndfile);
  return 0;
}

int getNumChannels(const std::string& file) {
  std::vector<char> fileData = readFile(file);  // Load the file into memory
  VirtualIOContext context = {0, &fileData};

  SF_VIRTUAL_IO virtualIO;
  virtualIO.get_filelen = vf_get_filelen;
  virtualIO.seek = vf_seek;
  virtualIO.read = vf_read;
  virtualIO.write = nullptr;  // No writing needed
  virtualIO.tell = vf_tell;

  SF_INFO sfinfo;
  sfinfo.format = 0;
  SNDFILE* sndfile = sf_open_virtual(&virtualIO, SFM_READ, &sfinfo, &context);
  if (!sndfile) {
    std::cerr << "Couldn't open file from memory: " << sf_strerror(sndfile) << std::endl;
    return -1;
  }

  int channels = sfinfo.channels;
  sf_close(sndfile);
  return channels;
}

int getNumFrames(const std::string& file) {
  std::vector<char> fileData = readFile(file);  // Load the file into memory
  VirtualIOContext context = {0, &fileData};

  SF_VIRTUAL_IO virtualIO;
  virtualIO.get_filelen = vf_get_filelen;
  virtualIO.seek = vf_seek;
  virtualIO.read = vf_read;
  virtualIO.write = nullptr;
  virtualIO.tell = vf_tell;

  SF_INFO sfinfo;
  sfinfo.format = 0;
  SNDFILE* sndfile = sf_open_virtual(&virtualIO, SFM_READ, &sfinfo, &context);
  if (!sndfile) {
    std::cerr << "Couldn't open file from memory: " << sf_strerror(sndfile) << std::endl;
    return -1;
  }

  int frames = sfinfo.frames;
  sf_close(sndfile);
  return frames;
}



int write(const std::string& file, float *buf, unsigned int channels, unsigned int frames, unsigned int samplerate)
{
	SNDFILE *sndfile;
	SF_INFO sfinfo;
	sfinfo.samplerate = samplerate;
	sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
	sfinfo.channels = channels;
	sndfile = sf_open(file.c_str(), SFM_WRITE, &sfinfo);
	if(!sndfile)
	{
		sf_close(sndfile);
		std::cerr << "writeSamples() Error on sf_open(): " << file << " " << sf_strerror(sndfile) << "\n";
		return 0;
	}
	int ret = sf_writef_float(sndfile, buf, frames);
	if(ret != frames)
	{
		sf_close(sndfile);
		std::cerr << "writeSamples() Error on sf_writef_float(): " << file << " " << sf_strerror(sndfile) << ". Written " << ret << " frames out of " << frames << "\n";
		return 0;
	}
	ret = sf_close(sndfile);
	if(ret)
	{
		std::cerr << "writeSamples() Error on close(): " << file << " " << sf_strerror(sndfile) << "\n";
		return 0;
	}
	sync();
	return frames;
}

int write(const std::string& file, const std::vector<std::vector<float> >& dataIn, unsigned int sampleRate)
{
	unsigned int channels = dataIn.size();
	if(channels < 1)
		return -1;
	// output file will have as many frames as the longest of the channels
	// shorter channels are padded with zeros
	unsigned int frames = 0;
	for(auto& c : dataIn) {
		if(c.size() > frames)
			frames = c.size();
	}

	std::vector<float> dataOut(frames * channels);

	// interleave data into dataOut
	for(unsigned int c = 0; c < channels; ++c)
	{
		size_t siz = dataIn[c].size();
		for(unsigned int n = 0; n < siz; ++n)
			dataOut[n * channels + c] = dataIn[c][n];
	}
	return AudioFileUtilities::write(file, dataOut.data(), channels, frames, sampleRate);
}

std::vector<std::vector<float> > load(const std::string& file, int maxCount, unsigned int start)
{
  std::vector<std::vector<float> > out;
  int numChannels = getNumChannels(file);
  if(numChannels <= 0)
    return out;
  out.resize(numChannels);
  unsigned int numFrames = getNumFrames(file);
  if(start > numFrames)
    return out;
  numFrames -= start;
  if(maxCount >= 0)
    numFrames = maxCount < numFrames ? maxCount : numFrames;
  for(unsigned int n = 0; n < out.size(); ++n)
  {
    auto& ch = out[n];
    ch.resize(numFrames);
    getSamples(file, ch.data(), n, start, start + numFrames);
  }
  return out;
}



std::vector<float> loadMono(const std::string& file)
{
	auto v = load(file);
	v.resize(1);
	return v[0];
}
}; // namespace AudioFileUtilities
