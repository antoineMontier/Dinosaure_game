#include "SDL_Basics.h"



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