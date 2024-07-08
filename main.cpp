#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//static verts and indicies for testing shape
#define X 0.525731112119133606
#define Z 0.850650808352039932
static float vdata[36] = {
	-X, 0.0, Z, X, 0.0, Z, -X, 0.0, -Z, X, 0.0, -Z,
	0.0, Z, X, 0.0, Z, -X, 0.0, -Z, X, 0.0, -Z, -X,
	Z, X, 0.0, -Z, X, 0.0, Z, -X, 0.0, -Z, -X, 0.0
};
static unsigned int tindicies[60] = {
	1,4,0, 4,0,9, 4,5,9, 8,5,4, 1,8,4,
	1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
	3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
	10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
};
const char* vertexShaderSource = "#version 450 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main(int argc, char* argv[]) {

	//Set up GLFW and the correct OpenGL version
	if (!glfwInit()) {
		std::cout << "failed to initialize GLFW" << std::endl;
		return 0;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//Create the window and check it's creation
	GLFWwindow* myWindow = glfwCreateWindow(1280, 720, "Renderer", NULL, NULL);
	if (!myWindow) {
		std::cout << "failed to create window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(myWindow);

	//set up GLAD and load in OpenGL commands
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//finish setting up openGL and making the viewport
	glViewport(0, 0, 1280, 720);
	glfwSetFramebufferSizeCallback(myWindow, framebuffer_size_callback);

	//set up our example vertex and fragment shader
	//-----------------
	//create id for vertex shader, attach the 'file' to it, and then compile
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//check for errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "vertex shader compile error: " << infoLog << std::endl;
	}
	//create id for fragment shader, attach the 'file' to it, and then compile
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//check for errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "fragment shader compile error: " << infoLog << std::endl;
	}
	//link the shader to the program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//check for link errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "shader linking error: " << infoLog << std::endl;
	}
	//now delete the shader programs
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//-----------------

	//create the buffers for the Icosahedron
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//first bind VAO, then VBO and EBO and attach their data
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tindicies), tindicies, GL_STATIC_DRAW);
	//add data to the VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Set up the render loop and drawing steps
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glfwSwapInterval(1);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(myWindow)) {
		processInput(myWindow);

		//render
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwPollEvents();
		glfwSwapBuffers(myWindow);
	}

	//terminate the program
	glfwDestroyWindow(myWindow);
	glfwTerminate();
	return 0;
}

//function to auto resize openGL viewport with window size
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//function to handle input
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}