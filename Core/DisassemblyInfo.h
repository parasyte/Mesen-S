#pragma once
#include "stdafx.h"

class Console;
class MemoryDumper;
class LabelManager;
class EmuSettings;

enum class SnesMemoryType;
enum class CpuType : uint8_t;

class DisassemblyInfo
{
private:
	uint8_t _byteCode[4];
	uint8_t _opSize;
	uint8_t _flags;
	uint8_t _valueSize;
	CpuType _cpuType;
	bool _initialized = false;

public:
	DisassemblyInfo();
	DisassemblyInfo(uint8_t *opPointer, uint8_t cpuFlags, CpuType type);

	void Initialize(uint8_t *opPointer, uint8_t cpuFlags, CpuType type);
	void Initialize(uint32_t cpuAddress, uint8_t cpuFlags, CpuType type, MemoryDumper* memoryDumper);
	bool IsInitialized();
	bool IsValid(uint8_t cpuFlags);
	void Reset();

	void GetDisassembly(string &out, uint32_t memoryAddr, LabelManager *labelManager, EmuSettings* settings);
	
	CpuType GetCpuType();
	uint8_t GetOpCode();
	uint8_t GetOpSize();
	uint8_t GetValueSize();
	uint8_t GetFlags();
	uint8_t* GetByteCode();

	void GetByteCode(uint8_t copyBuffer[4]);
	void GetByteCode(string &out);

	static uint8_t GetOpSize(uint8_t opCode, uint8_t flags, CpuType type);
	static uint8_t GetValueSize(uint8_t opCode, uint8_t flags, CpuType type);
	static bool IsJumpToSub(uint8_t opCode, CpuType type);
	static bool IsReturnInstruction(uint8_t opCode, CpuType type);

	bool IsUnconditionalJump();
	void UpdateCpuFlags(uint8_t& cpuFlags);

	int32_t GetEffectiveAddress(Console *console, void *cpuState, CpuType type);
	uint16_t GetMemoryValue(uint32_t effectiveAddress, MemoryDumper *memoryDumper, SnesMemoryType memType, uint8_t &valueSize);
};

