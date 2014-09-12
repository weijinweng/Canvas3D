#include "Externals.h"
#include "RenderSystem.h"

using namespace Canvas;

bool quit = false;

int main(int argc, char* args[])
{
	CanvasWindow* window = CanvasWindow::CVS_CreateWindow(20, 20, 500, 500, CVS_WDW_WINDOW);
	window->renderer;
	renderProgram firstProgram("first program", "./shaders/first.vert", "./shaders/first.frag");
	Mesh newMesh;
	newMesh.loadMeshFromFile("suzanne.obj");
	printf("number of indices is %d", newMesh.indices.size());
	GLuint MVPID = glGetUniformLocation(firstProgram.programID, "MVP");
	GLuint VID = glGetUniformLocation(firstProgram.programID,"V");
	GLuint MID = glGetUniformLocation(firstProgram.programID, "M");

	GLuint LightPosition_worldspace = glGetUniformLocation(firstProgram.programID, "LightPosition_worldspace");

	glm::mat4 Perspect = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);

	glm::mat4 Model(1.0f);

	glm::mat4 View = glm::lookAt(glm::vec3(3,4,3), glm::vec3(0,0,0), glm::vec3(0,1,0));

	glm::mat4 MVP = Perspect * Model * View;

	glBindVertexArray(newMesh.VAO);

	while(!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SDL_Event e;
		while(SDL_PollEvent(&e) > 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		glUniformMatrix4fv(MVPID, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(VID,1,GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(MID,1,GL_FALSE, glm::value_ptr(Model));

		glDrawElements(GL_TRIANGLES, newMesh.indices.size(), GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window->windowHandler);
	}
}