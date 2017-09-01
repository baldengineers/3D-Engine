#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

void setFaces(model_t *m, face_t *faces, int facec);

int main(int argc, char *argv[])
{
    model_t box;
    face_t faces[6] = {
                    (face_t)
                    {
                        (vector){0, 0, 0},
                        (vector){0, 1, 0},
                        (vector){1, 1, 0}
                    },
                    (face_t)
                    {
                        (vector){0, 0, 0},
                        (vector){1, 0, 0},
                        (vector){1, 0, 1}
                    },
                    (face_t)
                    {
                        (vector){0, 0, 1},
                        (vector){0, 1, 1},
                        (vector){1, 1, 1}
                    },
                    (face_t)
                    {
                        (vector){0, 1, 1},
                        (vector){0, 1, 0},
                        (vector){1, 1, 0}
                    },
                    (face_t)
                    {
                        (vector){0, 0, 0},
                        (vector){0, 1, 0},
                        (vector){0, 1, 1}
                    },
                    (face_t)
                    {
                        (vector){1, 0, 0},
                        (vector){1, 1, 0},
                        (vector){1, 1, 1}
                    }
                };
    setFaces(&box, faces, 6);
    printf("done");
    return 0;
}

void setFaces(model_t *m, face_t *faces, int facec)
{
    m->facec = facec;
    m->faces = malloc(m->facec * sizeof(m)); //dynamically assign array size to box.facec
    memcpy(m->faces, faces, sizeof(m->faces));
}
