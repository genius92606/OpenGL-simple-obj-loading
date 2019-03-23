/*
CG HW1
Please fill the functions whose parameters are replaced by ... in the following function 
static int add_obj(unsigned int program, const char *filename)(line 140) 
static void render() (line 202)

For example : line 156 //glGenVertexArrays(...);

*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tiny_obj_loader.h"

#define GLM_FORCE_RADIANS

float aspect;
GLint modelLoc;
GLint projLoc;

struct object_struct{
	unsigned int program;
	unsigned int vao;
	unsigned int vbo[3];
	unsigned int texture;
	glm::mat4 model;

	//struct's constructor
	object_struct(): model(glm::mat4(1.0f)){}
} ;

std::vector<object_struct> objects;//vertex array object,vertex buffer object and texture(color) for objs
unsigned int program;

//a vector to store indices size(number) of a obj
//the size of this vector means how many objs do we want to draw
std::vector<int> indicesCount;//Number of indice of objs

static void error_callback(int error, const char* description)
{
	//to print the error message
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//if input "ESC" , then this function will close the window~~~ 
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static unsigned int setup_shader(const char *vertex_shader, const char *fragment_shader)
{
	GLuint vs=glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, (const GLchar**)&vertex_shader, nullptr);

	glCompileShader(vs);	//compile vertex shader

	int status, maxLength;
	char *infoLog=nullptr;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);		//get compile status
	if(status==GL_FALSE)								//if compile error
	{
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);	//get error message length

		/* The maxLength includes the NULL character */
		infoLog = new char[maxLength];
		
		glGetShaderInfoLog(vs, maxLength, &maxLength, infoLog);		//get error message

		fprintf(stderr, "Vertex Shader Error: %s\n", infoLog);

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		delete [] infoLog;
		return 0;
	}
	//	for fragment shader --> same as vertex shader
	GLuint fs=glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, (const GLchar**)&fragment_shader, nullptr);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
	if(status==GL_FALSE)
	{
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		infoLog = new char[maxLength];

		glGetShaderInfoLog(fs, maxLength, &maxLength, infoLog);

		fprintf(stderr, "Fragment Shader Error: %s\n", infoLog);

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		delete [] infoLog;
		return 0;
	}

	unsigned int program=glCreateProgram();
	// Attach our shaders to our program
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if(status==GL_FALSE)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);


		/* The maxLength includes the NULL character */
		infoLog = new char[maxLength];
		glGetProgramInfoLog(program, maxLength, NULL, infoLog);

		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		fprintf(stderr, "Link Error: %s\n", infoLog);

		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		delete [] infoLog;
		return 0;
	}
	return program;
}

static std::string readfile(const char *filename)
{
	std::ifstream ifs(filename);
	if(!ifs)
		exit(EXIT_FAILURE);
	return std::string( std::istreambuf_iterator<char>(ifs),
			std::istreambuf_iterator<char>());
}

static int add_obj(unsigned int program, const char *filename)
{
	object_struct new_node;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, filename);

	if (!err.empty()||shapes.size()==0)
	{
		std::cerr<<err<<std::endl;
		exit(1);
	}

	//Generate memory for buffers.

	//to generate the VAO
	glGenVertexArrays(1, &new_node.vao);

	//to generate the VBOs
	glGenBuffers(3, new_node.vbo);


	glGenTextures(1,&new_node.texture);

	//Tell the program which VAO I am going to modify
	//to start VAO to store(point to) the information of VBO
	glBindVertexArray(new_node.vao);

	// Upload postion array

	//to bind the vbo[0] to GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, new_node.vbo[0]);

	//to send our obj information to vbo[0](GL_ARRAY_BUFFER)
	glBufferData(
		GL_ARRAY_BUFFER,										//the destination of our copying data
		sizeof(GLfloat)*shapes[0].mesh.positions.size(),		//the size of our copying data
		shapes[0].mesh.positions.data(),						//copying data
		GL_STATIC_DRAW											//management of this data
	);

	//to enable the vertex attribute buffer (location =0)
	glEnableVertexAttribArray(0);

	//if stride is set to 0 , it means that the stride will be determined by OpenGL
	//to give the data to position in vertex shader (location =0)
	glVertexAttribPointer(
		0,					//the vertex attribute we want to set (by location)
		3,					//depends on how much data we want to give to the vertex attribute
		GL_FLOAT,			//the type of our data,
		GL_FALSE,			//we want to normalize our data or not
		0,					//the stride
		0					//offset
	);					


	if(shapes[0].mesh.normals.size()>0)
	{
		//跟上面很像
		// Upload normal array
		glBindBuffer(GL_ARRAY_BUFFER, new_node.vbo[1]);
		glBufferData(
			GL_ARRAY_BUFFER,										//the destination of our copying data
			sizeof(GLfloat)*shapes[0].mesh.positions.size(),		//the size of our copying data
			shapes[0].mesh.positions.data(),						//copying data
			GL_STATIC_DRAW											//management of this data
		);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3, GL_FLOAT,GL_FALSE,0,0);
	}

	// Setup index buffer for glDrawElements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_node.vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(GLuint)*shapes[0].mesh.indices.size(), 
		shapes[0].mesh.indices.data(), 
		GL_STATIC_DRAW);
	indicesCount.push_back(shapes[0].mesh.indices.size());


	
	//glBindVertexArray(0);

	new_node.program = program;

	objects.push_back(new_node);
	return objects.size()-1;
}

static void releaseObjects()
{
	//to release all objs
	for(int i=0;i<objects.size();i++){
		glDeleteVertexArrays(1, &objects[i].vao);
		glDeleteTextures(1, &objects[i].texture);
		glDeleteBuffers(4, objects[i].vbo);
	}
	//to release the shader
	glDeleteProgram(program);
}

static void render()
{
	float scale = 15.0;
	float time = glfwGetTime() / 100.0f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glm::mat4 proj_matrix, model_matrix, view_matrix, eye(1.0f);

	//set camera matrix
	//proj_matrix = /*glm::frustum(...) or glm::perspective(...)*/;
	//view_matrix = /*glm::lookAt(...) * glm::mat4(1.0f)*/;

	proj_matrix = glm::frustum(-aspect / scale, aspect / scale, -1.0f / scale, 1.0f / scale, 0.10f, 1000.0f);
	view_matrix = glm::lookAt(glm::vec3(2.0f), glm::vec3(), glm::vec3(0, 1, 0)) * glm::mat4(1.0f);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj_matrix*view_matrix));

	for(int i=0;i<objects.size();i++){

		//Bind VAO
		glBindVertexArray(objects[i].vao);
		//If you don't want to rotate or move your object, you can comment the functions below.
		model_matrix = glm::translate(eye, glm::vec3(0.0f))
			* glm::rotate(eye, 98.70f * time, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::rotate(eye, 123.40f * time, glm::vec3(1.0f, 0.0f, 0.0f));
			
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
		
		//Draw object
		glDrawElements(GL_TRIANGLES, indicesCount[i], GL_UNSIGNED_INT, nullptr);
	}

	//Unbind VAO
	glBindVertexArray(0);
}

static void reshape(GLFWwindow* window, int width, int height)
{
	aspect = (float) width / height;
	glViewport(0, 0, width, height);
}	

void init_shader()
{
	modelLoc = glGetUniformLocation(program, "model");
	projLoc	 = glGetUniformLocation(program, "proj");
}

int main(int argc, char *argv[])
{
	GLFWwindow* window;

	//set error function
	glfwSetErrorCallback(error_callback);

	//initialization
	if (!glfwInit())
		exit(EXIT_FAILURE);
	// OpenGL 3.3, Mac OS X is reported to have some problem. However I don't have Mac to test
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//set hint to glfwCreateWindow, (target, hintValue)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//hint--> window not resizable,  explicit use core-profile,  opengl version 3.3
	// For Mac OS X
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(800, 600, "Simple Example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();   //end of glfw
		return EXIT_FAILURE;
	}
	glfwSetFramebufferSizeCallback(window, reshape);
	reshape(window, 800, 600);

	glfwMakeContextCurrent(window);	//set current window as main window to focus

	//tell glew to use more modern technique for managing OpenGL funtionality
	// This line MUST put below glfwMakeContextCurrent
	glewExperimental = GL_TRUE;		//tell glew to use more modern technique for managing OpenGL functionality
	glewInit();

	// Enable vsync(vertical synchronization)
	glfwSwapInterval(1);    //set the number of screen updates to wait from the time

	// Setup input callback
	glfwSetKeyCallback(window, key_callback);	//set key event handler

	// load shader program
	program = setup_shader(readfile("light.vert").c_str(), readfile("light.frag").c_str());
	//program = setup_shader(readfile("vs.txt").c_str(), readfile("fs.txt").c_str());
	init_shader();

	if (argc == 1)
		add_obj(program, "mug.obj");
	else
		add_obj(program, argv[1]);
	
	//to enable z buffer
	glEnable(GL_DEPTH_TEST);
	// prevent faces rendering to the front while they're behind other faces. 

	//cull those faces which are not in our sight
	glCullFace(GL_BACK);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		//program will keep draw here until you close the window
		render();
		
		glfwSwapBuffers(window);	//swap the color buffer and show it as output to the screen.
		glfwPollEvents();			//check if there is any event being triggered
	}

	releaseObjects();
	glfwDestroyWindow(window);

	//end of glfw
	glfwTerminate();
	return EXIT_SUCCESS;
}
