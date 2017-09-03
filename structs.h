//structures
//typedefs

#ifndef _STRUCTS_H
#define _STRUCTS_H

typedef struct rgbcolor_struct
{
    int r;
    int g;
    int b;
} rgbcolor;

typedef struct vector_struct
{
    double x;
    double y;
    double z;
} vector;

typedef struct line_struct
{
    vector point;
    vector direction;
} line;

typedef struct plane_struct
{
    vector   pointA;
    vector   pointB;
    vector   pointC;
    vector   pointD;
    vector   normal;
    rgbcolor color;
} plane;

typedef struct line_segment_struct
{
    vector   pointA;
    vector   pointB;
    rgbcolor color;
} line_segment;

typedef struct face
{
    vector vertices[3];
    rgbcolor color;
} face_t;

typedef struct model
{
    int facec; //face count
    face_t *faces;
} model_t;

#endif
