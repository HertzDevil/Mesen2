#include "stdafx.h"
#include "PCE/PceCdAudioPlayer.h"
#include "PCE/PceCdRom.h"
#include "PCE/PceTypes.h"
#include "Shared/Emulator.h"
#include "Shared/EmuSettings.h"
#include "Shared/CdReader.h"

PceCdAudioPlayer::PceCdAudioPlayer(Emulator* emu, PceCdRom* cdrom, DiscInfo& disc)
{
	_emu = emu;
	_cdrom = cdrom;
	_disc = &disc;
}

void PceCdAudioPlayer::Play(uint32_t startSector)
{
	int32_t track = _disc->GetTrack(startSector);
	if(track >= 0) {
		_state.StartSector = startSector;
		_state.Playing = true;

		_state.EndSector = _disc->GetTrackLastSector(track);
		_state.EndBehavior = CdPlayEndBehavior::Stop;

		_state.CurrentSample = 0;
		_state.CurrentSector = startSector;

		_clockCounter = 0;
	}
}

void PceCdAudioPlayer::SetEndPosition(uint32_t endSector, CdPlayEndBehavior endBehavior)
{
	_state.EndSector = endSector;
	_state.EndBehavior = endBehavior;
	_state.Playing = true;
}

void PceCdAudioPlayer::Stop()
{
	_state.Playing = false;
}

void PceCdAudioPlayer::PlaySample()
{
	if(_state.Playing) {
		_state.LeftSample = _disc->ReadLeftSample(_state.CurrentSector, _state.CurrentSample);
		_state.RightSample = _disc->ReadRightSample(_state.CurrentSector, _state.CurrentSample);
		_samplesToPlay.push_back(_state.LeftSample);
		_samplesToPlay.push_back(_state.RightSample);
		_state.CurrentSample++;
		if(_state.CurrentSample == 588) {
			//588 samples per 2352-byte sector
			_state.CurrentSample = 0;
			_state.CurrentSector++;

			if(_state.CurrentSector > _state.EndSector) {
				switch(_state.EndBehavior) {
					case CdPlayEndBehavior::Stop: _state.Playing = false; break;
					case CdPlayEndBehavior::Loop: _state.CurrentSector = _state.StartSector; break;

					case CdPlayEndBehavior::Irq:
						_state.Playing = false;
						_cdrom->ClearIrqSource(PceCdRomIrqSource::DataTransferReady);
						_cdrom->SetIrqSource(PceCdRomIrqSource::DataTransferDone);
						break;

				}

			}
		}
	} else {
		_state.LeftSample = 0;
		_state.RightSample = 0;
	}
}

void PceCdAudioPlayer::Exec()
{
	_clockCounter += 3;
	if(_clockCounter > 487) {
		//Output one sample every 487 master clocks (~44101.1hz)
		PlaySample();
		_clockCounter -= 487;
	}
}

void PceCdAudioPlayer::MixAudio(int16_t* out, uint32_t sampleCount, uint32_t sampleRate)
{
	_resampler.SetVolume(_emu->GetSettings()->GetPcEngineConfig().CdAudioVolume / 100.0);
	_resampler.SetSampleRates(44100, sampleRate);
	_resampler.Resample<true>(_samplesToPlay.data(), (uint32_t)_samplesToPlay.size() / 2, out, sampleCount);
	_samplesToPlay.clear();
}