#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


//definitions
#define H 600
#define W 800

static SDL_Surface* surface = NULL;

//typedefs
struct rgbcolor_struct
{
    int r;
    int g;
    int b;
};
typedef struct rgbcolor_struct rgbcolor;

struct vector_struct
{
    double x;
    double y;
    double z;
};
typedef struct vector_struct vector;

struct line_struct
{
    vector point;
    vector direction;
};
typedef struct line_struct line;

struct plane_struct
{
    vector   pointA;
    vector   pointB;
    vector   pointC;
    vector   pointD;
    vector   normal;
    rgbcolor color;  
};
typedef struct plane_struct plane;


//geo funcs
vector crossProduct(vector va, vector vb)
{
    vector rv = {va.y * vb.z - va.z * vb.y, -1*(va.x * vb.z - va.z * vb.x), va.x * vb.y - va.y * vb.x };
    return rv;
}

double dotProduct(vector va, vector vb)
{
    double dot = va.x * vb.x + va.y * vb.y + va.z * vb.z;
    return dot;
}

vector subtractVector(vector va, vector vb)
{
    vector rv = {va.x - vb.x, va.y - vb.y, va.z - vb.z};
    return rv;
}

void setVector(plane *p)
{
    vector a = subtractVector(p->pointA, p->pointB);
    vector b = subtractVector(p->pointC, p->pointB);
    p->normal = crossProduct(a, b);
}

vector addVector(vector va, vector vb)
{
    vector rv = {va.x + vb.x, va.y + vb.y, va.z + vb.z};
    return rv;
}

vector scaleVector(vector *v, double scalar)
{
    v->x = scalar*(v->x);
    v->y = scalar*(v->y);
    v->z = scalar*(v->z);
}

vector rScaleVector(vector v, double scalar)
{
    vector rv = {v.x * scalar, v.y * scalar, v.z * scalar};
    return rv;
}

vector getIntersection(plane pln, line ln)
{
    vector returnvector = {-1,-1,-1};
    double d;
    if ( dotProduct(ln.direction, pln.normal) == 0 )
    {
        return returnvector; //lines are parallel, return default vector
    }
    d = ( dotProduct(subtractVector(pln.pointA, ln.point), pln.normal )) / ( dotProduct(ln.direction, pln.normal) );
    //https://en.wikipedia.org/wiki/Line-plane_intersection#Algebraic_form
    returnvector = addVector( rScaleVector(ln.direction, d), ln.point );
    return returnvector;
}

//file i/o funcs
void readGeometry()
{
    int ch, line_count = 0;
    FILE *f = fopen("map.mp", "r");
    
    while(!feof(f))
    {
        ch = fgetc(f);
        if(ch == '\n')
        {
            line_count++;
        }
    }
    plane planeList[line_count];
    
}

//disp funcs
int placePoint(int px, int py, rgbcolor color)
{
    if (px < 0 || py < 0 || px > W-1 || py > H-1) return -1;
    int *pix = (int*) surface->pixels;
    int clr = pow(16,4)*color.r + pow(16,2)*color.g + color.b;
    pix[py*W+px] = clr;
    
    return 0;
}

int drawLine(int p1x, int p1y, int p2x, int p2y, rgbcolor color)
{
    if ( p1x < 0 || p1y < 0 || p2x < 0 || p2y < 0 ) return -1;
    if ( p1x > W-1 || p2x > W-1 || p1y > H-1 || p2y > H-1 ) return -1;
    
    int d = 1;
    int s = p1x;
    int e = p2x;
    if ( p1x > p2x ) { d = -1; s = p2x; e = p1x; }
    
    if ( p1x == p2x )
    {
        int h = abs(p1y-p2y);
        if ( p1y > p2y )
        {
            for ( h = p2y; h <= p1y; h++)
            {
                placePoint(p1x, h, color);
            }
        }
        else
        {
            for ( h = p1y; h <= p2y; h++)
            {
                placePoint(p1x, h, color);
            }
        }
    }
    else
    {
        int w;
        
        double dp1x = (double)p1x;
        double dp2x = (double)p2x;
        double dp1y = (double)p1y;
        double dp2y = (double)p2y;
        double m = (dp1y-dp2y)/(dp1x-dp2x);
        double b = (double)(p1y - (m*p1x));
        for ( w = s; w <= e; w+1)
        {
            int y  = (int)(w*m + b);
            int ny = (int)((++w)*m + b);
            int ns = y;
            int ne = ny;
            if (ny <= y) { ns = ny; ne = y; }
            int h;
            for(h = ns; h <= ne; h++)
            {
                placePoint(w, h, color);
            }
        }
    }
}

void drawScreen()
{
    rgbcolor c = {255,255,255};
    drawLine(0,0,100,400,c);
}

//main func
int main()
{
    surface = SDL_SetVideoMode(W, H, 32, 0);

    SDL_EnableKeyRepeat(150, 30);
    SDL_ShowCursor(SDL_DISABLE);

    int wsad[4]={0,0,0,0}, ground=0, falling=1, moving=0, ducking=0;
    for(;;)
    {
        SDL_LockSurface(surface);
        drawScreen();
        SDL_UnlockSurface(surface);
        SDL_Flip(surface);


        SDL_Event ev;
        while(SDL_PollEvent(&ev))
        {
            switch(ev.type)
            {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    switch(ev.key.keysym.sym)
                    {
                        case 'q': goto done;
                        default: break;
                    }
                    break;
                case SDL_QUIT: goto done;
            }
        }
        SDL_Delay(10);
    }
done:
    SDL_Quit();
    return 0;
}