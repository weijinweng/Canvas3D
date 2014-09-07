#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Canvas.h"

namespace Canvas{

	class RenderSys;
	class Mesh;
	class renderProgram;
	class renderNode;


	class Camera{
	public:
		Camera();
		void manageInput();
		virtual glm::mat4 getView();
		virtual glm::mat4 getPerspective();
	};

	class targetCamera:public Camera{
	protected:
		glm::vec3 target;
		glm::vec3 position;
	public:
		targetCamera(glm::vec3 target);
		targetCamera();
		targetCamera(glm::vec3  target, glm::vec3 position);
		//Camera manipulation
		void lookAt(glm::vec3 target);
		void move(glm::vec3 offset);
		void moveTo(glm::vec3 destination);
		//Camera data generation
		glm::mat4 getView();
		glm::mat4 getPerspective();
	};

	class fpsCamera:public Camera{

	};

	//Textures storage unit.
	struct Texture{
		//Texture pointer in opengl Memory
		GLuint textureID;
		int textureWidth;
		int textureHeight;
		//Texture type
		CANVASENUM textureType;
		//String properties
		std::string filePath;
		std::string name;
		Texture(int width, int height);
		Texture(std::string name);
		void setName();
		void loadFile(char* filePath);
		//cleanup, does not clean up strings;
		void unloadFromMemory();
		void reloadMemory();
		~Texture();
	};



	//Render nodes's shader texture.
	struct shaderTextureID{
	public:
		std::string name;
		Texture* texture;
		shaderTextureID(std::string name);
		shaderTextureID(char*name);
		~shaderTextureID();
	};

	//Nodes of a transform Tree, the node memory are stored inside renderNodes
	struct transformNode{
		void calculateMatrix();
	public:
		//Attributes of transforms
		glm::vec3 translation;
		glm::vec3 scale;
		glm::fquat orientation;
		//Resultant matrix
		glm::mat4 transformMatrix;
		//Node relationships
		transformNode* parent;
		std::vector<transformNode*> children;
		transformNode();
		//Tree modifiers
		void setParent(transformNode* parent);
		void addChild(transformNode* child);
		void removeChild(transformNode* child);
		//Calculate down a transformTree
		void calculateAllMatrices();
		~transformNode();
	};

	//GLSL shader program.
	//Memory resides in parent Scene
	class renderProgram{
	public:
		GLuint programID;
		std::string name;
		std::vector<renderNode*> childNodes;
		std::vector<shaderTextureID> textureTemplate;
		renderProgram(std::string name, char* vertPath, char* fragPath);
		~renderProgram();
	};

	//Renderable node within the program. Must contain a transform. 
	//If a mesh is not assigned, the node does not render.
	struct renderNode{
	public:
		std::string name;
		transformNode transform;
		renderProgram* programs;
		Mesh* mesh;
		std::vector<shaderTextureID> textureID;
		renderNode();
		bool setProgram(renderProgram* program);
		renderNode(Mesh* mesh);
	};

	//Vertex data. Requires offset to be correct.
	struct Vertex{
	public:
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal);
	};

	//Storage of vertices datas
	//Me5mory located in Parent RenderSys
	struct Mesh{
	public:
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		GLuint VAO;
		bool loadMeshFromFile(char* filePath);
		bool initMeshFromAiMesh(aiMesh* mesh);
		Mesh();
		Mesh(std::string name);
		void initialize();
		Mesh(char* path, std::string name);
		~Mesh();
	};

	//        Scene - setACamera
	//       /     \ 
	//    Programs   transformNodes
	//        /         \ GetAllMatrices
	//    renderNodes
	//       /    \
	//    Mesh    transformNode
	//    

	//Scene 
	class Scene{
	public:
		std::vector<Camera> cameras;
		RenderSys* parentSys;
		transformNode root;
		std::vector<renderProgram> programs;
		std::vector<renderNode> renderables;
		Scene(char* filepath);
		Scene();
		~Scene();
	};

	//A rendersystem that resides in a window, manages a window's graphics output.
	class RenderSys: public Subsystem{
	public:
		//Memory management within the RenderSys
		SDL_GLContext glContext;
		std::vector<Scene> scenes;
		std::vector<Mesh> meshes;
		std::vector<Texture> textures;
		RenderSys();
		bool initialize(SDL_Window* windowHandler);
		bool end();
		void draw();
		void draw(int x, int y, int width, int height);
		void loadMesh(char* path);
		void loadScene(char* path);
		void unloadAllMemory();
		~RenderSys();
	};


	//Window management
	class CanvasWindow{
	public:
		SDL_Window* windowHandler;
		RenderSys renderer;
		int width;
		int height;
		int x;
		int y;
		bool fullSreen;
		//Create a window of certain height and width.
		CanvasWindow(int x, int y, int width, int height, int flags);
		//Window management tools.
		void Minimize();
		void Maximize();
		void Move(int x, int y);
		void Resize(int width, int height);
		~CanvasWindow();
		static CanvasWindow* CVS_CreateWindow(int x, int y, int width, int height, int flags);
	};


};

#endif