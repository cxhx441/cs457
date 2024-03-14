#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>

//#include "glsl_reader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;
// minimum allowable scale factor:
const float MINSCALE = 0.05f;
// equivalent mouse movement when we click the scroll wheel:
const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;
// active mouse buttons (or them together):
const int LEFT   = 0; // per https://www.glfw.org/docs/3.3/group__buttons.html
const int RIGHT  = 1;
const int MIDDLE = 2;
// line width for the axes:
const GLfloat AXES_WIDTH   = 3.;
// non-constant global variables:
int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		NowProjection;		// ORTHO or PERSP
float	Scale = 1.f;					// scaling factor
//int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
double Xmouse, Ymouse;
int PressedButton = -1;
void Axes(float);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if( action == GLFW_PRESS ) {
		glfwGetCursorPos(window, &Xmouse, &Ymouse);
		PressedButton = button;
	}
	else PressedButton = -1;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	int direction = yoffset / abs(yoffset);
	Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR * direction;
	Scale = std::max(Scale, MINSCALE);
}


void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	double dx = xpos - Xmouse;
	double dy = ypos - Ymouse;

	if ( PressedButton == GLFW_MOUSE_BUTTON_LEFT ) 
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	else if( PressedButton == GLFW_MOUSE_BUTTON_MIDDLE ) 
	{
		std::cout << "scaler" << std::endl;
		Scale += SCLFACT * (float) ( dx - dy );
		Scale = std::max(Scale, MINSCALE);
		std::cout << "scale: " << Scale << std::endl;
	}
	Xmouse = xpos;			// new current position
	Ymouse = ypos;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // set opengl version 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //glfwSwapInterval(1); // synchronize framerate to vsync/framerate
    glfwSwapInterval(0); // no vsync

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	
	// Mouse Interactions
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

    GLenum err = glewInit();
    if (err != GLEW_OK) 
        std::cout << "Error: %s\n" << glewGetErrorString(err) << std::endl;
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

	// create the axes:
	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );

	{ // this block is to avoid infinite loop of glCheckError which provides an error when there is no gl context. stops infinite loop when we close the openggl window. 
		float axes_position[] = {
			1.5, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 1.5, 0.0,

			0.0, 0.0, 0.0,
			0.0, 0.0, 1.5,

			1.65f, -0.075f, 0.f, // X!
			1.8f, 0.075f, 0.f,
			1.65f, 0.075f, 0.f,
			1.8f, -0.075f, 0.f,

			0.f, 1.65f, 0.f, // Y!
			0.f, 1.74f, 0.f,
			-0.075f, 1.8f, 0.f,
			0.f, 1.74f, 0.f,
			0.075f, 1.8f, 0.f,

			0.f, 0.075f, 1.8f, // Z!
			0.f, 0.075f, 1.65f,
			0.f, -0.075f, 1.8f,
			0.f, -0.075f, 1.65f,
			0.f, 0.f, 1.6875f,
			0.f, 0.f, 1.7625f
		};
		unsigned int axes_indices[] = {
			0, 1,
			1, 2,

			3, 4,

			5, 6,
			7, 8,

			9, 10,
			10, 11,
			12, 13,

			14, 15,
			15, 16,
			16, 17,
			18, 19
		};



		// vertex positions
		float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 0
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 1
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 2
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 3
		};
		// indices of the vertices to be drawn
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glBlendEquation(GL_FUNC_ADD)); // default, but being explicit

		// generate vao
		VertexArray va;
		VertexBuffer vb(positions, 4 * 5 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3); // vertex position, 3 floats
		layout.Push<float>(2); // vertex texture coords, 2 floats
		va.AddBuffer(vb, layout);
		vb.Unbind();

		IndexBuffer ib(indices, 2 * 3 * sizeof(unsigned int));
		ib.Unbind();


		Shader shader = Shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniformVec4("uColor", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));
		shader.Unbind();
		
		Texture texture("res/textures/dice.png");
		texture.Bind(0);
		shader.Bind();
		shader.SetUniform1i("uTexture", 0);
		// clear 
		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		Renderer renderer;

		// gui setup
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGui::StyleColorsDark(); // Set Dark Style
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
		// Our state
		bool show_demo_window = true;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


		// for mvp
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 mv;
		glm::mat4 mvp;
		glm::mat3 normal;
	
		// for animate
		float r = 0.0f;
		float increment = 0.05f;
		glm::vec3 translation(0., 0., 0.);
		glm::vec3 scaler(1., 1., 1.);
		float rotater = 0;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();
			
			// update viewport when scaling window
			GLsizei vx, vy;
			glfwGetWindowSize(window, &vx, &vy);
			GLsizei v = vx < vy ? vx : vy;			// minimum dimension
			GLint xl = (vx - v) / 2;
			GLint yb = (vy - v) / 2;
			glViewport(xl, yb, v, v);

			//img
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				static float f = 0.0f;
				static int counter = 0;
				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
				ImGui::SliderFloat3("Translation", &translation.x, -1.f, 1.f);
				ImGui::SliderFloat3("Scale", &scaler.x, -2.f, 2.f);
				ImGui::SliderFloat("Rotation", &rotater, -200.f, 200.f);

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}
			
			// rotate and uniformly scale scene
			glm::vec3 eye(0., 0., 3.);
			glm::vec3 look(0., 0., 0.);
			glm::vec3 up(0., 1., 0.);
			view = glm::lookAt(eye, look, up);
			projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1000.f);
			//model = glm::mat4(1);
			//mv = view * model;
			//mvp = projection * view * model;
			//normal = glm::mat3(glm::inverseTranspose(mv));

			//view = glm::mat4(1);
			view = glm::rotate(view, glm::radians(Yrot), glm::vec3(0., 1., 0.));
			view = glm::rotate(view, glm::radians(Xrot), glm::vec3(1., 0., 0.));
			//Scale = std::max(Scale, MINSCALE);
			view = glm::scale(view, glm::vec3(Scale, Scale, Scale));
			model = glm::translate(glm::mat4(1), translation);
			model = glm::scale(model, scaler);
			//model = glm::mat4(1);
			//view = glm::scale(glm::mat4(1), glm::vec3(rotater));
			//view = glm::rotate(view, rotate)
			mvp = projection * view * model;

			shader.Bind();
			shader.SetUniformVec4((char*)"uColor", glm::vec4(r, 0.3f, 0.8f, 1.0f));
			shader.SetUniformMat4("uMVP", mvp);

			renderer.Draw(va, ib, shader);
			if (r > 1.0f) increment = -0.05f;
			else if (r < 0.0f) increment = 0.05f;
			r += increment;

			glColor3f( 1., 1., 0. );
			GLCall(glCallList(AxesList));

			// img 
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}


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

void Axes( float length )
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
		std::cout << "X!\n";
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
			std::cout << base + fact*xx[j] << "f, " << fact* xy[j] << "f, " << 0.0 << "f\n";
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		std::cout << "Y!\n";
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
			std::cout << fact*yx[j] << "f, " << base + fact*yy[j] << "f, " << 0.0 << "f\n";
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		std::cout << "Z!\n";
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
			std::cout << 0.0 << "f, " << fact*zy[j] << "f, " << base + fact*zx[j] << "f\n";
		}
	glEnd( );

}

