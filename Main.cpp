#include <FL/Fl.H>
#include "Gui.h"

// main routine: build the GUI, show it, go in an endless event-driven loop

Gui *gui;

int main(int argc, char *argv[])
{
   gui=new Gui;
    
   gui->show();

   return Fl::run();
}
