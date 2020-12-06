#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

// Loading from Memory
// 16 bits memory address

// Registers
// 5 bits for register address

//==== M Type ====//
//  Type    op      R0       R1         Memory Addr
// [ 00 ][ 0000 ][ 00000 ][ 00000 ][ 0000000000000000 ]
//    2      4       5        5             16

//==== C Type ====//
//  Type    op      R0       R1       R2          EX
// [ 00 ][ 0000 ][ 00000 ][ 00000 ][ 00000 ][ 00000000000 ]
//    2      4       5        5        5          11

//==== F Type ====//
//  Type    op                 EX                Error  Halt
// [ 00 ][ 0000 ][ 0000 0000 0000 0000 0000 000 ][ 00 ][ 0 ]
//    2      4                 23                   2    1

// 2^16 positions
#define MEM_SIZE 65536
// 65536

// 2^5 positions
#define REG_SIZE 32 

enum regName
{
	s0 = 0, s1, s2, s3, s4, s5, s6, s7,
	s8, s9, s10, s11, s12, s13, m14, s15,
	s16, s17, s18, s19, s20, s21, s22, s23,
	s24, s25, s26, s27, s28, s29, s30, s31
};

enum m_op
{
	NOP = 0, LOAD, STO, RCPY
};

enum c_op
{
	ADD = 0, SUB, MUL, DIV, MOD,
	ADDI, SUBI, MULI, DIVI, MODI,
	LT, GT, EQ, NEQ, LTE, GTE
};

enum j_op
{
	JUMP = 0, JLT, JGT, JEQ, JNE, JLE, JGE, JZR
};

enum z_op
{
	HALT = 0
};

typedef uint32_t i32;
typedef uint16_t i16;
typedef uint8_t i8;

static i32 mem[MEM_SIZE] = { 0 };
static i32 reg[REG_SIZE + 5] = { 0 };

#define MEM(addr) (mem[addr >> 2])
#define REG(addr) (reg[addr % 32])

#define PC (reg[REG_SIZE + 0])
#define SP (reg[REG_SIZE + 1])
#define LO (reg[REG_SIZE + 2])
#define HI (reg[REG_SIZE + 3])
#define CD (reg[REG_SIZE + 4])

static i32 instruction = 0;
static i32 halt = 0;
static i32 data1 = 0;
static i32 data2 = 0;

static i32 ty = 0;
static i32 op = 0;
static i32 r0 = 0;
static i32 r1 = 0;
static i32 r2 = 0;
static i32 im = 0;
static i32 m0 = 0;

i32 getType(i32 _instruction)
{
	return (_instruction >> 30);
}

i32 getMemoryAddr(i32 _instruction)
{
	return (_instruction & 65535);
}

i32 createM_op(i32 _op, i32 _r0, i32 _r1, i32 _mem);

void fetch()
{
	instruction = MEM(PC % 262144);

	printf("instruction: %lx\n", instruction);
}

void decode()
{
	ty = (instruction >> 30) & 3;
	op = (instruction >> 26) & 15;

	r0 = (instruction >> 21) & 31;
	r1 = (instruction >> 16) & 31;
	r2 = (instruction >> 11) & 31;

	im = (instruction >> 0) & 65535;
	m0 = (instruction >> 0) & 65535;

	printf("\tty: %i\n", ty);
	printf("\top: %i\n", op);
	printf("\tr0: %i\n", r0);
	printf("\tr1: %i\n", r1);
	printf("\tr2: %i\n", r2);
	printf("\tm0: %i\n", m0);
	printf("\tim: %i\n", im);
}

void exec()
{
	i32 _quotient;
	i32 _a;
	i32 _b;

	i32 _d1;
	i32 _d2;
	i32 _carry;
	i32 _add;

	switch (ty)
	{
		// M-type - Memory Instructions
		case 0:
			switch (op)
			{
				case NOP:
					printf("NOP\n");
					break;
				case LOAD:
					printf("LOAD\n");
					REG(r0) = MEM(m0);
					printf("m : %08x\n", MEM(m0));
					printf("r : %08x\n", REG(r0));
					break;
				case STO:
					printf("STORE\n");
					MEM(m0) = REG(r0);
					printf("m : %08x\n", MEM(m0));
					printf("r : %08x\n", REG(r0));
					break;
				case RCPY:
					printf("RCPY\n");
					REG(r0) = REG(r1);
					printf("r0 : %08x\n", REG(r0));
					printf("r1 : %08x\n", REG(r1));
					break;
			}
			break;

		// C-type - Arithmetic Instructions
		case 1:
			switch (op)
			{
				case ADD:
					printf("ADD\n");

					// Easy Way Out
					REG(r0) = REG(r1) + REG(r2);
					
					// More 'Accurate' Process
					// _d1 = REG(r1);
					// _d2 = REG(r2);
					// _carry = (_d1 & _d2) << 1;
					// _add = _d1 ^ _d2;

					// while (_carry > 0)
					// {
					// 	_d1 = _add;
					// 	_d2 = _carry;
					// 	_carry = (_d1 & _d2) << 1;
					// 	_add = (_d1 ^ _d2);
					// }

					// printf("%i + %i = %i\n", REG(r1), REG(r2), _add);
					// REG(r0) = _add;
					break;
				case SUB:
					printf("SUB\n");
					REG(r0) = REG(r1) - REG(r2);
					break;
				case MUL:
					printf("MUL\n");
					REG(r0) = REG(r1) * REG(r2);
					break;
				case DIV:
					// Simple Way Out
					// Has Errors without accounting for 0 in R2
					// REG(r0) = REG(r1) / REG(r2);

					// TODO: Interrupt Vector for Division By Zero

					printf("DIV\n");
					_quotient = 0;
					_a = REG(r1);
					_b = REG(r2);
					printf("%i / %i = ", _a, _b);

					while (_a >= _b && _b != 0)
					{
						_a -= _b;
						_quotient++;
					}
					REG(r0) = _quotient;
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case MOD:
					// Simple Way Out
					// Has errors without accounting for 0 in R2
					// REG(r0) = REG(r1) % REG(r2);

					// TODO: Interrupt Vector for Modulus By Zero

					printf("MOD\n");
					_a = REG(r1);
					_b = REG(r2);
					printf("%i %% %i = ", _a, _b);

					while (_a >= _b && _b != 0)
					{
						_a -= _b;
					}
					REG(r0) = _a;
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case ADDI:
					printf("ADDI\n");
					
					// Easy Way Out
					REG(r0) = REG(r1) + im;

					// More 'accurate' method
					// _d1 = REG(r1);
					// _d2 = im;
					// _carry = (_d1 & _d2) << 1;
					// _add = _d1 ^ _d2;

					// while (_carry > 0)
					// {
					// 	_d1 = _add;
					// 	_d2 = _carry;
					// 	_carry = (_d1 & _d2) << 1;
					// 	_add = (_d1 ^ _d2);
					// }

					// printf("%i + %i = %i\n", REG(r1), im, _add);
					// REG(r0) = _add;
					break;
				case SUBI:
					printf("SUBI\n");
					printf("%i - %i = ", REG(r1), im);
					REG(r0) = REG(r1) - im;
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case MULI:
					printf("MULI\n");
					printf("%i * %i = ", REG(r1), im);
					REG(r0) = REG(r1) * im;
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case DIVI:
					// Simple Way Out
					// REG(r0) = REG(r1) / im;

					// TODO: Interrupt Vector for Division By Zero

					printf("DIV\n");
					_quotient = 0;
					_a = REG(r1);
					_b = im;
					printf("%i / %i = ", _a, _b);

					while (_a >= _b && _b != 0)
					{
						_a -= _b;
						_quotient++;
					}
					REG(r0) = _quotient;
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case MODI:
					// Simple Way Out
					// REG(r0) = REG(r1) % im;

					// TODO: Interrupt Vector for Modulus By Zero

					printf("MODI\n");
					_a = REG(r1);
					_b = im;
					printf("%i %% %i = ", _a, _b);

					while (_a >= _b && _b != 0)
					{
						_a -= _b;
					}
					REG(r0) = _a;
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case LT:
					printf("LT\n");
					printf("%i < %i = ", REG(r1), REG(r2));
					REG(r0) = (REG(r1) < REG(r2));
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case GT:
					printf("GT\n");
					printf("%i > %i = ", REG(r1), REG(r2));
					REG(r0) = (REG(r1) > REG(r2));
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case EQ:
					printf("EQ\n");
					printf("%i == %i = ", REG(r1), REG(r2));
					REG(r0) = (REG(r1) == REG(r2));
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case NEQ:
					printf("NEQ\n");
					printf("%i != %i = ", REG(r1), REG(r2));
					REG(r0) = (REG(r1) != REG(r2));
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case LTE:
					printf("LTE\n");
					printf("%i <= %i = ", REG(r1), REG(r2));
					REG(r0) = (REG(r1) <= REG(r2));
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
				case GTE:
					printf("GTE\n");
					printf("%i >= %i = ", REG(r1), REG(r2));
					REG(r0) = (REG(r1) >= REG(r2));
					printf("%i, %u\n", REG(r0), REG(r0));
					break;
			}
			break;

		// M-type - Jump Instructions
		case 2:
			switch (op)
			{
				case JUMP:
					printf("JUMP\n");
					PC = m0;
					break;
				case JLT:
					printf("JLT\n");
					if (REG(r0) < REG(r1))
						PC = m0;
					else
						PC += 4;
					break;
				case JGT:
					printf("JGT\n");
					if (REG(r0) > REG(r1))
						PC = m0;
					else
						PC += 4;
					break;
				case JEQ:
					printf("JEQ\n");
					if (REG(r0) == REG(r1))
						PC = m0;
					else
						PC += 4;
					break;
				case JNE:
					printf("JNE\n");
					if (REG(r0) != REG(r1))
						PC = m0;
					else
						PC += 4;
					break;
				case JLE:
					printf("JLE\n");
					if (REG(r0) <= REG(r1))
						PC = m0;
					else
						PC += 4;
					break;
				case JGE:
					printf("JGE\n");
					if (REG(r0) >= REG(r1))
						PC = m0;
					else
						PC += 4;
					break;
				case JZR:
					printf("JZR\n");
					PC = 0;
					break;
			}
			break;

		// F-type - Miscellaneous Instructions
		case 3:
			switch (op)
			{
				case HALT:
					printf("HALT\n");
					halt = (instruction >> 0) & 1;
					break;
			}
			break;

		default:
			break;
	}

	if (ty != 2)
		PC += 4;

	if (HALT > 0)
		halt = 1;
	else
		halt = 0;
}

i32 createM_op(i32 _op, i32 _r0, i32 _r1, i32 _mem)
{
	i32 _instr = 0;
	_instr = (_instr << 2) | 0;
	_instr = (_instr << 4) | _op;
	_instr = (_instr << 5) | _r0;
	_instr = (_instr << 5) | _r1;
	_instr = (_instr << 16) | _mem;
	return _instr;
}

i32 createC_op(i32 _op, i32 _r0, i32 _r1, i32 _r2)
{
	i32 _instr = 0;
	_instr = (_instr << 2) | 1;
	_instr = (_instr << 4) | _op;
	_instr = (_instr << 5) | _r0;
	_instr = (_instr << 5) | _r1;
	_instr = (_instr << 5) | _r2;
	_instr = (_instr << 11) | 0;
	return _instr;
}

i32 createN_op(i32 _op, i32 _r0, i32 _r1, i32 _im)
{
	i32 _instr = 0;
	_instr = (_instr << 2) | 1;
	_instr = (_instr << 4) | _op;
	_instr = (_instr << 5) | _r0;
	_instr = (_instr << 5) | _r1;
	_instr = (_instr << 16) | _im;
	return _instr;
}

i32 createJ_op(i32 _op, i32 _r0, i32 _r1, i32 _addr)
{
	i32 _instr = 0;
	_instr = (_instr << 2) | 2;
	_instr = (_instr << 4) | _op;
	_instr = (_instr << 5) | _r0;
	_instr = (_instr << 5) | _r1;
	_instr = (_instr << 16) | _addr;
	return _instr;
}

i32 createZ_op(i32 _op, i32 _err, i32 _halt)
{
	i32 _instr = 0;
	_instr = (_instr << 2) | 3;
	_instr = (_instr << 4) | _op;
	_instr = (_instr << 23) | 0; // EX
	_instr = (_instr << 2) | _err;
	_instr = (_instr << 1) | _halt;
	return _instr;
}

void printRegisters()
{
	for (int i = 0; i < 32; i++)
	{
		// if (i < 8)
		// 	printf("s%i : ", i);
		// else if (i < 16)
		// 	printf("t%i : ", i - 8);
		// else if (i < 24)
		// 	printf("d%i : ", i - 16);
		// else if (i < 32)
		// 	printf("w%i : ", i - 24);

		printf("s%02i : 0x%02x : 0x%08x\n", i, i, REG(i));
	}

	printf("--------------------------------\n");
	printf("pc : 0x%08x\n", PC);
	printf("sp : 0x%08x\n", SP);
	printf("lo : 0x%08x\n", LO);
	printf("hi : 0x%08x\n", HI);
	printf("ht : 0x%08x\n", HALT);
}

void sampleProgram()
{
	MEM(0) = 5565;
	MEM(4) = 1234;
	MEM(8) = 10789;
	MEM(12) = 122;
	MEM(16) = createM_op(LOAD, 1, 0, 0);
	MEM(20) = createM_op(LOAD, 2, 0, 4);
	MEM(24) = createM_op(LOAD, 4, 0, 8);
	MEM(28) = createM_op(LOAD, 8, 0, 12);
	MEM(32) = createC_op(ADD, 3, 1, 2);
	MEM(36) = createC_op(ADD, 5, 3, 4);
	MEM(40) = createC_op(SUB, 6, 4, 1);
	MEM(44) = createC_op(MUL, 7, 1, 2);
	MEM(48) = createC_op(DIV, 9, 8, 1);
	MEM(52) = createM_op(STO, 9, 0, 4000);
	MEM(56) = createM_op(LOAD, 10, 0, 4000);
	MEM(60) = createC_op(MOD, 11, 1, 10);
	MEM(64) = createM_op(LOAD, 12, 0, 12);
	MEM(68) = createN_op(ADDI, 13, 13, 7);

	MEM(72) = createN_op(SUBI, 13, 13, 20);
	MEM(76) = createN_op(MULI, 13, 13, 255);
	MEM(80) = createJ_op(JUMP, 0, 0, 88);
	MEM(84) = createJ_op(JLT, 12, 11, 92);
	MEM(88) = createJ_op(JEQ, 9, 10, 84);

	MEM(92) = createC_op(LT, 31, 10, 11);
	MEM(96) = createC_op(GT, 30, 12, 11);
	MEM(100) = createJ_op(JEQ, 30, 31, 112);

	MEM(104) = createN_op(ADDI, 10, 10, 99999);
	MEM(108) = createJ_op(JUMP, 0, 0, 92);

	MEM(112) = createC_op(NEQ, 29, 30, 31);
	MEM(116) = createC_op(LTE, 28, 30, 31);
	MEM(120) = createJ_op(JNE, 28, 29, 384);

	MEM(388) = createJ_op(JGT, 22, 12, 412);
	MEM(392) = createN_op(SUBI, 12, 12, 1);
	MEM(396) = createN_op(ADDI, 22, 22, 6);
	MEM(400) = createM_op(STO, 12, 0, 12);
	MEM(404) = createJ_op(JZR, 0, 0, 0);
	MEM(408) = createJ_op(JZR, 0, 0, 0);
	MEM(412) = createZ_op(HALT, 0, 1);
}

int main(void)
{
	unsigned _cycle = 0;
	FILE* out = fopen("memdump.txt", "w");

	// PC = 262100;

	// MEM(0) = 5565;
	// MEM(4) = 1234;
	// MEM(8) = 10789;
	// MEM(12) = 122;
	// MEM(16) = createM_op(LOAD, 1, 0, 0);
	// MEM(20) = createM_op(LOAD, 2, 0, 4);
	// MEM(24) = createM_op(LOAD, 4, 0, 8);
	// MEM(28) = createM_op(LOAD, 8, 0, 12);
	// MEM(32) = createC_op(ADD, 3, 1, 2);
	// MEM(36) = createC_op(ADD, 5, 3, 4);
	// MEM(40) = createC_op(SUB, 6, 4, 1);
	// MEM(44) = createC_op(MUL, 7, 1, 2);
	// MEM(48) = createC_op(DIV, 9, 8, 1);
	// MEM(52) = createM_op(STO, 9, 0, 4000);
	// MEM(56) = createM_op(LOAD, 10, 0, 4000);
	// MEM(60) = createC_op(MOD, 11, 1, 10);
	// MEM(64) = createM_op(LOAD, 12, 0, 12);
	// MEM(68) = createN_op(ADDI, 13, 13, 7);

	// MEM(72) = createN_op(SUBI, 13, 13, 20);
	// MEM(76) = createN_op(MULI, 13, 13, 255);
	// MEM(80) = createJ_op(JUMP, 0, 0, 88);
	// MEM(84) = createJ_op(JLT, 12, 11, 92);
	// MEM(88) = createJ_op(JEQ, 9, 10, 84);

	// MEM(92) = createC_op(LT, 31, 10, 11);
	// MEM(96) = createC_op(GT, 30, 12, 11);
	// MEM(100) = createJ_op(JEQ, 30, 31, 112);

	// MEM(104) = createN_op(ADDI, 10, 10, 99999);
	// MEM(108) = createJ_op(JUMP, 0, 0, 92);

	// MEM(112) = createC_op(NEQ, 29, 30, 31);
	// MEM(116) = createC_op(LTE, 28, 30, 31);
	// MEM(120) = createJ_op(JNE, 28, 29, 384);

	// MEM(388) = createJ_op(JGT, 22, 12, 408);
	// MEM(392) = createN_op(SUBI, 12, 12, 1);
	// MEM(396) = createN_op(ADDI, 22, 22, 6);
	// MEM(400) = createM_op(STO, 12, 0, 12);
	// MEM(404) = createJ_op(JZR, 0, 0, 0);
	// MEM(408) = createJ_op(JZR, 0, 0, 0);
	// MEM(412) = createZ_op(HALT, 0, 1);

	sampleProgram();

	for (int i = 0; i < MEM_SIZE; i++)
	{
		fprintf(out, "%08x\n", MEM(i));
	}

	printRegisters();

	while (!halt)
	{
		for (int i = 0; i < 10; i++)
			printf("\n");

		printf("Cycle: %i\n", _cycle++);
		printf("PC: %i\n", PC);
		// printRegisters();
		fetch();
		decode();
		exec();
		// PC += 4;
		printRegisters();

		sleep(1);

		switch (1)
		{
			case 1:
				if (PC > 500)
					halt = 1;
				break;
			case 2:
				if (_cycle > 10000)
					halt = 1;
				break;
		}
	}

	return 0;
}