#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H


#include "Canvas.h"

namespace Canvas{

	class RenderSys;
	class Mesh;
	class renderProgram;
	class renderNode;

	struct cameraControls
	{
		bool w, a, s, d, z, x, shift;
		cameraControls(bool all):w(all),a(all),s(all),d(all),z(all),x(all),shift(all)
		{
		}
	};

	struct Light{
		glm::vec4 position;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		float constAttenuation;
		float linearAttentuation;
		float quadraticAttentuation;
		int type;
		glm::vec3 spotDirection;
		float spotExponent;
		float spotCosCutoff;
		Light(CANVASENUM type);
		GLuint shadowTexture;
		bool shadow;
	};

	struct spotLight{
	};

	struct pointLight:public Light{
	public:
		pointLight(float x, float y, float z, float constAtten = 1.0, float linAtten = 1.0, float quadAtten = 1.0);

	};

	class Camera{
	public:
		float FOV;
		float aspectRatio;
		glm::mat4 Perspective;
		glm::vec3 position;
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
	};

	class fpsCamera:public Camera{
	public:
		cameraControls controls;
		float verticleAngle, horizontalAngle;
		fpsCamera(float FOV, float aspectRatio, glm::vec3 position);
		void getEvents(SDL_Event e);
		void processEvents();
		glm::mat4 getView();
	};

	//Textures storage unit.
	struct Texture{
		//Texture pointer in opengl Memory
		Texture();
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
		RenderSys* parent;
		std::string name;
		Texture* texture;
		shaderTextureID(std::string name, RenderSys* rendersys);
		shaderTextureID(char*name);
		void setTexture(std::string textureName);
		//Efficient way to set texture indexvoid setTexture(int textureIndex);
		void addTexture(char* filePath);
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
		//Derived from assimp
		glm::mat4 assimpMatrix;
		//Node relationships
		transformNode* parent;
		std::vector<transformNode*> children;
		transformNode();
		//Tree modifiers
		void setParent(transformNode* parent);
		void addChild(transformNode* child);
		void removeChild(transformNode* child);
		void print(int level);
		void print();
		//Calculate down a transformTree
		void calculateAllMatrices();
		~transformNode();
	};

	//GLSL shader program.
	//Memory resides in parent Scene
	class renderProgram{
	public:
		RenderSys* parent;
		GLuint programID;
		std::string name;
		std::vector<shaderTextureID> textureTemplate;
		renderProgram(std::string name, char* vertPath, char* fragPath, RenderSys* renderer);
		~renderProgram();
	};


	struct renderProgramID{
		renderProgram* program;
		std::vector<renderNode*> childNodes;
		renderProgramID(renderProgram* program);
		void addNode(renderNode* node);
		void Render(Camera* cam, int lightNum);
		void toString();
	};



	//Renderable node within the program. Must contain a transform. 
	//If a mesh is not assigned, the node does not render.
	struct renderNode{
	public:
		std::string name;
		transformNode transform;
		renderProgram* programs;
		std::vector<Mesh*> meshes;
		std::vector<shaderTextureID> textureID;
		renderNode();
		bool setProgram(renderProgram* program);
		void Render();
		void setMesh(std::string meshName);
		void setTexture(std::string textureID, std::string texture);
		void initFromRenderNode(aiNode* child, std::vector<Mesh*> meshes);
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
		GLuint shadowMapFrameBuffer;
		GLuint light_ubo;
		renderProgram* shadowProgram;
		std::vector<Camera*> cameras;
		std::vector<Light*> lights;
		RenderSys* parentSys;
		transformNode root;
		std::vector<renderProgramID> programs;
		std::vector<renderNode> renderables;
		Scene(RenderSys* parent, char* filepath);
		Scene(RenderSys* parent);
		std::vector<Mesh*> loadFromFile(char* filepath);
		std::vector<Mesh*> initSceneFromAiScene(const aiScene* scene);
		void initRenderNodesFromAiScene(aiNode* root, std::vector<Mesh*> meshes);
		transformNode* addRenderNodesFromAiNodes(aiNode* child, std::vector<Mesh*> meshes);
		transformNode* createNewNodeFromRenderNode(aiNode* child, std::vector<Mesh*> meshes);
		renderNode* getNode(std::string nodeName);
		void generateLightBlock();
		void Render();
		void Render(int index);
		void getRenderProgram(std::string programName);
		void generateShadow();
		void initializeShadowMap();
		void setShadowShader(std::string name = "Shadow");
		void testShadowMap();
		~Scene();
	};

	//A rendersystem that resides in a window, manages a window's graphics output.
	class RenderSys: public Subsystem{
	public:
		//Memory management within the RenderSys
		SDL_GLContext glContext;
		std::vector<Scene> scenes;
		int maxMesh;
		std::vector<Mesh> meshes;
		int maxTextures;
		std::vector<Texture> textures;
		int maxShaders;
		std::vector<renderProgram> programs;
		RenderSys();
		bool initialize(SDL_Window* windowHandler);
		bool end();
		void draw();
		void draw(int x, int y, int width, int height);
		void loadMesh(char* path);
		void loadScene(char* path);
		void unloadAllMemory();
		renderProgram* createNewProgram(std::string name, char* vertPath, char* fragPath);
		Scene* createNewScene();
		Texture* createNewTexture(std::string name);
		Mesh* createNewMesh(std::string Mesh);
		renderProgram* getProgram(std::string name);
		Texture* getTexture(std::string name);
		std::vector<Mesh*> initMeshesFromAiScene(aiMesh** mesh, int meshCount);
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