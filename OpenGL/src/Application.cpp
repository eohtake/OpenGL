#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL ERROR] " << "("<< error <<"): " << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// ERROR HANDLING
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) 
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current. */
	glfwMakeContextCurrent(window);

	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW ERROR!\n";
	}
	else { // If glew is placed correctly.
		std::cout << "GLEW OK!\n";
	}

	// Creating a buffer. Dados que serao colcoados dentro do buffer.
	float positions[] = {
		-0.5f, -0.5f, // vertex 0
		 0.5f, -0.5f, // vertex 1
		 0.5f,  0.5f, // vertex 2
		-0.5f,  0.5f  // vertex 3
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	// Creates one vertex array object. The 1 literaly means create ONE vertex array object.
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao)); 
	GLCall(glBindVertexArray(vao));
	
	// Preparamos uma variavel para se tornar um vertex buffer object (VBO). Entretanto melhor utilizar um index buffer object
	// para economizar memoria.
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer)); // Gera 1 buffer e passa o endereco da variavel.
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); // Ativa o buffer, indicando o tipo deste buffer e o proprio VBO.
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW)); // Coloca os dados dentro do VBO
	
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)); // Primeiro parametro (0) aponta para o index do vao.
	
	/* INDEX BUFFER OBJECT - Renderizando utilizando indexes para economizar memoria*/
    
	// Preparamos uma variavel para se tornar um index buffer object (IBO)
	
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo)); // Gera 1 buffer e passa o endereco da variavel.
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); // Ativa o buffer, indicando o tipo deste buffer e o proprio index buffer object.
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW)); // Coloca os dados dentro do buffer

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));

	// Unbinding everything
	GLCall(glUseProgram(0));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


	float redChannel = 0.0f;
	float redChannelIncrement = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// bind the shader
		GLCall(glUseProgram(shader)); 

		GLCall(glUniform4f(location, redChannel, 0.3f, 0.8f, 1.0f));
		
		// bind the buffer and re-enable the attributes if not using Vertex Array Object (VAO)
		//GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); 
		//GLCall(glEnableVertexAttribArray(0));
		//GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		// Bind the vertex array object. This object is already bound with the attributes when glVertexAttribPointer was called.
		GLCall(glBindVertexArray(vao));

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo)); // bind the index buffer

		
		/* Utilizar a linha abaixo, caso esteja renderizando direto, sem index buffer */
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		if (redChannel > 1.0f)
			redChannelIncrement = -0.05f;
		else if (redChannel < 0.0f)
			redChannelIncrement = 0.05f;

		redChannel += redChannelIncrement;
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;
}