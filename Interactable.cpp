#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
 
/* Global variables */
int windowWidth  = 640;     // Windowed mode's width
int windowHeight = 480;     // Windowed mode's height
int windowPosX   = 50;      // Windowed mode's top-left corner x
int windowPosY   = 50;      // Windowed mode's top-left corner y

GLfloat anglePyramid = 0.0f;  // Rotational angle for pyramid [NEW]
int refreshMills = 15;        // refresh interval in milliseconds [NEW]
GLfloat	angleThing = 0.2f;

GLfloat xSpeed = 0.002f;      // change in speed for the angle in x and y directions
GLfloat ySpeed = 0.007f;

GLfloat xSpeedSaved, ySpeedSaved;  // To support resume

bool fullScreenMode = false;  // Full-screen or windowed mode?
bool paused = false;         // Movement paused or resumed
char *pausedChar;
GLfloat angleSaved;  // To support resume
 
 
 enum {
  FIRST, SECOND, THIRD, FOURTH
} MaterialType;
 
 enum {
  LIGHT_OFF, LIGHT_RED, LIGHT_WHITE, LIGHT_GREEN
} LightValues;

enum {
  FIRST_MATERIAL = 1, SECOND_MATERIAL = 2, THIRD_MATERIAL = 3,FOURTH_MATERIAL = 4
} MaterialDisplayList;
 
 //GL Light colors
 GLfloat red_light[] = {1.0, 0.0, 0.0, 1.0},
green_light[] = {0.0, 1.0, 0.0, 1.0}, 
white_light[] = {1.0, 1.0, 1.0, 1.0};


//GL light positions
GLfloat left_light_position[] = {-1.0, 0.0, 1.0, 0.0},
right_light_position[] = {1.0, 0.0, 1.0, 0.0};
 
 
int shade_model = GL_SMOOTH; 
char *left_light, *right_light; 
char *obj_material;
 
 
/* Initialize OpenGL Graphics */
void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}
 
 void 
output(GLfloat x, GLfloat y, char *format,...)
{
  va_list args;
  char buffer[200], *p;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  glPushMatrix();
  glTranslatef(x, y, 0);
  for (p = buffer; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}

 
 
 
/* Handler for window-repaint event. Called back when the window first appears and
   whenever the window needs to be re-painted. */
   
void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
 
 
   // Render a Solid Octahedron
   glLoadIdentity(); 				// Reset the model-view matrix
   glEnable(GL_DEPTH_TEST);        //Enables depth testing
   glEnable(GL_LIGHTING);                  // Enables lighting
   glTranslatef( 0.0f, 0.0f, -6.0f);  // Move left and into the screen
   glRotatef(anglePyramid, xSpeed, ySpeed, 0.0f);  // Rotate about the (1,1,0)-axis [NEW]
   glCallList(FIRST_MATERIAL);
   glColor3f(0.5f,0.5f,0.5f);
   glutSolidOctahedron();
   

 
   glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
 
   anglePyramid += angleThing ;// Update the rotational angle after each refresh [NEW]
   xSpeed = xSpeed;
   ySpeed = ySpeed;
}


// Creates a second display which only displays text
void nodisplay() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
 

   glPushMatrix();
   glDisable(GL_DEPTH_TEST);      //disables depth testing to make sure the text is not small
   glDisable(GL_LIGHTING);		 //disables lighting as it can cause the text to be hard to see	
   glLoadIdentity();
   gluOrtho2D(0, 3000, 0, 3000);       //Ortho perspective so Text isnt scewed by previous display handler
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   output(1200, 1500, "Paused", pausedChar);     //Sets text display in center
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glPopAttrib();
   
   
   glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}


 
/* Called back when timer expired [NEW] */
void timer(int value) {
   glutPostRedisplay();      // Post re-paint request to activate display()
   glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}
 
 
 void specialKeys(int key, int x, int y) {
   switch (key) {
      case GLUT_KEY_F1:    // F1: Toggle between full-screen and windowed mode
         fullScreenMode = !fullScreenMode;         // Toggle state
         if (fullScreenMode) {                     // Full-screen mode
            windowPosX   = glutGet(GLUT_WINDOW_X); // Save parameters for restoring later
            windowPosY   = glutGet(GLUT_WINDOW_Y);
            windowWidth  = glutGet(GLUT_WINDOW_WIDTH);
            windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
            glutFullScreen();                      // Switch into full screen
         } else {                                         // Windowed mode
            glutReshapeWindow(windowWidth, windowHeight); // Switch into windowed mode
            glutPositionWindow(windowPosX, windowPosX);   // Position top-left corner
         }
         break;
      case GLUT_KEY_RIGHT:    // Right: increase x speed
         xSpeed *= 1.05f; break;
      case GLUT_KEY_LEFT:     // Left: decrease x speed
         xSpeed *= 0.95f; break;
      case GLUT_KEY_UP:       // Up: increase y speed
         ySpeed *= 1.05f; break;
      case GLUT_KEY_DOWN:     // Down: decrease y speed
         ySpeed *= 0.95f; break;
   }
}
 
 //Keyboard handler that calls for display callbacks
 void keyboard(unsigned char key, int x, int y) {
   switch (key) {
      case 112:     // p key
         glutDisplayFunc(nodisplay); //Calls for nodisplay
         break;
      case 27:     // ESC key
	  	 glutDisplayFunc(display);  //Calls for display
         break;
	     
   }
}

//Creating a constant array values for my light properties
GLfloat light_ambient[]  = { 0.5f, 1.0f, 0.2f, 1.0f };
GLfloat light_diffuse[]  = { 0.095f, 0.05f, 0.2f, 1.0f };
GLfloat light_specular[] = {0.333, 0.85, 0.633};

//creating a constant array values for  my material properties
GLfloat mat_ambient[]    = { 10.7f, 5.7f, 25.7f, 1.0f };
GLfloat mat_diffuse[]    = { 0.8f, 0.5f, 0.2f, 1.0f };
GLfloat mat_specular[]   = { 10.0f, 5.0f, 3.5f, 1.0f }; 
GLfloat mat_shininess = 5.0f;




// Light menu selection
void light_select(GLenum which, int value, char **label)
{
  glEnable(which);
  switch (value) {
  case LIGHT_OFF:
    *label = "off";
    glDisable(which);
    break;
  case LIGHT_RED:
    *label = "red";
    glLightfv(which, GL_DIFFUSE, red_light);
    break;
  case LIGHT_WHITE:
    *label = "white";
    glLightfv(which, GL_DIFFUSE, white_light);
    break;
  case LIGHT_GREEN:
    *label = "green";
    glLightfv(which, GL_DIFFUSE, green_light);
    break;
  }
  glutPostRedisplay();
}
 
 void 
material(int dlist, GLfloat * ambient, GLfloat * diffuse,
  GLfloat * specular, GLfloat shininess)
{
  glNewList(dlist, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
  glEndList();
}
 
 char *
material_select(int object, int value)
{
  glutPostRedisplay();
  switch (value) {
  case FIRST:
    material(object, mat_ambient,
      mat_diffuse, mat_specular, mat_shininess);
    return "first";
  }
  return NULL; /* avoid bogus warning! */
}
 
 
 
 // Menu for  left light selection
 void left_light_select(int value)
{
  light_select(GL_LIGHT0, value, &left_light);
}

//Menu for right light selection
void right_light_select(int value)
{
  light_select(GL_LIGHT1, value, &right_light);
}


void 
obj_select(int value)
{
  obj_material = material_select(FIRST_MATERIAL, value);
}



// Main menu selection
void main_menu_select(int value)
{
  if (value == 666)
    exit(0);
  glShadeModel(shade_model = value);
  glutPostRedisplay();
}










 
/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}
 
 
 
 
 
/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
   
    int left_light_m, right_light_m,obj_m;
   
   glutInit(&argc, argv);            // Initialize GLUT
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Enable double buffered mode
   glutInitWindowSize(640, 480);   // Set the window's initial width & height
   glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
   glutCreateWindow(title);          // Create window with the given title
   glutDisplayFunc(display);       // Register callback handler for window re-paint event
   
   
   #define LIGHT_MENU_ENTRIES() \
   glutAddMenuEntry("Disable", LIGHT_OFF); \
   glutAddMenuEntry("Red", LIGHT_RED); \
   glutAddMenuEntry("White", LIGHT_WHITE); \
   glutAddMenuEntry("Green", LIGHT_GREEN);
   
   #define MATERIAL_MENU_ENTRIES() \
    glutAddMenuEntry("First", FIRST); \
    glutAddMenuEntry("Second", SECOND); \
    glutAddMenuEntry("Third", THIRD); \
    glutAddMenuEntry("Fourth", FOURTH);
   
   left_light_m = glutCreateMenu(left_light_select);
   LIGHT_MENU_ENTRIES();
   right_light_m = glutCreateMenu(right_light_select);
   LIGHT_MENU_ENTRIES();
   obj_m = glutCreateMenu(obj_select);
   MATERIAL_MENU_ENTRIES();

   glutCreateMenu(main_menu_select);
   glutAddMenuEntry("Smooth shading", GL_SMOOTH);
   glutAddMenuEntry("Flat shading", GL_FLAT);
   glutAddSubMenu("Left light", left_light_m);
   glutAddSubMenu("Right light", right_light_m);
   glutAddSubMenu("Object", obj_m);
   glutAddMenuEntry("Quit", 666);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   
   glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
   glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
   glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
   glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
   left_light_select(LIGHT_RED);
   right_light_select(LIGHT_GREEN);
  
  
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);
   
   glutReshapeFunc(reshape);       // Register callback handler for window re-size event
   glutSpecialFunc(specialKeys); 
   glutKeyboardFunc(keyboard); 
   initGL();                       // Our own OpenGL initialization
   glutTimerFunc(0, timer, 0);     // First timer call immediately [NEW]
   glutMainLoop();                 // Enter the infinite event-processing loop
   return 0;
}
