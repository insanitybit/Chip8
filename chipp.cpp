#include <iostream>

#include "CPU.h"
using namespace std;

int main()
{

	CPU cpu;
	cpu.load("PONG");

	int i = 0;
	while(cpu.cycle()){
		i++;
	}

	cout << "\ntotal cycle count:\t" << i << endl;

	return 0;
}