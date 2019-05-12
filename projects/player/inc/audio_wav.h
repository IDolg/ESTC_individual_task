
#ifndef __AUDIO_WAV_H__
#define __AUDIO_WAV_H__

#include <stdint.h>
#include <stdlib.h>


#define WAV_CHUNK_ID_RIFF    0x52494646
#define WAV_FORMAT_ID_WAVE   0x57415645
#define WAV_SUBCHUNK_ID_FMT  0x666d7420
#define WAV_SUBCHUNK_ID_DATA 0x64617461


#define AUDIO_FMT_PCM 1


typedef struct WavHeader_s
{
  uint32_t chunkId; /* should contain "RIFF" (0x52494646) in big-endian */
  uint32_t chunkSize; /* rest of file: length(WavFile) - 8 */
  uint32_t format; /* "wave" (0x57415645) in big-endian */
  uint32_t subchunk1Id; /* "fmt" (0x666d7420) in big-endian" */
  uint32_t subchunk1Size; /* 16 - rest of header */
  uint16_t audioFormat; /* for PCM is 1, other is compressed sound */
  uint16_t numChannels; /* number of channels */
  uint32_t sampleRate; /* 8KHz, 16KHz , 44.1KHz e.t.c. */
  uint32_t byteRate; /* SampleRate * NumOfChannels * BitsPerSample / 8 */
  uint16_t blockAlign; /* bits per sample */
  uint16_t bitsPerSample;
  uint32_t subchunk2Id; /* should be "data" (0x64617461) in big-endian */
  uint32_t subchunk2Size; /* size of raw data - sound */
} WavHdr_t;


int wav_parse_headers(const uint32_t* fptr, WavHdr_t* phdr, size_t* p_data_offset);
uint32_t _be32_to_le32(uint32_t be_data);



#endif /* __AUDIO_WAV_H__ */
