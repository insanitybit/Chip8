#include "CPU.h"
#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <err.h>

using namespace std;


#define MAX_X 64
#define MAX_Y 32

void gfx_update(std::array<std::array<BYTE, 64>, 32>&);

int main(int argc, const char *argv[]){
    if(argc < 2)
      err(0, "Need path to game file");

    // initscr();
    // start_color();
    // curs_set(0);
    // init_pair(1, COLOR_WHITE, COLOR_WHITE);
    // attron(COLOR_PAIR(1));

    string s(argv[1]);

    CPU cpu;
    if(!(cpu.load(s)))
      err(0, "load failed");

    auto dur = std::chrono::microseconds(16670);

    while(true){
    // while(cpu.cycle_count() < 30){
        cpu.cycle();
        // gfx_update(cpu.get_gfx());
        // refresh();
        std::this_thread::sleep_for(dur);
    }
    // endwin();
}

void gfx_update(std::array<std::array<BYTE, 64>, 32>& gfx){
      clear();
      for(int x = 0; x <  64; x++){
          for(int y = 0; y < 32; y++){
              if(gfx[y][x]){
                  mvaddch(y, x, ' ');
              }
          }
      }
}
