#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vertex
{
    int x;
    int y;
    int z;
} vertex_t;

typedef struct face
{
    vertex_t vertices[3];
} face_t;

typedef struct polygon
{
    int facec; //face count
    face_t *faces;
} polygon_t;

void set_faces(polygon_t *p, face_t *faces, int facec);

int main(int argc, char *argv[])
{
    polygon_t box;
    face_t faces[6] = {
                    (face_t)
                    {
                        (vertex_t){0, 0, 0},
                        (vertex_t){0, 1, 0},
                        (vertex_t){1, 1, 0}
                    },
                    (face_t)
                    {
                        (vertex_t){0, 0, 0},
                        (vertex_t){1, 0, 0},
                        (vertex_t){1, 0, 1}
                    },
                    (face_t)
                    {
                        (vertex_t){0, 0, 1},
                        (vertex_t){0, 1, 1},
                        (vertex_t){1, 1, 1}
                    },
                    (face_t)
                    {
                        (vertex_t){0, 1, 1},
                        (vertex_t){0, 1, 0},
                        (vertex_t){1, 1, 0}
                    },
                    (face_t)
                    {
                        (vertex_t){0, 0, 0},
                        (vertex_t){0, 1, 0},
                        (vertex_t){0, 1, 1}
                    },
                    (face_t)
                    {
                        (vertex_t){1, 0, 0},
                        (vertex_t){1, 1, 0},
                        (vertex_t){1, 1, 1}
                    }
                };
    set_faces(&box, faces, 6);
    printf("done");
    return 0;
}

void set_faces(polygon_t *p, face_t *faces, int facec)
{
    p->facec = facec;
    p->faces = malloc(p->facec * sizeof(p)); //dynamically assign array size to box.facec
    memcpy(p->faces, faces, sizeof(p->faces));
}
