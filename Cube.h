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