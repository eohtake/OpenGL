#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"


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

	{ // Scope to allow OpenGL run all its functions before the end of the context.

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
		//unsigned int vao;
		//GLCall(glGenVertexArrays(1, &vao));
		//GLCall(glBindVertexArray(vao));

		//TODO: Planejamento Vertex Array Class
		VertexArray va;
		// Create a Vertex Buffer
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));
				
		//TODO: Planejamento buffer layout class
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		
		/* INDEX BUFFER OBJECT - Renderizando utilizando indexes para economizar memoria*/
		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
		
		// Unbinding everything
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		float redChannel = 0.0f;
		float redChannelIncrement = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			// bind the shader
			shader.Bind();
			shader.SetUniform4f("u_Color", redChannel, 0.3f, 0.8f, 1.0f);

			// bind the buffer and re-enable the attributes if not using Vertex Array Object (VAO)
			//GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); 
			//GLCall(glEnableVertexAttribArray(0));
			//GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

			// Bind the vertex array object. This object is already bound with the attributes when glVertexAttribPointer was called.
			//GLCall(glBindVertexArray(vao)); // TODO: Comentar esta linha quando a classe VertexArray for implementada
			va.Bind(); // bind the vertex array
			
			ib.Bind(); // Bind the index buffer.

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
	} // End of the big scope.

	glfwTerminate();
	return 0;
}