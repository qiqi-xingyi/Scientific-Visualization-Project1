#ifndef _DisplayWindow_h_     // prevent multiple includes
#define _DisplayWindow_h_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include "Application.h"

class CDisplayWindow : public Fl_Gl_Window {
   public:
      CDisplayWindow(int x,int y,int w,int h,const char *l=0);
      void draw();

   
};

#endif