/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#pragma once

#include <cstdint>

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/textctrl.h>

#include "drawpane.hh"

class EmuGUI : public wxFrame {
  RenderTimer *timer;
  DrawPane *drawPane;
public:
  EmuGUI(const wxString& title);
  ~EmuGUI(); 

  void OnQuit(wxCommandEvent &evt);
  void OpenFile(wxCommandEvent &evt);  
  void Save(wxCommandEvent &evt);
  void onClose(wxCloseEvent &evt);
  void OnTimer();

  wxMenuBar * menubar;
  wxMenu * filemenu;
  DECLARE_EVENT_TABLE()
};

enum {
  MENU_LOAD,
  MENU_LOAD_SAVE,
  MENU_SAVE,
};
