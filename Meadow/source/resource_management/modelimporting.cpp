#include "modelimporting.h"
#if 0 
#include "imageloader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "materials/phongmaterial.h"

/*
TODO:
Refactor/clean up this mess of a file
*/





glm::mat4 matAssimpToGlm(aiMatrix4x4 aimat) {
	return glm::transpose(glm::mat4(
		aimat[0][0], aimat[0][1], aimat[0][2], aimat[0][3],
		aimat[1][0], aimat[1][1], aimat[1][2], aimat[1][3],
		aimat[2][0], aimat[2][1], aimat[2][2], aimat[2][3],
		aimat[3][0], aimat[3][1], aimat[3][2], aimat[3][3]));
}
static std::shared_ptr<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, ImageCache& textureCache, std::string directory)
{
	// Implement this comment block if you want to load multiple textures of the same type.
	// Remember to use imagecache.h to prevent loading the same texture multiple times!
	// ----

	/*std::vector<Texture> textures;
	aiString path;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiReturn ret = mat->GetTexture(type, i, &path);
		if (ret != aiReturn_SUCCESS) {
			std::cout << "GetTexture failed" << std::endl;
		}
		else {
			std::cout << path.C_Str() << std::endl;
			textures.push_back(loadTextureFromFile(std::string(path.C_Str()), typeName));
		}
	}*/
	// ----

	aiString path;
	aiReturn ret = mat->GetTexture(type, 0, &path);
	if (ret != aiReturn_SUCCESS) {
		Locator::getLogger()->getLogger()->info("Assimp failed to get texture type {} texture in folder: {}/\n", type, directory);
		return nullptr;
	}
	else {
		//textures.push_back(loadTextureFromFile(std::string(path.C_Str()), typeName));
		return std::make_shared<Texture>(
			std::string(directory + "/" + path.C_Str()),
			GL_TEXTURE_2D,
			textureCache);
		
	}
}

static std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, ImageCache& textureCache)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Loop through vertices and copy position, normal, texturecoords
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;
		glm::vec3 normal = glm::vec3(0.0f);
		if (mesh->HasNormals()) {
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
		}
		glm::vec2 texCoords = glm::vec2(0.0f);
		if (mesh->HasTextureCoords(0)) {
			texCoords.x = mesh->mTextureCoords[0][i].x;
			texCoords.y = mesh->mTextureCoords[0][i].y;

		}
		Vertex vertex(position, normal, texCoords);
		vertices.push_back(vertex);
	}
	// Loop through indices and copy their data
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		// triangulated mesh expected
		unsigned int indexCount = mesh->mFaces[i].mNumIndices;
		if (indexCount == 3) {
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[1]);
			indices.push_back(mesh->mFaces[i].mIndices[2]);
		}
		else
			std::cout << "model encountered a non-triangulated face with vertex count of: " + indexCount << std::endl;
	}
	// Implement this comment block if you want to load multiple textures of the same type.
	// Remember to use imagecache.h to prevent loading the same texture multiple times!
	// ----
		// Here we can choose what map types to load
	//std::vector<Texture> diffuseMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, "diffuse_map");
	//std::vector<Texture> specularMaps = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, "specular_map");
	//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// ----
	return std::make_shared<Mesh>(vertices, indices);
}
static void processNode(std::shared_ptr<GraphicsComponent> parent, aiNode* node, const aiScene* scene, ImageCache &textureCache, std::string directory)
{
	auto objptr = std::make_shared<GraphicsComponent>(); // assimp node => wilkan engine object
	objptr->name = node->mName.C_Str();
	parent->addChild(objptr);
	objptr->setModelMatrix(matAssimpToGlm(node->mTransformation)); //fbx imports in huge scale for some reason
	std::vector<unsigned int> addedMaterials;
	std::vector<unsigned int>::iterator it;
	int materialCount = 0;
	int materialSlot = 0;
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		it = std::find(addedMaterials.begin(), addedMaterials.end(), aimesh->mMaterialIndex);
		if (it == addedMaterials.end()) {
			if (materialCount < objptr->MAX_MATERIAL_SLOTS) {
				// Encountered a new material, create new material slot for object
				addedMaterials.push_back(aimesh->mMaterialIndex);
				objptr->setMaterial(std::make_shared<PhongMaterial>(
					loadMaterialTextures(scene->mMaterials[aimesh->mMaterialIndex], aiTextureType_DIFFUSE, textureCache, directory),
					loadMaterialTextures(scene->mMaterials[aimesh->mMaterialIndex], aiTextureType_SPECULAR, textureCache, directory)), materialCount);
				materialCount++;
			}
			else {
				Locator::getLogger()->getLogger()->info(
					"ERROR: Tried to import object with more than the max of {} materials",
					objptr->MAX_MATERIAL_SLOTS);
			}
		}
		else {
			materialSlot = std::distance(addedMaterials.begin(), it);
		}

		objptr->addMesh(processMesh(aimesh, scene, textureCache), materialSlot);
		
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(objptr, node->mChildren[i], scene, textureCache, directory);
	}

	
	return;
}

std::shared_ptr<GraphicsComponent> ModelImporting::importWavefront(std::string path, ImageCache &textureCache)
{
	auto objptr = std::make_shared<GraphicsComponent>();

	//ImageCache cache;
	//Assimp::Importer importer;
	//const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	//if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	//{
	//	std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
	//	return nullptr;
	//}
	//std::string directory = path.substr(0, path.find_last_of('/'));

	//processNode(objptr.get(), scene->mRootNode, scene, textureCache, directory);
	//// free image cache since all textures of the model are loaded at this point
	//cache.freeAllData();

	return objptr;
}

std::shared_ptr<GraphicsComponent> ModelImporting::objsFromFile(std::string path, ImageCache& textureCache)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return nullptr;
	}

	auto objptr = std::make_shared<GraphicsComponent>();
	objptr->name = path;
	ImageCache cache;
	std::string directory = path.substr(0, path.find_last_of('/'));

	processNode(objptr, scene->mRootNode, scene, textureCache, directory);
	// free image cache since all textures of the model are loaded at this point
	cache.freeAllData();

	return objptr;
}

#endif // if 0