#include "Externals.h"
#include "RenderSystem.h"

using namespace Canvas;

bool quit = false;

int main(int argc, char* args[])
{
	CanvasWindow* window = CanvasWindow::CVS_CreateWindow(20, 20, 500, 500, CVS_WDW_WINDOW);
	window->renderer;
	while(!quit)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e) > 0)
		{
			if(e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}
}