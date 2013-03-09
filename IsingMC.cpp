#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <string>
#include <list>
#include <vector>
#include <set>


	#include "SDL/SDL.h"
	
	#include "SDL/SDL_ttf.h"


#include <SDL/SDL_opengl.h>

using namespace std;

#define WIDTH 1400
#define HEIGHT 900
#define BPP 4
#define DEPTH 32
#define PI 3.14159265

double a0=1e-07;
double sourceHeight=100*a0;
double channelHeight=100*a0;
double sourceWidth=100*a0;
double channelWidth=100*a0;
double sinkWidth=100*a0;

int SDLxOffset=50;
int SDLyOffset=HEIGHT-50;
int boxSize=8;
int SDLscaling=boxSize;
	
int arraySize=100;

double EQtollerance=1e-5; //was 1e-5

double J=1;
double T=2.5;
double k=1;
#include "myClass.h"


int scaleX( double x) {

	return (int)((x*SDLscaling)+SDLxOffset);
}

int scale2X( double x) {

	return (int)((x*SDLscaling)+650);
}

int scaleY( double y) {

	return (int)(SDLyOffset-(y*SDLscaling));
}

void pause(double seconds) {
	int startpause=clock();
	int endpause=clock();
	while (endpause-startpause<seconds*CLOCKS_PER_SEC) endpause= clock();
	
}


bool eqtest( double a, double b) {
	if (fabs(a/b -1) < EQtollerance){
		 return true; }
	else
	{ return false; }
}
	

inline double det(double a, double b, double c, double d, double e, double f, double g, double h, double i){
	/*
	  a  b  c 
	  d  e  f
	  g  h  i
	*/

	return a*( e*i - h*f) -b*(d*i-g*f) +c*(d*h -g*e);
}

inline double circum(list<CDelTriangle>::iterator p,list<CVortex>::iterator q) {
	double Ax, Bx, Cx, Dx, Ay, By, Cy, Dy;
	Ax=p->get_Ax();
	Ay=p->get_Ay();
	Bx=p->get_Bx();
	By=p->get_By();
	Cx=p->get_Cx();
	Cy=p->get_Cy();
	Dx=q->get_x();
	Dy=q->get_y();
	return det(	Ax-Dx,Ay-Dy,Ax*Ax-Dx*Dx+Ay*Ay-Dy*Dy,
				Bx-Dx,By-Dy,Bx*Bx-Dx*Dx+By*By-Dy*Dy,
				Cx-Dx,Cy-Dy,Cx*Cx-Dx*Dx+Cy*Cy-Dy*Dy);
	
}



int roundspecial(double x)
{
	return (int)(x + 0.5);
}

int nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return roundspecial(pow(2,ceil(logbase2)));
}



void SDL_GL_RenderText(char *text, 
                      TTF_Font *font,
                      SDL_Color color,
                      SDL_Rect *location)
{
	SDL_Surface *initial;
	SDL_Surface *intermediary;
	SDL_Rect rect;
	int w,h;
	GLuint texture;
	//SDL_FillRect(initial, NULL, SDL_MapRGB(initial->format, 255, 255, 255));	
	/* Use SDL_TTF to render our text */
	initial = TTF_RenderText_Blended(font, text, color);
	
	/* Convert the rendered text to a known format */
	//w = nextpoweroftwo(initial->w);
	//h = nextpoweroftwo(initial->h);
	w=initial->w;
	h=initial->h;
	intermediary = SDL_CreateRGBSurface(0, w, h, 32, 
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_FillRect(intermediary, NULL, SDL_MapRGB(intermediary->format, 255, 255, 255));	
	
	SDL_BlitSurface(initial, 0, intermediary, 0);
	
	/* Tell GL about our new texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, 
			GL_UNSIGNED_BYTE, intermediary->pixels );
	
	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	/* Draw a quad at location */
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f); 
			glVertex2f(location->x    , location->y);
		glTexCoord2f(1.0f, 1.0f); 
			glVertex2f(location->x + w, location->y);
		glTexCoord2f(1.0f, 0.0f); 
			glVertex2f(location->x + w, location->y + h);
		glTexCoord2f(0.0f, 0.0f); 
			glVertex2f(location->x    , location->y + h);
	glEnd();
	
	/* Bad things happen if we delete the texture before it finishes */
	glFinish();
	
	/* return the deltas in the unused w,h part of the rect */
	location->w = initial->w;
	location->h = initial->h;
	
	/* Clean up */
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}


void glEnable2D()
{
	int vPort[4];
  
	glGetIntegerv(GL_VIEWPORT, vPort);
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
  
	glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void glDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
}


bool writeTextToSurface(string renderStr, SDL_Surface *screen, int x ,int y , TTF_Font *font, SDL_Color text_color) {
		bool error =false;
		
		char renderChar[400];
		
		// convert str stream to char
		strcpy(renderChar,renderStr.c_str());
		
		SDL_Rect DestR;
		
		
		SDL_Surface *initial;
		initial = TTF_RenderText_Blended(font, renderChar, text_color);
		
		
		DestR.x = x;
		DestR.y = HEIGHT-initial->h-y;
		
		
	
		
		
		/*	
		// Write text to surface
		SDL_Surface *text;
		  
		  
		text = TTF_RenderText_Solid(font,	renderChar,	text_color);
		
		if (text == NULL)
		{
			cerr << "TTF_RenderText_Solid() Failed: " << TTF_GetError() << endl;
			TTF_Quit();
			SDL_Quit();
			error =true;
		}
		
		    // Apply the text to the display
		if (SDL_BlitSurface(text, NULL, screen, &DestR) != 0)
			{
			cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
			error=true;
		}
	
		// SDL_FreeSurface(text);
		*/
		
		glEnable2D();
		SDL_GL_RenderText(renderChar, font, text_color, &DestR);
		glDisable2D();
		
		
		
		return error;
	
}

bool acceptMove(double array[102][102], int i, int j, int trial) {
	int upcount=0;
	int curcount=0;
	
	if (array[i][j-1]==array[i][j]) {
			curcount++;
	}
	if (array[i][j+1]==array[i][j]) {
			curcount++;
	}
	if (array[i-1][j]==array[i][j]) {
			curcount++;
	}
	if (array[i+1][j]==array[i][j]) {
			curcount++;
	}
	
	if (array[i][j-1]==trial) {
			upcount++;
	}
	if (array[i][j+1]==trial) {
			upcount++;
	}
	if (array[i-1][j]==trial) {
			upcount++;
	}
	if (array[i+1][j]==trial) {
			upcount++;
	}
					

				
	//cout << upcount << endl;	
	
	// checkerboard low energy
	//if (array[i][j]==1) deltaE=-upcount+(4-upcount)-(-(4-upcount)+upcount);
	//else if (array[i][j]==0) deltaE=-(4-upcount)+upcount-(-upcount+(4-upcount));
	double currentE=-curcount;
	double newE=-upcount;
	//cout << array[i][j] << " -> " << currentE << ", " << trial << " -> " << newE << endl;
	
	if (newE<=currentE) {
		return true;
	}
	else {
		double Pchange = exp((currentE-newE)/T);
		double roll = rand()/(double)RAND_MAX;
		//cout << roll << ", " << Pchange << endl;
		if (roll<Pchange) {
			//cout << "true" << endl;
			return true;
			
		}
		else {
			 //cout << "false" << endl;
			 return false;
		 }
	}
	
}




int main(int argc, char* argv[]) {
	

	SDL_Surface *screen;
	TTF_Font *font;
	TTF_Font *graphfont;
	SDL_Color text_color = {0, 0, 0}; 
	SDL_Color white_color= {255,255,255};
	SDL_Event event;
  
	//initialise Screen
	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
   
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	
    //if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE)))
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_HWSURFACE | SDL_OPENGL)))
    {
        SDL_Quit();
        return 1;
    }
    
    glEnable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );
 
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
    
    glClearColor(255, 255, 255, 0);
 
    glViewport(0, 0, WIDTH, HEIGHT);
 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    glOrtho(0, WIDTH, HEIGHT, 0, 1, -1);
 
    glMatrixMode(GL_MODELVIEW);
 
    glEnable(GL_TEXTURE_2D);
 
    glLoadIdentity();
    
	SDL_WM_SetCaption( "Program Name", 0 );
	
	// Initialize SDL_ttf library
   if (TTF_Init() != 0)
   {
      cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
      SDL_Quit();
      return 1;
   }

   // Load a font
  
   font = TTF_OpenFont("FreeSans.ttf", 20);
   if (font == NULL)
   {
      cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
      TTF_Quit();
      SDL_Quit();
      return 1;
   }
	
	graphfont = TTF_OpenFont("FreeSans.ttf", 14);
   if (font == NULL)
   {
      cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
      TTF_Quit();
      SDL_Quit();
      return 1;
   }
	
		srand( time(NULL));
	int newColours=2;
	
	 double grid[102][102]={0};
   int speciescount[10000];
   
   for (int i=1; i<=arraySize;i++){
		 for (int j=1;j<=arraySize;j++) {
			//if (rand()%2==1){
				grid[i][j]=rand()%(newColours);
			//}
		}
	}
  
   
   	
	
	//SDLscaling= 5/(1e-8);
	
	
	bool running=true;
	
	int starttick=clock();
	double seconds=0.1;
	int t=0;
	
	 
	starttick=clock();
   
		  
		
   do {
		//opengl	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();	
		
			
		///// SDL OUTPUT TO SCREEN ////////////////
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 255, 255));	
		ostringstream oss;
			oss.str("");
			oss << "Num species: " << newColours << "  Temp:" << T;
			writeTextToSurface(oss.str(), screen, 20 ,20 , graphfont, text_color); 
			
			
			
			while (SDL_PollEvent(&event))
			{
				// Check for the quit message
				if (event.type == SDL_QUIT)
				{
					// Quit the program
					cerr << "SDL_QUIT event"<< endl;
					running=false;
					break;
				}
				if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						
					}
					if (event.key.keysym.sym == SDLK_c) {
						
					}
					if (event.key.keysym.sym == SDLK_w) {
						T=T+0.1;
					}
					if (event.key.keysym.sym == SDLK_s) {
						T=T-0.1;
					}
					if (event.key.keysym.sym == SDLK_e) {
						T=T+0.01;
					}
					if (event.key.keysym.sym == SDLK_d) {
						T=T-0.01;
					}
					if (event.key.keysym.sym == SDLK_x) {
						T=0;
					}
					if (event.key.keysym.sym == SDLK_2) {
						newColours=2;
					  for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (event.key.keysym.sym == SDLK_3) {
						newColours=3;
						for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (event.key.keysym.sym == SDLK_4) {
						newColours=4;
						for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (event.key.keysym.sym == SDLK_5) {
						newColours=5;
						for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (event.key.keysym.sym == SDLK_6) {
						newColours=6;
						for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (event.key.keysym.sym == SDLK_7) {
						newColours=7;
						for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (event.key.keysym.sym == SDLK_8) {
						newColours=8;
						for (int i=1; i<=arraySize;i++){
							for (int j=1;j<=arraySize;j++) {
								grid[i][j]=rand()%(newColours);
							}
						}
					}
					if (T<0) T=0;
					
					
					
					
				}
			}
			
			
			
			
			for (int c= 1;c<=10000; c++) {
				for (int p=1;p<=arraySize;p++) {
					grid[p][0]=grid[p][arraySize];
					grid[p][arraySize+1]=grid[p][1];
					grid[0][p]=grid[arraySize][p];
					grid[arraySize+1][p]=grid[1][p];
					
					
				}
				
				
				int i = rand() %arraySize +1;
				int j = rand() %arraySize +1;
				
				 
				int trial =rand()%(newColours);
				
				
				if ( acceptMove(grid,i,j,trial)) {
					grid[i][j]=trial;
					//draw quads
					/*if (grid[i][j]==1) {
								glColor3f(0, 0, 1);
					}
					else if (grid[i][j]==0) {	
							glColor3f(1, 1, 1);
					}
							glBegin(GL_QUADS);
							glVertex3f(scaleX(i), scaleY(j),0);
							glVertex3f(scaleX(i), scaleY(j)+boxSize,0);
							glVertex3f(scaleX(i)+boxSize, scaleY(j)+boxSize,0);
							glVertex3f(scaleX(i)+boxSize, scaleY(j),0);
							glEnd();
					
				*/
			
				}
				
				
			}
							
							glBegin(GL_QUADS);
							glColor3f(0, 0, 1);
							glVertex3f(scaleX(arraySize)+90, scaleY(arraySize)-20,0);
							glVertex3f(scaleX(arraySize)+90, scaleY(arraySize)+20+newColours*20,0);
							glVertex3f(scaleX(arraySize)+118, scaleY(arraySize)+20+newColours*20,0);
							glVertex3f(scaleX(arraySize)+118, scaleY(arraySize)-20,0);
							glEnd();
		  for (int q=0;q<=newColours;q++ ){
				speciescount[q]=0;
			}
			
			
			
			for (int i=1; i<=arraySize; i++ ) {
				for (int j=1;j<=arraySize;j++) { 
						//if (grid[i][j]==1) {
							int specnum=grid[i][j];
							speciescount[specnum]++;
							glBegin(GL_QUADS);
							double r= grid[i][j]/(double)(newColours-1);
							double g = grid[i][j]/(double)(newColours-1);
							double b = grid[i][j]/(double)(newColours-1);
							//cout << r << ", " << g << ", " << b << endl;
							
							glColor3f(r, g, b);
							glVertex3f(scaleX(i), scaleY(j),0);
							glVertex3f(scaleX(i), scaleY(j)+boxSize,0);
							glVertex3f(scaleX(i)+boxSize, scaleY(j)+boxSize,0);
							glVertex3f(scaleX(i)+boxSize, scaleY(j),0);
							glEnd();
						//}
						   
					}
				}
    
				for (int p=0;p<=newColours-1;p++ ) {
				glBegin(GL_QUADS);
							double r= p/(double)(newColours-1);
							double g = p/(double)(newColours-1);
							double b = p/(double)(newColours-1);
							//cout << r << ", " << g << ", " << b << endl;
							
							glColor3f(r, g, b);
							glVertex3f(scaleX(arraySize)+100, scaleY(arraySize)+p*20,0);
							glVertex3f(scaleX(arraySize)+100, scaleY(arraySize)+p*20+boxSize,0);
							glVertex3f(scaleX(arraySize)+100+boxSize, scaleY(arraySize)+p*20+boxSize,0);
							glVertex3f(scaleX(arraySize)+100+boxSize, scaleY(arraySize)+p*20,0);
							glEnd();
						
						oss.str("");
						oss << speciescount[p];
						writeTextToSurface(oss.str(), screen, scaleX(arraySize)+120 ,scaleY(arraySize)-5+p*20 , graphfont, text_color); 
			
			
			}
			
			
			
			
    SDL_GL_SwapBuffers();	
	
	
	} while (running==true);
	
	TTF_CloseFont(font);
	TTF_CloseFont(graphfont);
	
	return 0;

}



