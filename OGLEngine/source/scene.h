#pragma once
#include "Object3D.h"
#include <vector>
class Scene
{
public:
	Scene();
	void addObject(Object3D* obj);
	void drawScene();
	void updateLighting();

private:
	int objIdCounter;

	std::vector<Object3D*> sceneObjects;
	std::unordered_map<Object3D*, LightSource*> sceneLights; // TODO: make into non dynamic array
};	

