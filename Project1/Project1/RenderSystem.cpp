#include "RenderSystem.h"


using namespace Canvas;

std::vector<CanvasWindow*> CVS_MainWindow;

Texture::Texture(int width, int height)
{
}

Texture::Texture(std::string name)
{
}

Texture::~Texture()
{
}

transformNode::transformNode()
{
}

transformNode::~transformNode()
{
}


/************************************************
*5
*
*
*
*
*
*
************************************************/
shaderTextureID::shaderTextureID(std::string name)
{
}

shaderTextureID::~shaderTextureID()
{
}

renderProgram::renderProgram(std::string name, char* vertPath, char* fragPath)
{

}

renderProgram::~renderProgram()
{
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

Scene::Scene()
{
}

Scene::~Scene()
{
}

RenderSys::RenderSys()
{

}

bool RenderSys::initialize(SDL_Window* windowHandler)
{
	glContext = SDL_GL_CreateContext(windowHandler);

	if (glContext == NULL)
	{
		printf("OpenGl context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}else
	{
		glewExperimental = GL_TRUE;
		GLenum glewError = glewInit();
		if (glewError != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		}
		if (SDL_GL_SetSwapInterval(0) < 0)
		{
			printf("Error setting swap interval %s\n", SDL_GetError());
		}
	}
	return true;
}

bool RenderSys::end()
{
	return true;
}

void RenderSys::draw()
{
}

void RenderSys::draw(int x, int y, int width, int height)
{
}

void RenderSys::loadMesh(char* path)
{
}

void RenderSys::loadScene(char* path)
{
}

void RenderSys::unloadAllMemory()
{
}

RenderSys::~RenderSys()
{
}

CanvasWindow::CanvasWindow(int x, int y, int width, int height, int flags):x(x), y(y), width(width), height(height)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL_Init error %s\n", SDL_GetError());
		return;
	}
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	Uint32 flag = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
	
	if(flags & CVS_WDW_FULLSCREEN)
		flag = SDL_WINDOW_FULLSCREEN|SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
	if(flags & CVS_WDW_WINDOW)
		flag = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;

	windowHandler = SDL_CreateWindow("Canvas Engine", x, y, width, height, flag);
	renderer.initialize(windowHandler);
}

void CanvasWindow::Minimize()
{

}

void CanvasWindow::Maximize()
{

}

void CanvasWindow::Move(int x,int y)
{

}

void CanvasWindow::Resize(int width, int height)
{

}

CanvasWindow* CanvasWindow::CVS_CreateWindow(int x, int y, int width, int height, int flags)
{
	CanvasWindow* window = new CanvasWindow(x, y, width, height, flags);
	CVS_MainWindow.push_back(window);
	return window;
}

