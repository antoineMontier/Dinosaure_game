#include "Cube.h"


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
