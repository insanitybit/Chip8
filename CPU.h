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
	std::mt19937 gen{std::random_device{}()};


	void OP_0NNN();
	void OP_00E0();
	void OP_00EE();
	void OP_1NNN();
	void OP_2NNN();
	void OP_3XNN();
	void OP_4XNN();
	void OP_5XY0();
	void OP_6XNN();
	void OP_7XNN();
	void OP_8XY0();
	void OP_8XY1();
	void OP_8XY2();
	void OP_8XY3();
	void OP_8XY4();
	void OP_8XY5();
	void OP_8XY6();
	void OP_8XY7();
	void OP_8XYE();
	void OP_9XY0();
	void OP_ANNN();
	void OP_BNNN();
	void OP_CXNN();
	void OP_DXYN();
	void OP_EX9E();
	void OP_EXA1();
	void OP_FX07();
	void OP_FX0A();
	void OP_FX15();
	void OP_FX18();
	void OP_FX1E();
	void OP_FX29();
	void OP_FX33();
	void OP_FX55();
	void OP_FX65();



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
		case 0x0:
				switch(fourth)
				{
					case 0x0:
							OP_00EE();
							break;
					case 0xE:
							OP_00E0();
							break;
					default:
							
							OP_0NNN();
				}
				break;

		case 0x1:
				OP_1NNN();
				break;

		case 0x2:
				OP_2NNN();
				break;

		case 0x3:
				OP_3XNN();
				break;

		case 0x4:
				OP_4XNN();
				break;

		case 0x5:
				OP_5XY0();
				break;

		case 0x6:
				OP_6XNN();
				break;

		case 0x7:
				OP_7XNN();
				break;

		case 0x8:
				switch(fourth)
				{
					case 0x0:
							OP_8XY0();
							break;
					case 0x1:
							OP_8XY1();
							break;
					case 0x2:
							OP_8XY2();
							break;
					case 0x3:
							OP_8XY3();
							break;
					case 0x4:
							OP_8XY4();
							break;
					case 0x5:
							OP_8XY5();
							break;
					case 0x6:
							OP_8XY6();
							break;
					case 0x7:
							OP_8XY7();
							break;
					case 0xE:
							OP_8XYE();
							break;
				}
				break;

		case 0x9:
				OP_9XY0();
				break;

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
				switch(fourth)
				{
					case 0xE:
							OP_EX9E();
							break;

					case 0x1:
							OP_EXA1();
							break;
				}
				
				break;

		case 0xF:
				switch(third)
				{
					case 0x0:
						switch(fourth)
						{
							case 0x7:
									OP_FX07();
									break;

							case 0xA:
									OP_FX0A();
									break;
						}
						break;
					case 0x1:
						switch(fourth)
						{
							case 0x5:
									OP_FX15();
									break;

							case 0x8:
									OP_FX18();
									break;

							case 0xE:
									OP_FX1E();
									break;

						}
					case 0x2:
							OP_FX29();
							break;

					case 0x3:
							OP_FX33();
							break;

					case 0x5:
							OP_FX55();
							break;

					case 0x6:
							OP_FX65();
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

// Calls RCA 1802 program at address NNN.
void CPU::OP_0NNN(){

}

// Clears the screen.
void CPU::OP_00E0(){

}

// Returns from a subroutine.
void CPU::OP_00EE(){

}

// Jumps to address NNN.
void CPU::OP_1NNN(){
	// pc = (opcode & 0x0FFF);
}

void CPU::OP_2NNN(){

}

// Skips the next instruction if VX equals NN.
void CPU::OP_3XNN(){
	BYTE NN = (opcode & 0x00FF);
	if(NN == V[((opcode >> 8) & 0x000F)])
		pc += 2;
}

// 4XNN	Skips the next instruction if VX doesn't equal NN.

void CPU::OP_4XNN(){
	BYTE NN = (opcode & 0x00FF);
	if(NN != V[((opcode >> 8) & 0x000F)])
		pc += 2;
}

// 5XY0	Skips the next instruction if VX equals VY.
void CPU::OP_5XY0(){
	BYTE X = ((opcode >> 8) & 0x000F);
	BYTE Y = ((opcode >> 4) & 0x000F);

	if(X == Y)
		pc += 2;
}

// 6XNN	Sets VX to NN.
void CPU::OP_6XNN(){

}

// 7XNN	Adds NN to VX.
void CPU::OP_7XNN(){

}

void CPU::OP_8XY0(){

}

void CPU::OP_8XY1(){

}

void CPU::OP_8XY2(){

}

void CPU::OP_8XY3(){

}

void CPU::OP_8XY4(){

}

void CPU::OP_8XY5(){

}

void CPU::OP_8XY6(){

}

void CPU::OP_8XY7(){

}

void CPU::OP_8XYE(){

}

void CPU::OP_9XY0(){

}

void CPU::OP_FX07(){

}

void CPU::OP_FX0A(){

}

void CPU::OP_FX15(){

}

void CPU::OP_FX18(){

}

void CPU::OP_FX1E(){

}

void CPU::OP_FX29(){

}

void CPU::OP_FX33(){

}

void CPU::OP_FX55(){

}

void CPU::OP_FX65(){

}



#endif