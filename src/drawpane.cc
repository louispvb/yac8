/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#include "drawpane.hh"
#include "interpreter.hh"
#include <iostream>

RenderTimer::RenderTimer(DrawPane *pane) : wxTimer() {
  RenderTimer::pane = pane;
}
void RenderTimer::Notify() {
  if (!Interpreter::isPaused()) Interpreter::cycle();
  if (Interpreter::draw) {
    pane->Refresh();
    Interpreter::draw = false;
  }
}
void RenderTimer::start() {
  wxTimer::Start(5);
}

//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DrawPane, wxPanel)
  EVT_PAINT(DrawPane::paintEvent)
  EVT_KEY_DOWN(DrawPane::OnKeyDown)
  EVT_KEY_UP(DrawPane::OnKeyUp)
END_EVENT_TABLE()

DrawPane::DrawPane(wxFrame* parent) : wxPanel(parent) {
  //Connect(wxEVT_PAINT, wxPaintEventHandler(DrawPane::paintEvent));
}

void DrawPane::paintEvent(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  render(dc);
}

void DrawPane::paintNow() {
  wxClientDC dc(this);
  render(dc);
}

void DrawPane::render(wxDC& dc) {
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(*wxBLACK_BRUSH);
  
  wxSize panelSize = GetSize();
  uint16_t width = panelSize.GetWidth();
  uint16_t height = panelSize.GetHeight();

  //Black BG
  dc.DrawRectangle(0,0,width,height);
  
  width  /= 64;
  height /= 32;

  uint8_t * screen = Interpreter::screenBuff;
  for (uint16_t y = 0; y < 32; ++y) {
    for (uint16_t x = 0; x < 64; ++x) {
      dc.SetPen(*wxWHITE_PEN);
      dc.SetBrush(*wxWHITE_BRUSH);
      if(screen[x+64*y]) dc.DrawRectangle(width*x,height*y,width,height);
    }
  }
}

inline void setKey(int index, uint8_t state) {
  std::cout<<"called";
  Interpreter::key[index] = state;
  Interpreter::pressedKey = index;
  if (Interpreter::isWaiting()) Interpreter::resume();
}

void setKeys(int keycode, uint8_t x) {
  switch (keycode) {
  case '1': setKey(0x1, x); break;
  case '2': setKey(0x2, x); break;
  case '3': setKey(0x3, x); break;
  case '4': setKey(0xc, x); break;
  case 'Q': setKey(0x4, x); break;
  case 'W': setKey(0x5, x); break;
  case 'E': setKey(0x6, x); break;
  case 'R': setKey(0xd, x); break;
  case 'A': setKey(0x7, x); break;
  case 'S': setKey(0x8, x); break;
  case 'D': setKey(0x9, x); break;
  case 'F': setKey(0xe, x); break;
  case 'Z': setKey(0xa, x); break;
  case 'X': setKey(0x0, x); break;
  case 'C': setKey(0xb, x); break;
  case 'V': setKey(0xf, x); break;
  }
}

void DrawPane::OnKeyDown(wxKeyEvent &evt) {
  if (Interpreter::isPaused() && !Interpreter::isWaiting()) {
    evt.Skip();
    return;
  }
  setKeys(evt.GetKeyCode(), 1);
}

void DrawPane::OnKeyUp(wxKeyEvent &evt) {
  if (Interpreter::isPaused()) {
    evt.Skip();
    return;
  }
  setKeys(evt.GetKeyCode(), 0);
}
