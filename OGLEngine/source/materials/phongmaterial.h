#pragma once
#include "material.h"
#include <memory>
class PhongMaterial: public Material
{
public:
	float shininess;
	glm::vec3 diffuse;
	glm::vec3 specular;
	PhongMaterial(glm::vec3 diffuse, glm::vec3 specular, float shininess = 10.0f);
	PhongMaterial(
		std::shared_ptr<Texture> diffuseMap = nullptr,
		std::shared_ptr<Texture> specularMap= nullptr,
		float shininess = 10.0f);
	void passToShader(Shader* shader) override;

private:
	std::shared_ptr<Texture> diffuseMap;
	std::shared_ptr<Texture> specularMap;
	
};

