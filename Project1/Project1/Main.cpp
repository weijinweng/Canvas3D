#include "Externals.h"
#include "RenderSystem.h"
#include <cmath>

using namespace Canvas;

bool quit = false;

bool Up = false,
	Down = false,
	Left = false,
	Right  = false,
	accelerate = false,
	ShiftUp = false,
	ShiftDown = false;

float horAngle = 0, vertAngle = 0;

glm::vec3 position(0);

float fclamp(float value, float max, float min)
{
	if(value > max)
		return max;
	if(value < min)
		return min;
	return value;
}

void computeMatrix(glm::mat4* view)
{
	static int lastTime = 0;
	int deltaTime = SDL_GetTicks() - lastTime;
	lastTime = SDL_GetTicks();
	float multiplier = 1.0f;

	int x,y;
	SDL_GetMouseState(&x,&y);

	vertAngle += (y - 900/2)*(0.01);
	horAngle -= (x - 1200/2)*(0.01);

	vertAngle = fclamp(vertAngle, 3.14/2 + 3.14, -3.14/2 + 3.14);

	glm::vec3 direction(std::cos(vertAngle)*std::sin(horAngle),
						std::sin(vertAngle),
						std::cos(vertAngle)*std::cos(horAngle));

	glm::vec3 directionRight(std::sin(horAngle-3.14f/2.0f),
							 0,
							 std::cos(horAngle-3.14f/2.0f));


	float timeSeconds = (float)deltaTime/1000.0f;
	if(accelerate)
		multiplier = 3.0f;
	if(Up)
	{
		position += direction*timeSeconds*multiplier;
	}
	if(Down)
	{
		position -= direction*timeSeconds*multiplier;
	}
	if (Left)
	{
		position += directionRight*timeSeconds*multiplier;
	}
	if (Right)
	{
		position -= directionRight*timeSeconds*multiplier;
	}
	if (ShiftUp)
	{
		position.y += 2*timeSeconds*multiplier;
	}
	if (ShiftDown)
	{
		position.y -= 2*timeSeconds*multiplier;
	}

	glm::vec3 directionUp(glm::cross(direction,directionRight));
	*view = glm::lookAt(position,direction + position, directionUp);


}

int main(int argc, char* args[])
{
	CanvasWindow* window = CanvasWindow::CVS_CreateWindow(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 900, CVS_WDW_WINDOW);
	window->renderer;
	renderProgram firstProgram("first program", "./shaders/first.vert", "./shaders/first.frag");
	glUseProgram(firstProgram.programID);
	
	Texture texture1;
	texture1.loadFile("dragon_knight.jpg");
	GLint texture2Dloc = glGetUniformLocation(firstProgram.programID, "myTexture");
	glUniform1i(texture2Dloc,0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture1.textureID);


	Mesh newMesh;
	newMesh.loadMeshFromFile("dk.3ds");
	printf("number of indices is %d", newMesh.indices.size());
	GLuint MVPID = glGetUniformLocation(firstProgram.programID, "MVP");
	GLuint VID = glGetUniformLocation(firstProgram.programID,"V");
	GLuint MID = glGetUniformLocation(firstProgram.programID, "M");

	GLuint LightPosition_worldspace = glGetUniformLocation(firstProgram.programID, "LightPosition_worldspace");

	glUniform3f(LightPosition_worldspace, 3, 1, 3);

	glm::mat4 Perspect = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);

	glm::mat4 Model(1.0);

	Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.02));

	glm::mat4 View = glm::lookAt(glm::vec3(3,4,3), glm::vec3(0,0,0), glm::vec3(0,1,0));

	glm::mat4 MVP = Perspect  * View * Model;

	glBindVertexArray(newMesh.VAO);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
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
			if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						Up = true;
						break;
					case SDLK_s:
						Down = true;
						break;
					case SDLK_a:
						Left = true;
						break;
					case SDLK_d:
						Right = true;
						break;
					case SDLK_q:
						quit = true;
						break;
					case SDLK_x:
						ShiftUp = true;
						break;
					case SDLK_z:
						ShiftDown = true;
						break;
					case SDLK_LSHIFT:
						accelerate = true;
				}
			}
			if(e.type == SDL_KEYUP)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						Up = false;
						break;
					case SDLK_s:
						Down = false;
						break;
					case SDLK_a:
						Left = false;
						break;
					case SDLK_d:
						Right = false;
						break;
					case SDLK_x:
						ShiftUp = false;
						break;
					case SDLK_z:
						ShiftDown = false;
						break;
					case SDLK_LSHIFT:
						accelerate = false;
				}
			}
		}
		computeMatrix(&View);
		SDL_WarpMouseInWindow(window->windowHandler,1200/2,900/2);
		glm::mat4 MVP = Perspect  * View * Model;
		glUniformMatrix4fv(MVPID, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(VID,1,GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(MID,1,GL_FALSE, glm::value_ptr(Model));
		
		glDrawElements(GL_TRIANGLES, newMesh.indices.size(), GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window->windowHandler);
	}
}