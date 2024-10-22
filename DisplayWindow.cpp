#include "DisplayWindow.h"
#include "Application.h"
#include "Gui.h"

extern Gui *gui;
extern Image curImage;   // defined in Application.cpp

// the constructor method

CDisplayWindow::CDisplayWindow(int x,int y,int w,int h,const char *l)
        : Fl_Gl_Window(x,y,w,h,l)
{
    // clear window

   glClearColor(0.0,0.0,0.0,0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
}


// the drawing method: the image gets drawn here

void CDisplayWindow::draw() {
	if (!valid()) {
      glLoadIdentity(); glViewport(0,0,w(),h()); gluOrtho2D(0,w(),0,h());
      make_current();
   }

// clear window first

   glClearColor(0.0,0.0,0.0,0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   

   if(curImage.nx==0)
	 return;

// display the image as a greylevel image in the center of the window
// h() returns the height of the window, w() returns the width

   glPixelStorei(GL_UNPACK_ALIGNMENT,1);  
   glRasterPos2i((w()-curImage.nx)/2,(h()-curImage.ny)/2); 
   if(curImage.ncolorChannels==1)  // grey level image
     glDrawPixels(curImage.nx,curImage.ny,GL_LUMINANCE,GL_UNSIGNED_BYTE,curImage.data);
   else if(curImage.ncolorChannels==3)  // color image
     glDrawPixels(curImage.nx,curImage.ny,GL_RGB,GL_UNSIGNED_BYTE,curImage.data);
}


