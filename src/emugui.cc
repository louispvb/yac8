/*                        YAC8 - Written 2012

  To the extent possible under law, the author(s) have dedicated all copyright 
  and related and neighboring rights to this software to the public domain 
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along 
  with this software. If not, see 
  <http://creativecommons.org/publicdomain/zero/1.0/legalcode>.               */

#include "emugui.hh"
#include "interpreter.hh"

BEGIN_EVENT_TABLE(EmuGUI, wxFrame)
  EVT_CLOSE(EmuGUI::onClose)
  EVT_MENU(wxID_EXIT, EmuGUI::OnQuit)
  EVT_MENU(MENU_LOAD, EmuGUI::OpenFile)
END_EVENT_TABLE()

EmuGUI::EmuGUI (const wxString & title)
  : wxFrame (NULL, wxID_ANY, title, wxDefaultPosition, wxSize(280,180))
{
  menubar = new wxMenuBar();
  filemenu = new wxMenu();
  drawPane = new DrawPane(this);
  
  timer = new RenderTimer(drawPane);

  SetMinSize(wxSize(64,32));
  wxBoxSizer * sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(drawPane,1,wxEXPAND);
  SetSizer(sizer);
  
  filemenu->Append(MENU_LOAD, wxT("&Load ROM"));
  filemenu->Append(wxID_ANY, wxT("&Save"));

  filemenu->AppendSeparator();
  filemenu->Append(wxID_EXIT, wxT("&Quit\tCtrl+W"));

  menubar->Append(filemenu, wxT("&File"));
  
  SetMenuBar(menubar);

  Centre();
  Show();
  timer->start();
}

EmuGUI::~EmuGUI() {
  delete timer;
}

void EmuGUI::onClose(wxCloseEvent &evt) {
  timer->Stop();
  evt.Skip();
}
void EmuGUI::Save(wxCommandEvent& WXUNUSED(event)) {}
void EmuGUI::OpenFile(wxCommandEvent& WXUNUSED(event)) {
  wxFileDialog * OpenDialog = new wxFileDialog
    ( this
    , wxT("Choose a ROM")
    , wxGetCwd()
    , wxEmptyString
    , wxT("ROM files (*)|*")
    , wxFD_OPEN
    , wxDefaultPosition);
  if (OpenDialog->ShowModal() == wxID_OK) {
    Interpreter::loadROM(std::string(OpenDialog->GetPath().mb_str()));
    drawPane->SetFocus(); 
    //MainEditBox->LoadFile(CurrentPath);
    //SetTitle(wxString("Edit - ") << OpenDialog->GetFilename());
  }
  OpenDialog->Destroy();
}
void EmuGUI::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  Close(true);
}


