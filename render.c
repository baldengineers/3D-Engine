#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
//#include "image.c"
#include "model.c"

//definitions
#define H 600
#define W 800
#define X_CALC 0
#define Y_CALC 1
#define X 0
#define Y 1
#define Z 2
#define HFOV 90
#define VFOV 45
#define PI 3.1415926
#define MVCNT 10
#define TRI_AMNT 4
#define MAXFLOAT 3.402823e+38


static SDL_Surface *surface = NULL;
vector playerPos = {0,0,0};
vector playerViewVect = {1,0,0};
double viewAngleH = 0;
double viewAngleV = 0;
float depthBuffer[W][H];
face_t triangles[TRI_AMNT]; //in the future the length will be defined by a certain input (likely reading a file)
vector faceVertices[TRI_AMNT][3]; //contains all the vertices on the 2D screen that are projected from the faces. 3 because that's the # of verts in a triangle

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

int pointInTriangle(vector s, vector a, vector b, vector c)
{
    //from stack overflow
    int as_x = s.x - a.x;
    int as_y = s.y - a.y;

    int s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;

    if ((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0 == s_ab) {return 0;}

    if ((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0 != s_ab) {return 0;}

    return 1;
}

void setFaces(model_t *m, face_t *faces, int facec)
{
	//create a model with the given faces
    m->facec = facec;
    m->faces = malloc(m->facec * sizeof(m)); //dynamically assign array size to box.facec
    memcpy(m->faces, faces, sizeof(m->faces));
}

void setNormal(plane *p)
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

void normalize(vector *v)
{
    double largest = v->x;
    if (v->y > v->x)
    {
        largest = v->y;
    }
    if ((v->z > v->y) && (v->z > v->x))
    {
        largest = v->z;
    }


    v->x = (v->x)/largest;
    v->y = (v->y)/largest;
    v->z = (v->z)/largest;
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

double edgeFunction(vector a, vector b, vector c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x); 
}

//disp funcs
void printVector(vector v)
{
    printf("x: %f, y: %f, z:%f\n", v.x, v.y, v.z);
}

int placePoint(int px, int py, rgbcolor color)
{
    if (px < 0 || py < 0 || px > W-1 || py > H-1) return -1;
    int *pix = (int*) surface->pixels;
    int clr = pow(16,4)*color.r + pow(16,2)*color.g + color.b;
    pix[py*W+px] = clr;

    return 0;
}

void rotate(float angle)
{
    //angle is in radians
    int i,j;
    for (i = 0; i < TRI_AMNT; i++)
    {
        for (j = 0; j < 3; j++)
        {
            vector tp = triangles[i].vertices[j];
            tp = (vector){tp.x*cos(angle) - tp.y*sin(angle), tp.x*sin(angle) + tp.y*cos(angle), tp.z};
            triangles[i].vertices[j] = tp;
        }
    }
}

void moveX(float distance)
{
    int i,j;
    for (i = 0; i < TRI_AMNT; i++)
    {
        for (j = 0; j < 3; j++)
        {
            vector tp = triangles[i].vertices[j];
            tp = (vector){tp.x - distance, tp.y, tp.z};
            triangles[i].vertices[j] = tp;
        }
    }
}

void moveY(float distance)
{
    int i,j;
    for (i = 0; i < TRI_AMNT; i++)
    {
        for (j = 0; j < 3; j++)
        {
            vector tp = triangles[i].vertices[j];
            tp = (vector){tp.x, tp.y - distance, tp.z};
            triangles[i].vertices[j] = tp;
        }
    }
}

/*
bool withinTriangle(int w, int h, vector &vertices[3])
{
    vertices[0].x;
}
*/

double calcLine(double p1x, double p1y, double p2x, double p2y, int type, int input)
{
    //Calculates a point on a line from (p1x, p1y) to (p2x, p2y),
    //using type as what value (X_CALC or Y_CALC) you're trying to find
    //and input for your input value
    double m = (p1y - p2y) / (p1x - p2x);
    double b = p1y - m * p1x;

    //printf("%f - %f / %f - %f\n", p1y, p2y, p1x, p2x);
    //printf("y = mx + b\ny = %f * %d + %f\n", m, input, b);

    if (type == X_CALC)
    {
        //y is given, trying to find x
        //x = (y-b)/m
        return (input - b) / m;
    }
    else if (type == Y_CALC)
    {
        //x is given, trying to find y
        //y = mx + b
        return m * input + b;
    }

}

int drawLine(int p1x, int p1y, int p2x, int p2y, rgbcolor color)
{
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
            if ( w != e )
            {
                for(h = ns; h <= ne; h++)
                {
                    placePoint(w, h, color);
                }
            }
            else
            {
                placePoint(w,ns,color);
            }
        }
    }
}

void drawImage(int sx, int sy, short int bin_data[])
{
    //int sx and int sy are offset values from origin
    //bin_data is an array of all the pixels in an image (1D)
    int i,x,y;
    i = 0;
    for( y = 0; y < H; y++)
    {
        for (x = 0; x < W; x++)
        {
            rgbcolor cc;
            cc.r = bin_data[i];
            cc.g = bin_data[i+1];
            cc.b = bin_data[i+2];
//            printf("color: {%d,%d,%d}\n",cc.r,cc.g,cc.b);
            placePoint(sx+x, sy+y, cc);
            i += 3;
//            sleep(1);
        }
    }
}

int renderLine(line_segment ln)
{
    int l1x, l2x, l1y, l2y;
    vector pointDirectionA = subtractVector(ln.pointA, playerPos);
    vector pointDirectionB = subtractVector(ln.pointB, playerPos);

    normalize(&pointDirectionA);
    normalize(&pointDirectionB);

     if ( (abs(viewAngleH - acos(pointDirectionB.x)*(180/PI)) < HFOV) || (abs( (360-viewAngleH) - acos(pointDirectionB.x)*(180/PI)) < HFOV) )
    {
        double angleOffHa = acos(pointDirectionA.y) - acos(playerViewVect.y);
        double angleOffVa = acos(pointDirectionA.z) - acos(playerViewVect.z);
        double angleOffHb = acos(pointDirectionB.y) - acos(playerViewVect.y);
        double angleOffVb = acos(pointDirectionB.z) - acos(playerViewVect.z);

        l1x = (int)floor(((W/2)+(((angleOffHa*(180/PI))/(HFOV/2))*(W/2))));
        l1y = (int)floor(((H/2)+(((angleOffVa*(180/PI))/(VFOV/2))*(H/2))));
        l2x = (int)floor(((W/2)+(((angleOffHb*(180/PI))/(HFOV/2))*(W/2))));
        l2y = (int)floor(((H/2)+(((angleOffVb*(180/PI))/(VFOV/2))*(H/2))));

        drawLine(l1x, l1y, l2x, l2y, ln.color);
    }
}


void drawScreen()
{
    //reset screen
    rgbcolor c = {0,0,0};
    int i,j;
    for(i = 0; i < W; i++)
    {
        for(j = 0; j < H; j++)
        {
            placePoint(i,j,c);
        }
    }
    line_segment ln1 = { {15,2,3}, {20,-2,2}, {255,255,255} };
    line_segment ln2 = { {15,2,3}, {15,2,0}, {255,255,255} };
    line_segment ln3 = { {15,2,0}, {20,-2,0}, {255,255,255} };
    line_segment ln4 = { {20,-2,2}, {20,-2,0}, {255,255,255} };
    renderLine(ln1);
    renderLine(ln2);
    renderLine(ln3);
    renderLine(ln4);
    printVector(playerViewVect);
}

plane getCameraPlaneCoords()
{
    //Return the plane used for intersections sent from triangles (the cameraPlane)
    vector distFromPlayer; //Assume camera is facing in positive x direction
    distFromPlayer.x = W / 2 / tan(HFOV/2);
    distFromPlayer.y = W / 2;
    distFromPlayer.z = H / 2;

    //Set the points of the plane - top-left, top-right, bottom-right, bottom-left
    vector pointA = {playerPos.x + distFromPlayer.x, playerPos.y - distFromPlayer.y, playerPos.z + distFromPlayer.z};
    vector pointB = {playerPos.x + distFromPlayer.x, playerPos.y + distFromPlayer.y, playerPos.z + distFromPlayer.z};
    vector pointC = {playerPos.x + distFromPlayer.x, playerPos.y + distFromPlayer.y, playerPos.z - distFromPlayer.z};
    vector pointD = {playerPos.x + distFromPlayer.x, playerPos.y - distFromPlayer.y, playerPos.z - distFromPlayer.z};

    //Make the plane object and set its points/normal
    plane cameraPlane;
    cameraPlane.pointA = pointA;
    cameraPlane.pointB = pointB;
    cameraPlane.pointC = pointC;
    cameraPlane.pointD = pointD;
    /*
    printVector(cameraPlane.pointA);
    printVector(cameraPlane.pointB);
    printVector(cameraPlane.pointC);
    printVector(cameraPlane.pointD);
    */
    setNormal(&cameraPlane);
    return cameraPlane;
}

void resetDepthBuffer(void)
{
    int i,j;
    for (i = 0; i < W; i++)
        for(j = 0; j < H; j++)
        {
            depthBuffer[i][j] = 0;
        }
}

void drawScreen2()
{

    rgbcolor c = {0,0,0};
    int i,j;
    for(i = 0; i < W; i++)
    {
        for(j = 0; j < H; j++)
        {
            placePoint(i,j,c);
        }
    }
    resetDepthBuffer();
    //drawImage(100,100,grad_data);
    plane cameraPlane = getCameraPlaneCoords();

    int w, h;
    for (j = 0; j < TRI_AMNT; j++)
    {
        for (i = 0; i < 3; i++)
        {
            //Get the intersection point of a ray casted from a vertex to the playerPos with the camera plane
            vector direction = subtractVector(playerPos, triangles[j].vertices[i]);
            faceVertices[j][i] = getIntersection(cameraPlane, (line){triangles[j].vertices[i], direction});
            double ty = faceVertices[j][i].y;
            double tz = faceVertices[j][i].z;
            faceVertices[j][i].x = -1*ty + W/2;
            faceVertices[j][i].y = -1*tz + H/2;
            faceVertices[j][i].z = triangles[j].vertices[i].x; //simply the depth of the point in 3d space
        }
    }


    for (i = 0; i < TRI_AMNT; i++)
    {
        int xMax = 0;
        int yMax = 0;
        int xMin = W;
        int yMin = H;
        
        
        for (j = 0; j < 3; j++)
        {
            double x = faceVertices[i][j].x;
            double y = faceVertices[i][j].y;
            if (x > xMax)
                xMax = x;
            if (y > yMax)
                yMax = y;
            if (x < xMin)
                xMin = x;
            if (y < yMin)
                yMin = y;
        }
        yMin = yMin;
	yMax = yMax;
	xMin = xMin;
	xMax = xMax;
        int yMinF = yMin; //"Fixed" yMin
        int yMaxF = yMax; //"Fixed" yMax
        if (yMinF < 0)  
			yMinF = 0;
        if (yMaxF >= H) 
			yMaxF = H-1;
        if (xMin < 0)  	
			xMin = 0;
        if (xMax >= W) 	
			xMax = W-1;
	
	vector a;
        a.x = faceVertices[i][0].x;
        a.y = faceVertices[i][0].y;
        a.z = faceVertices[i][0].z;
        
        vector b;
        b.x = faceVertices[i][1].x;
        b.y = faceVertices[i][1].y;
        b.z = faceVertices[i][1].z;

        vector c;
        c.x = faceVertices[i][2].x;
        c.y = faceVertices[i][2].y;
        c.z = faceVertices[i][2].z;
                    		
        float area = edgeFunction(a,b,c);			
			
        //printf("xMax: %d, yMax: %d, xMin: %d, yMin: %d\n", xMax, yMaxF, xMin, yMinF);
        for ( w = xMin; w <= xMax; w++) //replace with yMinF, yMaxF when working
        {
            for (h = yMinF; h <= yMaxF; h++) //replace with xMin, xMax when working
            {
                //printf("h = %d, w = %d\n", h, w);
                if (h >= 0 && h < H && w >= 0 && w < W)
                {
                    vector pixel;
					pixel.x = w;
					pixel.y = h;
					pixel.z = 0;

                    float w0 = edgeFunction(b, c, pixel);
                    float w1 = edgeFunction(c, a, pixel);
                    float w2 = edgeFunction(a, b, pixel);
                    
                    if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                    {
                        w0 /= area;
                        w1 /= area;
                        w2 /= area;
                        
                        double pointDepth = (1 / ( a.z * w0 + b.z * w1 + c.z * w2))*100;
                        
                            if (pointDepth > depthBuffer[w][h] && pointDepth > 0) 
                            {
                                placePoint(w, h, triangles[i].color);
                                depthBuffer[w][h] = pointDepth;
                            }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    #ifdef _WIN32
    freopen("CON", "w", stdout); // redirects stdout
    freopen("CON", "w", stderr); // redirects stderr
    #endif
    surface = SDL_SetVideoMode(W, H, 32, 0);

    SDL_EnableKeyRepeat(150, 30);
    SDL_ShowCursor(SDL_DISABLE);

/*
    triangles[1].vertices[0] = (vector){300, -100, 200};
    triangles[1].vertices[1] = (vector){300, -50, 300};
    triangles[1].vertices[2] = (vector){300, 150, -200};
    triangles[1].color       = (rgbcolor){0,   0, 255};
    triangles[0].vertices[0] = (vector){250, -100, 200};
    triangles[0].vertices[1] = (vector){250, -50, 300};
    triangles[0].vertices[2] = (vector){250, 150, -200};
    triangles[0].color       = (rgbcolor){255,255,255};
    triangles[2].vertices[0] = (vector){200, -100, 200};
    triangles[2].vertices[1] = (vector){200, -50, 300};
    triangles[2].vertices[2] = (vector){200, 150, -200};
    triangles[2].color       = (rgbcolor){255,  0,  0};
*/
    int i;
    for ( i = 0; i < 4; i++)
    {
        triangles[i] = model[i];
    }


    for(;;)
    {
        SDL_LockSurface(surface);
        drawScreen2();
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
                        case 'e':
                            rotate(PI/180);
                            break;
                        case 'r':
                            rotate(-PI/180);
                            break;
                        case 'a':
                            moveY(10);
                            break;
                        case 'd':
                            moveY(-10);
                            break;
                        case 'w':
                            moveX(10);
                            break;
                        case 's':
                            moveX(-10);
                            break;
                        default: break;
                    }
                    break;
                case SDL_QUIT: goto done;
            }
        }
//        SDL_Delay(10);
    }
done:
    SDL_Quit();
    return 0;
}

//main func
/*int main(int argc, char *argv[])
{
    surface = SDL_SetVideoMode(W, H, 32, 0);

    SDL_EnableKeyRepeat(150, 30);
    SDL_ShowCursor(SDL_DISABLE);

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
                        case 'a':
                            viewAngleH += 1;
                            playerViewVect.x = cos(viewAngleH*(PI/180));
                            playerViewVect.y = sin(viewAngleH*(PI/180));
                            break;
                        case 'd':
                            viewAngleH -= 1;
                            playerViewVect.x = cos(viewAngleH*(PI/180));
                            playerViewVect.y = sin(viewAngleH*(PI/180));
                            break;
                        case 'w':
                            playerPos.x += MVCNT*cos(playerViewVect.x);
                            playerPos.y += MVCNT*sin(playerViewVect.y);
                            break;
                        case 's':
                            playerPos.x -= MVCNT*playerViewVect.x;
                            playerPos.y -= MVCNT*playerViewVect.y;
                            break;
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
}*/
