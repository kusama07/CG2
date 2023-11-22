#pragma once
#include <xaudio2.h>

#pragma comment(lib,"xaudio2.lib")

#include <fstream>
#include <wrl.h>
#include <cassert>

// 音声データ
struct SoundData
{
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

class Audio
{
public:
	// namespace省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize();
	void Updata();

private:
	ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	HRESULT result_;

private:
	// チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];
		int32_t size;
	};

	// RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;
		char type[4];
	};

	// FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};

private:
	SoundData SoundLoadWave(const char* filename);

	//　音声データの解放
	void SoundUnload(SoundData* soundData);

	// 音声再生
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);

};

