#ifndef CPU_H
#define CPU_H

#include <array>
#include <stack>
#include <fstream>
#include <functional>
#include <random>
#include <assert.h>

typedef uint16_t WORD;
typedef uint8_t  BYTE;

class CPU
{
public:
	CPU();
	void load(const std::string&); // Takes in the name of the game file to open and store into 'game'
	bool cycle(); // emulate one CPU cycle


private:
	// general purpose registers
	std::array<BYTE, 16> V = {}; // V0 - VF

	std::array<BYTE, 4096> memory;
	std::stack<WORD> stack;	// used to store return addresses

	WORD opcode	= 0;
	WORD ar 	= 0; // "I" or "index" register
	WORD pc 	= 0; // program counter

	BYTE delay_timer;
	BYTE sound_timer;

	std::array<BYTE, 2048> gfx 	= {}; // 64 by 32 display

	std::array<BYTE, 16> keypad = {};

	std::function<void()> fn;

	bool fetch();
	bool decode();
	bool execute();

	WORD psize; // size of binary + pc

    std::mt19937 gen;

	void OP_ANNN();
	void OP_BNNN();
	void OP_CXNN();
	void OP_DXYN();
	void OP_EX9E();
	void OP_EXA1();
};

CPU::CPU(){
	pc = 0x200; // "most programs [...] begin at memory location 0x200"
}

// Create string out of gamefile, store that string 512 bytes into memory

void CPU::load(const std::string& s){
	std::ifstream file(s);

	assert(file);

	file.seekg( 0, std::ios::end );
	psize = file.tellg();
	file.seekg(0, std::ios::beg);

	file.read(reinterpret_cast<char *>(&memory[pc]), psize);

	psize += pc;
	file.close();	
}

/*
	Emulate one CPU cycle
*/

bool CPU::cycle(){
	bool term = true;
	term = (term && fetch());
	term = (term && decode());
	term = (term && execute());

	return term;
}

/*
	Sets opcode to 16bits of memory, increments program counter 16bits
*/

bool CPU::fetch(){
	opcode = memory[pc++]; 	// Set first 8 least significant bits to 8 from address memory[pc] - 0000 0000 XXXX XXXX
	opcode <<= 8;			// shift least significant bits to most significant bits XXXX XXXX 0000 0000
	opcode |= memory[pc++]; // set first 8 least significant bits to 8 from address memory[pc + 1]
	
	if(pc >= psize)
		return false;

	return true;
}

bool CPU::decode(){
	WORD op = opcode;

	// std::cout << "op: " << opcode << ";  ";

	BYTE first 	= ((op >> 12) & 0x000F);
	BYTE second = ((op >> 8) & 0x000F);
	BYTE third 	= ((op >> 4) & 0x000F);
	BYTE fourth = (op & 0x000F);

	switch(first){

		case 0xA:
				OP_ANNN();
				break;

		case 0xB:
				OP_BNNN();
				break;

		case 0xC:
				OP_CXNN();
				break;

		case 0xD:
				OP_DXYN();
				break;

		case 0xE:
				switch(fourth){
					case 0xE:
							OP_EX9E();
							break;
					case 0x1:
							OP_EXA1();
							break;
				}
				
				break;
	}

	return true;
}

bool CPU::execute(){

	return true;
}



/*
 *	Opcodes below are from : https://en.m.wikipedia.org/wiki/CHIP-8#Opcode_table
*/

// set ar to NNN
void CPU::OP_ANNN(){
	WORD op = (opcode & 0x0FFF);
	ar = op;
}

// Jumps to the address NNN plus V0
void CPU::OP_BNNN(){
	pc = (opcode & 0x0FFF);
	pc += V[0];
}

// Sets VX to a random number and NN.
void CPU::OP_CXNN(){
    std::uniform_int_distribution<BYTE> dist(1, 0xFF);
    BYTE rd = dist(gen);

    BYTE reg = (opcode & 0x0F00);
    V[reg] = rd & (opcode & 0x00FF);
}

/*
sprites stored in memory at location in index register (I), maximum 8bits wide. 
Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise 
it is zero. All drawing is XOR drawing (i.e. it toggles the screen pixels)
*/
void CPU::OP_DXYN(){

}

// Skips the next instruction if the key stored in VX is pressed.
void CPU::OP_EX9E(){

}

// Skips the next instruction if the key stored in VX isn't pressed.
void CPU::OP_EXA1(){

}


#endif