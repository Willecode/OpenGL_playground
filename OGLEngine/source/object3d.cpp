#include "Object3D.h"
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp> // vec/mat debug prints
Object3D::Object3D(): 
	modelMatrix(glm::mat4(1.0f)),
	name("none"),
	id(-1),
	shader(nullptr),
	light(nullptr)
{}

void Object3D::draw(std::unordered_map<Object3D*, LightSource*> sceneLights, int pointLightCount, int dirLightCount, glm::mat4 accTransform)
{
	if ((!meshes.empty()) && shader) {
		shader->use();

		// Pass lighting
		shader->setInt("pointLightCount", pointLightCount);
		shader->setInt("dirLightCount", dirLightCount);
		int pointLightId = 0;
		int dirLightId = 0;
		for (auto const& x : sceneLights) {
			// Lights don't affect the object they're attached to
			if (!(x.first->id == id)) {
				if (x.second->getType() == LightSource::LightType::POINTLIGHT) {
					x.second->passToShader(shader, pointLightId, x.first->getPosition());
					pointLightId++;
				}
				else if (x.second->getType() == LightSource::LightType::DIRLIGHT) {
					x.second->passToShader(shader, dirLightId, x.first->getPosition());
					dirLightId++;
				}
			}
		}
		// Pass model matrix
		shader->setMat4f("model", accTransform * modelMatrix);
		//glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		
		for (int i = 0; i < meshes.size(); i++) {
			materialSlots[meshes[i].materialSlot]->passToShader(shader);
			meshes[i].mesh->draw();
		}
	}
	for (auto obj : childObjects) {
		obj->draw(sceneLights, pointLightCount, dirLightCount, accTransform * modelMatrix);
	}
}

void Object3D::addLightSource(std::shared_ptr<LightSource> lightSrc)
{
	light = lightSrc;
}

void Object3D::setModelMatrix(const glm::mat4& model)
{
	modelMatrix = model;
}

void Object3D::setMaterial(std::shared_ptr<Material> mat, int materialSlot)
{
	if (mat != nullptr) {
		if (materialSlot < MAX_MATERIAL_SLOTS) {
			materialSlots[materialSlot] = mat;
		}
		else {
			fmt::print("ERROR: Tried to set to an out of bounds material slot in Object \"{}\"", name);
		}
	}
	else
	{
		fmt::print("ERROR: Tried to pass null material to Object \"{}\"", name);
	}

}

void Object3D::setShader(Shader* sdr)
{
	shader = sdr;
	for (auto child : childObjects) {
		child->setShader(sdr);
	}
}

Material* Object3D::getMaterial(int materialSlot)
{
	if (materialSlot < MAX_MATERIAL_SLOTS) {
		if (materialSlots[materialSlot] != nullptr)
			return materialSlots[materialSlot].get();
		
	}
	return nullptr;
}

glm::mat4 Object3D::getModelMatrix()
{
	return modelMatrix;
}

std::vector<Mesh*> Object3D::getMeshes()
{
	std::vector<Mesh*> retVec;
	for (size_t i = 0; i < meshes.size(); i++) {
		retVec.push_back(meshes[i].mesh.get());
	}
	return retVec;
}

void Object3D::addMesh(std::shared_ptr<Mesh> mesh, int materialSlot)
{
	if (mesh != nullptr) {
		MeshStruct newMesh;
		newMesh.mesh = mesh;
		newMesh.materialSlot = materialSlot;
		meshes.push_back(newMesh);
	}
	
}

void Object3D::addChild(std::shared_ptr<Object3D> child)
{
	childObjects.push_back(child);
	return;
}

std::shared_ptr<LightSource> Object3D::getLightSourceOwnerhip()
{
	return light;
}

LightSource* Object3D::getLightSource()
{
	return light.get();
}

glm::vec3 Object3D::getPosition()
{
	return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

Shader* Object3D::getShader()
{
	return shader;
}
