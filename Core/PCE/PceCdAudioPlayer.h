#pragma once
#include "stdafx.h"
#include "PCE/PceTypes.h"
#include "Shared/Interfaces/IAudioProvider.h"
#include "Utilities/Audio/HermiteResampler.h"

class Emulator;
class PceCdRom;
struct DiscInfo;

class PceCdAudioPlayer : public IAudioProvider
{
	Emulator* _emu = nullptr;
	DiscInfo* _disc = nullptr;
	PceCdRom* _cdrom = nullptr;

	PceCdAudioPlayerState _state = {};

	vector<int16_t> _samplesToPlay;
	uint32_t _clockCounter = 0;
	
	HermiteResampler _resampler;
	
	void PlaySample();

public:
	PceCdAudioPlayer(Emulator* emu, PceCdRom* cdrom, DiscInfo& disc);

	void Play(uint32_t startSector);
	void SetEndPosition(uint32_t endSector, CdPlayEndBehavior endBehavior);
	void Stop();
	
	PceCdAudioPlayerState& GetState() { return _state; }

	bool IsPlaying() { return _state.Playing; }
	uint32_t GetCurrentSector() { return _state.CurrentSector; }

	void Exec();

	int16_t GetLeftSample() { return _state.LeftSample; }
	int16_t GetRightSample() { return _state.RightSample; }

	void MixAudio(int16_t* out, uint32_t sampleCount, uint32_t sampleRate) override;
};