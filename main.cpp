#include <Windows.h>
#include<stdio.h>
#include<math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include<errno.h>
#include<GL\GL.h>
#include<GL\GLU.h>
#include<GL\glut.h>

float y = 1, x = -30, z = 100; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move
// Camera direction
float lx = 0.0, lz = -1; // camera points initially along y-axis
//float angle = 0.0; // angle of rotation for the camera direction
//float deltaAngle = 0.0; // additional angle change when dragging
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts
int move=0;
int vertmove=0;

unsigned char header[54];        // Each BMP file begins by a 54-bytes header
unsigned int dataPos;            // Position in the file where the actual data begins
unsigned int width, height;
unsigned int imageSize;            // = width*height*3
unsigned char * data=NULL;        // Actual RGB data


void draw_board()
{
    glColor3f(0.177,0.564,1);
    glBegin(GL_QUADS);
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);
        glVertex3f(1,2,0);
        glVertex3f(0,2,0);
        glVertex3f(9,0,0);
        glVertex3f(10,0,0);
        glVertex3f(10,2,0);
        glVertex3f(9,2,0);
    glEnd();
    glColor3f(0.690,0.878,0.901);
    glBegin(GL_QUADS);
        glVertex3f(0,2,0);
        glVertex3f(10,2,0);
        glVertex3f(10,4,0);
        glVertex3f(0,4,0);
    glEnd();
}

void disp_roads()			//display the roads in the campus
{
    glColor3f(0.411,0.411,0.411);
    glBegin(GL_QUADS);
        glVertex3f(-40,0.1,90);
        glVertex3f(-40,0.1,-70);
        glVertex3f(-20,0.1,-70);
        glVertex3f(-20,0.1,90);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(-20,0.1,55);
        glVertex3f(90,0.1,55);
        glVertex3f(90,0.1,60);
        glVertex3f(-20,0.1,60);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(-20,0.1,75);
        glVertex3f(40,0.1,75);
        glVertex3f(40,0.1,80);
        glVertex3f(-20,0.1,80);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f(35,0.1,75);
        glVertex3f(35,0.1,-70);
        glVertex3f(40,0.1,-70);
        glVertex3f(40,0.1,75);
    glEnd();
}

void trees()			//draw a tree
{
    GLUquadricObj *quadratic;
    GLUquadricObj *quadratic1;
    quadratic1 = gluNewQuadric();
    quadratic = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(0.721,0.525,0.043);
    gluCylinder(quadratic,1,1,10.0f,32,32);
    glPopMatrix();
    glTranslatef(0,2,0);
        glPushMatrix();
    float k=0;
    for(int i=0,j=0;i<3;i++,j+=0.5,k+=0.15)
    {
        glTranslatef(0,1.8,0);
        glColor3f(0.133+k,0.545+k,0.133-k);
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            gluCylinder(quadratic1,4-j,0,4.0f,32,32);
        glPopMatrix();
    }
    glPopMatrix();
}

void draw_gate(char text[5])		//draw the gate
{
    glColor3f(0,0,1);
    glPushMatrix();
    glTranslatef(0,3.5,0);
    glScalef(4,7,2);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(16,3.5,0);
    glScalef(4,7,2);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(8,9,0);
    glScalef(20,4,2);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(5,8,1.1);
    glScalef(.02, .02, .02);
    glLineWidth(4.5);
    glColor3f(1,1,1);
    for (int c=0; text[c] != 0; ++c)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
    glPopMatrix();
}

class building			//construction of the block buildings
{
    float structure[8][3];
public:
    building(float a, float b, float c)
    {
        structure[0][0]=0;
        structure[0][1]=0;
        structure[0][2]=0;
        structure[1][0]=a;
        structure[1][1]=0;
        structure[1][2]=0;
        structure[2][0]=a;
        structure[2][1]=b;
        structure[2][2]=0;
        structure[3][0]=0;
        structure[3][1]=b;
        structure[3][2]=0;
        structure[4][0]=0;
        structure[4][1]=0;
        structure[4][2]=c;
        structure[5][0]=a;
        structure[5][1]=0;
        structure[5][2]=c;
        structure[6][0]=a;
        structure[6][1]=b;
        structure[6][2]=c;
        structure[7][0]=0;
        structure[7][1]=b;
        structure[7][2]=c;
    }
    void disp_build(char text[15],char side='/0')
    {
        float door[3];
        glColor3f(1,0.980,0.980);
        glBegin(GL_QUADS);
            glVertex3fv(structure[0]);
            glVertex3fv(structure[1]);
            glVertex3fv(structure[2]);
            glVertex3fv(structure[3]);
        glEnd();
        glBegin(GL_QUADS);
                glVertex3fv(structure[0]);
                glVertex3fv(structure[4]);
                glVertex3fv(structure[7]);
                glVertex3fv(structure[3]);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3fv(structure[4]);
            glVertex3fv(structure[5]);
            glVertex3fv(structure[6]);
            glVertex3fv(structure[7]);
        glEnd();
        glBegin(GL_QUADS);
                glVertex3fv(structure[1]);
                glVertex3fv(structure[2]);
                glVertex3fv(structure[6]);
                glVertex3fv(structure[5]);
        glEnd();

        if(structure[1][0]>(-1*structure[4][2]))
        {
            for(float i=10; i<structure[2][1]; i+=10)
            {
                glPushMatrix();
                glTranslatef(0,i,0);
                for(float j=5; j<structure[1][0]; j+=15)
                {
                    glColor3f(0.1f, 0.0f, 0.1f);
                    glBegin(GL_POLYGON);
                        glVertex3f(j,0,0.1);
                        glVertex3f(j+5,0,0.1);
                        glVertex3f(j+5,5,0.1);
                        glVertex3f(j,5,0.1);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(j,0,structure[4][2]-0.1);
                        glVertex3f(j+5,0,structure[4][2]-0.1);
                        glVertex3f(j+5,5,structure[4][2]-0.1);
                        glVertex3f(j,5,structure[4][2]-0.1);
                    glEnd();
                }
                for(float j=0;j<structure[1][0];j+=15)
                {
                    glColor3f(0.0f, 0.5f, 0.5f);
                    glBegin(GL_POLYGON);
                        glVertex3f(j,-10,0.1);
                        glVertex3f(j+2,-10,0.1);
                        glVertex3f(j+2,10,0.1);
                        glVertex3f(j,10,0.1);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(j,-10,structure[4][2]-0.1);
                        glVertex3f(j+2,-10,structure[4][2]-0.1);
                        glVertex3f(j+2,10,structure[4][2]-0.1);
                        glVertex3f(j,10,structure[4][2]-0.1);
                    glEnd();
                }
                glPopMatrix();
            }
            glColor3f(0.0f, 1.0f, 0.0f);
            door[0]=(structure[1][0]/2);
            glBegin(GL_POLYGON);
                glVertex3f(door[0]-4,0,0.2);
                glVertex3f(door[0]+4,0,0.2);
                glVertex3f(door[0]+4,7,0.2);
                glVertex3f(door[0]-4,7,0.2);
            glEnd();
            glPushMatrix();
            glTranslatef(10,0,3);
            draw_board();
            glPushMatrix();
            glTranslatef(1,2,0.1);
            glScalef(.01, .01, .01);
            glLineWidth(2);
            glColor3f(0.0f, 1.0f, 0.0f);
            for (int c=0; text[c] != 0; ++c)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
            glPopMatrix();
            glPopMatrix();
        }
        else
        {
            for(float i=10; i<structure[2][1]; i+=10)
            {
                glPushMatrix();
                glTranslatef(0,i,0);
                for(float j=-5; j>structure[4][2]; j-=15)
                {
                    glColor3f(0.0f, 1.0f, 0.0f);
                    glBegin(GL_POLYGON);
                        glVertex3f(-0.1,0,j);
                        glVertex3f(-0.1,0,j-5);
                        glVertex3f(-0.1,5,j-5);
                        glVertex3f(-0.1,5,j);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(structure[1][0]+0.1,0,j);
                        glVertex3f(structure[1][0]+0.1,0,j-5);
                        glVertex3f(structure[1][0]+0.1,5,j-5);
                        glVertex3f(structure[1][0]+0.1,5,j);
                    glEnd();
                }
                for(float j=0;j>structure[4][2];j-=15)
                {
                    glColor3f(0.0f, 0.5f, 0.5f);
                    glBegin(GL_POLYGON);
                        glVertex3f(-0.1,-10,j);
                        glVertex3f(-0.1,-10,j-2);
                        glVertex3f(-0.1,10,j-2);
                        glVertex3f(-0.1,10,j);
                    glEnd();
                    glBegin(GL_POLYGON);
                        glVertex3f(structure[1][0]+0.1,-10,j);
                        glVertex3f(structure[1][0]+0.1,-10,j-2);
                        glVertex3f(structure[1][0]+0.1,10,j-2);
                        glVertex3f(structure[1][0]+0.1,10,j);
                    glEnd();
                }
                glPopMatrix();
            }
            door[2]=(structure[4][2]/2);
            door[0]=structure[1][0];
            glColor3f(0.0f, 1.0f, 0.0f);
            if(side=='r')
            {
                glBegin(GL_POLYGON);
                    glVertex3f(door[0]+0.2,0,door[2]-4);
                    glVertex3f(door[0]+0.2,0,door[2]+4);
                    glVertex3f(door[0]+0.2,7,door[2]+4);
                    glVertex3f(door[0]+0.2,7,door[2]-4);
                glEnd();
                glPushMatrix();
                glTranslatef(door[0]+3,0,-2);
                glRotatef(90,0,1,0);
                draw_board();
                glPushMatrix();
                glTranslatef(1,2,0.1);
                glScalef(.01, .01, .01);
                glLineWidth(2);
                glColor3f(0.1f, 0.0f, 0.1f);
                for (int c=0; text[c] != 0; ++c)
                    glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
                glPopMatrix();
                glPopMatrix();
            }
            else if(side=='l')
            {
                glBegin(GL_POLYGON);
                    glVertex3f(-0.2,0,door[2]-4);
                    glVertex3f(-0.2,0,door[2]+4);
                    glVertex3f(-0.2,7,door[2]+4);
                    glVertex3f(-0.2,7,door[2]-4);
                glEnd();
                glPushMatrix();
                glTranslatef(-3,0,-10);
                glRotatef(-90,0,1,0);
                draw_board();
                glPushMatrix();
                glTranslatef(1,2,0.1);
                glScalef(.01, .01, .01);
                glLineWidth(2);
                glColor3f(0.0f, 1.0f, 0.0f);
                for (int c=0; text[c] != 0; ++c)
                    glutStrokeCharacter(GLUT_STROKE_ROMAN, text[c]);
                glPopMatrix();
                glPopMatrix();
            }
        }
        glPushMatrix();
        glTranslatef(0,10,0);
        glColor3f(0,0,1);
        for(int i=0;i<structure[2][1]-5;i+=5)
        {
            glBegin(GL_LINES);
                glVertex3f(structure[0][0],i,structure[0][2]+0.1);
                glVertex3f(structure[1][0],i,structure[0][2]+0.1);
                glVertex3f(structure[0][0]-0.1,i,structure[0][2]);
                glVertex3f(structure[0][0]-0.1,i,structure[4][2]);
                glVertex3f(structure[4][0],i,structure[4][2]-0.1);
                glVertex3f(structure[5][0],i,structure[4][2]-0.1);
                glVertex3f(structure[5][0]+0.1,i,structure[5][2]);
                glVertex3f(structure[1][0]+0.1,i,structure[1][2]);

            glEnd();
        }
        glPopMatrix();
    }
};

building canteen(20,30,-30);
building mech(20,40,-40);
building mba(20,40,-40);
building admin(40,30,-20);
building ec(40,30,-30);
building cs(30,40,-40);


void display()
{
    glClearColor(0.7,0.7,1,0);//background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    gluLookAt(
            x,      y,      z,
            x+lx,    y,        z+lz,
            0.0,    1.0,    0.0);
    printf("\nz=%f\tx=%f\n",z,x);
    glColor3f(0,1,0);
    glBegin(GL_QUADS);
        glVertex3f(-100,0,100);
        glVertex3f(100,0,100);
        glVertex3f(100,0,20);
        glVertex3f(-100,0,20);
        glVertex3f(-100,0,20);
        glVertex3f(-15,0,20);
        glVertex3f(-15,0,-100);
        glVertex3f(-100,0,-100);
        glVertex3f(-15,0,-100);
        glVertex3f(100,0,-100);
        glVertex3f(100,0,-20);
        glVertex3f(-15,0,-20);
        glVertex3f(25,0,-20);
        glVertex3f(100,0,-20);
        glVertex3f(100,0,20);
        glVertex3f(25,0,20);
        glEnd();
    //draw_board();
    disp_roads();
    for(int i=-10;i<40;i+=10)
    {
        glPushMatrix();
            glTranslatef(i,0,67);
            trees();
        glPopMatrix();
    }
    for(int i=45;i<90;i+=10)
    {
        glPushMatrix();
            glTranslatef(i,0,65);
            trees();
        glPopMatrix();
        glPushMatrix();
            glTranslatef(i,0,75);
            trees();
        glPopMatrix();
    }
    for(int i=-10;i<35;i+=10)
    {
        glPushMatrix();
            glTranslatef(i,0,25);
            trees();
        glPopMatrix();
    }
    glPushMatrix();
    glTranslatef(-38,0,90);
        draw_gate("DIU");
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-70,0,80);
        canteen.disp_build("Green Garden",'r');
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-70,0,20);
        mech.disp_build("DIU Hostel",'r');
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-70,0,-30);
        mba.disp_build("AB-2",'r');
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-10,0,50);
        admin.disp_build("AB-4");
    glPopMatrix();
    glutSwapBuffers();
    glFlush();
}

void changeSize(int w, int h)
{
    float ratio =  ((float) w) / ((float) h); // window aspect ratio
    glMatrixMode(GL_PROJECTION); // projection matrix is active
    glLoadIdentity(); // reset the projection
    gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

void resetWindow()			//restrict movement within boundaries
{
    if(x>100){
            x=100;
    }
    else if(x<-100){
            x=-100;
    }
    if(y>60){
            y=60;
    }
    else if(y<0.5){
            y=0.5;
    }
    if(z>100){
            z=100;
    }
    else if(z<-100){
            z=-100;
    }
}
void update(void)
{
    if (deltaMove) { // update camera position
        x += deltaMove*lx * 0.38;
        z += deltaMove*lz * 0.38;
    }
    if(vertmove==1)    y+=0.1;
    if(vertmove==-1) y-=0.1;
    resetWindow();
    glutPostRedisplay(); // redisplay everything
}

void pressKey(unsigned char key, int xx, int yy)
{
    switch (key) {
        case 'w' :
            deltaMove = 1.0;
            glutIdleFunc(update);
            break;
        case 's' :
            deltaMove = -1.0;
            glutIdleFunc(update);
            break;
        case 'u' :
            vertmove=1;
            glutIdleFunc(update);
            break;
        case 'j' :
            vertmove=-1;
            glutIdleFunc(update);
            break;
    }
}

void releaseKey(unsigned char key, int x, int y)
{
    switch (key) {
        case 'w' :
            deltaMove = 0.0;
            glutIdleFunc(NULL);
            break;
        case 's' :
            deltaMove = 0.0;
            glutIdleFunc(NULL);
            break;
        case 'u' :
            vertmove=0;
            glutIdleFunc(NULL);
            break;
        case 'j' :
            vertmove=0;
            glutIdleFunc(NULL);
            break;

    }
}

void mouseMove(int x, int y)
{
    if (isDragging) { // only when dragging
        // update the change in angle
        deltaAngle = (x - xDragStart) * -0.005;
        // camera's direction is set to angle + deltaAngle
        lx = sin(angle + deltaAngle);
        lz = -cos(angle + deltaAngle);
        glutIdleFunc(update);
    }
}

void mouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = 1;
            xDragStart = x;
        }
        else  { /* (state = GLUT_UP) */
            angle += deltaAngle; // update camera turning angle
            isDragging = 0; // no longer dragging
            glutIdleFunc(NULL);

        }
    }
}
int main(int argc, char ** argv)
{   glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800,800);
    glutInitWindowPosition(40,40);
    glutCreateWindow("diuCampusViewProject_172-15-9600");
    glutReshapeFunc(changeSize); // window reshape callback
    glutDisplayFunc(display);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(pressKey);
    glutKeyboardUpFunc(releaseKey);
    glutMouseFunc(mouseButton); // process mouse button push/release
    glutMotionFunc(mouseMove); // process mouse dragging motion
    glutMainLoop();
}
