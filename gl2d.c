//#define GL_INFO
//#define GL_SHADER_DEBUG

// for 2d rendering with OpenGL in an SDL program

/*
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_opengles2.h"

SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
SDL_Window* winp = SDL_CreateWindow("h", 0, 0, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
SDL_GLContext* conp = gl_setup(winp);

GLuint shaders = {
SOLID_SHAD("0.0f, 0.0f, 0.0f")
};

int w, h;
SDL_GetWindowSize(winp, &w, &h);
glViewport(0, 0, w, h);

glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

drawNoCamera(10, 5 2d points, GL_TRIANGLE_STRIP);
drawPoints(&camera, 10, 5 2d points, offset x, offset y, GL_TRIANGLE_STRIP or other mode);
drawPoints changes the points
SDL_GL_SwapWindow(winp);


SDL_GL_DeleteContext(conp);
SDL_Quit();


 */
typedef struct Camera {
	float x0;
	float y0;
	float x2; // half-width and half-height, really
	float y2;
} Camera;

void drawNoCamera(int len, const float* points, GLenum mode) {
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glDrawArrays(mode, 0, len / 2);
	glDeleteBuffers(1, &vbo);
}

void drawPoints(Camera* camp, int len, float* points, float x, float y, GLenum mode) { // changes state of array
	float xoff = (camp->x2 + camp->x0) / 2 - x;
	float xscl = (camp->x2 - camp->x0) / 2;
	float yoff = (camp->y2 + camp->y0) / 2 - y;
	float yscl = (camp->y2 - camp->y0) / 2;
//	printf("rendering at %f %f %f %f\n", x, y, points[0], points[1]);
	for (int i = 0; i < len;) {
		points[i] -= xoff;
		points[i] /= xscl;
		++i;
		points[i] -= yoff;
		points[i] /= yscl;
		//printf("%d %f %f\n", len, points[i - 1], points[i]);
		++i;
	}
	drawNoCamera(len, points, mode);
}

SDL_GLContext* gl_setup(SDL_Window* winp) {
	SDL_GLContext* conp = SDL_GL_CreateContext(winp);
#ifdef GL_INFO
	printf("OpenGL version %s\n"
	       "GLSL version %s\n"
	       "GL vendor %s\n"
	       "GL renderer %s\n",
	       glGetString(GL_VERSION),
	       glGetString(GL_SHADING_LANGUAGE_VERSION),
	       glGetString(GL_VENDOR),
	       glGetString(GL_RENDERER));
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("OpenGL version %d.%d\n", major, minor);
#endif
	glEnable(GL_DEBUG_OUTPUT);
	return conp;
}

GLuint gl_shader_setup(const char* vsrc, const char* fsrc) {
//	char log[512];
	int res = 0;
	GLuint vert_num = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_num, 1, &vsrc, NULL);
	glCompileShader(vert_num);
	glGetShaderiv(vert_num, GL_INFO_LOG_LENGTH, &res);
#ifdef GL_SHADER_DEBUG
	printf("%d == 0  vertex shader compilation\n", res);
#endif

	GLuint frag_num = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_num, 1, (const char**) &fsrc, NULL);
	glCompileShader(frag_num);
	glGetShaderiv(frag_num, GL_INFO_LOG_LENGTH, &res);
#ifdef GL_SHADER_DEBUG
	printf("%d == 0  fragment shader compilation\n", res);
#endif
//	glGetShaderiv(vert_num, GL_COMPILE_STATUS, &res); res == 1
	GLuint shad = glCreateProgram();
	glAttachShader(shad, vert_num);
	glAttachShader(shad, frag_num);
	glLinkProgram(shad);
	glGetProgramiv(shad, GL_INFO_LOG_LENGTH, &res);
#ifdef GL_SHADER_DEBUG
	printf("%d == 0  shader program link\n", res);
#endif
//	glGetProgramiv(shad, GL_LINK_STATUS, &res);
	//glUseProgram(shad);
	glDeleteShader(vert_num);
	glDeleteShader(frag_num);
	glUseProgram(shad);
	return shad;
}

GLuint gl_shaderSetupSolid(const char* frag_src) {
	const char* vert_src = "#version 460 core\n"
		"layout (location = 0) in vec2 aPos;"
		"void main()"
		"{ gl_Position = vec4(aPos.x, -aPos.y, 0.0, 1.0); }";
#ifdef GL_SHADER_DEBUG
	printf("%s\n", frag_src);
#endif
	gl_shader_setup(vert_src, frag_src);
}

#define SOLID_SHAD(gerald) gl_shaderSetupSolid("#version 460 core\nout vec4 FragColor;void main() { FragColor = vec4(" gerald ", 1.0f); }")
