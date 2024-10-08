#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "glut.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Project 1 -- Craig Harris";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 20000;		// 10000 milliseconds = 10 seconds

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
bool	Freeze;
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;			// ORTHO or PERSP
float	Scale;					// scaling factor
float	Time;					// used for animation, this has a value between 0. and 1.
float	AnimationCycleTime;		// used for Keytime class animation
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

int		SphereList;
int		TorusList;
int		BoxList;
int		PosXList;
int		PosYList;
int		PosZList;
int		NegXList;
int		NegYList;
int		NegZList;
GLuint	TexPosXList;
GLuint	TexPosYList;
GLuint	TexPosZList;
GLuint	TexNegXList;
GLuint	TexNegYList;
GLuint	TexNegZList;
GLuint  CubeMapReflectTex;
GLuint  CubeMapRefractTex;
int  CubeMapReflectUnit = 5;
int  CubeMapRefractUnit = 6;
char* CubeFaceFiles[6]
{
	"..\\..\\turned_in\\proj4_reflect_refract\\nvposx.bmp",
	"..\\..\\turned_in\\proj4_reflect_refract\\nvnegx.bmp",
	"..\\..\\turned_in\\proj4_reflect_refract\\nvposy.bmp",
	"..\\..\\turned_in\\proj4_reflect_refract\\nvnegy.bmp",
	"..\\..\\turned_in\\proj4_reflect_refract\\nvposz.bmp",
	"..\\..\\turned_in\\proj4_reflect_refract\\nvnegz.bmp"
};



// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	Cross(float[3], float[3], float[3]);
float	Dot(float [3], float [3]);
float	Unit(float [3], float [3]);
float	Unit(float [3]);
void LoadAndSetTexture(char*, GLuint*);


// utility to create an array from 3 separate values:

float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

//#include "setmaterial.cpp"
//#include "setlight.cpp"
#include "osusphere.cpp"
//#include "osucone.cpp"
#include "osutorus.cpp"
#include "bmptotexture.cpp"
//#include "loadobjfile.cpp"
//#include "keytime.cpp"
#include "glslprogram.cpp"

GLSLProgram CubeMapShader;
GLSLProgram TextureShader;


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	fprintf(stderr, "Starting.\n");
	glutInit(&argc, argv);

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	AnimationCycleTime = ms / (float)1000.f;
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );

	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );

	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat3 normal;
	glm::mat4 modelview;
	glm::mat4 modelviewprojection;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		//glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
		projection = glm::ortho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		//gluPerspective( 70.f, 1.f,	0.1f, 1000.f );
		projection = glm::perspective( glm::radians(70.f), 1.f,	0.1f, 1000.f );
	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:

	//gluLookAt( 0.f, 0.f, 3.f,     0.f, 0.f, 0.f,     0.f, 1.f, 0.f );
	//glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	//glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );
	//if( Scale < MINSCALE ) Scale = MINSCALE;
	//glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	glm::vec3 eye(0., 0., 3.);
	glm::vec3 look(0., 0., 0.);
	glm::vec3 up(0., 1., 0.);
	view = glm::lookAt(eye, look, up);

	// ROTATE THE SCENE
	view = glm::rotate(view, glm::radians(Yrot), glm::vec3(0, 1, 0));
	view = glm::rotate(view, glm::radians(Xrot), glm::vec3(1, 0, 0));
	// UNIFORMLY SCALE THE SCENE
	if( Scale < MINSCALE ) Scale = MINSCALE;
	view = glm::scale(view, glm::vec3(Scale, Scale, Scale));


	model = glm::mat4(1); // identity matrix
	normal = glm::mat3(glm::transpose(glm::inverse(model)));
	modelview = view * model;
	modelviewprojection = projection * view * model;
	//glMultMatrixf(glm::value_ptr(projection));
	//glMultMatrixf(glm::value_ptr(view)); // send matrix to opengl
	//glMultMatrixf(glm::value_ptr(model)); // send matrix to opengl

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );

	// draw the box object by calling up its display list:

	CubeMapShader.Use( );
	CubeMapShader.SetUniformVariable( (char*)"uM", model);
	CubeMapShader.SetUniformVariable( (char*)"uV", view);
	CubeMapShader.SetUniformVariable( (char*)"uP", projection);
	CubeMapShader.SetUniformVariable( (char*)"uN", normal);
	CubeMapShader.SetUniformVariable( (char*)"uMV", modelview);
	CubeMapShader.SetUniformVariable( (char*)"uMVP", modelviewprojection);

	CubeMapShader.SetUniformVariable( "uXrot", F_PI/180 * Xrot);
	CubeMapShader.SetUniformVariable( "uXrot", F_PI/180 * Xrot);
	CubeMapShader.SetUniformVariable( "uYrot", F_PI/180 * Yrot);

	glColor3f(0, 1, 1);
	//glCallList( SphereList );
	//glCallList( TorusList );
	glCallList( BoxList );

	CubeMapShader.UnUse( );       // OvalShader.Use(0);  also works

	//glBindTexture(GL_TEXTURE_2D, TexPosXList);
	//TextureShader.Use();
	//TextureShader.SetUniformVariable( "uModelMat", model);
	//TextureShader.SetUniformVariable( "uViewMat", view);
	//TextureShader.SetUniformVariable( "uProjectionMat", projection);
	//glBindTexture(GL_TEXTURE_2D, TexPosXList);
	//glCallList(PosXList);
	//glBindTexture(GL_TEXTURE_2D, TexPosYList);
	//glCallList(PosYList);
	//glBindTexture(GL_TEXTURE_2D, TexPosZList);
	//glCallList(PosZList);
	//glBindTexture(GL_TEXTURE_2D, TexNegXList);
	//glCallList(NegXList);
	//glBindTexture(GL_TEXTURE_2D, TexNegYList);
	//glCallList(NegYList);
	//glBindTexture(GL_TEXTURE_2D, TexNegZList);
	//glCallList(NegZList);
	//TextureShader.UnUse();

	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	//glDisable( GL_DEPTH_TEST );
	//glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	//glDisable( GL_DEPTH_TEST );
	//glMatrixMode( GL_PROJECTION );
	//glLoadIdentity( );
	//gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	//glMatrixMode( GL_MODELVIEW );
	//glLoadIdentity( );
	//glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:

	// SHADERS
	CubeMapShader.Init( );
	bool valid = CubeMapShader.Create( "cubemap_3d.vert", "cubemap_3d.frag" );
	if( !valid )
		fprintf( stderr, "Could not create the CubeMap shader!\n" );
	else
		fprintf( stderr, "CubeMap shader created!\n" );

	// set the uniform variables that will not change:
	
	CubeMapShader.Use( );
	CubeMapShader.SetUniformVariable( "uReflectUnit", CubeMapReflectUnit );
	CubeMapShader.SetUniformVariable( "uRefractUnit", CubeMapRefractUnit );
	CubeMapShader.SetUniformVariable( "uNoiseAmp", 0.0f );
	CubeMapShader.SetUniformVariable( "uNoiseFreq", 1.0f );
	CubeMapShader.SetUniformVariable( "uIndexOfRefraction", 1.00f );
	CubeMapShader.SetUniformVariable( "uReflect_VS_Refract", 0.f );
	CubeMapShader.SetUniformVariable( "uColorMix", 0.1f );
	CubeMapShader.SetUniformVariable( "uXrot", 0.f);
	CubeMapShader.SetUniformVariable( "uYrot", 0.f );
	CubeMapShader.UnUse( );

	TextureShader.Init( );
	valid = TextureShader.Create( "texture.vert", "texture.frag" );
	if( !valid )
		fprintf( stderr, "Could not create the texture shader!\n" );
	else
		fprintf( stderr, "texture shader created!\n" );

	// KEYTIMES
	//uAd_kt.Init();
	//uBd_kt.Init();
	//uTol_kt.Init();

	//uAd_kt.AddTimeValue(0.f, 0.1f); // set start and end
	//uBd_kt.AddTimeValue(0.f, 0.1f);
	//uAd_kt.AddTimeValue(19.99f, 0.1f);
	//uBd_kt.AddTimeValue(19.99f, 0.1f);

	//uAd_kt.AddTimeValue(5.f, 0.5fo
	//uBd_kt.AddTimeValue(5.f, 0.1f);

	//uAd_kt.AddTimeValue(10.f, 0.1f);
	//uBd_kt.AddTimeValue(10.f, 0.5f);

	//uAd_kt.AddTimeValue(15.f, 0.5f);
	//uBd_kt.AddTimeValue(15.f, 0.5f);

	//uAd_kt.AddTimeValue(18.f, 0.05f);
	//uBd_kt.AddTimeValue(18.f, 0.05f);
	
	// SKY BOX
	LoadAndSetTexture(CubeFaceFiles[0], &TexPosXList);
	LoadAndSetTexture(CubeFaceFiles[1], &TexNegXList);
	LoadAndSetTexture(CubeFaceFiles[2], &TexPosYList);
	LoadAndSetTexture(CubeFaceFiles[3], &TexNegYList);
	LoadAndSetTexture(CubeFaceFiles[4], &TexPosZList);
	LoadAndSetTexture(CubeFaceFiles[5], &TexNegZList);

	// REFLECT CUBE MAP
	glGenTextures(1, &CubeMapReflectTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapReflectTex);
	for (int file = 0; file < 6; file++) 
	{
		int nums, numt;
		unsigned char* texture2d = BmpToTexture(CubeFaceFiles[file], &nums, &numt);
		if (texture2d == NULL) fprintf(stderr, "Could not open BMP 2D texture '%s'\n", CubeFaceFiles[file]);
		else fprintf(stderr, "BMP 2D texture '%s' read -- nums = %d, numt = %d\n", CubeFaceFiles[file], nums, numt);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + file, 0, 3, nums, numt, 0, GL_RGB, GL_UNSIGNED_BYTE, texture2d);
		delete[] texture2d;
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// REFRACT CUBE MAP
	glGenTextures(1, &CubeMapReflectTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapRefractTex);
	for (int file = 0; file < 6; file++) 
	{
		int nums, numt;
		unsigned char* texture2d = BmpToTexture(CubeFaceFiles[file], &nums, &numt);
		if (texture2d == NULL) fprintf(stderr, "Could not open BMP 2D texture '%s'\n", CubeFaceFiles[file]);
		else fprintf(stderr, "BMP 2D texture '%s' read -- nums = %d, numt = %d\n", CubeFaceFiles[file], nums, numt);
		
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + file, 0, 3, nums, numt, 0, GL_RGB, GL_UNSIGNED_BYTE, texture2d);
		delete[] texture2d;
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	glutSetWindow( MainWindow );

	// create the object:

	SphereList = glGenLists( 1 );
	glNewList( SphereList, GL_COMPILE );
		OsuSphere( 1., 64, 64 );
	glEndList( );

	TorusList = glGenLists( 1 );
	glNewList( TorusList, GL_COMPILE );
		glPushMatrix;
			glRotatef(90, 1, 0, 0);
			OsuTorus(0.5, 1, 64, 64);
		glPopMatrix;
	glEndList( );

	// create the object:
	//float dx = 0.5;
	//float dy = 0.5;
	//float dz = 0.5;
	//BoxList = glGenLists( 1 );
	//glNewList( BoxList, GL_COMPILE );
	//	glBegin( GL_QUADS );
	//			glNormal3f( 1., 0., 0. );
	//				glVertex3f(  dx, -dy,  dz );
	//				glVertex3f(  dx, -dy, -dz );
	//				glVertex3f(  dx,  dy, -dz );
	//				glVertex3f(  dx,  dy,  dz );
	//			glNormal3f(-1., 0., 0.);
	//				glVertex3f( -dx, -dy,  dz);
	//				glVertex3f( -dx,  dy,  dz );
	//				glVertex3f( -dx,  dy, -dz );
	//				glVertex3f( -dx, -dy, -dz );
	//			glNormal3f(0., 1., 0.);
	//				glVertex3f( -dx,  dy,  dz );
	//				glVertex3f(  dx,  dy,  dz );
	//				glVertex3f(  dx,  dy, -dz );
	//				glVertex3f( -dx,  dy, -dz );
	//			glNormal3f(0., -1., 0.);
	//				glVertex3f( -dx, -dy,  dz);
	//				glVertex3f( -dx, -dy, -dz );
	//				glVertex3f(  dx, -dy, -dz );
	//				glVertex3f(  dx, -dy,  dz );
	//			glNormal3f(0., 0., 1.);
	//				glVertex3f(-dx, -dy, dz);
	//				glVertex3f( dx, -dy, dz);
	//				glVertex3f( dx,  dy, dz);
	//				glVertex3f(-dx,  dy, dz);
	//			glNormal3f(0., 0., -1.);
	//				glVertex3f(-dx, -dy, -dz);
	//				glVertex3f(-dx,  dy, -dz);
	//				glVertex3f( dx,  dy, -dz);
	//				glVertex3f( dx, -dy, -dz);
	//	glEnd( );
	//glEndList( );

	float dx = 5;
	float dy = 5;
	float dz = 5;
	PosXList = glGenLists( 1 );
	glNewList( PosXList, GL_COMPILE );
		glBegin( GL_QUADS );
			//glBindTexture(GL_TEXTURE_2D, TexPosXList);
				glNormal3f( 1., 0., 0. );
					glTexCoord2f(0.f, 0.f);
					glVertex3f(  dx, -dy,  dz );
					glTexCoord2f(1.f, 0.f);
					glVertex3f(  dx, -dy, -dz );
					glTexCoord2f(1.f, 1.f);
					glVertex3f(  dx,  dy, -dz );
					glTexCoord2f(0.f, 1.f);
					glVertex3f(  dx,  dy,  dz );
		glEnd( );
	glEndList( );

	NegXList = glGenLists( 1 );
	glNewList( NegXList, GL_COMPILE );
		glBegin( GL_QUADS );
			//glBindTexture(GL_TEXTURE_2D, TexNegXList);
				glNormal3f(-1., 0., 0.); 
					glTexCoord2f(0.f, 0.f);
					glVertex3f( -dx, -dy,  -dz);
					glTexCoord2f(0.f, 1.f);
					glVertex3f( -dx,  dy,  -dz );
					glTexCoord2f(1.f, 1.f);
					glVertex3f( -dx,  dy, dz );
					glTexCoord2f(1.f, 0.f);
					glVertex3f( -dx, -dy, dz );
		glEnd( );
	glEndList( );

	PosYList = glGenLists( 1 );
	glNewList( PosYList, GL_COMPILE );
		glBegin( GL_QUADS );
			//glBindTexture(GL_TEXTURE_2D, TexPosYList);
				glNormal3f(0., 1., 0.); 
					glTexCoord2f(0.f, 0.f);
					glVertex3f( -dx, dy,  dz);
					glTexCoord2f(0.f, 1.f);
					glVertex3f( -dx, dy,  -dz );
					glTexCoord2f(1.f, 1.f);
					glVertex3f(  dx, dy,  -dz );
					glTexCoord2f(1.f, 0.f);
					glVertex3f(  dx, dy,  dz );
		glEnd( );
	glEndList( );

	NegYList = glGenLists( 1 );
	glNewList( NegYList, GL_COMPILE );
		glBegin( GL_QUADS );
			//glBindTexture(GL_TEXTURE_2D, TexNegYList);
				glNormal3f(0., -1., 0.);
					glTexCoord2f(0.f, 0.f);
					glVertex3f( -dx, -dy,  -dz);
					glTexCoord2f(0.f, 1.f);
					glVertex3f( -dx, -dy,  dz );
					glTexCoord2f(1.f, 1.f);
					glVertex3f(  dx, -dy,  dz );
					glTexCoord2f(1.f, 0.f);
					glVertex3f(  dx, -dy,  -dz );
		glEnd( );
	glEndList( );

	PosZList = glGenLists( 1 );
	glNewList( PosZList, GL_COMPILE );
		glBegin( GL_QUADS );
			//glBindTexture(GL_TEXTURE_2D, TexPosZList);
				glNormal3f(0., 0., 1.);
					glTexCoord2f(0.f, 0.f);
					glVertex3f(-dx, -dy, dz);
					glTexCoord2f(0.f, 1.f);
					glVertex3f(-dx,  dy, dz);
					glTexCoord2f(1.f, 1.f);
					glVertex3f(dx,  dy, dz);
					glTexCoord2f(1.f, 0.f);
					glVertex3f(dx, -dy, dz);
		glEnd( );
	glEndList( );

	NegZList = glGenLists( 1 );
	glNewList( NegZList, GL_COMPILE );
		glBegin( GL_QUADS );
			//glBindTexture(GL_TEXTURE_2D, TexNegZList);
				glNormal3f(0., 0., -1.);
					glTexCoord2f(0.f, 0.f);
					glVertex3f(dx, -dy, -dz);
					glTexCoord2f(0.f, 1.f);
					glVertex3f(dx,  dy, -dz);
					glTexCoord2f(1.f, 1.f);
					glVertex3f(-dx,  dy, -dz);
					glTexCoord2f(1.f, 0.f);
					glVertex3f(-dx, -dy, -dz);
		glEnd( );
	glEndList( );

	BoxList = glGenLists(1);
	glNewList(BoxList, GL_COMPILE);
		glPushMatrix();
			glScalef(.1, .1, .1);
			glBegin(GL_QUADS);
				glCallList(PosXList);
				glCallList(NegXList);
				glCallList(PosYList);
				glCallList(NegYList);
				glCallList(PosZList);
				glCallList(NegZList);
			glEnd();
		glPopMatrix();
	glEndList();



	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
	case 'f':
	case 'F':
		Freeze = !Freeze;
		if (Freeze)
			glutIdleFunc(NULL);
		else
			glutIdleFunc(Animate);
		break;

		case 'o':
		case 'O':
			NowProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			NowProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	Freeze = false;
	Scale  = 1.0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}

void LoadAndSetTexture(char* filePath, GLuint* textureID) {
    int width, height;
    unsigned char* texture = BmpToTexture(filePath, &width, &height);

    if (texture == NULL) {
        fprintf(stderr, "Cannot open texture '%s'\n", filePath);
    } else {
        fprintf(stderr, "Opened '%s': width = %d ; height = %d\n", filePath, width, height);

        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    }
}

