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
#include <iostream>
#include <cstdio>

namespace Interpreter {
  extern func opTable[0x10];
  extern func op8Table[0x10];
  func opFTable[0x100];
  
  inline uint8_t getX() { return (OP&0x0F00)>>8; }
  inline uint8_t getY() { return (OP&0x00F0)>>4; }
  inline uint8_t getN() { return OP&0x000F; }
  inline uint16_t getNN() { return OP&0x00FF; }
  inline uint16_t getNNN() { return OP& 0x0FFF; }
  inline uint8_t & getVX() { return V[getX()]; }
  inline uint8_t & getVY() { return V[getY()]; }
  inline void skip() { PC += 2; }

  //! Fetch and execute next opcode
  void executeOpcode() {
    OP = static_cast<uint16_t>(memory[PC] << 8 | memory[PC + 1]);
    skip();
    opTable[(OP & 0xF000)>>12] ();
  }
  //! Either clear screen or return 
  void op00MM () {
    if (getNN() == 0xE0) { //Clear screen
      std::fill(screenBuff, screenBuff + 64*32, 0);
      draw = true;
    } else if (getNN() == 0xEE) { //Return
      PC = stack.back();
      stack.pop_back();
    }
  }
  //! jmp to 0x0NNN
  void op1NNN () {
    PC = getNNN();
  }
  //! call 0x0NNN
  void op2NNN () {
    stack.push_back(PC);
    PC = getNNN();
  }
  //! Skips an instruction if VX == NN
  void op3XNN () { 
    if (getVX() == getNN()) skip();
  }
  //! Skips an instruction if VX != NN
  void op4XNN () { 
    if (getVX() != getNN()) skip();
  }
  //! Skips an instruction if VX == VY
  void op5XY0 () { 
    if (getVX() == getVY()) skip();
  }
  //! Sets VX to NN
  void op6XNN () { 
    getVX() = getNN();
  }
  //! Adds NN to VX
  void op7XNN () { 
    getVX() += getNN();
  }
  //! Lookup and execute 8XYM opcode
  void op8XYM () {
    op8Table[getN()] ();
  }
  //! Sets VX to VY
  void op8XY0 () { 
    getVX() = getVY();
  }
  //! Sets VX to VX or VY
  void op8XY1 () { 
    getVX() |= getVY();
  }
  //! Sets VX to VX and VY
  void op8XY2 () { 
    getVX() &= getVY();
  }
  //! Sets VX to VX xor VY
  void op8XY3 () { 
    getVX() ^= getVY();
  }
  //! VX = VX + VY, VF = carry if 1 else 0
  void op8XY4 () {
    int i = static_cast<int>(getVX()) + static_cast<int>(getVY());
    if (i > 255) V[0xF] = 1;
    else V[0xF] = 0;
    getVX() = i;
  }
  //! VX = VX - VY. VF set to 0 iff borrow
  void op8XY5 () { 
    uint8_t x = getX();
    uint8_t y = getY();
    if (V[x] >= V[y]) V[0xF] = 1;
    else V[0xF] = 0;
    V[x] -= V[y];
  }
  //! Shifts VX right by 1. VF set to LSBit of VX before shift
  void op8XY6 () { 
    uint8_t x = getX();
    V[0xF] = V[x] & 0x01;
    V[x] >>= 1;
  }
  //! VX = VY - VX. VF set to 0 iff borrow, 1 otherwise
  void op8XY7 () { 
    uint8_t x = getX();
    uint8_t y = getY();
    if (V[y] >= V[x]) V[0xF] = 1;
    else V[0xF] = 0;
    V[x] = V[y] - V[x];
  }
  //! Shifts VX left by 1. VF set to MSBit of VX before shift
  void op8XYE () { 
    uint8_t x = getX();
    V[0xF] = (V[x] >> 7) & 0x01;
    V[x] <<= 1;
  }
  //! Skips next instruction if VX != VY
  void op9XY0 () { 
    if (getVX() != getVY()) skip();
  }
  //! I = 0x0NNN
  void opANNN () { 
    I = getNNN();
  }
  //! jmp to 0x0NNN + V0
  void opBNNN () { 
    PC = getNNN() + V[0];
  }
  //! Sets VX to a random number and NN
  void opCXNN () { 
    getVX() = (rand()%0xFF) & getNN();
  }
  /* Draw a sprite at (VX, VY) with width of 8px and height of Npx. Each row
   * of 8px is read as bit-coded (with MSBit of each byte displayed on the
   * left) starting from memory location I; I value doesn't change after
   * execution of this instruction. As described above, VF is set to 1 iff
   * any screen pixels are flipped from set to unset when the sprite is drawn.
   */  
  void opDXYN () {
    uint8_t x = getVX();
    uint8_t y = getVY();
    uint8_t height = getN();
    uint8_t pixel;
    
    V[0xF] = 0;
    for (int ypos = 0; ypos < height; ++ypos) {
      pixel = memory[I+ypos];
      for (int xpos = 0; xpos < 8; ++xpos) {
        if ((pixel & (0x80 >> xpos)) != 0) {
          if (x+xpos<64 && x+xpos>=0 && y+ypos < 32 && y+ypos >= 0) {
          if (screenBuff[x+xpos+(y+ypos)*64] == 1)
            V[0xF] = 1;
          screenBuff[x+xpos+(y+ypos)*64] ^= 1;
          }
        }
      }
    }
    draw = true;
  }
  void opEXMM () {
    //9E Skips next instruction if the key stored in VX is pressed.
    //A1 Skips next instruction if the key stored in VX isn't pressed.
    if (getNN() == 0x009E) {
      if (key[getVX()]) skip();
    } else {
      if (!key[getVX()]) skip();
    }
  }
  //! Lookup and execute FXMM opcode
  void opFXMM () {
    opFTable[getNN()] ();
  } 

  //! VX = DT
  void opFX07 () { 
    getVX() = DT;
  }
  //! A key press is awaited and then stored in VX
  void opFX0A () { 
    pause();
    wait = true;
  }
  void opResume() { wait = false; getVX() = pressedKey; }
  //! DT = VX
  void opFX15 () { 
    DT = getVX();
  }
  //! ST = VX
  void opFX18 () { 
    ST = getVX();
  }
  //! Adds VX to I
  void opFX1E () {
    if ((I += getVX()) > 0xFFF) V[0xF] = 1;
    else V[0xF] = 0;
  }
  /* Sets I to the location of the sprite for the character in VX.
   * Characters 0-F (in hex) are represented by a 4x5 font.
   */
  void opFX29 () {
    I = getVX()*5;
  }
  /* Stores the Binary-coded decimal representation of VX, with the most
   * significant of 3 digits at the address in I, the middle digit at 
   * I+1, and the least significant digit at I+2
   */
  void opFX33 () {
    uint8_t vx = getVX();
    memory[I]   = (vx-(vx%100)) / 100;
    vx -= memory[I] * 100;
    memory[I+1] = (vx - (vx % 10)) / 10;
    vx -= memory[I+1] * 10;
    memory[I+2] = vx;
  }
  //! Stores V0 to VX in memory starting at address I
  void opFX55 () { 
    for (uint8_t i = 0; i <= getX(); ++i)
      memory[I+i] = V[i];
    //I += getX() + 1;
  }
  //! Fills V0 to VX with mem vals starting at address I
  void opFX65 () { 
    for (uint8_t i = 0; i <= getX(); ++i)
      V[i] = memory[I+i];
    //I += getX() + 1;
  }

  void opNULL () {throw std::runtime_error("Null opcode encountered");}

  void initTables() {
    std::fill(opFTable, opFTable + 0x100, opNULL);
    opFTable[0x07] = opFX07;
    opFTable[0x0A] = opFX0A;
    opFTable[0x15] = opFX15;
    opFTable[0x18] = opFX18;
    opFTable[0x1E] = opFX1E;
    opFTable[0x29] = opFX29;
    opFTable[0x33] = opFX33;
    opFTable[0x55] = opFX55;
    opFTable[0x65] = opFX65;
  }

  func opTable[0x10] = {
    op00MM, op1NNN, op2NNN, op3XNN, op4XNN, op5XY0, op6XNN, op7XNN,
    op8XYM, op9XY0, opANNN, opBNNN, opCXNN, opDXYN, opEXMM, opFXMM
  };

  func op8Table[0x10] = {
    op8XY0, op8XY1, op8XY2, op8XY3, op8XY4, op8XY5, op8XY6, op8XY7,
    opNULL, opNULL, opNULL, opNULL, opNULL, opNULL, op8XYE, opNULL, 
  };
}
