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

