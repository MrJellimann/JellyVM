# JellyVM
A custom VM I've built with its own instruction set. It's purpose is to illustrate how ISA's work, and to be the first step in a compiler process that will compile a simple language to the custom ISA and run on the VM.

---

## VM Information

The VM uses 16-bit memory addresses, and 5-bit register addresses. Meaning that the VM can support 32 registers and 65536 addresses in memory.

---

## VM Instruction Set

From the InstructionSet.txt file in this repo. I don't have a name for this ISA yet, its just a very simple, bare-bones ISA that runs on the vm.exe in /VM

//==== M Type ====//

  Type    op      R0       R1         Memory Addr

 [ 00 ][ 0000 ][ 00000 ][ 00000 ][ 0000000000000000 ]

    2      4       5        5             16

//==== C Type ====//

  Type    op      R0       R1       R2          EX

 [ 00 ][ 0000 ][ 00000 ][ 00000 ][ 00000 ][ 00000000000 ]

    2      4       5        5        5          11

//==== F Type ====//

  Type    op                 EX                Error  Halt

 [ 00 ][ 0000 ][ 0000 0000 0000 0000 0000 000 ][ 00 ][ 0 ]

    2      4                 23                   2    1

#### 00 Memory Instructions - 3

	00		LOAD $destination, $address
	01		STO $register, $address
	10		RCPY $r0, $r1
	11		<None>

#### 01 Arithmetic Instructions - 16

	0000	ADD $r0, $r1, $r2
	0001	SUB $r0, $r1, $r2
	0010	MUL $r0, $r1, $r2
	0011	DIV $r0, $r1, $r2
	0100	MOD $r0, $r1, $r2
	0101	ADDI $r0, $r1, number
	0110	SUBI $r0, $r1, number
	0111	MULI $r0, $r1, number
	1000	DIVI $r0, $r1, number
	1001	MODI $r0, $r1, number
	1010	LT $r0, $r1, $r2
	1011	GT $r0, $r1, $r2
	1100	EQ $r0, $r1, $r2
	1101	NEQ $r0, $r1, $r2
	1110	LTE $r0, $r1, $r2
	1111	GTE $r0, $r1, $r2

#### 10 Jump Instructions - 8

	000		JUMP $address
	001		JLT $r0, $r1, $address
	010		JGT $r0, $r1, $address
	011		JEQ $r0, $r1, $address
	100		JNE $r0, $r1, $address
	101		JLE $r0, $r1, $address
	110		JGE $r0, $r1, $address
	111		JZR

#### 11 Other Instructions

	00		NOP