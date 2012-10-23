/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#include "interpreter.hh"
#include "opcodefuncs.hh"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace Interpreter {
  /*** MEMORY MAP ******************
   * 0x000 Start of interpreter mem
   * 0x1FF End of interpreter mem
   * 0x200 Start of program mem
   * 0xFFF End of Chip-8 RAM
  **********************************/
  //! Machine memory, first 80 bytes store the bitfont.
  uint8_t memory[0x1000] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
  //! CPU Registers
  uint8_t * V = memory+0x100;
  std::vector<uint16_t> stack;

  //! Instruction pointer
  uint16_t I;
  //! Program counter
  uint16_t PC;
  //! Opcode to be executed
  uint16_t OP;
  //! Delay timer
  uint8_t DT = 0;
  //! Sound timer
  uint8_t ST = 0;

  uint8_t screenBuff[SCREEN_MAX_WIDTH*SCREEN_MAX_HEIGHT];
  //! Chip8 virtual key state
  bool key[0x10]; 

  bool draw = false;
  bool run = false;
  bool wait = false;
  int pressedKey;

  //! Load ROM into memory
  void loadROM(std::string fileName) {
    reset();
    std::ifstream file(fileName.c_str(), std::ios::binary);
    if (!file)
      throw std::runtime_error("ROM not found: " + fileName);
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);
    if (length > ROM_MAX_SIZE)
      throw std::runtime_error
        ("ROM larger than Chip8 memory: " + fileName);
    file.read(reinterpret_cast<char*>(&memory[ROM_START_OFFSET]), length);
    file.close();
    run = true;
  }
  void init() {
    reset();
    //for (int i = 0; i<32; ++i) screenBuff[i] = 1;
    initTables();
    srand(time(NULL));
  }
  void pause() {
    run = false;
  }
  void resume() {
    run = true;
    opResume();
  }
  bool isPaused() {
    return !run;
  }
  bool isWaiting() {
    return wait;
  }
  //! Reset machine state
  void reset() {
    I = 0;
    ST = 0;
    DT = 0;
    PC = ROM_START_OFFSET - 2;
    stack.erase(stack.begin(), stack.end());
    std::fill(key,key+0x10,false);
    std::fill(memory + ROM_START_OFFSET, memory + MEMORY_SIZE, 0);
    std::fill(screenBuff, screenBuff + 64*32, 0);
  }
  //! Decrement Sound and Delay timers
  void updateTimers() {
    if (DT > 0) --DT;
    if (ST > 0) --ST;
  }
  void printDebug() {
    printf("I: %X, PC: %X\n", I, PC);
    for (uint8_t i = 0; i < 4; ++i) {
      for (uint8_t j = 0; j < 4; ++j)
        printf("%X:%X ", i*4+j, V[i*4+j]);
      printf("\n");
    }
    std::cout << std::endl;
  }
  //! Execute one processor cycle
  void cycle() {
    executeOpcode();
    updateTimers();
    printDebug();
  }
}
