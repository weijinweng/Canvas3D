#include "RenderSystem.h"


using namespace Canvas;

std::vector<CanvasWindow*> CVS_MainWindow;

Light::Light(CANVASENUM m_type):position(0.0f), ambient(1.0f,1.0f,1.0f,1.0f),diffuse(1.0f,1.0f,1.0f,1.0f), specular(1.0f,1.0f,1.0f,1.0f), constAttenuation(1.0f), linearAttentuation(0.4f), quadraticAttentuation(0.4f)
{
	switch (m_type)
	{
	case CVS_LGT_POINT:
		type = 0;
		break;
	default:
		type = 0;
	}
}


pointLight::pointLight(float x, float y, float z, float constAtten, float linAtten, float quadAtten):Light(CVS_LGT_POINT)
{
	position = glm::vec4(x,y,z,1);
}

void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to)
{
	to[0][0] = (GLfloat)from->a1; to[1][0] = (GLfloat)from->a2;
    to[2][0] = (GLfloat)from->a3; to[3][0] = (GLfloat)from->a4;
    to[0][1] = (GLfloat)from->b1; to[1][1] = (GLfloat)from->b2;
    to[2][1] = (GLfloat)from->b3; to[3][1] = (GLfloat)from->b4;
    to[0][2] = (GLfloat)from->c1; to[1][2] = (GLfloat)from->c2;
    to[2][2] = (GLfloat)from->c3; to[3][2] = (GLfloat)from->c4;
    to[0][3] = (GLfloat)from->d1; to[1][3] = (GLfloat)from->d2;
    to[2][3] = (GLfloat)from->d3; to[3][3] = (GLfloat)from->d4;
}

Camera::Camera()
{

}

glm::mat4 Camera::getView()
{
	return glm::mat4(1.0f);
}

glm::mat4 Camera::getPerspective()
{
	Perspective = glm::perspective(FOV, aspectRatio, 0.1f, 100.0f);
	return Perspective;
}

Texture::Texture()
{
}

Texture::Texture(int width, int height)
{
}

Texture::Texture(std::string name):name(name)
{
}

fpsCamera::fpsCamera(float FOV, float aspectRatio, glm::vec3 position):verticleAngle(0.0f),horizontalAngle(0.0f), controls(false)
{
	this->FOV = FOV;
	this->aspectRatio = aspectRatio;
	this->position = position;
	this->getPerspective();
}

void fpsCamera::getEvents(SDL_Event e)
{
			if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						controls.w = true;
						break;
					case SDLK_s:
						controls.s = true;
						break;
					case SDLK_a:
						controls.a = true;
						break;
					case SDLK_d:
						controls.d = true;
						break;
					case SDLK_x:
						controls.x = true;
						break;
					case SDLK_z:
						controls.z = true;
						break;
					case SDLK_LSHIFT:
						controls.shift = true;
				}
			}
			if(e.type == SDL_KEYUP)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_w:
						controls.w = false;
						break;
					case SDLK_s:
						controls.s = false;
						break;
					case SDLK_a:
						controls.a = false;
						break;
					case SDLK_d:
						controls.d = false;
						break;
					case SDLK_x:
						controls.x = false;
						break;
					case SDLK_z:
						controls.z = false;
						break;
					case SDLK_LSHIFT:
						controls.shift = false;
				}
			}
}

glm::mat4 fpsCamera::getView()
{
	glm::vec3 direction(std::cos(verticleAngle)*std::sin(horizontalAngle),
						std::sin(verticleAngle),
						std::cos(verticleAngle)*std::cos(horizontalAngle));

	glm::vec3 directionRight(std::sin(horizontalAngle-3.14f/2.0f),
							 0,
							 std::cos(horizontalAngle-3.14f/2.0f));

	glm::vec3 up = glm::cross(direction, directionRight);

	glm::mat4 View = glm::lookAt(position,direction+position, up);

	return View;
}

void Texture::loadFile(char* filePath)
{
	SDL_Surface* img = IMG_Load(filePath);

	if(img == NULL)
	{
		printf("Error loading image %s\n", filePath);
		return;
	}

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	int Mode = GL_RGB;

	printf("pixel format = %d\n", img->format->BitsPerPixel);

	if(img->format->BytesPerPixel == 4)
	{
		Mode = GL_RGBA;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	this->textureWidth = img->w;
	this->textureHeight = img->h;
	this->filePath = filePath;
	this->textureType = Mode == GL_RGBA? CVS_TXT_RGBA:CVS_TXT_RGB;
}

Texture::~Texture()
{
}

transformNode::transformNode():translation(0,0,0),orientation(glm::vec3(0,0,0)),scale(1,1,1)
{
	parent = NULL;
}

void transformNode::setParent(transformNode* parent)
{
	if (this->parent != NULL)
	{
		this->parent->removeChild(this);
	}
	parent->addChild(this);
}

void transformNode::removeChild(transformNode* child)
{
	for (std::vector<transformNode*>::iterator it = children.begin(), et = children.end(); it != et; it++)
	{
		if ((*it) == child)
			children.erase(it);
	}
}

void transformNode::addChild(transformNode* child)
{
	children.push_back(child);
	child->parent = this;
}

void transformNode::calculateMatrix()
{
	transformMatrix = glm::mat4(1.0);

	transformMatrix = glm::scale(transformMatrix, scale);
	transformMatrix = transformMatrix*glm::mat4_cast(orientation);
	transformMatrix = glm::translate(transformMatrix, translation);


	if (parent != NULL)
	{
		transformMatrix = parent->transformMatrix * transformMatrix;
	}
}

void transformNode::calculateAllMatrices()
{
	calculateMatrix();
	for (int i = 0; i < children.size(); i++)
	{

		children[i]->calculateAllMatrices();
	}
}

void transformNode::print(int level)
{
	printf("subnode\n");
	printf("translate x%d y%d z%d\n", translation.x, translation.y, translation.z);
	for(int i = 0, e = children.size(); i < e; ++i)
	{
		for(int j = 0, je = level; j < je; j++)
		{
			printf("-");
		}
		children[i]->print(level+1);
	}

}

void transformNode::print()
{
	printf("node\n");
	printf("translate x%d y%d z%d\n", translation.x, translation.y, translation.z);
	for(int i = 0, e = children.size(); i < e; ++i)
	{
			printf("-");
			children[i]->print(2);
	}
}

void printAiNode(const aiNode* node, int depth)
{
	printf("aiNode with meshes%d\n", node->mNumMeshes);
	for(int i = 0; i < node->mNumChildren; i++)
	{
		printf("-");
		for (int j = 0; j < depth; j ++)
		{
			printf("-");
		}
		printAiNode(node->mChildren[i], depth+1);
	}
}

transformNode::~transformNode()
{
}

renderNode::renderNode()
{
}

void renderNode::initFromRenderNode(aiNode* child, std::vector<Mesh*> meshes)
{
	name = std::string(child->mName.data);
	for(int i = 0, e = child->mNumMeshes; i < e; i++)
	{
		printf("Node %s added mesh %s\n",name.c_str(), meshes[child->mMeshes[i]]->name.c_str());
		this->meshes.push_back(meshes[child->mMeshes[i]]);
	}

	printf("added new render node:%s\n", name.c_str());
	glm::mat4 assMatrix;
	copyAiMatrixToGLM(&child->mTransformation, assMatrix);
	glm::vec4 transform(0,0,0,1);
	transform = assMatrix*transform;
	//get translate
	this->transform.translation.x = transform.x;
	this->transform.translation.y = transform.y;
	this->transform.translation.z = transform.z;
	printf("translate x%f y%f z%f\n", transform.x, transform.y, transform.z);
	//get scale
	transform = glm::vec4(1,0,0,1);
}

bool renderNode::setProgram(renderProgram* program)
{
	textureID.clear();
	for(int i = 0; i < program->textureTemplate.size(); ++i)
	{
		textureID.push_back(program->textureTemplate[i]);
	}
	return true;
}

void renderNode::setTexture(std::string name, std::string texturename)
{
	for(int i = 0, e = textureID.size(); i < e; ++i)
	{
		if(textureID[i].name == name)
		{
			textureID[i].setTexture(texturename);
			return;
		}
	}

	printf("Cannot find correct texture\n");
}

void renderNode::Render()
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
shaderTextureID::shaderTextureID(std::string name, RenderSys* parent):name(name),parent(parent)
{
	
}

void shaderTextureID::setTexture(std::string name)
{
	texture = parent->getTexture(name);
	if(texture == NULL)
	{
		printf("Error! Texture not found\n");
		texture = parent->getTexture("default");
	}
}

shaderTextureID::~shaderTextureID()
{
}

renderProgram::renderProgram(std::string name, char* vertex_file_path, char* fragment_file_path, RenderSys* parent):name(name)
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

	programID = ProgramID;

	GLint uniformCount;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &uniformCount);
	std::vector<GLchar> buffer(248);
	for(int i = 0;  i < uniformCount; ++i)
	{
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;
		glGetActiveUniform(programID, i, buffer.size(), &actualLength, &arraySize, &type, &buffer[0]);
		std::string name((char*)&buffer[0], actualLength);
		if(type == GL_SAMPLER_2D)
		{
			shaderTextureID newtex(name, parent);
			newtex.setTexture("default");
			textureTemplate.push_back(newtex);
			printf("added texture %s\n", newtex.name.c_str());
		}
	}

	
}
renderProgram::~renderProgram()
{
}



renderProgramID::renderProgramID(renderProgram* renderProgram):program(renderProgram)
{

}

void renderProgramID::addNode(renderNode* node)
{
	childNodes.push_back(node);
	printf("Node added to renderprogram %s childNodes size %d\n", childNodes.back()->name.c_str(), childNodes.size());
	node->setProgram(program);
}

void renderProgramID::toString()
{
	for(int i = 0, e = this->childNodes.size(); i < e; ++i)
	{
		printf("the name is %s\n", childNodes[i]->name.c_str());
	}
}

void renderProgramID::Render(Camera* cam, int num_lights)
{
	glUseProgram(program->programID);

	glm::mat4 perspective = cam->Perspective;
	glm::mat4 View = cam->getView();

	GLuint numLightLoc = glGetUniformLocation(program->programID, "num_lights");

	glUniform1i(numLightLoc, num_lights);

	GLuint ViD = glGetUniformLocation(program->programID, "V");
	GLuint MVPiD = glGetUniformLocation(program->programID,"MVP");
	GLuint MiD = glGetUniformLocation(program->programID,"M");

	glUniformMatrix4fv(ViD, 1, GL_FALSE, glm::value_ptr(View));

	
	for(int i = 0, e = this->childNodes.size(); i < e; ++i)
	{

		renderNode* node = childNodes[i];

		for(int j = 0, je = node->textureID.size(); j < je; ++j)
		{
			GLuint textureLOC = glGetUniformLocation(program->programID, node->textureID[j].name.c_str());
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, node->textureID[j].texture->textureID);
			glUniform1i(textureLOC, j); 
		}

		//Draw mesh

		for(int j = 0, je = node->meshes.size(); j < je; ++j)
		{
			glBindVertexArray(node->meshes[j]->VAO);
			
			glUniformMatrix4fv(MiD, 1, GL_FALSE, glm::value_ptr(node->transform.transformMatrix));
			
			glm::mat4 MVP = perspective * View * node->transform.transformMatrix;

			glUniformMatrix4fv(MVPiD, 1, GL_FALSE, glm::value_ptr(MVP));

			glDrawElements(GL_TRIANGLES, node->meshes[j]->indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

}

Mesh::Mesh()
{

}

Mesh::Mesh(std::string name)
{
	this->name = name;
}

void Mesh::initialize()
{
	GLuint vertexBuffer, indiceBuffer;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	printf("mesh initialized!\n");

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
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_FlipUVs|aiProcess_GenSmoothNormals);
	if (scene)
	{
		initMeshFromAiMesh(scene->mMeshes[0]);
	}
	return true;
}

bool Mesh::initMeshFromAiMesh(aiMesh* mesh)
{
	glm::vec2 nullUV(0,0);
	this->name = std::string(mesh->mName.data);
	printf("Init mesh %s\n", name.c_str());
	for (int i = 0, e = mesh->mNumVertices; i < e; i++)
	{
		glm::vec3 pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		glm::vec2 uv = glm::vec2(mesh->HasTextureCoords(0)? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y):nullUV);
		glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		
		this->vertices.push_back(Vertex(pos,uv,normal));
	}

	for (int i = 0, e = mesh->mNumFaces; i < e; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3)
		{
			printf("Triangle is no longer a triangle!! it may have more than 3 vertices!");
			return false;
		}
		this->indices.push_back(mesh->mFaces[i].mIndices[0]);
		this->indices.push_back(mesh->mFaces[i].mIndices[1]);
		this->indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	this->initialize();
	return true;
}

Mesh::~Mesh()
{
}

Vertex::Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal):position(position), uv(uv), normal(normal)
{
}


Scene::Scene(RenderSys* parent):parentSys(parent)
{
	this->renderables.reserve(20);
	this->getRenderProgram("Standard");
	this->setShadowShader();
	printf("initialization capacity %d\n", renderables.capacity());
}

Scene::Scene(RenderSys* parent, char* filepath):parentSys(parent)
{
	this->renderables.reserve(20);
	this->getRenderProgram("Standard");
	this->loadFromFile(filepath);
}



std::vector<Mesh*> Scene::loadFromFile(char* filepath)
{
	if(parentSys == NULL)
	{
		printf("Error parentSys does not exists\n");
	}
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filepath, aiProcess_FlipUVs|aiProcess_GenSmoothNormals);

	if(scene )
	{
		return initSceneFromAiScene(scene);
	}

	std::vector<Mesh*> meshes;

	return meshes;
}

void Scene::generateLightBlock()
{
	glGenBuffers(1, &light_ubo);

	glBindBuffer(GL_UNIFORM_BUFFER, light_ubo);

	const GLchar* uniformNames[1] = {
		"Lights.light"
	};

	GLuint uniformIndices;

	GLuint program = programs[0].program->programID;

	glGetUniformIndices(programs[0].program->programID, 1, uniformNames,&uniformIndices);

	GLint uniformOffsets[1];
	glGetActiveUniformsiv(program, 1, &uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);

	GLuint uniformBlockIndex = glGetUniformBlockIndex(program,"Lights");
	
	printf("light index is%d\n", uniformBlockIndex);



	GLsizei uniformBlockSize(0);
	glGetActiveUniformBlockiv(program, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

	printf("uniform block size %d\n", uniformBlockSize);

	const GLchar *names[] =
	{
		"Lights.light[0].position",
		"Lights.light[0].ambient",
		"Lights.light[0].diffuse",
		"Lights.light[0].specular",
		"Lights.light[0].constant_attenuation",
		"Lights.light[0].linear_attenuation",
		"Lights.light[0].quadratic_attenuation",
		"Lights.light[0].spot_direction",
		"Lights.light[0].spot_cutoff",
		"Lights.light[0].spot_exponent",
		"Lights.light[1].position",
		"Lights.light[1].ambient",
		"Lights.light[1].diffuse",
		"Lights.light[1].specular",
		"Lights.light[1].constant_attenuation",
		"Lights.light[1].linear_attenuation",
		"Lights.light[1].quadratic_attenuation",
		"Lights.light[1].spot_direction",
		"Lights.light[1].spot_cutoff",
		"Lights.light[1].spot_exponent",
		"Lights.light[2].position",
		"Lights.light[2].ambient",
		"Lights.light[2].diffuse",
		"Lights.light[2].specular",
		"Lights.light[2].constant_attenuation",
		"Lights.light[2].linear_attenuation",
		"Lights.light[2].quadratic_attenuation",
		"Lights.light[2].spot_direction",
		"Lights.light[2].spot_cutoff",
		"Lights.light[2].spot_exponent",
		"Lights.light[3].position",
		"Lights.light[3].ambient",
		"Lights.light[3].diffuse",
		"Lights.light[3].specular",
		"Lights.light[3].constant_attenuation",
		"Lights.light[3].linear_attenuation",
		"Lights.light[3].quadratic_attenuation",
		"Lights.light[3].spot_direction",
		"Lights.light[3].spot_cutoff",
		"Lights.light[3].spot_exponent",
	};

	GLuint indices[40];

	glGetUniformIndices(program, 40, names, indices);

	for(int i = 0; i < 40; ++i)
	{
		printf("Indice index %d with value %d\n",i, indices[i]);
	}

	std::vector<GLint> lightUniformOffsets (40);
	
	glGetActiveUniformsiv(program, lightUniformOffsets.size(), indices, GL_UNIFORM_OFFSET, &lightUniformOffsets[0]);


	GLint* offsets = &lightUniformOffsets[0];

	const unsigned int uboSize(uniformBlockSize);
	std::vector<char> buffer(uboSize);

	int offset;

	printf("R is %f b is %f g is %f\n", lights[0]->diffuse[0], lights[0]->diffuse[1], lights[0]->diffuse[2]);
	
	for(int i = 0; i < 40;++i)
	{
		printf("offset for index %d is %d\n", i, offsets[i]);
	}

	for(unsigned int n = 0, e = lights.size(); n < e; ++n)
	{
		printf("lighting it up\n");
		// Light position (vec4)
		offset = offsets[0 + n * 10];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->position[i];
			offset += sizeof (GLfloat);
		}
		// Light ambient color (vec4)
		offset = offsets[1 + n * 10];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->ambient[i];
			offset += sizeof (GLfloat);
		}
		// Light diffuse color (vec4)
		offset = offsets[2 + n * 10];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->diffuse[i];
			offset += sizeof (GLfloat);
		}
		// Light specular color (vec4)
		offset = offsets[3 + n * 10];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->specular[i];
			offset += sizeof (GLfloat);
		}
		// Light constant attenuation (float)
		offset = offsets[4 + n * 10];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->constAttenuation;
		// Light linear attenuation (float)
		offset = offsets[5 + n * 10];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->linearAttentuation;
		// Light quadratic attenuation (float)
		offset = offsets[6 + n * 10];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->quadraticAttentuation;
		// Light spot direction (vec3)
		offset = offsets[7 + n * 10];
		for (int i = 0; i < 3; ++i)
		{
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->spotDirection[i];
			offset += sizeof (GLfloat);
		}
		// Light spot cutoff (float)
		offset = offsets[8 + n * 10];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->spotCosCutoff;
		// Light spot exponent (float)
		offset = offsets[9 + n * 10];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				lights[n]->spotExponent;
	}
	glBufferData(GL_UNIFORM_BUFFER, uboSize, &buffer[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, light_ubo);
	glUniformBlockBinding(program, uniformBlockIndex, 0);
	for(int i = 0, e = programs.size(); i < e; ++i)
	{
		
		program = programs[i].program->programID;
		uniformBlockIndex = glGetUniformBlockIndex(program, "Lights");
		glUniformBlockBinding(program, uniformBlockIndex, 0);
	}
	initializeShadowMap();
}

transformNode* Scene::addRenderNodesFromAiNodes(aiNode* child, std::vector<Mesh*> meshes)
{
	static renderNode* nodepointer = 0;
	renderables.push_back(renderNode());
	renderNode* ournode = &(renderables.back());
	printf("The capacity is %d\n", renderables.capacity());
	for(int i = 0, e = child->mNumChildren; i < e; ++i)
	{
		ournode->transform.addChild(addRenderNodesFromAiNodes(child->mChildren[i], meshes));
	}

	ournode->initFromRenderNode(child, meshes);
	programs[0].addNode(ournode);
	printf("node name is %s\n", ournode->name.c_str());
	if(nodepointer != &renderables[0])
		printf("secondary error found!\n");
	nodepointer = &renderables[0];
	if(&renderables[0] != programs[0].childNodes[0])
		printf("error found!\n");

	return &ournode->transform;
}

renderNode* Scene::getNode(std::string name)
{
	for(int i = 0, e = renderables.size(); i < e; ++i)
	{
		if(renderables[i].name == name)
			return &renderables[i];
	}

	printf("Error finding\n");
	return NULL;
}

void Scene::initRenderNodesFromAiScene(aiNode* root, std::vector<Mesh*> meshes)
{

	for(int i = 0, e = root->mNumChildren; i < e; ++i)
	{
		this->root.addChild(addRenderNodesFromAiNodes(root->mChildren[i], meshes));
	}
}

std::vector<Mesh*> Scene::initSceneFromAiScene(const aiScene* scene)
{
	std::vector<Mesh*> meshes = parentSys->initMeshesFromAiScene(scene->mMeshes, scene->mNumMeshes);

	printf("renderable capacity %d\n", renderables.capacity());

	initRenderNodesFromAiScene(scene->mRootNode, meshes);

	printAiNode(scene->mRootNode,0);
	return meshes;
}

void Scene::Render()
{
	Render(0);
}

void Scene::Render(int index)
{
	if((cameras.size()==0))
	{
		printf("No Cameras in scene! add one before rendering\n");
		return;
	}
	root.calculateAllMatrices();

	generateShadow();

	for(int i = 0, e = programs.size(); i < e; ++i)
	{
		programs[i].Render(cameras[index], lights.size());
	}
}

void Scene::setShadowShader(std::string name)
{
	shadowProgram = parentSys->getProgram(name);
}

Scene::~Scene()
{
}

void Scene::getRenderProgram(std::string name)
{
	this->programs.push_back(renderProgramID(parentSys->getProgram(name)));
}

void Scene::initializeShadowMap()
{
	glGenFramebuffers(1,&shadowMapFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer);

	for (int i = 0; i < lights.size(); i++)
	{
		glGenTextures(1, &lights[i]->shadowTexture);
		glBindTexture(GL_TEXTURE_2D, lights[0]->shadowTexture);
		glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT16, 1024,1024,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights[i]->shadowTexture,0);
		glDrawBuffer(GL_NONE);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("shadowInitialization error!!!!!");
			return;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::generateShadow()
{
	glUseProgram(shadowProgram->programID);

	GLuint depthMatrixID = glGetUniformLocation(shadowProgram->programID, "depthMVP");

	glm::vec3 pointLightDir = glm::vec3(0,0,0);

	GLuint program = shadowProgram->programID;

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);


	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFrameBuffer);

	for(int i = 0, e = lights.size(); i < e; ++i)
	{
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lights[i]->shadowTexture,0);

		glDrawBuffer(GL_NONE);

		for(int j = 0, je = renderables.size(); j < je; ++j)
		{
			glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(lights[i]->position.x,lights[i]->position.y,lights[i]->position.z),glm::vec3(0,0,0),glm::vec3(0,1,0));
			glm::mat4 depthModelMatrix = renderables[j].transform.transformMatrix;
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	
			glUniformMatrix4fv(program, 1,GL_FALSE, glm::value_ptr(depthMVP));

			for(int k = 0, ke = renderables[j].meshes.size(); k < ke; ++k)
			{
				glBindVertexArray(renderables[j].meshes[k]->VAO);

				glDrawElements(GL_TRIANGLES, renderables[j].meshes[k]->indices.size(), GL_UNSIGNED_INT, 0);
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::testShadowMap()
{
	glUseProgram(shadowProgram->programID);

	GLuint depthMatrixID = glGetUniformLocation(shadowProgram->programID, "depthMVP");

	if(depthMatrixID == -1)
	{
	//	printf("error");
	}

	glm::vec3 pointLightDir = glm::vec3(0,0,0);

	GLuint program = shadowProgram->programID;

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);

	for(int i = 0, e = lights.size(); i < e; ++i)
	{

		for(int j = 0, je = renderables.size(); j < je; ++j)
		{
			glm::mat4 depthViewMatrix = cameras[0]->getView();
			glm::mat4 depthModelMatrix = renderables[j].transform.transformMatrix;
			glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	
			glUniformMatrix4fv(program, 1,GL_FALSE, glm::value_ptr(depthMVP));

			for(int k = 0, ke = renderables[j].meshes.size(); k < ke; ++k)
			{
				glBindVertexArray(renderables[j].meshes[k]->VAO);
				glDrawElements(GL_TRIANGLES, renderables[j].meshes[k]->indices.size(), GL_UNSIGNED_INT, 0);
			}
		}
	}
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
	glClearColor( 0.0f, 0.0f, 0.5f, 1.0f);
	meshes.reserve(50);
	textures.reserve(50);
	programs.reserve(50);
	Texture* tex = this->createNewTexture("default");
	tex->loadFile("./textures/default.tga");

	this->createNewProgram("Standard", "./shaders/Blinn.vert", "./shaders/Blinn.frag");
	this->createNewProgram("Shadow", "./shaders/shadow.vert", "./shaders/shadow.frag");
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

Scene* RenderSys::createNewScene()
{
	scenes.push_back(Scene(this));
	scenes.back().renderables.reserve(20);
	printf("creation capacity %d\n", scenes.back().renderables.capacity());
	return &scenes.back();
}

renderProgram* RenderSys::createNewProgram(std::string name, char* vertPath, char* fragPath)
{
	programs.push_back(renderProgram(name, vertPath, fragPath, this));
	return &programs.back();
}

Texture* RenderSys::createNewTexture(std::string name)
{
	textures.push_back(Texture(name));
	printf("Texture created\n");
	return &textures.back();
}

renderProgram* RenderSys::getProgram(std::string name)
{
	for(int i = 0, e = programs.size(); i < e; ++i)
	{
		if(programs[i].name == name)
			return &programs[i];
	}
	printf("Error getting program %s\n", name.c_str());
	return NULL;
}

RenderSys::~RenderSys()
{
}

std::vector<Mesh*> RenderSys::initMeshesFromAiScene(aiMesh** mesh, int meshcount)
{
	std::vector<Mesh*> newmeshes;
	for(int i = 0, e = meshcount; i < e; ++i)
	{
		meshes.push_back(Mesh());
		Mesh* newMesh = &meshes.back();
		newMesh->initMeshFromAiMesh(mesh[i]);
		newmeshes.push_back(newMesh);
	}
	return newmeshes;
}

Texture* RenderSys::getTexture(std::string name)
{
	printf("textures size %d\n", textures.size());
	for(int i = 0, e = textures.size(); i < e; ++i)
	{
		printf("textures name %s\n", textures[i].name.c_str());
		if(textures[i].name == name)
			return &textures[i];
	}
	return NULL;
}

Mesh* RenderSys::createNewMesh(std::string mesh)
{
	meshes.push_back(Mesh(mesh));
	return &meshes.back();
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
	
	int imgFlags = IMG_INIT_PNG;

	if( !(IMG_Init( imgFlags ) & imgFlags))
	{
		printf( "SDL_image could not be initialize! SDL_image Error: %s\n", IMG_GetError() );
		return;
	}

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
