#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Assignment 1 variables

float theta = 0.0f;
float phi = 0.0f;

float r = 50.0f;
float eyeX = 0;
float eyeY = 0;
float eyeZ = 0;
float upX = 0;
float upY = 0;
float upZ = 0;

float windowHeight;
float windowWidth;

float zoom = 1.0f;

enum PolygonMode {WIREFRAME, SHADED};
enum ViewMode {PERSPECTIVE, ORTHOGRAPHIC};


PolygonMode polygonMode = SHADED;
ViewMode viewMode = PERSPECTIVE;

float ratio;

GLUquadricObj *obj = gluNewQuadric();

//Assignment 2 variables

float const PI = 3.14159265f;

float ro = -180.0f;

float curX = (float) 100*((cos(ro*PI/180) / (1 + sin(ro*PI/180)*sin(ro*PI/180))));
float curZ = (float) 100*((cos(ro*PI/180) * (sin(ro*PI/180))) / (1 + sin(ro*PI/180)*sin(ro*PI/180)));

float heliRot = PI;
float speed = 0.1f;
float fig8PointX = 0.0f;
float fig8PointZ = 0.0f;
bool moving = false;

// actual vector representing the camera's direction
float lx=-0.0f,lz=0.0f;
// XZ position of the camera
float x=0.0f,z=5.0f;

float bladeAngle = 0.0f;

enum ViewPerson {FIRST, THIRD};
ViewPerson viewPerson = THIRD;

// variables to compute frames per second
int frame;
long time, timebase;
char s[50];

bool sunlight = true;
bool headlight = false;

GLfloat ambient[4] = {0.0, 0.0, 0.0, 0.0};
GLfloat diffuse[4] = {1.0, 0.9, 0.7, 0.0};
GLfloat light[4] = {0.0, 1.0, 0.0, 0.0};

GLfloat ltPosition[4] = {0, -3, 6.5, 1.0};
GLfloat ltDirection[4] = {0, 0, 1, 0.0};

GLfloat hlight[4] = {1.0, 1.0, 1.0, 1.0};

float cutoffAngle = 60.0f;

//Assignment 3 variables

bool highBeam = false;

bool circleLightMoving = true;

float roo = -180;

float speed1 = 1000.0f;

float px;
float pz;

bool lightingEnabled = true;
bool thirdPersonLight = true;

GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;

bool shine = true;

bool circleLight = true;

void setView(int w, int h) {
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
        h = 1;
    
    windowWidth = w;
    windowHeight = h;
    
    ratio =  w * 1.0 / h;
    

}

GLuint LoadTexture( const char * filename )
{
    
    GLuint texture;
    
    int width, height;
    
    unsigned char * data;
    
    FILE * file;
    
    file = fopen( filename, "rb" );
    
    if ( file == NULL ) return 0;
    width = 1024;
    height = 512;
    data = (unsigned char *)malloc( width * height * 3 );
    //int size = fseek(file,);
    fread( data, width * height * 3, 1, file );
    fclose( file );
    
    for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];
        
        data[index] = R;
        data[index+2] = B;
        
    }
    
    
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free( data );
    
    return texture;
}

void setSunlight(){
    
    glEnable(GL_LIGHT0);    // using light 0
    glShadeModel(GL_SMOOTH); // smooth out lighting
    glEnable(GL_NORMALIZE);  // normalize lighting
    glEnable(GL_COLOR_MATERIAL);  // keeps some of orginal colors
    
    if (shine == true){
        
        GLfloat specular[] = {0.8,0.8,0.8,1};           // Softer white
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);    // brightes part of the light
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);  // way it affects the ojects
        
        GLfloat ambient[] ={0.25,0.25,0.25,1}; //{ 0.4,0.4,0.4,1 };          // greyish color
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);      // Color given out when there is little to no light
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        
        GLfloat diffuse[] = {0.4,0.4,0.4,1 };           // greyish color
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);      // the way the moves form bright to dark ... shaded
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        
        GLfloat shininess = 7.0;                                // How shinny the object looks
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        
    } else if (shine == false){
        
        GLfloat specular[] = {0.1,0.1,0.1,1};           // Softer white
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);    // brightes part of the light
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);  // way it affects the ojects
        
        GLfloat ambient[] = {0.25,0.25,0.25,1};          // greyish color
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);      // Color given out when there is little to no light
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        
        GLfloat diffuse[] = {0.4,0.4,0.4,1 };           // greyish color
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);      // the way the moves form bright to dark ... shaded
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        
        GLfloat shininess = 0;                                // How shinny the object looks
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    
    GLfloat position[] = { 0, 25, 0, 0};           // pointing dowards, along -y axis
    glLightfv(GL_LIGHT0, GL_POSITION, position);  //not sure how to stop making move with the camera
    
    float specReflection[] = { 0.3f, 0.3f, 0.3f, 1.0f };    // How bright the white spots are
    glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);    // How much the object reflects light
}

void setHeadlight(){
    // Turn light 1 on
    glEnable(GL_LIGHT1);
    // Aspets of light 1
    
    
    glEnable(GL_NORMALIZE);  // normalize lighting
    glEnable(GL_COLOR_MATERIAL);  // keeps some of orginal colors
    
    GLfloat specular[] = {1, 1, 1, 0.0};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);    // brightes part of the light
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);  // way it affects the ojects
    
    GLfloat diffuse[] = {1, 1, 1, 0.0};
    glLightfv(GL_LIGHT1,GL_DIFFUSE, diffuse); // color of the direct illumination
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    
    GLfloat specular1[] = {1, 1, 1, 0.0};
    glLightfv(GL_LIGHT1,GL_SPECULAR, specular1); // color of the highlight
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular1);
    
    GLfloat ambient[] = {1, 1, 1, 0.0};
    glLightfv(GL_LIGHT1,GL_AMBIENT, ambient); // color of the reflected light
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    
    
    if (highBeam == true){
        GLfloat ltPosition[4] = {0, -3, 6.5, 1.0};
        glLightfv(GL_LIGHT1,GL_POSITION, ltPosition);
        
        GLfloat ltDirection[4] = {0, 0, 1, 0.0};
        glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, ltDirection); // which way it points
        
        glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, cutoffAngle); // width of the beam
        
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,50);
        
    }else if (highBeam == false){
        
        GLfloat ltPosition[4] = {0, -3, 6.5, 1.0};
        glLightfv(GL_LIGHT1,GL_POSITION, ltPosition);
        
        GLfloat ltDirection[] = {0, -1, 1, 0.0};
        glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, ltDirection); // which way it points
        
        glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, cutoffAngle); // width of the beam
        
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,15);
        
    }
    
    float specReflection[] = { 0.2f, 0.2f, 0.2f, 1.0f };    // How bright the white spots are
    glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);    // How much the object reflects light
    
    
}

void setCircularLight(){
    
    // Turn light 2 on
    glEnable(GL_LIGHT2);
    
    // Aspets of light 2
    glEnable(GL_NORMALIZE);  // normalize lighting
    glEnable(GL_COLOR_MATERIAL);  // keeps some of orginal colors
    
    GLfloat diffuse[] = {0, 0, 1, 0.0};
    glLightfv(GL_LIGHT2,GL_DIFFUSE, diffuse); // color of the direct illumination
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    
    GLfloat specular1[] = {0, 0, 1, 0.0};
    glLightfv(GL_LIGHT2,GL_SPECULAR, specular1); // color of the highlight
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular1);
    
    GLfloat ambient[] = {0, 0, 0.5, 0.0};
    glLightfv(GL_LIGHT2,GL_AMBIENT, ambient); // color of the reflected light
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    
    
    GLfloat ltPosition[] = {px, 15, pz, 1.0};
    glLightfv(GL_LIGHT2,GL_POSITION, ltPosition);
    
    GLfloat ltDirection[] = {0, -1, 0, 0.0};
    glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION, ltDirection); // which way it points
    
    float ca = 40.0f;
    glLightf(GL_LIGHT2,GL_SPOT_CUTOFF, ca); // width of the beam
    
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT,15);
    
    float specReflection[] = { 0.2f, 0.2f, 0.2f, 1.0f };    // How bright the white spots are
    glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);    // How much the object reflects light
    
}

void moveLight(void) {
    
    if (circleLightMoving) {
        roo += speed;
    }
    
    if (roo >= 180) {
        roo = -180;
    }
    
    float d = (float) 20*( (1 + sin(roo*PI/180)*sin(roo*PI/180)))  ;
    float c = (float) 20*((cos(roo*PI/180) * (sin(roo*PI/180))) ) ;
    
    px=d - 30;
    pz=c;
    
}

void setThirdPersonLight(){
    
    // Turn light 3 on
    glEnable(GL_LIGHT3);
    
    // Aspets of light 3
    glEnable(GL_NORMALIZE);  // normalize lighting
    glEnable(GL_COLOR_MATERIAL);  // keeps some of orginal colors
    
    GLfloat diffuse[] = {1, 0, 0, 0.0};
    glLightfv(GL_LIGHT3,GL_DIFFUSE, diffuse); // color of the direct illumination
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    
    GLfloat specular1[] = {1, 0, 0, 0.0};
    glLightfv(GL_LIGHT3,GL_SPECULAR, specular1); // color of the highlight
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular1);
    
    GLfloat ambient[] = {0.5, 0, 0, 0.0};
    glLightfv(GL_LIGHT3,GL_AMBIENT, ambient); // color of the reflected light
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    
    GLfloat ltPosition[] = {eyeX+curX, eyeY, eyeZ+curZ, 1.0};
    glLightfv(GL_LIGHT3,GL_POSITION, ltPosition);
    
    float cang = 40.0f;
    glLightf(GL_LIGHT3,GL_SPOT_CUTOFF, cang); // width of the beam
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT,15);
    
    float specReflection[] = { 0.2f, 0.2f, 0.2f, 1.0f };    // How bright the white spots are
    glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);    // How much the object reflects light
    
}

void drawBody(void) {
    
    //Main Body
    glColor3f(0.33f, 0.42f, 0.18f);
    glPushMatrix();
    glScalef(1.0f, 1.0f, 2.0f);
    glutSolidCube(6.0f);
    glPopMatrix();

    
    //Right Side
    glPushMatrix();
    glScalef(1.0f, 3.0f, 1.0f);
    glTranslatef(3.0f, 0.0f, -6.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 1, 1, 12, 300, 200);
    glEnd();
    glPopMatrix();
    
    //Right Side Lid
    glPushMatrix();
    glTranslatef(3.0f, 0.0f, 6.0f);
    glScalef(1.05f, 3.05f, 0.1f);
    glutSolidSphere(1.0, 100, 100);
    glPopMatrix();
    
    //Left Side
    glPushMatrix();
    glScalef(1.0f, 3.0f, 1.0f);
    glTranslatef(-3.0f, 0.0f, -6.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 1, 1, 12, 300, 200);
    glEnd();
    glPopMatrix();
    
    //Left Side Lid
    glPushMatrix();
    glTranslatef(-3.0f, 0.0f, 6.0f);
    glScalef(1.05f, 3.05f, 0.1f);
    glutSolidSphere(1.0, 100, 100);
    glPopMatrix();
    
    //Top Side
    glPushMatrix();
    
    glTranslatef(0.0f, 3.0f, -6.0f);
    glScalef(3.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 1, 1, 12, 100, 100);
    glEnd();
    glPopMatrix();

}

void drawCockpit(void) {
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture4);
    
    //Cockpit Windshield
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    glScalef(1.33f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, 6.0f);
    glutSolidSphere(3, 100, 100);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    //Cockpit Base
    glColor3f(0.33f, 0.42f, 0.18f);
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, 6.0f);
    glScalef(1.33f, 0.65f, 1.33f);
    glutSolidSphere(3, 100, 100);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
}

void drawBlade(void) {
    glPushMatrix();
    glRotatef(10., 0.0, 0.0, 1.0);
    glScalef(0.75, 0.2, 10.0);
    GLUquadric *blade = gluNewQuadric();
    gluCylinder(blade, 0.5, 0.5, 1, 16, 1);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    gluDisk(blade, 0.0, 0.5, 16, 1);
    glPopMatrix();
    glPopMatrix();
}

void drawBladeCap() {
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glScalef(1.0, 1.0, 1.0);
    GLUquadric *cap = gluNewQuadric();
    gluCylinder(cap, 0.5, 0.5, 1, 16, 1);
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    gluDisk(cap, 0.0, 0.5, 16, 1);
    glPopMatrix();
    glPopMatrix();
}

void drawHelicopter(void) {
    
    glRotatef(180, 0.0f, 1.0f, 0.0f);
    
    if (lightingEnabled) {
    
    if (headlight) {
        
        setHeadlight();
        
    }
    
    else {
        glDisable(GL_LIGHT1);
    }
        
    }
    
    
    glClearColor(0.49f, 0.75f, 0.93f, 1.0f);
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    drawBody();
    
    //Tail
    glPushMatrix();
    
    glTranslatef(0.0f, 1.5f, -16.0f);
    glScalef(0.25f, 1.0f, 2.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 1.5, 1.5, 6, 100, 100);
    glEnd();
    glPopMatrix();
    
    //Back Body
    glPushMatrix();
    glScalef(1.33f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -6.0f);
    glutSolidSphere(3, 100, 100);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    drawCockpit();
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glColor3f(0.50f, 0.50f, 0.50f);
    //Front Left Leg
    glPushMatrix();
    glTranslatef(2.5f, -3.0f, 2.5f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 0.25, 0.25, 1, 100, 100);
    glEnd();
    glPopMatrix();
    
    //Back Left Leg
    glPushMatrix();
    glTranslatef(2.5f, -3.0f, -2.5f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 0.25, 0.25, 1, 100, 100);
    glEnd();
    
    glPopMatrix();
    
    //Front Right Leg
    glPushMatrix();
    glTranslatef(-2.5f, -3.0f, 2.5f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 0.25, 0.25, 1, 100, 100);
    glEnd();
    glPopMatrix();
    
    //Back Right Leg
    glPushMatrix();
    glTranslatef(-2.5f, -3.0f, -2.5f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 0.25, 0.25, 1, 100, 100);
    glEnd();
    glPopMatrix();
    
    glColor3f(0.25f, 0.25f, 0.25f);
    //Landing Ski Left
    glPushMatrix();
    glTranslatef(2.5f, -4.25f, 0.0f);
    glScalef(1.0f, 0.5f, 10.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    //Landing Ski Right
    glPushMatrix();
    glTranslatef(-2.5f, -4.25f, 0.0f);
    glScalef(1.0f, 0.5f, 10.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glColor3f(0.33f, 0.42f, 0.18f);
    //Tail Fin
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, -15.0f);
    glScalef(0.5f, 6.0f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    /*Rotors drawn in separate methods in case we want to rotate them at some point.
     It would be easier to modify the code that way. */
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture3);
    
    // Draw top rotor
    glColor3f(0.50f, 0.50f, 0.50f);
    glPushMatrix();
    glTranslatef(0.0, 4.0, 0.0);
    glRotatef(bladeAngle, 0.0, 1.0, 0.0);
    drawBladeCap();
    glTranslatef(0.0, 1.0, 0.0);
    drawBlade();
    glRotatef(120.0, 0.0, 1.0, 0.0);
    drawBlade();
    glRotatef(120.0, 0.0, 1.0, 0.0);
    drawBlade();
    glPopMatrix();
    
    // Draw tail rotor
    glPushMatrix();
    glTranslatef(0.5, 4.5, -15.0);
    glScalef(0.3, 0.3, 0.3);
    glRotatef(-90.0, 0.0, 0.0, 1.0);
    glRotatef(bladeAngle, 0.0, 1.0, 0.0);
    drawBladeCap();
    glTranslatef(0.0, 0.5, 0.0);
    drawBlade();
    glRotatef(120.0, 0.0, 1.0, 0.0);
    drawBlade();
    glRotatef(120.0, 0.0, 1.0, 0.0);
    drawBlade();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glColor3f(0.24f, 0.32f, 0.16f);
    //Left Wing
    glPushMatrix();
    glTranslatef(6.5f, 0.5f, -2.0f);
    glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 0.075f, 0.60f);
    glutSolidCube(8.0f);
    glPopMatrix();
    
    //Right Wing
    glPushMatrix();
    glTranslatef(-6.5f, 0.5f, -2.0f);
    glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 0.075f, 0.60f);
    glutSolidCube(8.0f);
    glPopMatrix();
    
    //Tail Wings
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, -11.0f);
    glScalef(1.0f, 0.10f, 0.5f);
    glutSolidCube(4.0f);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, texture3);
    
    glColor3f(0.50f, 0.50f, 0.50f);
    //Missile Launcher Left
    glPushMatrix();
    glTranslatef(6.0f, -0.25f, -5.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 1, 1, 6, 100, 100);
    glEnd();
    glPopMatrix();

    //Missile Launcher Right
    glPushMatrix();
    glTranslatef(-6.0f, -0.25f, -5.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 1, 1, 6, 100, 100);
    glEnd();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    
    glColor3f(0.25f, 0.25f, 0.25f);
    //Left Side Missile Head
    glPushMatrix();
    glTranslatef(6.0f, -0.25f, 1.0f);
    glutSolidSphere(1.0, 50, 50);
    glPopMatrix();
    
    //Right Side Missile Head
    glPushMatrix();
    glTranslatef(-6.0f, -0.25f, 1.0f);
    glutSolidSphere(1.0, 50, 50);
    glPopMatrix();
    
    //Machine gun
    glPushMatrix();
    glTranslatef(0.0f, -2.5f, 9.0f);
    glBegin(GL_POLYGON);
    gluCylinder(obj, 0.25, 0.25, 2, 100, 100);
    glEnd();
    glPopMatrix();
    
    
}

void drawGround(float size, int numSegments) {
    float init = -size / 2.;
    float d = size / numSegments;
    float x = init;
    for (int i = 0; i <= numSegments; ++i) {
        float z = init;
        
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0, 1, 0);
        glColor3f(0.5, 0.5, 0.5);
        for (int j = 0; j <= numSegments; ++j) {
            glVertex3f(x, 0.0, z);
            glVertex3f(x+d, 0.0, z);
            z += d;
        }
        glEnd();
        
        x += d;
    }
}

float lemniscate(float a, float t) {
    return a * cos(t) / (sin(t) * sin(t) + 1);
}

void drawTrack(float a, int numSegments) {
    const float width = 10.;
    float t = 0.0;
    float dt = 2.0 * PI / numSegments;
    
    // init values
    float lastX = lemniscate(a, t);
    float lastZ = lastX * sin(t);
    
    float curX, curZ;
    
    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0, 1, 0);
    glColor3f(0.3, 0.3, 0.3);
    // loop and draw
    for (int i = 0; i <= numSegments; ++i)
    {
        
        // increment state
        t += dt;
        curX = lemniscate(a, t);
        curZ = curX * sin(t);
        float dX = curX - lastX;
        float dZ = curZ - lastZ;
        lastX = curX;
        lastZ = curZ;
        
        // normalize dX, dZ
        float factor = 1. / sqrt(dX*dX + dZ*dZ);
        dX *= factor;
        dZ *= factor;
        
        // rotate dX, dZ by 90 deg, and apply track width
        float tX = dX;
        dX = -dZ * width;
        dZ = tX * width;
        
        // draw vertices
        glVertex3f(curX-dX, 0.0, curZ-dZ);
        glVertex3f(curX+dX, 0.0, curZ+dZ);
    }
    glEnd();
}



void moveHelicopter(void) {
    
    if (moving) {
        ro += speed;
        bladeAngle += 31.f;
    }
    
    if (ro >= 180) {
        ro = -180;
    }
    
    float prevX = curX;
    float prevZ = curZ;
    
    curX = (float) 100*((cos(ro*PI/180) / (1 + sin(ro*PI/180)*sin(ro*PI/180))));
    curZ = (float) 100*((cos(ro*PI/180) * (sin(ro*PI/180))) / (1 + sin(ro*PI/180)*sin(ro*PI/180)));
    
    float dirX = curX - prevX;
    float dirZ = curZ - prevZ;
    
    float magnitude = sqrt(curX*curX + curZ*curZ);
    
    dirX = dirX / magnitude;
    dirZ = dirZ / magnitude;
    
    float dot = dirZ*-1.0f;
    float det = dirX*-1.0f;
    
    if (moving) {
        heliRot = atan2f(det, dot);
    }
        
    glTranslatef(curX,0.0f,curZ);
}

//method for camera motion
void moveCamera(void) {
    
    if (viewPerson == THIRD) {
        
        // Restrict the angles within 0~360 deg (optional)
        if(theta > 360)theta = fmod((double)theta,360.0);
        if(phi > 360)phi = fmod((double)phi,360.0);
        
        // Spherical to Cartesian conversion.
        // Degrees to radians conversion factor 0.0174532
        eyeX = r * sin(theta*0.0174532) * sin(phi*0.0174532);
        eyeY = r * cos(theta*0.0174532);
        eyeZ = r * sin(theta*0.0174532) * cos(phi*0.0174532);
        
        // Reduce theta slightly to obtain another point on the same longitude line on the sphere.
        GLfloat dt=1.0;
        GLfloat eyeXtemp = r * sin(theta*0.0174532-dt) * sin(phi*0.0174532);
        GLfloat eyeYtemp = r * cos(theta*0.0174532-dt);
        GLfloat eyeZtemp = r * sin(theta*0.0174532-dt) * cos(phi*0.0174532);
        
        // Connect these two points to obtain the camera's up vector.
        upX=eyeXtemp-eyeX;
        upY=eyeYtemp-eyeY;
        upZ=eyeZtemp-eyeZ;
    }
    

}

void restorePerspectiveProjection() {
    
    glMatrixMode(GL_PROJECTION);
    // restore previous projection matrix
    glPopMatrix();
    
    // get back to modelview mode
    glMatrixMode(GL_MODELVIEW);
}


void setOrthographicProjection() {
    
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    
    // reset matrix
    glLoadIdentity();
    
    // set a 2D orthographic projection
    gluOrtho2D(0, windowWidth, windowHeight, 0);
    
    // switch back to modelview mode
    glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(
                        float x,
                        float y,
                        float z,
                        void *font,
                        char *string) {
    
    char *c;
    glRasterPos3f(x, y,z);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void renderScene(void) {
    
    //Set up Viewport 1 scene
    
    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    
    // Reset Matrix
    glLoadIdentity();
    
    
    glScissor(0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight*0.9);
    glEnable(GL_SCISSOR_TEST);
    glClearDepth(1.0);
    
    glClearColor(0.49f, 0.75f, 0.93f, 1.0f);
    
    // Set the viewport to be the entire window
    glViewport (0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight*0.9);
    
    // Set the correct perspective.
    
    if (viewMode == PERSPECTIVE)
        gluPerspective(45.0f*zoom, ratio, 1.0f, 1000.0f);
    else if (viewMode == ORTHOGRAPHIC)
        glOrtho((-windowWidth/50)*zoom, (windowWidth/50)*zoom, (-windowHeight/50)*zoom, (windowHeight/50)*zoom, 1.0f, 1000);
    
    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Decide if wire frame or shaded
    if (polygonMode == WIREFRAME)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (polygonMode == SHADED)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glLoadIdentity();

    moveCamera();
    
    lx = (sin(heliRot+PI));
    lz = (cos(heliRot+PI));
    
    float camX = curX + lx * 10.0f;
    float camZ = curZ + lz * 10.0f;
    
    //Set camera
    if (viewPerson == THIRD) {
        gluLookAt(eyeX+curX, eyeY, eyeZ+curZ,
                  curX, 0.0f, curZ,
                  upX, upY, upZ);
    }

    else if (viewPerson == FIRST) {
        
        gluLookAt(camX, 1.5f, camZ,
                  camX+lx, 1.5f, camZ+lz,
                  0.0f, 1.0f, 0.0f);
        
    }
    
    glEnable(GL_DEPTH_TEST);
    
    glShadeModel(GL_SMOOTH); // smooth out lighting
    glEnable(GL_NORMALIZE);  // normalize lighting
    
    GLfloat mat_specular[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat mat_shininess[] = { 10.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    if (lightingEnabled) {
        
        glEnable(GL_LIGHTING);
    
        if (sunlight) {

            setSunlight();
    
        }
    
        else {
            glDisable(GL_LIGHT0);
        }
        
        
        if (thirdPersonLight) {
            setThirdPersonLight();
        }
        
        else {
            glDisable(GL_LIGHT3);
        }
        
        
    }
    
    else {
        glDisable(GL_LIGHTING);
    }

    glPushMatrix();
    glTranslatef(0.0f, -10, 0.0f);
    drawGround(250, 100);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0f, -9.9, 0.0f);
    drawTrack(100.0, 100.0);
    glPopMatrix();
    
    glPushMatrix();
    moveHelicopter();
    glRotatef(heliRot/PI*180, 0.0f, 1.0f, 0.0f);
    drawHelicopter();
    
    
    if (lightingEnabled) {
    
        if (circleLight) {
    moveLight();
    setCircularLight();
        }
        
        else {
            glDisable(GL_LIGHT2);
        }
    
    }
    
    glPopMatrix();
    
    //Set up Viewport 2 scene
    
    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    
    // Reset Matrix
    glLoadIdentity();
    
    glScissor(0, windowHeight*0.9, (GLsizei) windowWidth, (GLsizei) windowHeight*0.1);
    glEnable(GL_SCISSOR_TEST);
    glClearDepth(1.0);
    glClearColor(1, 1, 1, 1);

    
    // Set the viewport to be the entire window
    glViewport (0, windowHeight*0.9, (GLsizei) windowWidth, (GLsizei) windowHeight*0.1);
    
    // Set the correct perspective.
    
    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    
    frame++;
    time=glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        sprintf(s,"FPS:%4.2f",
                frame*1000.0/(time-timebase));
        timebase = time;
        frame = 0;
    }
    
    glColor3f(0.0f,0.0f,0.0f);
    
    glPushMatrix();
    glLoadIdentity();
    setOrthographicProjection();
    glTranslatef(100.0f, 150, 0.0f);
    renderBitmapString(5,30,0,GLUT_BITMAP_HELVETICA_18,s);
    glPopMatrix();
    restorePerspectiveProjection();
    
    glutSwapBuffers();
    
}

//method to set default values
void resetDefaults(void) {
    
    setView(windowHeight, windowWidth);
    
    theta = 0.0f;
    phi = 0.0f;
    r = 50.0f;
    eyeX = 0;
    eyeY = 0;
    eyeZ = 0;
    upX = 0;
    upY = 0;
    upZ = 0;
    
    zoom = 1.0f;
    
    polygonMode = SHADED;
    viewMode = PERSPECTIVE;
    viewPerson = THIRD;
    
    setView(windowHeight, windowWidth);
    
}

//keyboard method
void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 'w':
            polygonMode = WIREFRAME;
            break;
        case 'W':
            polygonMode = SHADED;
            break;
        case 'p':
            viewMode = PERSPECTIVE;
            setView(windowWidth, windowHeight);
            break;
        case 'o':
            if (viewPerson == THIRD)
                viewMode = ORTHOGRAPHIC;
            setView(windowWidth, windowHeight);
            break;
        case 'z':
            if (zoom >= 0.05 && viewPerson == THIRD) {
                zoom -= 0.01f;
            }
            setView(windowWidth, windowHeight);
            break;
        case 'Z':
            if (zoom < 1.0f && viewPerson == THIRD) {
                zoom += 0.01f;
            }
            setView(windowWidth, windowHeight);
            break;
        case 'f':
            if (r > 1.0f && viewPerson == THIRD)
                r -= 4.0f;
            break;
        case 'b':
            if (viewPerson == THIRD)
                r += 4.0f;
            break;
        case 'c':
            resetDefaults();
            break;
        case 27:
            exit(0);
            break;
        case 's':
            moving = true;
            break;
        case 'e':
            moving = false;
            break;
        case 'a':
            if (moving && speed < 1.0f) {
                speed += 0.1f;
            }
            break;
        case 'A':
            if (moving && speed > 0.1f) {
                speed -= 0.1f;
                if (speed < 0.1f) {
                    speed = 0.1f;
                }
            }
            break;
        case '1':
            viewPerson = FIRST;
            viewMode = PERSPECTIVE; //Orthographic first person doesn't really make sense.
            setView(windowWidth, windowHeight);
            break;
        case '3':
            viewPerson = THIRD;
            break;
        case 'l':
            if (lightingEnabled)
                lightingEnabled = false;
            else
                lightingEnabled = true;
        case 'h':
            highBeam = true;
            break;
        case 'H':
            highBeam = false;
            break;
        case '2':
            if (lightingEnabled) {
                
                if (circleLightMoving)
                    circleLightMoving = false;
                else
                    circleLightMoving = true;
            }
            break;
        default:
            break;
    }
    
    moveCamera();
    
    glutPostRedisplay(); /* this redraws the scene without
                          waiting for the display callback so that any changes appear 
                          instantly */
}

//special keyboard method
void specialKeyboard(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            if (viewPerson == THIRD)
                theta -= 4.0f;
            break;
        case GLUT_KEY_DOWN:
            if (viewPerson == THIRD)
                theta += 4.0f;
            break;
        case GLUT_KEY_RIGHT:
            if (viewPerson == THIRD)
                phi += 4.0f;
            break;
        case GLUT_KEY_LEFT:
            if (viewPerson == THIRD)
                phi -= 4.0f;
            break;
        case GLUT_KEY_F1:
            if (lightingEnabled) {
            if (sunlight)
                sunlight = false;
            else
                sunlight = true;
            }
            break;
        case GLUT_KEY_F2:
            if (lightingEnabled) {
            if (headlight)
                headlight = false;
            else
                headlight = true;
            }
            break;
        case GLUT_KEY_F3:
            if (lightingEnabled) {
                if (circleLight)
                    circleLight = false;
                else
                    circleLight = true;
            }
            break;

        case GLUT_KEY_F4:
            if (lightingEnabled) {
                
                if (thirdPersonLight)
                    thirdPersonLight = false;
                else
                    thirdPersonLight = true;
            }
            break;
            
        case GLUT_KEY_F5:
            if (lightingEnabled) {
                
                if (shine)
                    shine = false;
                else
                    shine = true;
            }
            break;
        default:
            break;
    }
    
    moveCamera();
    
    glutPostRedisplay();
    
}

int main(int argc, char **argv) {
    

    
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    windowWidth=320;
    windowHeight=320;
    glutCreateWindow("COMP 371 - Assignment 1");
    
    // register callbacks
    glutDisplayFunc(renderScene);
    glutReshapeFunc(setView);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutIdleFunc(renderScene);
    
    texture1 = LoadTexture("/Users/SamArmand/Development/FlyingHelicopter/FlyingHelicopter/texture1.bmp");
    texture2 = LoadTexture("/Users/SamArmand/Development/FlyingHelicopter/FlyingHelicopter/texture2.bmp");
    texture3 = LoadTexture("/Users/SamArmand/Development/FlyingHelicopter/FlyingHelicopter/texture3.bmp");
    texture4 = LoadTexture("/Users/SamArmand/Development/FlyingHelicopter/FlyingHelicopter/texture4.bmp");
    
    moveCamera();
    // enter GLUT event processing cycle
    glutMainLoop();
    
    return 1;
}