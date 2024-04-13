#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>
#include <format>
#include <string>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "glsl_reader.h"
#include "Texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

// multiplication factors for input interaction:
const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;
//const float MINSCALE = 0.05f;
const float MINSCALE = 1e-30f;
const float SCROLL_WHEEL_CLICK_FACTOR = 1.f;
const int LEFT   = 0; // per https://www.glfw.org/docs/3.3/group__buttons.html
const int RIGHT  = 1;
const int MIDDLE = 2;

const int TIME_MULTIPLIER = 1000000;

// non-constant global variables:
int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		NowProjection;		    // ORTHO or PERSP
float	Scale = 1.f;			// scaling factor
float	Xrot, Yrot;				// rotation angles in degrees
double Xmouse, Ymouse;
int PressedButton = -1;

// for mvp
glm::vec3 eye(0.5, 3., 3.f);
glm::vec3 look(0., 0., 0.);
glm::vec3 up(0., 1., 0.);
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normal;

// for animate
float r = 0.0f;
float increment = 0.05f;
double cur_time_sec; 
double cur_time_ms; 


// for imgui
bool show_demo_window = true;
bool show_another_window = true;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
glm::vec3 translation(0., 0., 0.);
glm::vec3 scaler(1., 1., 1.);
float rotater = 0;
float framerate;


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void update_viewport_size(GLFWwindow* window);
void imgui_show_example_frame();
void imgui_show_sand_frame();
glm::mat4 mv() { return view * model; }
glm::mat4 mvp() { return projection * view * model; }
glm::mat3 n(glm::mat4 mat) { return glm::mat3(glm::inverseTranspose(mat)); }

// for bodies
void initBody();

unsigned int body_vao;
unsigned int body_vbo;
unsigned int body_ibo;
struct body 
{
	glm::f64 mass;
	glm::f64vec3 position;
	glm::f64vec3 velocity;
	glm::f64 radius;
	glm::vec3 color;
};

glm::f64 gravitationalForce(body* b0, body* b1)
{
	// F = gravitational force between two masses m1 and m2 which are r distance apart. G is gravitational constant. 
	// F = G * m_1 * m_2 / r ^ 2;
	glm::f64 G = 6.674e-11; // (N*m^2) / kg^2
	glm::f64 r = glm::distance(b0->position, b1->position);
	if (r == 0) return 0;
	return (G * b0->mass * b1->mass) / (r*r);
}
void updateBodies(body* b0, body* b1) {
	// Update positions
	//b0->position += b0->velocity * (1.f / framerate);
	//b1->position += b1->velocity * (1.f / framerate);
	//b2->position += b2->velocity * (1.f / framerate);
	glm::f64 dt = 1.0f / framerate;
	dt *= TIME_MULTIPLIER;
	b0->position += b0->velocity * dt;
	b1->position += b1->velocity * dt;


	glm::f64 gF01 = gravitationalForce(b0, b1);

	glm::f64vec3 dir01 = normalize(b1->position - b0->position);
	glm::f64vec3 dir10 = -dir01;

	glm::f64vec3 f01 = dir01 * gF01;
	glm::f64vec3 f10 = dir10 * gF01;

	glm::f64vec3 f0 = f01;
	glm::f64vec3 f1 = f10;

	glm::f64vec3 a0 = f0 / b0->mass;
	glm::f64vec3 a1 = f1 / b1->mass;
	
	b0->velocity += a0 * dt;
	b1->velocity += a1 * dt;
}

// for axes
void initAxes();
unsigned int axes_vao;
unsigned int axes_vbo;
unsigned int axes_ibo;

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
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 1200, "Chladni Plate Simulation", NULL, NULL);
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
	glfwSetKeyCallback(window, key_callback);

    GLenum err = glewInit();
    if (err != GLEW_OK) 
        std::cout << "Error: %s\n" << glewGetErrorString(err) << std::endl;
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;


	{ // this block is to avoid infinite loop of glCheckError which provides an error when there is no gl context. stops infinite loop when we close the openggl window. 
		// texture blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glBlendEquation(GL_FUNC_ADD)); // default, but being explicit

		initAxes();
		initBody();
		float axes_len = 1.f;
		body earth;
		earth.mass		= 5.9722e24;
		earth.radius	= 6.371e6;
		earth.position	= glm::f64vec3(0.f);
		earth.color		= glm::vec3(1, 0, 0);
		earth.velocity	= glm::f64vec3(0.f);

		body moon;
		moon.mass		= 7.348e22;
		moon.radius		= 1.7374e6;
		moon.position	= glm::f64vec3(0.f, 0.f, 3.84e8);
		moon.color		= glm::vec3(0, 1, 0);
		moon.velocity	= glm::f64vec3(1022, 0, 0);

		/* SETUP SHADERS */
		Shader axes_shader = Shader("res/shaders/Axes.glsl");
		Shader body_shader = Shader("res/shaders/Body.glsl");

		Renderer renderer;

		/* SETUP GUI */
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGui::StyleColorsDark(); // Set Dark Style
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();
			cur_time_sec = glfwGetTime();
			cur_time_ms = 1000.f * cur_time_sec;

			// DEPTH BUFFER
			GLCall(glDrawBuffer(GL_BACK));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			GLCall(glEnable(GL_DEPTH_TEST));
			glClearColor(0.15f, 0.15f, 0.35f, 1.0f);
			
			update_viewport_size(window); // update viewport size after scaling window

			//img
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			framerate = io.Framerate;
			//imgui_show_example_frame();
			imgui_show_sand_frame();

			// rotate and uniformly scale scene
			model = glm::mat4(1);
			view = glm::lookAt(eye, look, up);
			//view = glm::lookAt(glm::vec3(1.f), look, up);
			projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1000.f);
			//projection = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 1000.f);
			view = glm::rotate(view, glm::radians(Yrot), glm::vec3(0., 1., 0.));
			view = glm::rotate(view, glm::radians(Xrot), glm::vec3(1., 0., 0.));
			view = glm::scale(view, glm::vec3(Scale, Scale, Scale));


			// DRAW BODIES
			body_shader.Bind();
			GLCall(glBindVertexArray(body_vao));
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, body_ibo));

			// earth
			GLCall(glPointSize(15 * 2));
			model = glm::translate(glm::f64mat4(1), earth.position / 3e8);
			body_shader.SetUniformVec3("uColor", earth.color);
			body_shader.SetUniform1f("uRadius", earth.radius);
			body_shader.SetUniformMat4("uMVP", mvp());
			GLCall(glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0));
			// moon
			GLCall(glPointSize(15 * 2));
			model = glm::translate(glm::f64mat4(1), moon.position / 3e8);
			body_shader.SetUniformVec3("uColor", moon.color);
			body_shader.SetUniform1f("uRadius", moon.radius);
			body_shader.SetUniformMat4("uMVP", mvp());
			GLCall(glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0));

			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

			updateBodies(&moon, &earth);

			// DRAW AXES
			axes_shader.Bind();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1));
			axes_shader.SetUniformMat4("uMVP", mvp());
			GLCall(glBindVertexArray(axes_vao));
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axes_ibo));
			GLCall(glLineWidth(3));
			GLCall(glEnable(GL_LINE_SMOOTH)); // enable antialiasing
			GLCall(glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, 0));
			GLCall(glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, (const GLvoid*) (5 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, (const GLvoid*) (7 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, (const GLvoid*) (9 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, (const GLvoid*) (14 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, (const GLvoid*) (18 * sizeof(unsigned int)))); 
			GLCall(glLineWidth(1));
			GLCall(glDisable(GL_LINE_SMOOTH)); // disable antialiasing
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


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


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
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

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) 
{
	double dx = xpos - Xmouse;
	double dy = ypos - Ymouse;

	if ( PressedButton == GLFW_MOUSE_BUTTON_RIGHT ) 
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	else if( PressedButton == GLFW_MOUSE_BUTTON_MIDDLE ) 
	{
		//std::cout << "scaler" << std::endl;
		Scale += SCLFACT * (float) ( dx - dy );
		Scale = std::max(Scale, MINSCALE);
		//std::cout << "scale: " << Scale << std::endl;
	}
	Xmouse = xpos;			// new current position
	Ymouse = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
	//	if (key == GLFW_KEY_D && action == GLFW_PRESS) { uChladni_DX += 1; }
	//	else if (key == GLFW_KEY_A && action == GLFW_PRESS) { uChladni_DX -= 1; }
	//	else if (key == GLFW_KEY_W && action == GLFW_PRESS) { uChladni_DZ += 1; }
	//	else if (key == GLFW_KEY_S && action == GLFW_PRESS) { uChladni_DZ -= 1; }
	//}
	//else if (key == GLFW_KEY_D && action == GLFW_PRESS) { uChladni_N += 1; }
	//else if (key == GLFW_KEY_A && action == GLFW_PRESS) { uChladni_N -= 1; }
	//else if (key == GLFW_KEY_W && action == GLFW_PRESS) { uChladni_M += 1; }
	//else if (key == GLFW_KEY_S && action == GLFW_PRESS) { uChladni_M -= 1; }
	//else if (key == GLFW_KEY_R && action == GLFW_PRESS) { uQuickRespawn = true; }
	//else if (key == GLFW_KEY_Z && action == GLFW_PRESS) { uChladni_M = rand()%20, uChladni_N = rand()%20; }
}

void update_viewport_size(GLFWwindow* window)
{
	GLsizei vx, vy;
	glfwGetWindowSize(window, &vx, &vy);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);
}

std::string intToStrComma(int value1, int value2) {
    // Use std::to_string to convert integers to strings and concatenate them
    return std::to_string(value1) + "/" + std::to_string(value2);
}

void imgui_show_sand_frame()
{
	//static float f = 0.0f;
	//static int counter = 0;
	ImGui::Begin("Bodies");                          // Create a window called "Hello, world!" and append into it.
	//ImGui::SliderFloat("uPlateNormalScale", &uPlateNormalScale, 0.001f, 1.f);
	//ImGui::SliderFloat("uCubeSize", &uCubeSize, 0.001f, 0.2f);
	//ImGui::SliderFloat("uGravityMetersPerSec", &uGravityMetersPerSec, -10.f, 10.f);
	//ImGui::SliderFloat("uSpawnHeight", &uSpawnHeight, -1.f, 10.f);
	//ImGui::SliderFloat("uDeathHeight", &uDeathHeight, -5.f, 5.f);
	//ImGui::SliderFloat3("uPlateColor", &uPlateColor.x, 0.f, 1.f);
	//ImGui::SliderFloat("uPlateDim", &uPlateDim, 0.0f, 4.f);
	////ImGui::SliderFloat("uBounceFactor", &uBounceFactor, 0.0f, 1.f);
	//ImGui::SliderFloat("uChladniResAmp", &uChladniResAmp, 0.0f, 5.f);
	//if (ImGui::IsItemActive()) {
	//	lastChladniResAmp = uChladniResAmp;
	//}
	//ImGui::SliderInt("uChladni_N", &uChladni_N, 0, 40);
	//ImGui::SliderInt("uChladni_M", &uChladni_M, 0, 40);
	//ImGui::SliderFloat("uChladni_DX", &uChladni_DX, -2.f, 2.f);
	//ImGui::SliderFloat("uChladni_DZ", &uChladni_DZ, -2.f, 2.f);
	//ImGui::Checkbox("uUseChladniNormalsForLighting", &uUseChladniNormalsForLighting);
	//ImGui::Checkbox("uUseChladniNormals", &uUseChladniNormals);
	//if (ImGui::Button("uQuickRespawn"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//	uQuickRespawn = true;
	//ImGui::Checkbox("uSignalMode", &uPulseMode);

	//bool doRespawn = false;
	//for (int n = 1; n < 10; n++) {
	//	ImGui::Button(intToStrComma(n, 1).c_str());
	//	if (ImGui::IsItemActive()) {
	//		uChladni_N = n; 
	//		uChladni_M = 1; 
	//		uQuickRespawn = doRespawn; 
	//		uChladniResAmp = lastChladniResAmp;
	//	}
	//	for (int m = 2; m < 10; m++) {
	//		ImGui::SameLine(); 
	//		ImGui::Button(intToStrComma(n, m).c_str());
	//		if (ImGui::IsItemActive()) {
	//			uChladni_N = n; 
	//			uChladni_M = m; 
	//			uQuickRespawn = doRespawn; 
	//			uChladniResAmp = lastChladniResAmp;
	//		}
	//	}
	//}

	//if (ImGui::Button("Random (n, m)"))
	//{ 
	//	uChladni_N = rand()%20; 
	//	uChladni_M = rand()%20; 
	//	uQuickRespawn = doRespawn; 
	//}
	//ImGui::SliderFloat3("Scale", &scaler.x, -2.f, 2.f);
	//ImGui::SliderFloat("Rotation", &rotater, -200.f, 200.f);

	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//	counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
	ImGui::End();

	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}
}
void imgui_show_example_frame()
{
	static float f = 0.0f;
	static int counter = 0;
	ImGui::Begin("Chladni Plate Simulation");                          // Create a window called "Hello, world!" and append into it.
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

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
	ImGui::End();
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void initBody() {
	/* SETUP AXES */
	float body_positions[] = {0.0f, 0.0f, 0.0f }; 
	unsigned int body_indices[] = {0};

	//unsigned int axes_vao;
	GLCall(glGenVertexArrays(1, &body_vao));
	GLCall(glBindVertexArray(body_vao));

	//unsigned int axes_vbo;
	GLCall(glGenBuffers(1, &body_vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, body_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 3 * 1 * sizeof(float), body_positions, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0)); // sets to currently bound vao
	//GLCall(glEnableVertexArrayAttrib(axes_vao, 0)); // should do the same thing? 
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), 0))
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	//unsigned int axes_ibo;
	GLCall(glGenBuffers(1, &body_ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, body_ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1 * sizeof(unsigned int), body_indices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
void initAxes() {
	/* SETUP AXES */
	float axes_positions[] = {
		 1.0f, 0.0f, 0.0f, // lines
		 0.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 1.0f,

		 1.1f, -0.05f, 0.0f, // X
		 1.2f,  0.05f, 0.0f,
		 1.1f,  0.05f, 0.0f,
		 1.2f, -0.05f, 0.0f,

		 0.0f,  1.1f,  0.0f, // Y
		 0.0f,  1.16f, 0.0f,
		-0.05f, 1.2f,  0.0f,
		 0.0f,  1.16f, 0.0f,
		 0.05f, 1.2f,  0.0f,

		 0.0f,  0.05f, 1.2f, // Z
		 0.0f,  0.05f, 1.1f,
		 0.0f, -0.05f, 1.2f,
		 0.0f, -0.05f, 1.1f,
		 0.0f,  0.0f,  1.125f,
		 0.0f,  0.0f,  1.175f,
	};
	unsigned int axes_indices[] = {
		0, 1, 2, 3, 4,         // lines
		5, 6, 7, 8,            // X
		9, 10, 11, 12, 13,     // Y
		14, 15, 16, 17, 18, 19 // Z	
	};

	//unsigned int axes_vao;
	GLCall(glGenVertexArrays(1, &axes_vao));
	GLCall(glBindVertexArray(axes_vao));

	//unsigned int axes_vbo;
	GLCall(glGenBuffers(1, &axes_vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, axes_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 3 * 20 * sizeof(float), axes_positions, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0)); // sets to currently bound vao
	//GLCall(glEnableVertexArrayAttrib(axes_vao, 0)); // should do the same thing? 
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), 0))
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	//unsigned int axes_ibo;
	GLCall(glGenBuffers(1, &axes_ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axes_ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 20 * sizeof(unsigned int), axes_indices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
