#pragma once
#include "asset.h"
#include <memory>
#include <vector>
#include <array>
#include "shader/shadermanager.h"
class Cubemap : public Asset
{
public:
	/*
	* param images: right, left, top, bottom, back, front image
	*/
	Cubemap(
		std::array<std::unique_ptr<std::vector<unsigned char>>, 6> images,
		int width,
		int height,
		std::string name = "Nameless cubemap");
	void load();
	void draw();

	AssetType getAssetType() override;
private:
	std::array<std::unique_ptr<std::vector<unsigned char>>, 6> m_images;
	int m_imgWidth, m_imgHeight;
	unsigned int m_cubeId;
};

