#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <array>
#include <stack>
#include <fstream>
#include <functional>
#include <random>


typedef uint8_t BYTE;
typedef uint16_t WORD;

class CPU
{
public:
		bool load(const std::string&);
		void cycle();
		size_t cycle_count();
		std::array<std::array<BYTE, 64>, 32>& get_gfx();

private:
		void fetch();
		void decode();
		void execute();

		size_t cycles;

		std::array<BYTE, 4096> memory = {};
		std::array<BYTE, 16> V = {};

		WORD I;
		WORD pc;
		WORD psize; // size of binary
		WORD opcode;

		std::stack<WORD> st;

		WORD sound_timer;
		WORD delay_timer;

		std::array<BYTE, 16> keypad;
		// std::array<BYTE, 64 * 32> gfx;
		std::array<std::array<BYTE, 64>, 32> gfx;
		BYTE MAX_X = 64;
		BYTE MAX_Y = 32;

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

#endif
