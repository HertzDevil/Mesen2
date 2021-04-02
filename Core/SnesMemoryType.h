#pragma once

enum class SnesMemoryType
{
	CpuMemory,
	SpcMemory,
	Sa1Memory,
	NecDspMemory,
	GsuMemory,
	Cx4Memory,
	GameboyMemory,
	PrgRom,
	WorkRam,
	SaveRam,
	VideoRam,
	SpriteRam,
	CGRam,
	SpcRam,
	SpcRom,
	DspProgramRom,
	DspDataRom,
	DspDataRam,
	Sa1InternalRam,
	GsuWorkRam,
	Cx4DataRam,
	BsxPsRam,
	BsxMemoryPack,
	GbPrgRom,
	GbWorkRam,
	GbCartRam,
	GbHighRam,
	GbBootRom,
	GbVideoRam,
	GbSpriteRam,

	NametableRam,
	ChrRam,
	ChrRom,
	NesInternalRam,
	PaletteRam,

	Register
};