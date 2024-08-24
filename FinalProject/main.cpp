#pragma warning(disable : 5208)

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <assimp/Importer.hpp>

// Project includes
#include "maths_funcs.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// MESH TO LOAD
// Load dae and obj files
#define MODEL_ONE		"Model/Spider.dae"
#define MODEL_TWO		"Model/castle.obj"
#define MODEL_THREE		"Model/UFO.dae"
#define MODEL_FOUR		"Model/Spider.dae"
#define MODEL_FIVE		"Model/Tower.obj"
#define POINT_LIGHT		"Model/cube.dae"
#define SPOT_LIGHT		"Model/cube.dae"


// TEXTURES TO LOAD
#define TEX_ONE			"Texture/Sp2.jpg"
#define TEX_ONE_TWO			"Texture/Spider.jpg"
#define TEX_TWO			"Texture/castle.jpg"
#define TEX_TWO_TWO			"Texture/wall.jpg"
#define TEX_THREE			"Texture/UFO_color.jpg"
#define TEX_THREE_TWO			"Texture/UFO.jpg"
#define TEX_FOUR			"Texture/Spider.jpg"
#define TEX_FOUR_TWO			"Texture/Spider.jpg"
#define TEX_FIVE			"Texture/castle.jpg"
#define TEX_FIVE_TWO			"Texture/wall.jpg"
#define TEX_SIX			"Texture/white.jpg"
#define TEX_TERRAIN			"Texture/terrain.png"

struct Material {
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float shininess;
};

#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
	std::vector<unsigned int> mIndices;
	std::vector<Material> mMaterials;
} ModelData;
#pragma endregion SimpleTypes

using namespace std;
GLuint shaderProgramID;
GLuint loc1[3];
GLuint loc2[3];
GLuint loc3[3];
GLuint loc4[3];
GLuint loc5[3];
GLuint loc_point_light[3];
GLuint loc_spot_light[3];
GLuint loc_terrain[3];

// Models
ModelData model1, model2, model3;
ModelData model4, model5, model_point_light, terrainModel, model_spot_light;

// Screen Size
int width = 1000;
int height = 600;

// Textures
unsigned int tex_1, tex_2, tex_3, tex_4, tex_5, tex_6;
unsigned int tex_1_2, tex_2_2, tex_3_2, tex_4_2, tex_5_2, tex_6_2;
unsigned int tex_terrain, tex_terrain_2;

// Buffers
unsigned int VP_VBOs[15]; // vertex positions
unsigned int VN_VBOs[15]; // vertex normals
unsigned int VT_VBOs[15]; // vertex textures
unsigned int VAOs[1];

GLfloat rotate_y = 0.0f;
GLfloat rotate_x = 0.0f;
GLfloat rotate_z = 0.0f;

float ufo_rotation = 0.0f;

boolean mouse_movement = false;
// translation
float translation_x = -2.0f;
float translation_y = 0.05f;
float translation_z = 2.0f;
float translation_x2 = -8.0f;
float translation_y2 = 0.0f;
float translation_z2 = 0.0f;
float translation_x3 = 8.0f;
float translation_y3 = 3.0f;
float translation_z3 = 1.0f;
float translation_x4 = -1.0f;
float translation_y4 = 1.0f;
float translation_z4 = -8.0f;
float point_light_x = -1.0f;
float point_light_y = 10.0f;
float point_light_z = -8.0f;
float spot_light_x = -1.0f;
float spot_light_y = 6.0f;
float spot_light_z = -8.0f;
float terrain_x = -50.0f;
float terrain_y = -10.0f;
float terrain_z = -80.0f;
// Scaling
float scaling_x = 0.4f;
float scaling_y = 0.4f;
float scaling_z = 0.4f;
// Camera
float camera_x = 0.0f;
float camera_y = 5.0f;
float camera_z = 15.0f;
float target_x = 0.0f;
float target_y = 0.0f;
float target_z = 0.0f;
vec3 camera_pos = vec3(0.0f, 0.0f, 0.0f);		// initial position of camera
vec3 camera_target = vec3(0.0f, 0.0f, 0.0f);	// initial position of target
vec3 up_dir = vec3(0.0f, 1.0f, 0.0f);				// up direction
float cameraDirection_x = 0.0f;
float cameraDirection_y = 0.0f;
float cameraDirection_z = 0.0f;
float cameraSpeed = 0.01f;
// Mouse
float mouse_x = 0.0f;
float mouse_y = 0.0f;
float mouse_x_previous = 0.0f;
float mouse_y_previous = 0.0f;
// 3d Perspective
mat4 persp_proj = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);

ModelData generateTerrain(int width, int height, float scale) {
	ModelData terrain;
	// Generate vertices
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			float x = static_cast<float>(i) * scale;
			float z = static_cast<float>(j) * scale;
			float y = std::sin(x * 1.0f) * std::cos(z * 1.0f) * 2.0f * scale;  // Use a sine and cos for wave-like terrain
			terrain.mVertices.push_back(vec3(x, y, z));
			terrain.mNormals.push_back(vec3(0.0f, 1.0f, 0.0f));
			terrain.mTextureCoords.push_back(vec2(static_cast<float>(i) / width, static_cast<float>(j) / height));
		}
	}
	for (int i = 0; i < width - 1; ++i) {
		for (int j = 0; j < height - 1; ++j) {
			terrain.mIndices.push_back(i * height + j);       // generate triangle 1
			terrain.mIndices.push_back((i + 1) * height + j);
			terrain.mIndices.push_back(i * height + j + 1);
			terrain.mIndices.push_back((i + 1) * height + j);    // generate triangle 2
			terrain.mIndices.push_back((i + 1) * height + j + 1);
			terrain.mIndices.push_back(i * height + j + 1);
		}
	}
	terrain.mPointCount = terrain.mVertices.size();
	return terrain;
}


#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */

	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate | aiProcess_PreTransformVertices);

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			}
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			else {
				// default values if the mesh doesn't have texture coordinates
				modelData.mTextureCoords.push_back(vec2(0.0f, 0.0f));
			}
		}
	}
	// get material information
	for (unsigned int m_i = 0; m_i < scene->mNumMaterials; m_i++) {
		const aiMaterial* material = scene->mMaterials[0];
		Material mat;
		aiColor3D color;
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
			mat.ambientColor = glm::vec3(color.r, color.g, color.b);
			std::cout << "ambientColor: (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			mat.diffuseColor = glm::vec3(color.r, color.g, color.b);
			std::cout << "diffuseColor: (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
			mat.specularColor = glm::vec3(color.r, color.g, color.b);
			std::cout << "specularColor: (" << color.r << ", " << color.g << ", " << color.b << ")" << std::endl;
		}
		material->Get(AI_MATKEY_SHININESS, mat.shininess);
		std::cout << "Shininess: (" << mat.shininess<< std::endl;
		modelData.mMaterials.push_back(mat);
    }
	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

#pragma region TEXTURE LOADING
unsigned int load_tex(const char* file_name) {
	// load and create a texture 
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_MIRRORED_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file_name, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return textureID;
}
#pragma endregion TEXTURE LOADING

// Shader Functions
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL) { return NULL; }
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';
	fclose(fp);
	return buf;
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions 
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh() {
	// load meshes
	terrainModel = generateTerrain(100, 1000, 1.0f);
	model1 = load_mesh(MODEL_ONE);
	model2 = load_mesh(MODEL_TWO);
	model3 = load_mesh(MODEL_THREE);
	model4 = load_mesh(MODEL_FOUR);
	model5 = load_mesh(MODEL_FIVE);
	model_point_light = load_mesh(POINT_LIGHT);
	model_spot_light = load_mesh(SPOT_LIGHT);
	// load textures
	tex_1 = load_tex(TEX_ONE);
	tex_1_2 = load_tex(TEX_ONE_TWO);
	tex_2 = load_tex(TEX_TWO);
	tex_2_2 = load_tex(TEX_TWO_TWO);
	tex_3 = load_tex(TEX_THREE);
	tex_3_2 = load_tex(TEX_THREE_TWO);
	tex_4 = load_tex(TEX_FOUR);
	tex_4_2 = load_tex(TEX_FOUR_TWO);
	tex_5 = load_tex(TEX_FIVE);
	tex_5_2 = load_tex(TEX_FIVE_TWO);
	tex_6 = load_tex(TEX_SIX);
	tex_6_2 = load_tex(TEX_SIX);
	tex_terrain = load_tex(TEX_TERRAIN);
	tex_terrain_2 = load_tex(TEX_TERRAIN);
	// Generate Buffers
	glGenBuffers(15, VP_VBOs); // position buffer
	glGenBuffers(15, VN_VBOs); // normal buffer
	glGenBuffers(15, VT_VBOs); // texture buffer

	// Model 1
	loc1[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc1[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc1[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, model1.mPointCount * sizeof(vec3), &model1.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, model1.mPointCount * sizeof(vec3), &model1.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, model1.mPointCount * sizeof(vec2), &model1.mTextureCoords[0], GL_STATIC_DRAW);
	// Model 2
	loc2[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc2[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, model2.mPointCount * sizeof(vec3), &model2.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, model2.mPointCount * sizeof(vec3), &model2.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, model2.mPointCount * sizeof(vec2), &model2.mTextureCoords[0], GL_STATIC_DRAW);
	// Model 3
	loc3[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc3[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, model3.mPointCount * sizeof(vec3), &model3.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, model3.mPointCount * sizeof(vec3), &model3.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, model3.mPointCount * sizeof(vec2), &model3.mTextureCoords[0], GL_STATIC_DRAW);
	// Model 4
	loc4[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc4[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc4[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, model4.mPointCount * sizeof(vec3), &model4.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, model4.mPointCount * sizeof(vec3), &model4.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[3]);
	glBufferData(GL_ARRAY_BUFFER, model4.mPointCount * sizeof(vec2), &model4.mTextureCoords[0], GL_STATIC_DRAW);
	// Model 5
	loc5[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc5[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc5[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[4]);
	glBufferData(GL_ARRAY_BUFFER, model5.mPointCount * sizeof(vec3), &model5.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[4]);
	glBufferData(GL_ARRAY_BUFFER, model5.mPointCount * sizeof(vec3), &model5.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[4]);
	glBufferData(GL_ARRAY_BUFFER, model5.mPointCount * sizeof(vec2), &model5.mTextureCoords[0], GL_STATIC_DRAW);
	// Point Light
	loc_point_light[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc_point_light[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc_point_light[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[5]);
	glBufferData(GL_ARRAY_BUFFER, model_point_light.mPointCount * sizeof(vec3), &model_point_light.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[5]);
	glBufferData(GL_ARRAY_BUFFER, model_point_light.mPointCount * sizeof(vec3), &model_point_light.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[5]);
	glBufferData(GL_ARRAY_BUFFER, model_point_light.mPointCount * sizeof(vec2), &model_point_light.mTextureCoords[0], GL_STATIC_DRAW);
	// Spot Light
	loc_spot_light[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc_spot_light[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc_spot_light[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[7]);
	glBufferData(GL_ARRAY_BUFFER, model_spot_light.mPointCount * sizeof(vec3), &model_spot_light.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[7]);
	glBufferData(GL_ARRAY_BUFFER, model_spot_light.mPointCount * sizeof(vec3), &model_spot_light.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[7]);
	glBufferData(GL_ARRAY_BUFFER, model_spot_light.mPointCount * sizeof(vec2), &model_spot_light.mTextureCoords[0], GL_STATIC_DRAW);
	// terrain 
	loc_terrain[0] = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc_terrain[1] = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc_terrain[2] = glGetAttribLocation(shaderProgramID, "vertex_texture");
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, terrainModel.mPointCount * sizeof(vec3), &terrainModel.mVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, terrainModel.mPointCount * sizeof(vec3), &terrainModel.mNormals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[6]);
	glBufferData(GL_ARRAY_BUFFER, terrainModel.mPointCount * sizeof(vec2), &terrainModel.mTextureCoords[0], GL_STATIC_DRAW);
}
#pragma endregion VBO_FUNCTIONS

void display() {
	// Tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthMask(GL_TRUE); //update the depth buffer
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // sky colour - same colour as fog
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	// Declare your uniform variables that will be used in your shader
	GLuint matrix_location = glGetUniformLocation(shaderProgramID, "model");
	GLuint view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	GLuint proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	int pointLightLoc = glGetUniformLocation(shaderProgramID, "pointLightPosition");
	glUniform3f(pointLightLoc, point_light_x, point_light_y, point_light_z);

	int spotLightLoc = glGetUniformLocation(shaderProgramID, "spotLightPosition");
	glUniform3f(spotLightLoc, spot_light_x, spot_light_y, spot_light_z);

	// camera view
	mat4 view = identity_mat4();
	camera_target = vec3(target_x, target_y, target_z);
	camera_pos = vec3(camera_x, camera_y, camera_z);
	view = look_at(camera_pos, camera_target, up_dir);
	// Update Uniforms & Draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	// Model 1
	mat4 model = identity_mat4();
	model = rotate_y_deg(model, 0.0f);
	model = rotate_x_deg(model, 0.0f);
	model = rotate_z_deg(model, 0.0f);
	model = translate(model, vec3(translation_x, translation_y, translation_z));
	model = scale(model, vec3(scaling_x, scaling_y, scaling_z));
	int ambiColorLoc = glGetUniformLocation(shaderProgramID, "material_ambiColor");
	glUniform3f(ambiColorLoc, 0, 0, 0);
	int diffColorLoc = glGetUniformLocation(shaderProgramID, "material_diffColor");
	glUniform3f(diffColorLoc, 0.6, 0.4, 0.2);
	int specColorLoc = glGetUniformLocation(shaderProgramID, "material_specColor");
	glUniform3f(diffColorLoc, 1, 1, 1);
	int shininessLoc = glGetUniformLocation(shaderProgramID, "material_shininess");
	glUniform1f(shininessLoc, 20.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
	glEnableVertexAttribArray(loc1[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[0]);
	glVertexAttribPointer(loc1[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc1[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[0]);
	glVertexAttribPointer(loc1[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc1[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[0]);
	glVertexAttribPointer(loc1[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_1);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_1_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model1.mPointCount);
	// Model 2
	mat4 modelChild = identity_mat4();
	modelChild = rotate_y_deg(modelChild, 0.0f); // only rotates on z axis-s
	modelChild = rotate_x_deg(modelChild, 0.0f);
	modelChild = rotate_z_deg(modelChild, 0.0f);
	modelChild = translate(modelChild, vec3(translation_x2, translation_y2, translation_z2));
	modelChild = scale(modelChild, vec3(scaling_x*2, scaling_y * 2, scaling_z * 2));
	//int ambiColorLoc = glGetUniformLocation(shaderProgramID, "ambiColor");
	glUniform3f(ambiColorLoc, 0, 0, 0);
	//int diffColorLoc = glGetUniformLocation(shaderProgramID, "diffColor");
	glUniform3f(diffColorLoc, 0.6, 0.6, 0.6);
	//int specColorLoc = glGetUniformLocation(shaderProgramID, "specColor");
	glUniform3f(specColorLoc, 0, 0, 0);
	glUniform1f(shininessLoc, 10.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild.m);
	glEnableVertexAttribArray(loc2[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[1]);
	glVertexAttribPointer(loc2[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[1]);
	glVertexAttribPointer(loc2[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc2[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[1]);
	glVertexAttribPointer(loc2[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	/*glBindTexture(GL_TEXTURE_2D, tex_2);*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_2_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model2.mPointCount);
	// Model 3 (UFO) Root of the Hierarchy
	mat4 parent_UFO = identity_mat4();
	parent_UFO = rotate_y_deg(parent_UFO, rotate_x);
	parent_UFO = rotate_x_deg(parent_UFO, rotate_y);
	parent_UFO = rotate_z_deg(parent_UFO, rotate_z);
	parent_UFO = translate(parent_UFO, vec3(translation_x3, translation_y3, translation_z3));
	parent_UFO = scale(parent_UFO, vec3(scaling_x, scaling_y, scaling_z));
	//int ambiColorLoc = glGetUniformLocation(shaderProgramID, "ambiColor");
	glUniform3f(ambiColorLoc, 0.1, 0.1, 0.1);
	//int diffColorLoc = glGetUniformLocation(shaderProgramID, "diffColor");
	glUniform3f(diffColorLoc, 0.0, 0.3, 0.3);
	//int specColorLoc = glGetUniformLocation(shaderProgramID, "specColor");
	glUniform3f(specColorLoc, 0.4, 0.4, 0.4);
	glUniform1f(shininessLoc, 20.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, parent_UFO.m);
	glEnableVertexAttribArray(loc3[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[2]);
	glVertexAttribPointer(loc3[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc3[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[2]);
	glVertexAttribPointer(loc3[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc3[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[2]);
	glVertexAttribPointer(loc3[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_3);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_3_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model3.mPointCount);
	// Set up the child UFO
	mat4 modelChild_UFO = identity_mat4();
	modelChild_UFO = rotate_y_deg(modelChild_UFO, rotate_x);
	modelChild_UFO = rotate_x_deg(modelChild_UFO, rotate_y);
	modelChild_UFO = rotate_z_deg(modelChild_UFO, rotate_z);
	modelChild_UFO = translate(modelChild_UFO, vec3(translation_x3+3.0, translation_y3, translation_z3+3.0));
	modelChild_UFO = scale(modelChild_UFO, vec3(scaling_x, scaling_y, scaling_z));
	modelChild_UFO = parent_UFO * modelChild_UFO;
	//int ambiColorLoc = glGetUniformLocation(shaderProgramID, "ambiColor");
	glUniform3f(ambiColorLoc, 0.1, 0.1, 0.1);
	//int diffColorLoc = glGetUniformLocation(shaderProgramID, "diffColor");
	glUniform3f(diffColorLoc, 0.3, 0.0, 0.5);
	//int specColorLoc = glGetUniformLocation(shaderProgramID, "specColor");
	glUniform3f(specColorLoc, 0.4, 0.4, 0.4);
	glUniform1f(shininessLoc, 20.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild_UFO.m);
	glDrawArrays(GL_TRIANGLES, 0, model3.mPointCount);
	// Model 4
	mat4 modelChild4 = identity_mat4();
	modelChild4 = rotate_y_deg(modelChild4, 0.0f);
	modelChild4 = rotate_x_deg(modelChild4, 0.0f);
	modelChild4 = rotate_z_deg(modelChild4, 0.0f);
	modelChild4 = translate(modelChild4, vec3(translation_x4, translation_y4, translation_z4));
	modelChild4 = scale(modelChild4, vec3(scaling_x, scaling_y, scaling_z));
	glUniform3f(ambiColorLoc, 0, 0, 0);
	glUniform3f(diffColorLoc, 0.6, 0.4, 0.2);
	glUniform3f(specColorLoc, 0.4, 0.4, 0.4);
	glUniform1f(shininessLoc, 10.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild4.m);
	glEnableVertexAttribArray(loc4[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[3]);
	glVertexAttribPointer(loc4[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc4[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[3]);
	glVertexAttribPointer(loc4[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc4[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[3]);
	glVertexAttribPointer(loc4[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindTexture(GL_TEXTURE_2D, tex_4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_4);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_4_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model4.mPointCount);
	// Model 5
	mat4 modelChild5 = identity_mat4();
	modelChild5 = rotate_y_deg(modelChild5, 0.0f);
	modelChild5 = rotate_x_deg(modelChild5, 0.0f);
	modelChild5 = rotate_z_deg(modelChild5, 0.0f);
	modelChild5 = translate(modelChild5, vec3(-8.0f, -8.0f, -8.0f));
	modelChild5 = scale(modelChild5, vec3(scaling_x, scaling_y, scaling_z));
	glUniform3f(ambiColorLoc, 0, 0, 0);
	glUniform3f(diffColorLoc, 0.6, 0.6, 0.6);
	glUniform3f(specColorLoc, 0, 0, 0);
	glUniform1f(shininessLoc, 10.0f);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild5.m);
	glEnableVertexAttribArray(loc5[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[4]);
	glVertexAttribPointer(loc5[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc5[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[4]);
	glVertexAttribPointer(loc5[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc5[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[4]);
	glVertexAttribPointer(loc5[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_5);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_5_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model5.mPointCount);
	// Model point light
	mat4 modelChild_pl = identity_mat4();
	modelChild_pl = rotate_y_deg(modelChild_pl, 0.0f);
	modelChild_pl = rotate_x_deg(modelChild_pl, 0.0f);
	modelChild_pl = rotate_z_deg(modelChild_pl, 0.0f);
	modelChild_pl = translate(modelChild_pl, vec3(point_light_x, point_light_y, point_light_z));
	modelChild_pl = scale(modelChild_pl, vec3(scaling_x/2, scaling_y/2, scaling_z/2));
	glUniform3f(ambiColorLoc, 1.0, 1.0, 1.0);
	glUniform3f(diffColorLoc, 1.0, 1.0, 1.0);
	glUniform3f(specColorLoc, 1.0, 1.0, 1.0);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild_pl.m);
	glEnableVertexAttribArray(loc_point_light[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[5]);
	glVertexAttribPointer(loc_point_light[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc_point_light[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[5]);
	glVertexAttribPointer(loc_point_light[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc_point_light[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[5]);
	glVertexAttribPointer(loc_point_light[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_6);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_6_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model_point_light.mPointCount);
	// Model spot light
	mat4 modelChild_sl = identity_mat4();
	modelChild_sl = rotate_y_deg(modelChild_sl, 0.0f);
	modelChild_sl = rotate_x_deg(modelChild_sl, 0.0f);
	modelChild_sl = rotate_z_deg(modelChild_sl, 0.0f);
	modelChild_sl = translate(modelChild_sl, vec3(spot_light_x, spot_light_y, spot_light_z));
	modelChild_sl = scale(modelChild_sl, vec3(scaling_x / 2, scaling_y / 2, scaling_z / 2));
	glUniform3f(ambiColorLoc, 1.0, 1.0, 1.0);
	glUniform3f(diffColorLoc, 1.0, 1.0, 1.0);
	glUniform3f(specColorLoc, 1.0, 1.0, 1.0);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild_sl.m);
	glEnableVertexAttribArray(loc_spot_light[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[7]);
	glVertexAttribPointer(loc_spot_light[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc_spot_light[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[7]);
	glVertexAttribPointer(loc_spot_light[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[0]);
	glEnableVertexAttribArray(loc_spot_light[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[7]);
	glVertexAttribPointer(loc_spot_light[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//glBindTexture(GL_TEXTURE_2D, tex_6);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_6);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_6_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, model_spot_light.mPointCount);
	// terrain
	mat4 terrainModelMatrix = identity_mat4();
	terrainModelMatrix = translate(terrainModelMatrix, vec3(terrain_x, terrain_y, terrain_z));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, terrainModelMatrix.m);
	glEnableVertexAttribArray(loc_terrain[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VP_VBOs[6]);
	glVertexAttribPointer(loc_terrain[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc_terrain[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VN_VBOs[6]);
	glVertexAttribPointer(loc_terrain[1], 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(VAOs[6]);
	glEnableVertexAttribArray(loc_terrain[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VT_VBOs[6]);
	glVertexAttribPointer(loc_terrain[2], 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_terrain);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_1"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_terrain_2);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture_2"), 0);
	glDrawArrays(GL_TRIANGLES, 0, terrainModel.mPointCount);

	glutSwapBuffers();
}

void updateScene() {
	// Delta Calculation
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;
	// Rotate Model
	rotate_x += ufo_rotation * delta;
	rotate_x = fmodf(rotate_x, 360.0f);
	// Draw the next frame
	glutPostRedisplay();
}

void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// Load mesh into a vertex buffer array
	generateObjectBufferMesh();
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {
	// set mouse movement
	if (key == 'm') {
		if (mouse_movement == false) {
			mouse_movement = true;
		}
		else {
			mouse_movement = false;
		}
	}
	// model rotation
	if (key == 'z') {	// rotate left
		ufo_rotation = 20.0f;
	}
	if (key == 'x') {	// rotate right
		ufo_rotation = -20.0f;
	}
	// camera control
	if (key == 'w') {	// Move camera forwards
		camera_x += cameraDirection_x * cameraSpeed;
		camera_z += cameraDirection_z * cameraSpeed;
		target_x += cameraDirection_x * cameraSpeed;
		target_z += cameraDirection_z * cameraSpeed;
	}
	if (key == 's') {	// Move camera backwards
		camera_x -= cameraDirection_x * cameraSpeed;
		camera_z -= cameraDirection_z * cameraSpeed;
		target_x -= cameraDirection_x * cameraSpeed;
		target_z -= cameraDirection_z * cameraSpeed;
	}
	if (key == 'a') {	// Move camera left
		vec3 left = cross(vec3(cameraDirection_x, cameraDirection_y, cameraDirection_z), up_dir);
		camera_x -= left.v[0] * cameraSpeed;
		camera_z -= left.v[2] * cameraSpeed;
		target_x -= left.v[0] * cameraSpeed;
		target_z -= left.v[2] * cameraSpeed;
	}
	if (key == 'd') {	// Move camera right
		vec3 right = cross(vec3(cameraDirection_x, cameraDirection_y, cameraDirection_z), up_dir);
		camera_x += right.v[0] * cameraSpeed;
		camera_z += right.v[2] * cameraSpeed;
		target_x += right.v[0] * cameraSpeed;
		target_z += right.v[2] * cameraSpeed;
	}
	// scaling
	if (key == 'c') {	// Scale up
		scaling_x += 0.1f;
		scaling_y += 0.1f;
		scaling_z += 0.1f;
	}
	if (key == 'v') {	// Scale down
		scaling_x -= 0.1f;
		scaling_y -= 0.1f;
		scaling_z -= 0.1f;
	}
	// model movement
	if (key == 'u') {
		translation_y4 -= 0.1;
	}
	if (key == 'i') {
		translation_y4 += 0.1;
	}
	if (key == 'o') {
		translation_x4 -= 0.1;
	}
	if (key == 'p') {
		translation_x4 += 0.1;
	}
	if (key == 'h') {
		translation_y3 -= 0.1;
	}
	if (key == 'j') {
		translation_y3 += 0.1;
	}
	if (key == 'k') {
		translation_x3 -= 0.1;
	}
	if (key == 'l') {
		translation_x3 += 0.1;
	}
	if (key == '1') {
		translation_y -= 0.1;
	}
	if (key == '2') {
		translation_y += 0.1;
	}
	if (key == '3') {
		translation_x -= 0.1;
	}
	if (key == '4') {
		translation_x += 0.1;
	}
	// move spot light
	if (key == '5') {
		spot_light_z -= 0.1;
	}
	if (key == '6') {
		spot_light_z += 0.1;
	}
	if (key == '7') {
		spot_light_y -= 0.1;
	}
	if (key == '8') {
		spot_light_y += 0.1;
	}
	if (key == '9') {
		spot_light_x -= 0.1;
	}
	if (key == '0') {
		spot_light_x += 0.1;
	}
	// stop rotation
	if (key == 'r') {
		ufo_rotation = 0.0f;
	}
	// reset position
	if (key == 't') {
		translation_x = -2.0f;
		translation_y = 0.05f;
		translation_z = 2.0f;
		translation_x2 = -8.0f;
		translation_y2 = 0.0f;
		translation_z2 = 0.0f;
		translation_x3 = 8.0f;
		translation_y3 = 3.0f;
		translation_z3 = 1.0f;
		translation_x4 = -1.0f;
		translation_y4 = 1.0f;
		translation_z4 = -8.0f;
		point_light_x = -1.0f;
		point_light_y = 10.0f;
		point_light_z = -8.0f;
		spot_light_x = -1.0f;
		spot_light_y = 6.0f;
		spot_light_z = -8.0f;
		terrain_x = -50.0f;
		terrain_y = -10.0f;
		terrain_z = -80.0f;
		camera_x = 0.0f;
		camera_y = 5.0f;
		camera_z = 15.0f;
		target_x = 0.0f;
		target_y = 0.0f;
		target_z = 0.0f;
		camera_pos = vec3(0.0f, 0.0f, 0.0f);		// initial position of camera
		camera_target = vec3(0.0f, 0.0f, 0.0f);	// initial position of target
		cameraDirection_x = 0.0f;
		cameraDirection_y = 0.0f;
		cameraDirection_z = 0.0f;
	}
	glutPostRedisplay();
}

void arrows(int key, int x, int y) {
	if (key == GLUT_KEY_DOWN) {
		point_light_y -= 0.1;
	}
	if (key == GLUT_KEY_UP) {
		point_light_y += 0.1;
	}
	if (key == GLUT_KEY_LEFT) {
		point_light_x -= 0.1;
	}
	if (key == GLUT_KEY_RIGHT) {
		point_light_x += 0.1;
	}
	glutPostRedisplay();
}

void mouse(int x, int y) {
	// calc forward vector for camera movement
	cameraDirection_x = target_x - camera_x;
	cameraDirection_y = target_y - camera_y;
	cameraDirection_z = target_z - camera_z;
	// save old mouse vals
	mouse_x_previous = mouse_x;
	mouse_y_previous = mouse_y;
	mouse_x = x;
	mouse_y = y;
	if (mouse_movement == true) {
		if ((mouse_x - mouse_x_previous) > 0)	// moved right
			target_x += 0.2f;
		else if ((mouse_x - mouse_x_previous) < 0)	// moved left
			target_x -= 0.2f;
		if ((mouse_y - mouse_y_previous) > 0)	// moved up
			target_y += 0.2f;
		else if ((mouse_y - mouse_y_previous) < 0)	// moved down
			target_y -= 0.2f;
	}
}

int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Final Project");
	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutSpecialFunc(arrows);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouse);
	glutIdleFunc(updateScene);
	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}