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

void computeMatrix(fpsCamera* cam)
{
	static int lastTime = 0;
	int deltaTime = SDL_GetTicks() - lastTime;
	lastTime = SDL_GetTicks();
	float multiplier = 1.0f;

	int x,y;
	SDL_GetMouseState(&x,&y);
	cam->verticleAngle += (y-900/2)*(0.01);
	cam->horizontalAngle -= (x - 1200/2)*(0.01);

	cam->verticleAngle = fclamp(cam->verticleAngle, 3.14/2 + 3.14, -3.14/2 + 3.14);

	glm::vec3 direction(std::cos(cam->verticleAngle)*std::sin(cam->horizontalAngle),
						std::sin(cam->verticleAngle),
						std::cos(cam->verticleAngle)*std::cos(cam->horizontalAngle));

	glm::vec3 directionRight(std::sin(cam->horizontalAngle-3.14f/2.0f),
							 0,
							 std::cos(cam->horizontalAngle-3.14f/2.0f));
	float timeSeconds = (float)deltaTime/1000.0f;
	if(accelerate)
		multiplier = 3.0f;
	if(Up)
	{
		cam->position += direction*timeSeconds*multiplier;
	}
	if(Down)
	{
		cam->position -= direction*timeSeconds*multiplier;
	}
	if (Left)
	{
		cam->position += directionRight*timeSeconds*multiplier;
	}
	if (Right)
	{
		cam->position -= directionRight*timeSeconds*multiplier;
	}
	if (ShiftUp)
	{
		cam->position.y += 2*timeSeconds*multiplier;
	}
	if (ShiftDown)
	{
		cam->position.y -= 2*timeSeconds*multiplier;
	}
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
	printf("%d\n", deltaTime);

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
	Scene* myScene = window->renderer.createNewScene();

	std::vector<Mesh*> meshes = myScene->loadFromFile("test2.obj");
	fpsCamera mycam(45.0f, 4.0f/3.0f, glm::vec3(0,0,0));
	myScene->cameras.push_back(&mycam);
	printf("Mesh number %d\n", meshes.size());
	for(int i = 0, e = meshes.size(); i < e; ++i)
	{
		printf("Mesh vao is %d\n", meshes[i]->VAO);
	}
	/*auto bodyTexture = window->renderer.createNewTexture("Body");

	bodyTexture->loadFile("dragon_knight.jpg");

	renderNode* node = myScene->getNode("knight:kni");
	node->setTexture("myTextureSampler", "Body");

	Texture* weaponTexture = window->renderer.createNewTexture("Weapon");
	weaponTexture->loadFile("knight_weapon_color.jpg");

	renderNode* weaponsNode = myScene->getNode("weapon:wea");
	weaponsNode->setTexture("myTextureSampler", "Weapon");

	Texture* skirtsTexture = window->renderer.createNewTexture("Skirts");
	skirtsTexture->loadFile("skirt_color.jpg");

	renderNode* skirtsNode = myScene->getNode("skirt1:ski");
	skirtsNode->setTexture("skirts1:ski", "Skirts");
	
	Texture* shoulderTexture = window->renderer.createNewTexture("Shoulder");
	shoulderTexture->loadFile("shoulders_color.jpg");
	
	renderNode* shoulderNode = myScene->getNode("shoulders:");
	shoulderNode->setTexture("myTextureSampler", "Shoulder");

	window->renderer.createNewTexture("helmet")->loadFile("helmet_color.jpg");

	myScene->getNode("helmet1:he")->setTexture("myTextureSampler", "helmet");

	window->renderer.createNewTexture("shield")->loadFile("shield_color.jpg");

	myScene->getNode("shield:shi")->setTexture("myTextureSampler", "shield");*/

	pointLight light(0.0f,1.0f,0.0f);

	
	pointLight light2(1.5f,0.0f,0.0f);

	myScene->lights.push_back(&light);
	myScene->lights.push_back(&light2);


	myScene->generateLightBlock();
	//renderProgram firstProgram("first_program", "./shaders/first.vert", "./shaders/first.frag", &window->renderer);
	//glUseProgram(firstProgram.programID);
	/*
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	SDL_GetWindowWMInfo(window->windowHandler, &wmInfo);
	
	HWND hwnd = wmInfo.info.win.window;

	SetWindowLong(hwnd, GWL_EXSTYLE,
				GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, (255*70)/100, LWA_ALPHA);
	
    // Extend the frame into the client area.
	MARGINS margins = {0,0,0,25};

	HRESULT hr = S_OK;
	BOOL Stuff = 0;
	hr = DwmIsCompositionEnabled(&Stuff);
	BOOL enabled = FALSE;

	    // Create and populate the blur-behind structure.
    DWM_BLURBEHIND bb = {0};

    // Specify blur-behind and blur region.
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = true;
    bb.hRgnBlur = NULL;

	HRESULT hr2 = DwmEnableBlurBehindWindow(hwnd, &bb);

    if (SUCCEEDED(hr))
    {
        printf("the fuck\n");
    }
	
	SDL_Rect somerect = {-10, -10, 200, 200};
	*/

	/*Scene scene(&window->renderer);

	scene.loadFromFile("dk.3ds");

	scene.root.print();

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

	glBindVertexArray(newMesh.VAO);*/
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);

	myScene->root.scale = glm::vec3(0.02,0.02,0.02);

	bool turnoff = false;
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
						break;
					case SDLK_e:
						turnoff = true;
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
		if(!turnoff)
		{
			computeMatrix(&mycam);
			SDL_WarpMouseInWindow(window->windowHandler, 1200/2, 900/2);
		}
		myScene->Render();
		/*
		glm::mat4 MVP = Perspect  * View * Model;
		glUniformMatrix4fv(MVPID, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(VID,1,GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(MID,1,GL_FALSE, glm::value_ptr(Model));
		
		glDrawElements(GL_TRIANGLES, newMesh.indices.size(), GL_UNSIGNED_INT, 0);
		*/

		SDL_GL_SwapWindow(window->windowHandler);




	}
}