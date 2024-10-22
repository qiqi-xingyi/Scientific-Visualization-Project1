#define _CRT_SECURE_NO_DEPRECATE
#include "Application.h"
#include <FL/fl_file_chooser.H>
#include "Gui.h"
#include <algorithm> 
#define KERNEL_SIZE 3
#include <cmath>


extern Gui *gui;
Image curImage; // make inImage a global, need it in other classes
Image oldImage;
Volume vol;

// these are declared in transFunc.cpp
extern TransferFunction transFunc[4];
extern int maxTransFunc;              
extern int activeTransFunc;       
extern float transFuncColor[4][3]; 

// the constructor method for the Application class

Application::Application()
{
  // initialize the image data structure
  curImage.nx=curImage.ny=curImage.n=curImage.ncolorChannels=0;
  //curImage.data = nullptr;

  // add more initialization here:
  oldImage.nx = oldImage.ny = oldImage.n = oldImage.ncolorChannels = 0;
  //oldImage.data = nullptr;

}

// the method that gets executed when the readFile callback is called


int histogram[3][256] = { 0 };

void Application::ReadFile()
{
   FILE *fp;
   char imageType[3],str[100];
   int dummy;
   int i,j;

   char *file = fl_file_chooser("Pick a file to READ from", "*.{pgm,ppm,vol}", "");
   if(file == NULL)
		return;

   // Read PGM image file with filename "file"

   // The PGM file format for a GREYLEVEL image is:
   // P5 (2 ASCII characters) <CR>
   // two ASCII numbers for nx ny (number of rows and columns <CR>
   // 255 (ASCII number) <CR>
   // binary pixel data with one byte per pixel

   // The PGM file format for a COLOR image is:
   // P6 (2 ASCII characters) <CR>
   // two ASCII numbers for nx ny (number of rows and columns <CR>
   // 255 (ASCII number) <CR>
   // binary pixel data with three bytes per pixel (one byte for eacg RGB)

    fp=fopen(file,"rb");

    // free memory from old image, if any
    if(curImage.n>0)
    {
	  delete[] curImage.data;
	}

    // read the first ASCII line to find out if we read a color image or
	// a greylevel image

    fgets(str,100,fp);
	sscanf(str,"%s",imageType);

    if(!strncmp(imageType,"P5",2)) // greylevel image 
    {
       curImage.ncolorChannels=1;
       maxTransFunc=1;  // have read a greylevel image: need only one transfer function
       activeTransFunc=0;    // transFunc 0 is active (all the time)
       transFuncColor[0][0]=1.0; transFuncColor[0][1]=0.0; transFuncColor[0][2]=0.0;  // draw transfer function curve in red 
	} 
	else if(!strncmp(imageType,"P6",2)) // color image 
    {
	   curImage.ncolorChannels=3;
       maxTransFunc=1;  // have read a color image: if you want 3 transfer functions (HSV) change this value to 3
       activeTransFunc=0;    // transFunc 0 is active (for now)
       transFuncColor[0][0]=1.0; transFuncColor[0][1]=0.0; transFuncColor[0][2]=0.0;  // draw transfer function curve in red 
       transFuncColor[1][0]=0.0; transFuncColor[1][1]=1.0; transFuncColor[1][2]=0.0;  // draw transfer function curve in red 
       transFuncColor[2][0]=0.0; transFuncColor[2][1]=0.0; transFuncColor[2][2]=1.0;  // draw transfer function curve in red 
	}

	// skip comments embedded in header
    fgets(str,100,fp);  
	while(str[0]=='#')
		fgets(str,100,fp);

    // read image dimensions 
    sscanf(str,"%d %d",&curImage.nx,&curImage.ny);

	// read the next line into dummy variable
    fgets(str,100,fp);  
	 
   	// allocate the memory
	curImage.n=curImage.nx*curImage.ny;

    // read the image data 
	curImage.data = new unsigned char [curImage.n*curImage.ncolorChannels];
	fread(curImage.data,sizeof(unsigned char),curImage.n*curImage.ncolorChannels,fp);


    ComputeHistogram(&curImage);

  	// unfortunately OpenGL displays the image upside-down
 	// we have to flip it at read time.
    FlipImage(&curImage);

	fclose(fp);

    // call the window drawing routines to display the image curImage
	// and to draw image-related transFuncs
	gui->DisplayWindow->redraw();
	gui->EditorWindow->redraw();
}

void Application::ComputeHistogram(Image* img)
{
    // Create data of histogram
    memset(histogram, 0, sizeof(histogram));

    // caculate
    for (int i = 0; i < img->n; i++) {
        if (img->ncolorChannels == 3) {
            // color img
            histogram[0][img->data[i * 3]]++;     // red
            histogram[1][img->data[i * 3 + 1]]++; // green
            histogram[2][img->data[i * 3 + 2]]++; // blue
        }
        else if (img->ncolorChannels == 1) {
            // grey img
            histogram[0][img->data[i]]++;
        }
    }
}


void Application::WriteFile()
{
   FILE *fp;
   char imageType[3],str[100];
   int dummy,i;
   char *file = fl_file_chooser("Specify a filename to WRITE to", "*.{vol,ppm,pgm}", "");
   if(file == NULL)
		return;

   // Write PGM image file with filename "file"

   // The PGM file format for a GREYLEVEL image is:
   // P5 (2 ASCII characters) <CR>
   // two ASCII numbers for nx ny (number of rows and columns <CR>
   // 255 (ASCII number) <CR>
   // binary pixel data with one byte per pixel

   // The PGM file format for a COLOR image is:
   // P6 (2 ASCII characters) <CR>
   // two ASCII numbers for nx ny (number of rows and columns <CR>
   // 255 (ASCII number) <CR>
   // binary pixel data with three bytes per pixel (one byte for each R,G,B)

    fp=fopen(file,"wb");

    // write the first ASCII line with the file type
	if(curImage.ncolorChannels==1)
   	  fprintf(fp,"P5\n"); //greylevel image
    else if(curImage.ncolorChannels==3)
      fprintf(fp,"P6\n");  // color image

    // write image dimensions 
    fprintf(fp,"%d %d\n",curImage.nx,curImage.ny);

	// write '255' to the next line 
    fprintf(fp,"255\n");

	// since we flipped the image upside-down when we read it
 	// we have to write it upside-down so it's stored the right way
    for(i=curImage.ny-1;i>=0;i--)
  	  fwrite(&curImage.data[i*curImage.nx*curImage.ncolorChannels],sizeof(unsigned char),curImage.nx*curImage.ncolorChannels,fp);

	fclose(fp);
}

// flips an image upside down
// you will not have to change anything here



// put your application routines here:

void Application::SaveCurrentImage() {
    if (curImage.data != nullptr) {
        // delete old img
        if (oldImage.data != nullptr) {
            delete[] oldImage.data;
        }

        // copy curImage to oldImage
        oldImage.nx = curImage.nx;
        oldImage.ny = curImage.ny;
        oldImage.n = curImage.n;
        oldImage.ncolorChannels = curImage.ncolorChannels;
        oldImage.data = new unsigned char[curImage.n * curImage.ncolorChannels];
        memcpy(oldImage.data, curImage.data, curImage.n * curImage.ncolorChannels);
    }
}

void Application::FlipImage(Image *img)
{
    int i,j,k,rowOffsetSrc,rowOffsetDest,columnOffset;
    unsigned char ctmp;

	for(i=0;i<img->ny/2;i++)
	{
	   rowOffsetSrc=i*img->nx*img->ncolorChannels;
	   rowOffsetDest=(img->ny-1-i)*img->nx*img->ncolorChannels;
       for(j=0;j<img->nx;j++)
	   {
		   columnOffset=j*img->ncolorChannels;
		   for(k=0;k<img->ncolorChannels;k++)
		   {
			   ctmp=img->data[rowOffsetSrc+columnOffset+k];
               img->data[rowOffsetSrc+columnOffset+k]=img->data[rowOffsetDest+columnOffset+k];
               img->data[rowOffsetDest+columnOffset+k]=ctmp;
		   }
	   }
	}
}


void Application::Update()
{
    // check img
    if (curImage.data == nullptr) {
        printf("No image loaded!\n");
        return;
    }

    SaveCurrentImage();

    // Update data
    for (int i = 0; i < curImage.n; i++) {
        if (curImage.ncolorChannels == 3) {
            // For color img
            curImage.data[i * 3] = transFunc[0][curImage.data[i * 3]];       // Red channel
            curImage.data[i * 3 + 1] = transFunc[0][curImage.data[i * 3 + 1]]; // Green channel
            curImage.data[i * 3 + 2] = transFunc[0][curImage.data[i * 3 + 2]]; // Blue channel
        }
        else if (curImage.ncolorChannels == 1) {
            // For Grey
            curImage.data[i] = transFunc[0][curImage.data[i]];
        }
    }

    // recaculate the Histogram
    ComputeHistogram(&curImage);

    // use redraw fun
    gui->DisplayWindow->redraw();
    gui->EditorWindow->redraw();
}

void Application::AverageSmooth()
{
        // check img
        if (curImage.data == nullptr) {
            printf("No image loaded!\n");
            return;
        }

        SaveCurrentImage();

        // create a new arrary to save data
        unsigned char* smoothedData = new unsigned char[curImage.n * curImage.ncolorChannels];

        // use 3*3 kernal
        for (int y = 1; y < curImage.ny - 1; y++) {
            for (int x = 1; x < curImage.nx - 1; x++) {
                for (int c = 0; c < curImage.ncolorChannels; c++) {
                    int sum = 0;

                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int neighborX = x + dx;
                            int neighborY = y + dy;
                            int index = (neighborY * curImage.nx + neighborX) * curImage.ncolorChannels + c;

                            sum += curImage.data[index];
                        }
                    }

                    // find the average value
                    int newIndex = (y * curImage.nx + x) * curImage.ncolorChannels + c;
                    smoothedData[newIndex] = static_cast<unsigned char>(sum / 9);

                }
            }
        }


        // create new img
        memcpy(curImage.data, smoothedData, curImage.n * curImage.ncolorChannels);
        delete[] smoothedData;

        // recaculate
        ComputeHistogram(&curImage);

        //// redraw
        gui->DisplayWindow->redraw();
        gui->EditorWindow->redraw();

}

void Application::MedianSmooth()
{   

    SaveCurrentImage();
    // Create a copy of the original image data to store the new values
    unsigned char* newData = new unsigned char[curImage.n * curImage.ncolorChannels];

    
    // Traverse each pixel in the image (excluding the boundary pixels)
    for (int y = 1; y < curImage.ny - 1; y++) {
        for (int x = 1; x < curImage.nx - 1; x++) {
            for (int c = 0; c < curImage.ncolorChannels; c++) {
                // Collect the 3x3 neighborhood values
                unsigned char neighborhood[KERNEL_SIZE * KERNEL_SIZE];
                int index = 0;

                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        int neighborX = x + kx;
                        int neighborY = y + ky;
                        int neighborIndex = (neighborY * curImage.nx + neighborX) * curImage.ncolorChannels + c;
                        neighborhood[index++] = curImage.data[neighborIndex];
                    }
                }

                // Sort the neighborhood to find the median
                std::nth_element(neighborhood, neighborhood + 4, neighborhood + 9);
                unsigned char medianValue = neighborhood[4];

                // Set the new value to the corresponding pixel in newData
                int pixelIndex = (y * curImage.nx + x) * curImage.ncolorChannels + c;
                newData[pixelIndex] = medianValue;
            }
        }
    }

    // Update the original image data with the new smoothed data
    delete[] curImage.data;
    curImage.data = newData;

    // Recalculate the histogram for the new image
    ComputeHistogram(&curImage);

    // Redraw the display and editor windows
    gui->DisplayWindow->redraw();
    gui->EditorWindow->redraw();
}

void Application::GaussianSmooth() {

    SaveCurrentImage();

    // Gaussian kernal
    const int kernelSize = 5;
    int gaussianKernel[5][5] = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}
    };
    int kernelWeight = 256; // Sum of weight

    // create a new img
    unsigned char* newImageData = new unsigned char[curImage.n * curImage.ncolorChannels];

    for (int y = 2; y < curImage.ny - 2; y++) {
        for (int x = 2; x < curImage.nx - 2; x++) {
            // for every color
            for (int c = 0; c < curImage.ncolorChannels; c++) {
                int sum = 0;

                // kernal caculate
                for (int ky = -2; ky <= 2; ky++) {
                    for (int kx = -2; kx <= 2; kx++) {
                        int pixelVal = curImage.data[((y + ky) * curImage.nx + (x + kx)) * curImage.ncolorChannels + c];
                        int kernelVal = gaussianKernel[ky + 2][kx + 2];
                        sum += pixelVal * kernelVal;
                    }
                }

                sum /= kernelWeight;
                newImageData[(y * curImage.nx + x) * curImage.ncolorChannels + c] = (unsigned char)sum;
            }
        }
    }

    // update new img
    memcpy(curImage.data, newImageData, curImage.n * curImage.ncolorChannels);
    delete[] newImageData;

    // update histogram
    ComputeHistogram(&curImage);

    // redraw
    gui->DisplayWindow->redraw();
    gui->EditorWindow->redraw();
}

void Application::EdgeDetect() {

    SaveCurrentImage();

    // Define Sobel filters for x and y directions
    int sobel_x[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int sobel_y[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    // Create a new image to store edge detection results
    Image newImage;
    newImage.nx = curImage.nx;
    newImage.ny = curImage.ny;
    newImage.ncolorChannels = curImage.ncolorChannels;
    newImage.n = curImage.n;
    newImage.data = new unsigned char[curImage.n * curImage.ncolorChannels];

    // Apply Sobel filter to detect edges
    for (int y = 1; y < curImage.ny - 1; ++y) {
        for (int x = 1; x < curImage.nx - 1; ++x) {
            int gx[3] = { 0, 0, 0 };
            int gy[3] = { 0, 0, 0 };

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int pixelIndex = ((y + ky) * curImage.nx + (x + kx)) * curImage.ncolorChannels;
                    for (int c = 0; c < curImage.ncolorChannels; ++c) {
                        gx[c] += curImage.data[pixelIndex + c] * sobel_x[ky + 1][kx + 1];
                        gy[c] += curImage.data[pixelIndex + c] * sobel_y[ky + 1][kx + 1];
                    }
                }
            }

            int newPixelIndex = (y * curImage.nx + x) * curImage.ncolorChannels;
            for (int c = 0; c < curImage.ncolorChannels; ++c) {
                int magnitude = static_cast<int>(sqrt(gx[c] * gx[c] + gy[c] * gy[c]));
                if (magnitude > 255) {
                    magnitude = 255;
                }
                else if (magnitude < 0) {
                    magnitude = 0;
                }
                newImage.data[newPixelIndex + c] = magnitude;
            }
        }
    }

    // Update current image with the new image
    delete[] curImage.data;
    curImage = newImage;

    // Compute histogram for the new image
    ComputeHistogram(&curImage);

    // Redraw the windows to reflect the new image
    gui->DisplayWindow->redraw();
    gui->EditorWindow->redraw();
}

void Application::Undo() {
    if (oldImage.data != nullptr) {
        // exchange curImage & oldImage
        Image temp = curImage;
        curImage = oldImage;
        oldImage = temp;

        ComputeHistogram(&curImage);
        gui->DisplayWindow->redraw();
        gui->EditorWindow->redraw();
    }
    else {
        printf("Don't need to undo!\n");
    }
}

