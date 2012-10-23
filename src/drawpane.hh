/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>

class DrawPane;

class RenderTimer : public wxTimer {
  DrawPane *pane;
public:
  RenderTimer(DrawPane* pane);
  void Notify();
  void start();
};

class DrawPane : public wxPanel {
public:
  DrawPane(wxFrame* parent);
  void paintEvent (wxPaintEvent& evt);
  void paintNow();
  void render(wxDC & dc);
  void OnKeyDown(wxKeyEvent &evt);
  void OnKeyUp(wxKeyEvent &evt);  
  void SDTimer(wxCommandEvent &evt);

  DECLARE_EVENT_TABLE()
};
