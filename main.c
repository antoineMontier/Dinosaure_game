#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1420
#define HEIGHT 720
#define DINO_WIDTH 40
#define DINO_HEIGHT 50
#define DINO_JUMP_POWER 20
#define OBSTACLE_SPEED 10
#define ACCELERATION 1
#define BULBS_NUMBER 3


typedef struct{
    double x;//center down of the bulb
    double y;//center down of the bulb
    double r;//height of the bulb
}bulb;



void SDL_ExitWithError(const char *string);
void point(SDL_Renderer* r, int x, int y);
void mark(SDL_Renderer* r, int x, int y, int thickness);
void line(SDL_Renderer* r, int x1, int y1, int x2, int y2);
void color(SDL_Renderer* r, int red, int green, int blue, int alpha);
void rect(SDL_Renderer* r, int x, int y, int height, int width, int filled);
void circle(SDL_Renderer * r, int centreX, int centreY, int radius);
void openSDL(int x, int y, int mode, SDL_Window**w, SDL_Renderer**r);
void closeSDL(SDL_Window**w, SDL_Renderer**r);
void background(SDL_Renderer* r, int red, int green, int blue);
void drawLandscape(SDL_Renderer* r);
void drawDino(SDL_Renderer* r, double x, double y);//x and y values relates to the left bottom corner of the "dino"
void updateDinoPosition(double*x, double*y, double*vy, double*ay);
void drawBulbs(SDL_Renderer* r, bulb *bulb);
void moveBulbs(bulb *bulb, int tc);

int main(int argc, char *argv[]){//compile and execute with     gcc main.c -o main $(sdl2-config --cflags --libs) && ./main


    SDL_Window *w;//open a window command
    SDL_Renderer *ren;//render creation
    double d_x = WIDTH/4;
    double d_y = 3*HEIGHT/4;
    double d_vy = 0;//speed
    double d_ay = ACCELERATION;//acceleration

    bulb* bulbs = malloc(BULBS_NUMBER*sizeof(bulb));//array for the bulbs


    for(int i = 0 ; i < BULBS_NUMBER ; i++){ //initialise like this in order to be regenerated randomly
        bulbs[i].x = -10 + i*10;
        bulbs[i].y = 3*HEIGHT/4;
        bulbs[i].r = 50;
    }

    /*for(int i = 0 ; i < BULBS_NUMBER ; i++){
        printf("%d     x : %f ; y : %f ; r = %f\n", i, bulbs[i].x , bulbs[i].y, bulbs[i].r);        //display the bulbs by force
    }*/



    openSDL(WIDTH, HEIGHT, 0, &w, &ren);
    SDL_bool program_launched = SDL_TRUE; //SDL_FALSE or SDL_TRUE
    int tick_count = 0;

    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    while(program_launched){//principal loop
        SDL_Event evt;

            //printf("dino coordinates       y:%f\tvy:%f\tay:%f\n", d_y, d_vy, d_ay);

            moveBulbs(bulbs, tick_count);
            updateDinoPosition(&d_x, &d_y, &d_vy, &d_ay);

            background(ren, 255, 255, 255);
            drawBulbs(ren, bulbs);

            drawLandscape(ren);



            drawDino(ren, d_x, d_y);

            /*color(ren, 255, 0, 0, 255);
            mark(ren, 80, 160, 5);*/




    

    



        SDL_RenderPresent(ren);//refresh the render


        //controls
        while(SDL_PollEvent(&evt)){//reads all the events (mouse moving, key pressed...)        //possible to wait for an event with SDL_WaitEvent
            switch(evt.type){

                case SDL_QUIT:
                    program_launched = SDL_FALSE;//quit the program if the user closes the window
                    break;

                case SDL_KEYDOWN:                   //SDL_KEYDOWN : hold a key            SDL_KEYUP : release a key
                    switch (evt.key.keysym.sym){//returns the key ('0' ; 'e' ; 'SPACE'...)

                        case SDLK_v:
                            if(d_y >= 3*HEIGHT/4)//assert the dino isn't already jumping
                                d_vy = -DINO_JUMP_POWER;
                            break;

                        case SDLK_ESCAPE:
                            program_launched = SDL_FALSE;//escape the program by pressing esc
                            break;


                        default:
                            break;
                    }



                default:
                    break; 

            }
        }

        

    
        


    
        tick_count++;
        SDL_Delay(33);//1000/30 = 33 this delay is for 30fps

    }




    /*SDL_RenderPresent(ren);//refresh the render
    SDL_Delay(5000);//waiting delay, in ms*/
    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    free(bulbs);
    closeSDL(&w, &ren);
    printf("closed successfully !\n");
    return 0;
}

void SDL_ExitWithError(const char *string){
    SDL_Log("Error : %s > %s\n", string, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void mark(SDL_Renderer* r, int x, int y, int thickness){
    for(int a = y - thickness ; a <= y + thickness ; a++){
        for(int b = x - thickness ; b <= x + thickness; b++){
            point(r, b, a);
        }
    }
}

void point(SDL_Renderer* r, int x, int y){
    if(SDL_RenderDrawPoint(r, x, y) != 0)
        SDL_ExitWithError("failed to draw point");
}

void line(SDL_Renderer* r, int x1, int y1, int x2, int y2){
    if(SDL_RenderDrawLine(r, x1, y1, x2, y2) != 0)//line
        SDL_ExitWithError("failed to draw line");
}

void color(SDL_Renderer* r, int red, int green, int blue, int alpha){
    if(SDL_SetRenderDrawColor(r, red, green, blue, alpha) != 0)
        SDL_ExitWithError("failed to set color");
}

void rect(SDL_Renderer* r, int x, int y, int width, int height, int filled){
    SDL_Rect rectangle;
    rectangle.x = x;
    rectangle.y = y;
    rectangle.w = width;
    rectangle.h = height;

    if(filled){
        if(SDL_RenderFillRect(r, &rectangle) != 0)
            SDL_ExitWithError("failed to draw a full rectangle");
    }
    if(!filled){
        if(SDL_RenderDrawRect(r, &rectangle) != 0)
            SDL_ExitWithError("failed to draw a full rectangle");
    }
}

void circle(SDL_Renderer * r, int cx, int cy, int radius){
   const int diameter = (radius * 2);

   int x = (radius - 1);
   int y = 0;
   int tx = 1;
   int ty = 1;
   int error = (tx - diameter);

   while (x >= y){
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(r, cx + x, cy - y);
      SDL_RenderDrawPoint(r, cx + x, cy + y);
      SDL_RenderDrawPoint(r, cx - x, cy - y);
      SDL_RenderDrawPoint(r, cx - x, cy + y);
      SDL_RenderDrawPoint(r, cx + y, cy - x);
      SDL_RenderDrawPoint(r, cx + y, cy + x);
      SDL_RenderDrawPoint(r, cx - y, cy - x);
      SDL_RenderDrawPoint(r, cx - y, cy + x);

      if (error <= 0){
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0){
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void openSDL(int x, int y, int mode, SDL_Window**w, SDL_Renderer**r){

    if(0 != SDL_Init(/*flag*/ SDL_INIT_VIDEO))//lots of flags like SDL_INIT_AUDIO ; *_VIDEO ; *_EVERYTHING... To separe with '|'
        SDL_ExitWithError("Initialisation SDL failed");
    //at this point, the SDL is well initialised, we can afford it because of the if


    if(SDL_CreateWindowAndRenderer(x, y, mode, w, r) !=0)
        SDL_ExitWithError("window and render creation failed");

}

void closeSDL(SDL_Window**w, SDL_Renderer**r){
    SDL_DestroyRenderer(*r);
    SDL_DestroyWindow(*w);
    SDL_Quit();
}

void background(SDL_Renderer* r, int red, int green, int blue){
    color(r, red, green, blue, 255);
    rect(r, 0, 0, WIDTH, HEIGHT, 1);
}

void drawLandscape(SDL_Renderer* r){
    int alpha = 255;
    for(int y = 3*HEIGHT/4 ; y < HEIGHT && alpha > 0; y ++){
        color(r, 255-alpha, 255-alpha, 255-alpha, alpha);
        line(r, 0, y, WIDTH, y);
        alpha -= 1;
    }
    
}

void drawDino(SDL_Renderer* r, double x, double y){
    color(r, 0, 0, 0, 255);
    rect(r, x, y - DINO_HEIGHT, DINO_WIDTH , DINO_HEIGHT, 1);
}

void updateDinoPosition(double*x, double*y, double*vy, double*ay){
    //x doesn't have to change since the background moves, not the dino
    *y += (*vy);
    *vy += (*ay);

    if(*y >= 3*HEIGHT/4){//if the dino is under the ground
        *y = 3*HEIGHT/4;
        *vy = 0;
        //*ay = 0;
    }


}

void drawBulbs(SDL_Renderer* r, bulb *bulb){//!\ to draw the bulb before the background, otherwise the bulb will be a simple circle
    color(r, 255, 0, 0, 255);
    for(int b = 0 ; b < BULBS_NUMBER ; b ++){
        for(int i = bulb[b].r ; i >= bulb[b].r - 5 ; i--){//thicken the border of the bulb
            circle(r, bulb[b].x, bulb[b].y, i);
        }
    }
}

int distanceBeetweenWithOtherBulbsisLargerThan(bulb b, bulb*bb, unsigned int n){
    for(int i = 0 ; i < BULBS_NUMBER ; i++){
        if(b.x != bb[i].x){//make sure we're not treation acutal bulb
            if(b.x < bb[i].x && (bb[i].x - bb[i].r - b.x - b.r) < n)
                return 0;
            if(b.x > bb[i].x && (b.x - bb[i].r - bb[i].x - b.r) < n)
                return 0;
        }
    }
    return 1;//True
}

void moveBulbs(bulb *bulbs, int tc){//to upgrade : bulbs will go faster and faster with time

    int *inTheGame = malloc(BULBS_NUMBER*sizeof(int));
    for(int i = 0 ; i < BULBS_NUMBER ; i++){
        if(bulbs[i].x + bulbs[i].r < 0)
            inTheGame[i] = 0;
        else
            inTheGame[i] = 1;
    } 

    //here, the table is 0 if the bulbs has to die

    //let's regenerate every died bulbs :

    srand(time(0));

    for(int i = 0 ; i < BULBS_NUMBER ; i++){
        if(inTheGame[i] == 0){
            do
            {
                bulbs[i].x = WIDTH + rand() % WIDTH;                               //distance greater than 50 might be enought
            } while (!distanceBeetweenWithOtherBulbsisLargerThan(bulbs[i], bulbs, 50));
            
        }
    }

    for(int i = 0 ; i < BULBS_NUMBER ; i++)
        (bulbs[i].x) -= OBSTACLE_SPEED + 0.01*tc; //move the bulbs


}


