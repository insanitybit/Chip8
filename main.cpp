#include "CPU.h"
#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>

using namespace std;


#define MAX_X 64
#define MAX_Y 32

void gfx_update(std::array<BYTE, 2048>&);

int main(int argc, const char *argv[]){

    initscr();
    start_color();
    curs_set(0);
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    attron(COLOR_PAIR(1));

    CPU cpu;
    cpu.load("PONG");

    auto dur = std::chrono::microseconds(16670);

    while(true){
        cpu.cycle();
        gfx_update(cpu.get_gfx());
        refresh();
        std::this_thread::sleep_for(dur);
    }
    endwin();
}

void gfx_update(std::array<BYTE, 2048>& gfx){
      clear();
      for(int x = 0; x <  64; x++){
          for(int y = 0; y < 32; y++){
              if(gfx[(MAX_X * y) + x]){
                  mvaddch(y, x, ' ');
              }
          }
      }
}
