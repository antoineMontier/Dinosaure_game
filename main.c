#include "SDL_Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define DINO_WIDTH 40
#define DINO_HEIGHT 40
#define DINO_JUMP_POWER 20
#define OBSTACLE_SPEED 15
#define ACCELERATION 180
#define BULBS_NUMBER 4
#define SUPER_JUMP_TIMER 100
#define FRAMES_PER_SECOND 36
#define BUTTON_WIDTH 70
#define BUTTON_HEIGHT 70
#define PALETTE 4



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



void drawLandscape(SDL_Renderer* r, Color*c, int p);
void drawDino(SDL_Renderer* r, double x, double y, Color*c, int p, int yy);//x and y values relates to the left bottom corner of the "dino"
void updateDinoPosition(double*x, double*y, double*vy, double*ay);
void drawBulbs(SDL_Renderer* r, bulb *bulb, Color*c, int p);
void moveBulbs(bulb *bulb, int tc);
int distanceBeetweenWithOtherBulbsisLargerThan(bulb b, bulb*bb, unsigned int n);
void superJumpCircle(SDL_Renderer* r, int n, Color*c, int p);
int died(bulb *bulbs, double x, double y);
void printRestartButton(SDL_Renderer* r, Color*c, int p);
void restartGame(bulb* b, double *x, double *y, double *vy, double *ay, int *tc, int *p);
void jump(double*y, double*vy, int*sj);
int inTheTriangle(double x1, double y1, double x2, double y2, double x3, double y3, double a, double b);
void background(SDL_Renderer* r, Color*c, int p);



int main(int argc, char *args[]){//compile and execute with     gcc main.c -o main -lm $(sdl2-config --cflags --libs) && ./main

    Color *colors = malloc(4*PALETTE*sizeof(Color));//store 9*4 colors that behave nicely 4 to 4

    //1 // Swap sans

    colors[0].r = 15;
    colors[0].g = 32;
    colors[0].b = 97;

    colors[1].r = 255;
    colors[1].g = 255;
    colors[1].b = 255;

    colors[2].r = 97;
    colors[2].g = 237;
    colors[2].b = 228;

    colors[3].r = 42;
    colors[3].g = 121;
    colors[3].b = 161;

    //2 //Dark codes

    colors[4].r = 0;
    colors[4].g = 12;
    colors[4].b = 67;

    colors[5].r = 25;
    colors[5].g = 25;
    colors[5].b = 25;

    colors[6].r = 68;
    colors[6].g = 68;
    colors[6].b = 68;

    colors[7].r = 253;
    colors[7].g = 170;
    colors[7].b = 50;

    //3 // lightaeon #2

    colors[8].r = 242;
    colors[8].g = 75;
    colors[8].b = 106;

    colors[9].r = 31;
    colors[9].g = 49;
    colors[9].b = 81;

    colors[10].r = 89;
    colors[10].g = 47;
    colors[10].b = 239;

    colors[11].r = 114;
    colors[11].g = 250;
    colors[11].b = 154;

     //4 // Bold decision

    colors[12].r = 232;
    colors[12].g = 157;
    colors[12].b = 86;

    colors[13].r = 26;
    colors[13].g = 58;
    colors[13].b = 70;

    colors[14].r = 205;
    colors[14].g = 84;
    colors[14].b = 29;

    colors[15].r = 115;
    colors[15].g = 133;
    colors[15].b = 100;



    SDL_Window *w;//open a window command
    SDL_Renderer *ren;//render creation

    int palette = 0;
    double d_x, d_y, d_vy, d_ay;
    char*tmp = malloc(20);
    bulb* bulbs = malloc(BULBS_NUMBER*sizeof(bulb));//array for the bulbs



    openSDL(WIDTH, HEIGHT, 0, &w, &ren);
    SDL_bool program_launched = SDL_TRUE; //SDL_FALSE or SDL_TRUE
    int tick_count = 0;
    int super_jump = 0; // 0 if super jump is avaible ; otherwise it's a timer
    int survived = 0;
    int last_survived = 0;
    int p_y = 3*HEIGHT/4;
    TTF_Font *font;
    setFont(&font, "./Roboto-Regular.ttf", 50);
    restartGame(bulbs, &d_x, &d_y, &d_vy, &d_ay, &tick_count, &palette);


    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    while(program_launched){//principal loop
        SDL_Event evt;

            if(!died(bulbs, d_x, d_y)){
                moveBulbs(bulbs, tick_count);
                updateDinoPosition(&d_x, &d_y, &d_vy, &d_ay);

                background(ren, colors, palette);
                superJumpCircle(ren, super_jump, colors, palette);

                drawDino(ren, d_x, d_y, colors, palette, p_y);

                drawBulbs(ren, bulbs, colors, palette);

                drawLandscape(ren, colors, palette);
                toChar(tmp, (survived - last_survived)/1000);
                text(ren, WIDTH -100, 25, tmp, font, colors[4*palette + 1].r, colors[4*palette + 1].g, colors[4*palette + 1].b);



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

        
        if(abs(p_y - d_y) > 2)
            p_y = d_y;
        tick_count++;
        if(super_jump)
            super_jump--;
        SDL_Delay(1000/FRAMES_PER_SECOND);
    }
    TTF_CloseFont(font);
    free(tmp);
    free(colors);
    free(bulbs);
    closeSDL(&w, &ren);
    printf("closed successfully !\n");
    return 0;
}

void drawLandscape(SDL_Renderer* r, Color*c, int p){
    int alpha = 255;
    for(int y = 3*HEIGHT/4 ; y < HEIGHT && alpha > 0; y ++){
        color(r, c[4*p+1].r, c[4*p+1].g, c[4*p+1].b, alpha);
        line(r, 0, y, WIDTH, y);
        alpha -= 1;
    }

}

void drawDino(SDL_Renderer* r, double x, double y, Color*c, int p, int yy){

    double a = 150;
    for(int u = 0 ; u <= DINO_WIDTH*5 && a > 0; u++){
        color(r, (255 - a)*c[4*p+3].r/255, (255 - a)*c[4*p+3].g/255, (255 - a)*c[4*p+3].b/255, 0);
        triangle(r, x+1, y, x+1, y - DINO_HEIGHT, x-u, yy - DINO_HEIGHT/2, 0);
        a-= 0.8;
    } 
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
    color(r, c[4*p+2].r, c[4*p+2].g, c[4*p+2].b, 255);
    for(int b = 0 ; b < BULBS_NUMBER ; b ++){
        for(int i = bulb[b].r ; i >= bulb[b].r - 5 ; i--){//thicken the border of the bulb
            circle(r, bulb[b].x, bulb[b].y, i, 1);
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
        (bulbs[i].x) -= OBSTACLE_SPEED + 0.003*tc; //move the bulbs



   /*for(int i = 0 ; i < BULBS_NUMBER ; i++){
        printf("%d     x : %f ; y : %f ; r = %f\n", i, bulbs[i].x , bulbs[i].y, bulbs[i].r);
    }*/

}

void superJumpCircle(SDL_Renderer* r, int n, Color*c, int p){
    //origin at x = 50 y = 150
    color(r, c[4*p+1].r, c[4*p+1].g, c[4*p+1].b, 255);//circle color
    circle(r, 50, 50, 20, 1);

    if(!n)
        return;//beautify the circle if it's full
    

    //angle calcul :
    double a = (SUPER_JUMP_TIMER - n)*2*3.1415/SUPER_JUMP_TIMER - 3.1415*0.5;

    color(r, c[4*p+3].r, c[4*p+3].g, c[4*p+3].b, 255);//background color

    if(a < 0){//4 cases
        rect(r, 30, 30, 20, 40, 1);
        rect(r, 50, 50, 20, 20, 1);
        triangle(r, 50, 50, 100, 50, 50 + 25*cos(a), 50 +25*sin(a), 1);
    }else if(a < 3.1415*0.5){
        rect(r, 30, 30, 20, 40, 1);
        triangle(r, 50, 50, 50, 100, 50 + 25*cos(a), 50 +25*sin(a), 1);
    }else if(a < 3.1415){
        rect(r, 30, 30, 20, 20, 1);
        triangle(r, 50, 50, 0, 50, 50 + 25*cos(a), 50 +25*sin(a), 1);
    }else
        triangle(r, 50, 50, 50, 0, 50 + 25*cos(a), 50 +25*sin(a), 1);
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
    color(r, c[4*p].r, c[4*p].g, c[4*p].b, 255);
    rect(r, WIDTH/2 - BUTTON_WIDTH/2, HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT, 1);
    color(r, 20, 20, 20, 255);
    rect(r, WIDTH/2 - BUTTON_WIDTH/2, HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT, 0);

    color(r, c[4*p+1].r, c[4*p+1].g, c[4*p+1].b, 255);

    triangle(r  , WIDTH/2 -  BUTTON_WIDTH/2 +  BUTTON_WIDTH/5
                , HEIGHT/2 - BUTTON_HEIGHT/2 + BUTTON_HEIGHT/5
                , WIDTH/2 -  BUTTON_WIDTH/2 +  BUTTON_WIDTH/5
                , HEIGHT/2 - BUTTON_HEIGHT/2 + 4*BUTTON_HEIGHT/5
                , WIDTH/2 -  BUTTON_WIDTH/2 +  4*BUTTON_WIDTH/5
                , HEIGHT/2
                , 1);

}

void restartGame(bulb* b, double *x, double *y, double *vy, double *ay, int *tc, int*p){
    srand(time(0));
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

void background(SDL_Renderer* r, Color*c, int p){
    color(r, c[4*p+3].r, c[4*p+3].g, c[4*p+3].b, 255);
    rect(r, 0, 0, WIDTH, HEIGHT, 1);
}
