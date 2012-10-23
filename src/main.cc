/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#include "main.hh"
#include "interpreter.hh"

IMPLEMENT_APP(Chip8)

bool Chip8::OnInit () {
  gui = new EmuGUI(wxT("Chip8"));
  gui->Show(true);
  Interpreter::init();
  return true;
}

