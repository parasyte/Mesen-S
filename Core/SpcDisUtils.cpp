#include "stdafx.h"
#include "SpcDisUtils.h"
#include "DisassemblyInfo.h"
#include "../Utilities/FastString.h"
#include "../Utilities/HexUtilities.h"

constexpr const char* _opTemplate[256] = {
	"NOP",	"TCALL 0",	"SET1 d.0",	"BBS d.0, q",	"OR A, d",		"OR A, !a",		"OR A, (X)",	"OR A, [d+X]",		"OR A, #i",		"OR t, s",		"OR1 C, m.b",	"ASL d",			"ASL !a",	"PUSH PSW",	"TSET1 !a",		"BRK",
	"BPL r",	"TCALL 1",	"CLR1 d.0",	"BBC d.0, q",	"OR A, d+X",	"OR A, !a+X",	"OR A, !a+Y",	"OR A, [d]+Y",		"OR e, #i",		"OR (X), (Y)",	"DECW d",		"ASL d+X",		"ASL A",		"DEC X",		"CMP X, !a",	"JMP [!a+X]",
	"CLRP",	"TCALL 2",	"SET1 d.1",	"BBS d.1, q",	"AND A, d",		"AND A, !a",	"AND A, (X)",	"AND A, [d+X]",	"AND A, #i",	"AND t, s",		"OR1 C, /m.b",	"ROL d",			"ROL !a",	"PUSH A",	"CBNE d, q",	"BRA r",
	"BMI r",	"TCALL 3",	"CLR1 d.1",	"BBC d.1, q",	"AND A, d+X",	"AND A, !a+X",	"AND A, !a+Y",	"AND A, [d]+Y",	"AND e, #i",	"AND (X), (Y)","INCW d",		"ROL d+X",		"ROL A",		"INC X",		"CMP X, d",		"CALL !a",
	"SETP",	"TCALL 4",	"SET1 d.2",	"BBS d.2, q",	"EOR A, d",		"EOR A, !a",	"EOR A, (X)",	"EOR A, [d+X]",	"EOR A, #i",	"EOR t, s",		"AND1 C, m.b",	"LSR d",			"LSR !a",	"PUSH X",	"TCLR1 !a",		"PCALL u",
	"BVC r",	"TCALL 5",	"CLR1 d.2",	"BBC d.2, q",	"EOR A, d+X",	"EOR A, !a+X",	"EOR A, !a+Y",	"EOR A, [d]+Y",	"EOR e, #i",	"EOR (X), (Y)","CMPW YA, d",	"LSR d+X",		"LSR A",		"MOV X, A",	"CMP Y, !a",	"JMP !a",
	"CLRC",	"TCALL 6",	"SET1 d.3",	"BBS d.3, q",	"CMP A, d",		"CMP A, !a",	"CMP A, (X)",	"CMP A, [d+X]",	"CMP A, #i",	"CMP t, s",		"AND1 C, /m.b","ROR d",			"ROR !a",	"PUSH Y",	"DBNZ d, q",	"RET",
	"BVS r",	"TCALL 7",	"CLR1 d.3",	"BBC d.3, q",	"CMP A, d+X",	"CMP A, !a+X",	"CMP A, !a+Y",	"CMP A, [d]+Y",	"CMP e, #i",	"CMP (X), (Y)","ADDW YA, d",	"ROR d+X",		"ROR A",		"MOV A, X",	"CMP Y, d",		"RET1",
	"SETC",	"TCALL 8",	"SET1 d.4",	"BBS d.4, q",	"ADC A, d",		"ADC A, !a",	"ADC A, (X)",	"ADC A, [d+X]",	"ADC A, #i",	"ADC t, s",		"EOR1 C, m.b",	"DEC d",			"DEC !a",	"MOV Y, #i","POP PSW",		"MOV e, #i",
	"BCC r",	"TCALL 9",	"CLR1 d.4",	"BBC d.4, q",	"ADC A, d+X",	"ADC A, !a+X",	"ADC A, !a+Y",	"ADC A, [d]+Y",	"ADC e, #i",	"ADC (X), (Y)","SUBW YA, d",	"DEC d+X",		"DEC A",		"MOV X, SP","DIV YA, X",	"XCN A",
	"EI",		"TCALL 10",	"SET1 d.5",	"BBS d.5, q",	"SBC A, d",		"SBC A, !a",	"SBC A, (X)",	"SBC A, [d+X]",	"SBC A, #i",	"SBC t, s",		"MOV1 C, m.b",	"INC d",			"INC !a",	"CMP Y, #i","POP A",			"MOV (X)+, A",
	"BCS r",	"TCALL 11",	"CLR1 d.5",	"BBC d.5, q",	"SBC A, d+X",	"SBC A, !a+X",	"SBC A, !a+Y",	"SBC A, [d]+Y",	"SBC e, #i",	"SBC (X), (Y)","MOVW YA, d",	"INC d+X",		"INC A",		"MOV SP, X","DAS A",			"MOV A, (X)+",
	"DI",		"TCALL 12",	"SET1 d.6",	"BBS d.6, q",	"MOV d, A",		"MOV !a, A",	"MOV (X), A",	"MOV [d+X], A",	"CMP X, #i",	"MOV !a, X",	"MOV1 m.b, C",	"MOV d, Y",		"MOV !a, Y","MOV X, #i","POP X",			"MUL YA",
	"BNE r",	"TCALL 13",	"CLR1 d.6",	"BBC d.6, q",	"MOV d+X, A",	"MOV !a+X, A",	"MOV !a+Y, A",	"MOV [d]+Y, A",	"MOV e, X",		"MOV d+Y, X",	"MOVW d, YA",	"MOV d+X, Y",	"DEC Y",		"MOV A, Y",	"CBNE d+X, q",	"DAA A",
	"CLRV",	"TCALL 14",	"SET1 d.7",	"BBS d.7, q",	"MOV A, d",		"MOV A, !a",	"MOV A, (X)",	"MOV A, [d+X]",	"MOV A, #i",	"MOV X, !a",	"NOT1 m.b",		"MOV Y, d",		"MOV Y, !a","NOTC",		"POP Y",			"SLEEP",
	"BEQ r",	"TCALL 15",	"CLR1 d.7",	"BBC d.7, q",	"MOV A, d+X",	"MOV A, !a+X",	"MOV A, !a+Y",	"MOV A, [d]+Y",	"MOV X, d",		"MOV X, d+Y",	"MOV t, s",		"MOV Y, d+X",	"INC Y",		"MOV Y, A",	"DBNZ Y, q",	"STOP"
};

constexpr const uint8_t _opSize[256] = {
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 1, 3, 1,
	2, 1, 2, 3, 2, 3, 3, 2, 3, 1, 2, 2, 1, 1, 3, 3,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 1, 3, 2,
	2, 1, 2, 3, 2, 3, 3, 2, 3, 1, 2, 2, 1, 1, 2, 3,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 1, 3, 2,
	2, 1, 2, 3, 2, 3, 3, 2, 3, 1, 2, 2, 1, 1, 3, 3,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 1, 3, 1,
	2, 1, 2, 3, 2, 3, 3, 2, 3, 1, 2, 2, 1, 1, 2, 1,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 2, 1, 3,
	2, 1, 2, 3, 2, 3, 3, 2, 3, 1, 2, 2, 1, 1, 1, 1,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 2, 1, 1,
	2, 1, 2, 3, 2, 3, 3, 2, 3, 1, 2, 2, 1, 1, 1, 1,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 2, 1, 1,
	2, 1, 2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 1, 1, 3, 1,
	1, 1, 2, 3, 2, 3, 1, 2, 2, 3, 3, 2, 3, 1, 1, 1,
	2, 1, 2, 3, 2, 3, 3, 2, 2, 2, 3, 2, 1, 1, 2, 1,
};

void SpcDisUtils::GetDisassembly(DisassemblyInfo &info, string &out, uint32_t memoryAddr)
{
	FastString str;

	uint8_t* byteCode = info.GetByteCode();
	const char* op = _opTemplate[byteCode[0]];
	int i = 0;
	while(op[i]) {
		switch(op[i]) {
			case 'r': str.Write('$', HexUtilities::ToHex(memoryAddr + (int8_t)byteCode[1])); break;
			case 'q': str.Write('$', HexUtilities::ToHex(memoryAddr + (int8_t)byteCode[2])); break; //relative 2nd byte

			case 'a': str.Write('$', HexUtilities::ToHex(byteCode[1] | (byteCode[2] << 8)));	break;
			
			case 'd': str.Write('$', HexUtilities::ToHex(byteCode[1])); break;
			case 'e': str.Write('$', HexUtilities::ToHex(byteCode[2])); break; //direct 2nd byte

			case 's': str.Write('$', HexUtilities::ToHex(byteCode[1])); break;
			case 't': str.Write('$', HexUtilities::ToHex(byteCode[2])); break;

			case 'i': str.Write('$', HexUtilities::ToHex(byteCode[1])); break;

			case 'm': str.Write('$', HexUtilities::ToHex((byteCode[1] | (byteCode[2] << 8)) & 0x1FFF)); break;
			case 'b': str.Write('$', (char)('0' + (byteCode[2] >> 5))); break;

			default: str.Write(op[i]);
		}
		i++;
	}

	out += str.ToString();
}

uint8_t SpcDisUtils::GetOpSize(uint8_t opCode)
{
	return _opSize[opCode];
}
