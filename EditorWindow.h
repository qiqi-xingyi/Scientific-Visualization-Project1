#ifndef _EditorWindow_h_     // prevent multiple includes
#define _EditorWindow_h_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include "Application.h"

typedef int TransferFunction[256];

class CEditorWindow : public Fl_Gl_Window {
   public:
      CEditorWindow(int x,int y,int w,int h,const char *l=0);
      void draw();
      int handle(int eventType);

   
};

#endif