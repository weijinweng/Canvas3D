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
*
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

renderProgram::renderProgram(std::string name, char* vertex_file_path, char* fragment_file_path):name(name)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
 
    GLint Result = GL_FALSE;
    int InfoLogLength;
 
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
 
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
}
renderProgram::~renderProgram()
{
}

Mesh::Mesh()
{

}

void Mesh::initialize()
{
	GLuint vertexBuffer, indiceBuffer;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	printf("mesh initialize!");

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indiceBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,3,GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);

	glBindVertexArray(0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indiceBuffer);

}

bool Mesh::loadMeshFromFile(char* filePath)
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

