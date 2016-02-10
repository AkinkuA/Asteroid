#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include "asteroid.h"


std::vector <Asteroid> asteroids;//Vector object for asteroids, dynamically allocates
std::vector <Vec> stars; //vector object for stars, dynamically allocates

Vec explosion[10]; //Contains the explotion vector

GLUquadricObj *quad; //Quadric object

float camPos[] = {15, 0, 0}; // Camera Position
float lookat[] = {0, 0, 0}; // Camera lookat coordinates
float shipPos[] = {2,2,0}; // Ship position
int cnt = 0;
float shipRot[] = {0, 0, 0}; //Ship rotation
float missilePos[]= {shipPos[0],shipPos[1],shipPos[2]}; //missle position


float light0pos[4] = {0, -20, 20, 0}; //Light position
float light0amb[4] = {0.25, 0.25, 0.25, 0}; // ambient light intensity
float light0diff[4] = {10, 10, 10, 0}; // Diffuse light intensity
float light0spec[4] = {1, 1, 1, 0};// specula light intesnsity

/*float light1pos[4] = {0, 0, 0, 0};
float light1amb[4] = {0.25, 0.25, 0, 0};
float light1diff[4] = {10, 10, 0, 0};
float light1spec[4] = {100, 100, 0, 0};*/

GLubyte *asteroidtex2; //asteriud texture
GLuint textures[2]; // holds texture
int width = 0;// width of texture
int height = 0;//height of texture

int speed = 0;

GLubyte *ship; //bitmap for HUD for ship lives
int hudwidth; // HUD width
int hudheight; // HUD height
int lives = 3; // number of lives player has

float genRand (float Min, float Max) // generates random points
{
    return ((static_cast <float>(rand()) / static_cast <float>(RAND_MAX)) * (Max - Min)) + Min;
}

// draws the ship using heircal matric transform
void drawShip()
{
    glPushMatrix();

    glRotatef(shipRot[1], 0, 1, 0);
    glTranslatef(shipPos[0], shipPos[1], shipPos[2]);

    glColor3f(1,0,0);
    glutSolidSphere(0.35, 20, 20);

    glPushMatrix();
    glTranslatef(0.5, 0, 0);
    quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, 0.5, 3, 3);
    glPushMatrix();
    glTranslatef(0.15, 0, -0.75);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, 0.35, 3, 3);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.15, 0, 0.75);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, 0.35, 3, 3);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}
//function for drawing geometric texts
void drawStrokeText(char* text, int x, int y)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(x, y, 0);
    glScalef(0.25, 0.25, 0.25);

    char* c;  //character iterator (ptr)
    for(c = text; *c != '\0'; c++) //stop when we hit null character
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c); //print one char
    }
}

//Generates the explosion x, y and z coordinates
void genExplosion (float x, float y, float z)
{
    for (int i = 0; i < 10; i++)
    {
        explosion[i].x = x; //explosion x coordinates
        explosion[i].y = y;//explosion y coordinates
        explosion[i].z = z;//explosion z coordinates

    }
}

//draws explosion unto screen
void drawExplosion ()
{
    for (int i = 0; i < 10; i++)
    {
        glPushMatrix();
        glTranslatef(explosion[i].x+genRand(-1, 1), explosion[i].y+genRand(-1, 1), explosion[i].z+genRand(-1, 1));
        glutSolidSphere(0.5, 4, 4); // draws explsion particles
        glPopMatrix();
    }
}

//detects if ship has collided with asteroid and respawns it if it has
void shipCollision()
{

    for (std::vector<Asteroid>::iterator it = asteroids.begin(); it != asteroids.end(); ++it)
    {
        if (shipPos[0] >= (it->x - it->radius) && shipPos[0] <= (it->x + it->radius))
        {

            if (shipPos[1] <= (it->y + it->radius) && shipPos[1] >= (it->y - it->radius))
            {
                if (shipPos[2] <= (it->z + it->radius) && shipPos[2] >= (it->z - it->radius))
                {
                    std::cout << '\a';
                    genExplosion(shipPos[0], shipPos[1], shipPos[2]);
                    lives--;
                    shipPos[0] = 2;
                    shipPos[1] = 2;
                    shipPos[2] = 0;
                }
            }
        }
    }
}

//drwas the missle from the ship
void drawMissile()
{

    glPushMatrix();

    glTranslatef(missilePos[0], missilePos[1], missilePos[2]);

    glColor3f(1,0,0);
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();
}

//moves the missle forward, detects if it has hit an asteroid and calls for it to explode if it has
void shootMissile()
{
    cnt++;
    if(cnt%1 == 0)
    {
        missilePos[0]--;
    }

    for (std::vector<Asteroid>::iterator it = asteroids.begin(); it != asteroids.end(); ++it)
    {

        if (missilePos[0] >= (it->x - it->radius) && missilePos[0] <= (it->x + it->radius))
        {

            if (missilePos[1] <= (it->y + it->radius) && missilePos[1] >= (it->y - it->radius))
            {
                if (missilePos[2] <= (it->z + it->radius) && missilePos[2] >= (it->z - it->radius))
                {
                    //explosion
                    genExplosion(it->x, it->y, it->z);

                    it->x = 200;
                }
            }
        }
    }
    glutPostRedisplay();
}

/* LoadPPM -- loads the specified ppm file, and returns the image data as a GLubyte
 *  (unsigned byte) array. Also returns the width and height of the image, and the
 *  maximum colour value by way of arguments
 */
GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;

    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        //printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    // printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);

    //printf("%d rows  %d columns  max value= %d\n",n,m,k);

    nm = n*m;

    img = static_cast<GLubyte *>(malloc(3*sizeof(GLuint)*nm));



    s=255.0/k;


    for(i=0; i<nm; i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }

    *width = n;
    *height = m;
    *max = k;

    return img;
}

//generates stars in background
void genStars()
{

    Vec temp;
    for (int i = 0; i < 100; i++)
    {
        {
            temp.x = -60;
            temp.y = genRand(-30, 30);
            temp.z = genRand(-30, 30);
            stars.push_back(temp);
        }
    }
}


//draws the star unto the screen
void drawStars()
{

    for (std::vector<Vec>::iterator it = stars.begin(); it != stars.end(); ++it)
    {
        glColor3f(1,1,1);
        glPushMatrix();
        glTranslatef(it->x, it->y, it->z);
        glutSolidSphere(0.1, 5, 5);
        glPopMatrix();
    }
}

//draws the light source
void drawSun(float x, float y, float z)
{
    glColor3f(1,1,0);
    glPushMatrix();
    glTranslatef(light0pos[1], light0pos[2], light0pos[3]);
    glutSolidSphere(.5, 20, 20);
    glPopMatrix();
}

void addAsteroids(int num)
{
    Asteroid temp;
    for (int i = 0; i <= num; i++)
    {
        temp.x=genRand(-200, -90);
        temp.y=genRand(-10, 10);
        temp.z=genRand(-10, 10);
        temp.speed=genRand(0.05, 0.2);
        temp.radius=genRand(1, 3);
        temp.slices=genRand(5, 10);
        temp.stacks=genRand(5, 10);
        asteroids.push_back(temp);
    }
}

//generates asterods
void genAsteroid(float radius, float slices, float stacks)
{
    glColor3f(0, 1, 0);
    quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluSphere(quad, radius, slices, stacks);
}

//draws asteroids unto screen
void drawAsteroid()
{
    glColor3f(1, 1, 1);
    for (std::vector<Asteroid>::iterator it = asteroids.begin(); it != asteroids.end(); ++it)
    {
        glPushMatrix();
        glTranslatef(it->x, it->y, it->z);
        genAsteroid(it->radius, it->slices, it->stacks);
        glPopMatrix();
        if (it->x > camPos[0] && lives >= 1)
        {
            it->x = -200;
        }
    }
}

//moves the asteroid
void moveAsteroid()
{
    speed++;
    for (std::vector<Asteroid>::iterator it = asteroids.begin(); it != asteroids.end(); ++it)
    {
        if(speed % 5 == 0)
        {
            it->x = it->x + it->speed;
        }
        glutPostRedisplay();
    }
}

//converts to 3D imaging, initilies lighting parameters and texture parameters
void init(void)
{
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);//backface culling
    glEnable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);//Gouraud shading
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0amb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0spec);

    /*glEnable(GL_LIGHT1);
    //glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 15.f);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1diff);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1amb);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1spec);*/

    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, asteroidtex2);
    //create a texture using the "asteroidtex2" array

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    //set texture parameters

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



    gluPerspective(45, 1, 1, 100);
    addAsteroids(50);
    genStars();

    for (int i = 0; i < 10; i++)
    {
        explosion[i].x = 50;
    }

    glutPostRedisplay();
}

//draws the HUD display unto screen
void drawHUD ()
{

    if (lives == 1)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(hudwidth, 0);
        glPixelZoom(-1, 1);
        glDrawPixels(hudwidth,hudheight,GL_RGB, GL_UNSIGNED_BYTE, ship);
    }
    else if (lives == 2)
    {

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(hudwidth, hudheight);
        glPixelZoom(-1, 1);
        glDrawPixels(hudwidth,hudheight,GL_RGB, GL_UNSIGNED_BYTE, ship);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(hudwidth,0);
        glPixelZoom(-1, 1);
        glDrawPixels(hudwidth,hudheight,GL_RGB, GL_UNSIGNED_BYTE, ship);
    }
    else if(lives == 3)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(hudwidth, hudheight*2);
        glPixelZoom(-1, 1);
        glDrawPixels(hudwidth,hudheight,GL_RGB, GL_UNSIGNED_BYTE, ship);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(hudwidth, hudheight);
        glPixelZoom(-1, 1);
        glDrawPixels(hudwidth,hudheight,GL_RGB, GL_UNSIGNED_BYTE, ship);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 800, 0, 600);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRasterPos2i(hudwidth,0);
        glPixelZoom(-1, 1);
        glDrawPixels(hudwidth,hudheight,GL_RGB, GL_UNSIGNED_BYTE, ship);
    }
}

//Calls all drawn items to be be displayed
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float m_amb[] = {0.745, 0.745, 0.745, 1.0};
    float m_dif[] = {1, 1, 1, 1.0};
    float m_spec[] = {1, 1, 1, 1.0};
    float shiny = 27;
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

    /*light1pos[1]=shipPos[0];
    light1pos[2]=shipPos[1];
    light1pos[3]=shipPos[2];
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);*/

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camPos[0], camPos[1], camPos[2], lookat[0],lookat[1],lookat[2], 0,1,0);

    drawAsteroid();
    moveAsteroid();
    drawSun(0, 0, 0);
    drawShip();
    drawMissile();
    shootMissile();
    drawStars();
    drawExplosion();
    drawHUD();
    drawStrokeText("Lives:", 0, 200);
    shipCollision();

    if (lives <= 0) // if lives are exausted end game
    {
        drawStrokeText("Game over:", 300, 300);
        shipPos[0] = 50;
        for (std::vector<Asteroid>::iterator it = asteroids.begin(); it != asteroids.end(); ++it)
        {
            it->speed = 1;
        }
    }

    glutSwapBuffers();
}

//Takes user input for the game through keboard keys
void keyboard(unsigned char key, int x, int y)
{

    /* key presses move the cube, if it isn't at the extents (hard-coded here) */
    switch (key)
    {
    case 'q':
    case 27:
        exit (0);
        break;

    case ' ':
        if (missilePos[0] <= -100)
        {
            std::cout << '\a';
            missilePos[0] = shipPos[0];
            missilePos[1] = shipPos[1];
            missilePos[2] = shipPos[2];
            drawMissile();
            shootMissile();

        }
        break;
    case 't':
        light0pos[2]++;
        break;
    case 'f':
        light0pos[3]++;

        break;
    case 'g':
        light0pos[2]--;

        break;
    case 'h':
        light0pos[3]--;
    }
    glutPostRedisplay();
}
//for special keys that are on the keyboard
void special(int key, int x, int y)
{
    /* arrow key presses move the camera */
    switch(key)
    {
    case GLUT_KEY_LEFT:
        if (shipPos[2] < 5)
        {
            shipPos[2]+= 0.2;
        }
        break;

    case GLUT_KEY_RIGHT:
        if(shipPos[2] > -5)
        {
            shipPos[2]-= 0.2;
        }
        break;

    case GLUT_KEY_UP:
        if (shipPos[1] < 5)
        {
            shipPos[1]+= 0.2;
        }
        break;

    case GLUT_KEY_DOWN:
        if (shipPos[1] > -5)
        {
            shipPos[1]-= 0.2;
        }
        break;

    }
    glutPostRedisplay();
}

//main function
int main(int argc, char** argv)
{
    srand (time(NULL));

    int k;
    int l;

    /* use PPM load function to load the snail - other PPMs also provided so you can test them.
     * You can also create your own using the process outlined in the slides
     */
    asteroidtex2 = LoadPPM("asteroidtex2.ppm", &width, &height, &k);
    ship = LoadPPM("ship.ppm", &hudwidth, &hudheight, &l);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 25);
    glutCreateWindow("Comp Sci 3GC3: Final Project-3D Asteroids(tm)");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glEnable(GL_DEPTH_TEST);
    init();
    glutMainLoop();

    return 0;
}
