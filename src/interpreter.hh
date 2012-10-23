/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#pragma once

#include <cstdint>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

size_t   const MEMORY_SIZE       = 0xFFF;
uint16_t const ROM_START_OFFSET  = 0x200;
size_t   const ROM_MAX_SIZE      = MEMORY_SIZE - ROM_START_OFFSET;
uint8_t  const SCREEN_MAX_WIDTH  = 64;
uint8_t  const SCREEN_MAX_HEIGHT = 32;

namespace Interpreter {
  extern uint8_t memory[0x1000];
  extern uint8_t * V; //registers 0x0 to 0xF
  extern std::vector<uint16_t> stack; //stack vector
  
  extern uint16_t I;
  extern uint16_t PC; //program counter
  extern uint16_t OP; //current opcode
  
  extern uint8_t DT; //delay timer
  extern uint8_t ST; //sound timer
  extern uint8_t screenBuff[SCREEN_MAX_WIDTH*SCREEN_MAX_HEIGHT];
  extern bool draw;
  extern bool key[0x10]; //4x4 keypad
  extern bool wait;
  extern int pressedKey;

  void init();
  void pause();
  bool isPaused();
  bool isWaiting();
  void resume();
  void reset();
  void cycle();
  void updateTimers();
  void loadROM(std::string fileName);
}
