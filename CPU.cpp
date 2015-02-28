#ifndef OPCODES_H
#define OPCODES_H

#include "CPU.h"

std::array<BYTE, 2048>& CPU::get_gfx(){
  return this->gfx;
}

void CPU::cycle(){

    fetch();
    decode();
    // execute();
    cycles++;
    if(sound_timer > 0)
      sound_timer--;
    if(delay_timer > 0)
      delay_timer--;
}

void CPU::fetch(){
    opcode = memory[pc++];
    opcode <<= 8;
    opcode |= memory[pc++];
}

void CPU::decode(){
    WORD op = opcode;

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

    default:
      std::cout << "default case\n";
  }
}

// void CPU::execute(){

// }

size_t CPU::cycle_count(){
  return this->cycles;
}

void CPU::load(const std::string& s){
    pc = 0x200;
    std::ifstream file(s);

  file.seekg( 0, std::ios::end );
  psize = file.tellg();
  file.seekg(0, std::ios::beg);

  file.read(reinterpret_cast<char *>(&memory[pc]), psize);

  file.close();

}


/*
 *	Opcodes below are from : https://en.m.wikipedia.org/wiki/CHIP-8#Opcode_table
*/

// set address to NNN
void CPU::OP_ANNN(){
   WORD op = (opcode & 0x0FFF);
   I = op;
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
  BYTE N = opcode        & 0x000F;
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  for(BYTE i = 0; i < N; i++){
    BYTE temp = memory[I + i];
    for(BYTE j = 0; j < 7; j++){
      gfx[(MAX_X * ((Y + i) % MAX_Y)) + ((X + j) % MAX_X)] ^= ((temp >> (7 - j)) & 0b00000001);
    }
  }
}

// Skips the next instruction if the key stored in VX is pressed.
void CPU::OP_EX9E(){
  BYTE X = (opcode >> 8) & 0x000F;
  if(keypad[V[X]] == 1)
    pc += 2;
}

// Skips the next instruction if the key stored in VX isn't pressed.
void CPU::OP_EXA1(){
  BYTE X = (opcode >> 8) & 0x000F;
  if(keypad[V[X]] != 1)
    pc += 2;
}

// Calls RCA 1802 program at address NNN.
void CPU::OP_0NNN(){
  pc = (opcode & 0x0FFF);
}

// Clears the screen.
void CPU::OP_00E0(){
  gfx.fill(0);
}

// Returns from a subroutine.
void CPU::OP_00EE(){
  if(!st.empty()){
    pc = st.top();
    st.pop();
    pc += 2;
  }
}

// Jumps to address NNN.
void CPU::OP_1NNN(){
   pc = (opcode & 0x0FFF);
}

// 2NNN	Calls subroutine at NNN.
void CPU::OP_2NNN(){
  st.push(pc);
  pc = (opcode & 0x0FFF);
}

// Skips the next instruction if VX equals NN.
void CPU::OP_3XNN(){
   BYTE NN = (opcode & 0x00FF);
    BYTE X  = (opcode >> 8) & 0xF;
   if(NN == V[X])
     pc += 2;
}

// 4XNN	Skips the next instruction if VX doesn't equal NN.
void CPU::OP_4XNN(){
  BYTE NN = (opcode & 0x00FF);
    BYTE X  = (opcode >> 8) & 0xF;
   if(NN != V[X])
     pc += 2;
}

// 5XY0	Skips the next instruction if VX equals VY.
void CPU::OP_5XY0(){
  BYTE X = ((opcode >> 8) & 0x000F);
  BYTE Y = ((opcode >> 4) & 0x000F);

  if(V[X] == V[Y])
    pc += 2;
}

// 6XNN	Sets VX to NN.
void CPU::OP_6XNN(){
   BYTE NN = opcode & 0x00FF;
   BYTE X  = (opcode >> 8) & 0x000F;
   V[X] = NN;
}

// 7XNN	Adds NN to VX.
void CPU::OP_7XNN(){
   BYTE NN = opcode & 0x00FF;
   BYTE X  = (opcode >> 8) & 0x000F;
   V[X] += NN;
}

// 8XY0	Sets VX to the value of VY.
void CPU::OP_8XY0(){
   BYTE X = (opcode >> 8) & 0x000F;
   BYTE Y = (opcode >> 4) & 0x000F;
   V[X] = V[Y];
}


// 8XY1	Sets VX to VX or VY.
void CPU::OP_8XY1(){
   BYTE X = (opcode >> 8) & 0x000F;
   BYTE Y = (opcode >> 4) & 0x000F;
   V[X] = V[X] or V[Y];
}


// 8XY2	Sets VX to VX and VY.
void CPU::OP_8XY2(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;
  V[X] = V[X] and V[Y];
}


// 8XY3	Sets VX to VX xor VY.
void CPU::OP_8XY3(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;
  V[X] = V[X] xor V[Y];
}


// 8XY4	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
void CPU::OP_8XY4(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  if(V[X] < (V[X] + V[Y]))
    V[0xF] = 1;

  V[X] += V[Y];
}


// 8XY5	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
void CPU::OP_8XY5(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  if(V[Y] > V[X])
    V[0xF] = 0;

  V[X] -= V[Y];
}


// 8XY6	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
void CPU::OP_8XY6(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  V[0xF] = V[X] & 0x1;
  V[X] >>= 1;
}


// 8XY7	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
void CPU::OP_8XY7(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  if(V[X] > V[Y])
    V[0xF] = 0;

  V[X] = V[Y] - V[X];
}


// 8XYE	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
void CPU::OP_8XYE(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  BYTE most = (X & (1 << 7));

  V[X] <<= 1;
  V[0xF] = most;
}

// 9XY0	Skips the next instruction if VX doesn't equal VY.
void CPU::OP_9XY0(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE Y = (opcode >> 4) & 0x000F;

  if(V[X] != V[Y])
    pc += 2;
}

// FX07	Sets VX to the value of the delay timer.
void CPU::OP_FX07(){
  BYTE X = (opcode >> 8) & 0x000F;
  V[X] = delay_timer;
}

// FX0A	A key press is awaited, and then stored in VX.
void CPU::OP_FX0A(){
  BYTE X = (opcode >> 8) & 0x000F;
  BYTE pressed = 0;

  for(BYTE i = 0; i < 16; i++){
    if(keypad[i]){
      pressed = 1;
      V[X] = i;
      break;
    }
  }
  if(!pressed)
    pc--;
}

// FX15	Sets the delay timer to VX.
void CPU::OP_FX15(){
  BYTE X = (opcode >> 8) & 0x000F;
  delay_timer = V[X];
}


// FX18	Sets the sound timer to VX.
void CPU::OP_FX18(){
  BYTE X = (opcode >> 8) & 0x000F;
  sound_timer = V[X];
}


// FX1E	Adds VX to I.[3]
void CPU::OP_FX1E(){
  BYTE X = (opcode >> 8) & 0x000F;
  I += V[X];
}

// FX29	Sets I to the location of the sprite for the character in VX.
// Characters 0-F (in hexadecimal) are represented by a 4x5 font.
void CPU::OP_FX29(){
  BYTE X = (opcode >> 8) & 0x000F;
  I = V[X] * 5;
}

/*
Stores the Binary-coded decimal representation of VX, with the most significant of three digits at
the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2.
(In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I,
the tens digit at location I+1, and the ones digit at location I+2.)
*/
void CPU::OP_FX33(){
  BYTE bcd = V[(opcode >> 8) & 0x000F];
  BYTE hundreds = bcd / 100;
  bcd -= hundreds * 100;

  BYTE tens = bcd / 10;
  bcd -= tens * 10;

  BYTE ones = bcd;

  memory[I] = hundreds;
  memory[I+1] = tens;
  memory[I+2] = ones;
}

// FX55	Stores V0 to VX in memory starting at address I.[4]
void CPU::OP_FX55(){
  auto m = I;
  BYTE X = (opcode >> 8) & 0x000F;
  for(BYTE i = 0; i <= X; i++)
    memory[m++] = V[i];
}

// FX65	Fills V0 to VX with values from memory starting at address I.[4]
void CPU::OP_FX65(){
  auto m = I;
  BYTE X = (opcode >> 8) & 0x000F;
  for(BYTE i = 0; i <= X; i++)
    V[i] = 	memory[m++];
}


#endif
