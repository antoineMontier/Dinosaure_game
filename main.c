#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 1420
#define HEIGHT 720
#define DINO_WIDTH 40
#define DINO_HEIGHT 50
#define DINO_JUMP_POWER 20
#define OBSTACLE_SPEED 10
#define ACCELERATION 180
#define BULBS_NUMBER 4
#define SUPER_JUMP_TIMER 100
#define FRAMES_PER_SECOND 40
#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 60
#define PALETTE 3



typedef struct{
    double x;//center down of the bulb
    double y;//center down of the bulb
    double r;//height of the bulb
}bulb;

typedef struct{
    unsigned int r;//red 0-255
    unsigned int g;//green 0-255
    unsigned int b;//blue 0-255
}Color;



void SDL_ExitWithError(const char *string);
void point(SDL_Renderer* r, int x, int y);
void mark(SDL_Renderer* r, int x, int y, int thickness);
void line(SDL_Renderer* r, int x1, int y1, int x2, int y2);
void color(SDL_Renderer* r, int red, int green, int blue, int alpha);
void rect(SDL_Renderer* r, int x, int y, int height, int width, int filled);
void circle(SDL_Renderer * r, int centreX, int centreY, int radius);
void openSDL(int x, int y, int mode, SDL_Window**w, SDL_Renderer**r);
void closeSDL(SDL_Window**w, SDL_Renderer**r);
void background(SDL_Renderer* r, Color*c, int p);
void drawLandscape(SDL_Renderer* r, Color*c, int p);
void drawDino(SDL_Renderer* r, double x, double y, Color*c, int p);//x and y values relates to the left bottom corner of the "dino"
void updateDinoPosition(double*x, double*y, double*vy, double*ay);
void drawBulbs(SDL_Renderer* r, bulb *bulb, Color*c, int p);
void moveBulbs(bulb *bulb, int tc);
int distanceBeetweenWithOtherBulbsisLargerThan(bulb b, bulb*bb, unsigned int n);
void superJumpBar(SDL_Renderer* r, int n, Color*c, int p);
int died(bulb *bulbs, double x, double y);
double dist(double x1, double y1, double x2, double y2);
void printRestartButton(SDL_Renderer* r, Color*c, int p);
int rollover(int mx, int my, int x, int y, int w, int h);
void restartGame(bulb* b, double *x, double *y, double *vy, double *ay, int *tc, int *p);
void jump(double*y, double*vy, int*sj);



int main(int argc, char *args[]){//compile and execute with     gcc main.c -o main -lm $(sdl2-config --cflags --libs) && ./main

    Color *colors = malloc(4*PALETTE*sizeof(Color));//store 9*4 colors that behave nicely 4 to 4

    //first palette // blue theme

    colors[0].r = 9;
    colors[0].g = 43;
    colors[0].b = 73;

    colors[1].r = 8;
    colors[1].g = 34;
    colors[1].b = 58;

    colors[2].r = 25;
    colors[2].g = 78;
    colors[2].b = 125;

    colors[3].r = 43;
    colors[3].g = 101;
    colors[3].b = 153;

    //second palette // green theme

    colors[4].r = 43;
    colors[4].g = 142;
    colors[4].b = 0;

    colors[5].r = 27;
    colors[5].g = 88;
    colors[5].b = 0;

    colors[6].r = 43;
    colors[6].g = 142;
    colors[6].b = 0;

    colors[7].r = 7;
    colors[7].g = 23;
    colors[7].b = 0;

    //third palette // red theme

    colors[8].r = 97;
    colors[8].g = 0;
    colors[8].b = 0;

    colors[9].r = 114;
    colors[9].g = 0;
    colors[9].b = 0;

    colors[10].r = 155;
    colors[10].g = 0;
    colors[10].b = 0;

    colors[11].r = 182;
    colors[11].g = 2;
    colors[11].b = 2;



    SDL_Window *w;//open a window command
    SDL_Renderer *ren;//render creation

    int palette = 0;
    double d_x, d_y, d_vy, d_ay;

    bulb* bulbs = malloc(BULBS_NUMBER*sizeof(bulb));//array for the bulbs



    openSDL(WIDTH, HEIGHT, 0, &w, &ren);
    SDL_bool program_launched = SDL_TRUE; //SDL_FALSE or SDL_TRUE
    int tick_count = 0;
    int super_jump = 0; // 0 if super jump is avaible ; otherwise it's a timer
    int survived = 0;
    int last_survived = 0;

    restartGame(bulbs, &d_x, &d_y, &d_vy, &d_ay, &tick_count, &palette);


    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    while(program_launched){//principal loop
        SDL_Event evt;

            if(!died(bulbs, d_x, d_y)){
                moveBulbs(bulbs, tick_count);
                updateDinoPosition(&d_x, &d_y, &d_vy, &d_ay);

                background(ren, colors, palette);
                drawBulbs(ren, bulbs, colors, palette);

                drawLandscape(ren, colors, palette);

                superJumpBar(ren, super_jump, colors, palette);


                drawDino(ren, d_x, d_y, colors, palette);

                /*color(ren, 255, 0, 0, 255);
                mark(ren, 80, 160, 5);*/
                survived = SDL_GetTicks();

            }else{
                if(survived){
                    printf("you survived %d seconds\n", (survived - last_survived)/1000);
                    last_survived = survived;
                    survived = 0;
                }
                printRestartButton(ren, colors, palette);
            }
                SDL_RenderPresent(ren);//refresh the render


        //controls
        while(SDL_PollEvent(&evt)){//reads all the events (mouse moving, key pressed...)        //possible to wait for an event with SDL_WaitEvent
            switch(evt.type){

                case SDL_QUIT:
                    program_launched = SDL_FALSE;//quit the program if the user closes the window
                    break;

                case SDL_KEYDOWN:                   //SDL_KEYDOWN : hold a key            SDL_KEYUP : release a key
                    switch (evt.key.keysym.sym){//returns the key ('0' ; 'e' ; 'SPACE'...)

                        case SDLK_ESCAPE:
                            program_launched = SDL_FALSE;//escape the program by pressing esc
                            break;

                        case SDLK_SPACE:
                            jump(&d_y, &d_vy, &super_jump);
                            break;

                        default:
                            break;
                    }


                case SDL_MOUSEBUTTONDOWN:
                    if(!survived){//wait for the rollover only if the game is not running
                        int k = rollover(evt.button.x, evt.button.y, WIDTH/2 - BUTTON_WIDTH/2, HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT);
                        if(k){
                            restartGame(bulbs, &d_x, &d_y, &d_vy, &d_ay, &tick_count, &palette);
                            last_survived = SDL_GetTicks();
                        }

                    }else   
                        jump(&d_y, &d_vy, &super_jump);

                default:
                    break; 

            }
        }

        

        tick_count++;
        if(super_jump)
            super_jump--;
        SDL_Delay(1000/FRAMES_PER_SECOND);

    }




    /*SDL_RenderPresent(ren);//refresh the render
    SDL_Delay(5000);//waiting delay, in ms*/
    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    free(colors);
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

void background(SDL_Renderer* r, Color*c, int p){
    color(r, c[4*p+3].r, c[4*p+3].g, c[4*p+3].b, 255);
    rect(r, 0, 0, WIDTH, HEIGHT, 1);
}

void drawLandscape(SDL_Renderer* r, Color*c, int p){
    int alpha = 255;
    for(int y = 3*HEIGHT/4 ; y < HEIGHT && alpha > 0; y ++){
        color(r, c[4*p+1].r, c[4*p+1].g, c[4*p+1].b, alpha);
        line(r, 0, y, WIDTH, y);
        alpha -= 1;
    }
    
}

void drawDino(SDL_Renderer* r, double x, double y, Color*c, int p){
    color(r, c[4*p].r, c[4*p].g, c[4*p].b, 255);
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

void drawBulbs(SDL_Renderer* r, bulb *bulb, Color*c, int p){//!\ to draw the bulb before the background, otherwise the bulb will be a simple circle
    color(r, c[4*p+1].r, c[4*p+1].g, c[4*p+1].b, 255);
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
    int iteration_counter;
    for(int i = 0 ; i < BULBS_NUMBER ; i++){
        if(inTheGame[i] == 0){
            iteration_counter = 0;//avoid too much calculations(that would kill the game)
            do
            {
                bulbs[i].r = DINO_HEIGHT/2 + rand() % DINO_HEIGHT/2;
                bulbs[i].x = WIDTH + rand() % WIDTH;                               //distance might be enought
                iteration_counter++;
            } while(!distanceBeetweenWithOtherBulbsisLargerThan(bulbs[i], bulbs, DINO_WIDTH*4) && iteration_counter < 100);
            
        }
    }

    for(int i = 0 ; i < BULBS_NUMBER ; i++)
        (bulbs[i].x) -= OBSTACLE_SPEED + 0.01*tc; //move the bulbs



   /*for(int i = 0 ; i < BULBS_NUMBER ; i++){
        printf("%d     x : %f ; y : %f ; r = %f\n", i, bulbs[i].x , bulbs[i].y, bulbs[i].r);
    }*/

}

void superJumpBar(SDL_Renderer* r, int n, Color*c, int p){
    //origin at x = 50 y = 150
    int x = 50;
    double pace = SUPER_JUMP_TIMER/100.0; // 100 is the size of the bar;
    color(r, c[4*p+3].r, c[4*p+3].g, c[4*p+3].b, 255);//background

    for(int y = 140 ; y >= 40 ; y--){
        for(int x = 47 ; x <= 53 ; x++)
            point(r, x, y);
    }

    color(r, c[4*p+1].r, c[4*p+1].g, c[4*p+1].b, 255);//bar color


    for(int y = 40 ; y <= 140; y++){
        for(int x = 47 ; x <= 53 ; x++)
            if(y > n/pace + 40)
                point(r, x, y);
    }

    

}

double dist(double x1, double y1, double x2, double y2){
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

int died(bulb *bulbs, double x, double y){//there's a 1 pixel delta
    for(int b = 0 ; b < BULBS_NUMBER ; b++){
        if(dist(x, y, bulbs[b].x, bulbs[b].y) < bulbs[b].r - 1)//left down corner
            return 1;
        if(dist(x + DINO_WIDTH, y, bulbs[b].x, bulbs[b].y) < bulbs[b].r - 1)//right down corner
            return 1;
        if(dist(x + DINO_WIDTH/3, y, bulbs[b].x, bulbs[b].y) < bulbs[b].r - 1)//1/3
            return 1;
        if(dist(x + 2*DINO_WIDTH/3, y, bulbs[b].x, bulbs[b].y) < bulbs[b].r - 1)//2/3
            return 1;
    }
    return 0;//no died
}

void printRestartButton(SDL_Renderer* r, Color*c, int p){
    color(r, c[4*p].r, c[4*p].g + 30, c[4*p].b, 150);
    rect(r, WIDTH/2 - BUTTON_WIDTH/2, HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT, 1);
    color(r, 20, 20, 20, 255);
    rect(r, WIDTH/2 - BUTTON_WIDTH/2, HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT, 0);

}

int rollover(int mx, int my, int x, int y, int w, int h){
    if(mx >= x && mx < x + w && my > y && my < y + h)
        return 1;//true
    return 0;//false
}

void restartGame(bulb* b, double *x, double *y, double *vy, double *ay, int *tc, int*p){
    for(int i = 0 ; i < BULBS_NUMBER ; i++){ //initialise like this in order to be regenerated randomly
        b[i].x = -100 + i*10;
        b[i].y = 3*HEIGHT/4;
        b[i].r = 50;
    }

    *x = WIDTH/4;
    *y = 3*HEIGHT/4;
    *vy = 0;//speed
    *ay = ACCELERATION/100.0;//acceleration
    *tc = 0; //reset ticks
    int k = *p;
    do{
        *p = rand() % PALETTE;
    } while (*p == k);
    
}

void jump(double*y, double*vy, int*sj){
    if(*y >= 3*HEIGHT/4 - 0.1*DINO_HEIGHT)//assert the dino isn't already jumping
        *vy = -DINO_JUMP_POWER;
    else if(*sj == 0 && *vy < 3*HEIGHT/4 - 0.1*DINO_HEIGHT){ //assert the dino can double jump and he's already in the air
        *vy = -DINO_JUMP_POWER;
        *sj = SUPER_JUMP_TIMER;
    }
}
