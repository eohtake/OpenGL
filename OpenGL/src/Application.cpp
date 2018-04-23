#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"


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
			-0.5f, -0.5f, 0.0f, 0.0f, // vertex 0
			 0.5f, -0.5f, 1.0f, 0.0f, // vertex 1
			 0.5f,  0.5f, 1.0f, 1.0f,// vertex 2
			-0.5f,  0.5f, 0.0f, 1.0f  // vertex 3
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		// Create a Vertex Buffer
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
				
		//TODO: Planejamento buffer layout class
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		
		/* INDEX BUFFER OBJECT - Renderizando utilizando indexes para economizar memoria*/
		IndexBuffer ib(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		Texture texture("res/textures/japan.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);
		
		// Unbinding everything
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		float redChannel = 0.0f;
		float redChannelIncrement = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			
			/* RENDER HERE */

			renderer.Clear();
			
			shader.Bind();
			shader.SetUniform4f("u_Color", redChannel, 0.3f, 0.8f, 1.0f);
			
			renderer.Draw(va, ib, shader);

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