#ifndef __BOILERPLATE_H__	//guard against cyclic dependancy
#define __BOILERPLATE_H__

typedef struct
{
    float x;
    float y;
    float z;
    float speed;

    float radius;
    float slices;
    float stacks;

} Asteroid;

typedef struct
{
    float x;
    float y;
    float z;
} Vec;
#endif

/*typedef struct
{
    float centre[3];

    float north[3];

    float slice1a[3];
    float slice1b[3];
    float slice1c[3];
    float slice1d[3];

    float slice2a[3];
    float slice2b[3];
    float slice2c[3];
    float slice2d[3];

    float slice3a[3];
    float slice3b[3];
    float slice3c[3];
    float slice3d[3];

    float south[3];

    float radius;

    float xmov;
    float ymov;
    float zmov;

} Asteroid;*/

