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
    int clr = pow(16,2)*color.r + 16*color.g + color.b;
    
    pix[py*W+px] = clr;
    
    return 0;
}


//main func
int main()
{
    vector pa = {2.0,3.0,0.0}; vector pb = {6.0f,3.0f,0.0f}; vector pc = {6,3,6}; vector pd = {2,3,6};
    vector nm = {0,0,0}; rgbcolor cl = {255,255,255};
    plane mainpln = { pa, pb, pc, pd, nm, cl };
    line  mainln  = { {0,0,0}, {3,4,2} };
    setVector(&mainpln);
    vector intersection = getIntersection(mainpln, mainln);
    printf("x: %f, y: %f, z: %f\n", intersection.x, intersection.y, intersection.z);
    
    
    return 0;
}