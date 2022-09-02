#include "shadermanager.h"
#include "service_locator/rendererlocator.h"
#include "input/inputevents.h"
ShaderManager::ShaderManager() :
	m_currentShader(nullptr),
	m_shaderMap(),
	m_boolMapDraw(),
	m_intMapDraw(),
	m_floatMapDraw(),
	m_vec3MapDraw(),
	m_mat4MapDraw(),
	m_boolMapFrame(),
	m_intMapFrame(),
	m_floatMapFrame(),
	m_vec3MapFrame(),
	m_mat4MapFrame(),
	m_texSamplerMap()
{
	m_texSamplerMap.insert({ Texture::TextureType::CUBE_MAP, 0 });
	m_texSamplerMap.insert({ Texture::TextureType::DIFFUSE_MAP, 0 });
	m_texSamplerMap.insert({ Texture::TextureType::SPECULAR_MAP, 1 });
	m_texSamplerMap.insert({ Texture::TextureType::OPACITY_MAP, 2 });
	m_texSamplerMap.insert({ Texture::TextureType::NORMAL_MAP, 3 });

	/*
	* Subscribe to events
	*/
	InputEvents::ShaderHotReloadEvent::subscribe(
		std::bind(&ShaderManager::hotReloadShader, this, std::placeholders::_1)
	);
}


void ShaderManager::provideShader(std::string name, std::unique_ptr<Shader> shader)
{
	m_shaderMap.insert({ name, std::move(shader) });
}

void ShaderManager::setCurrentShader(std::string name)
{
	auto it = m_shaderMap.find(name);
	if (it != m_shaderMap.end())
		m_currentShader = (it->second).get();
		RendererLocator::getRenderer()->useShaderProgram(it->second->getId());
}

Shader* ShaderManager::getCurrentShader()
{
	return m_currentShader;
}

void ShaderManager::setUniformDrawSpecific(std::string uName, bool uValue)
{
	m_boolMapDraw[uName] = uValue;
}

void ShaderManager::setUniformDrawSpecific(std::string uName, int uValue)
{
	m_intMapDraw[uName] = uValue;
}

void ShaderManager::setUniformDrawSpecific(std::string uName, unsigned int uValue)
{
	m_intMapDraw[uName] = uValue;
}

void ShaderManager::setUniformDrawSpecific(std::string uName, float uValue)
{
	m_floatMapDraw[uName] = uValue;
}

void ShaderManager::setUniformDrawSpecific(std::string uName, glm::vec3 uValue)
{
	m_vec3MapDraw[uName] = uValue;
}

void ShaderManager::setUniformDrawSpecific(std::string uName, glm::mat4 uValue)
{
	m_mat4MapDraw[uName] = uValue;
}

void ShaderManager::setFrameUniform(std::string uName, bool uValue)
{
	m_boolMapFrame[uName] = uValue;
}

void ShaderManager::setFrameUniform(std::string uName, int uValue)
{
	m_intMapFrame[uName] = uValue;
}

void ShaderManager::setFrameUniform(std::string uName, unsigned int uValue)
{
	m_uintMapFrame[uName] = uValue;
}

void ShaderManager::setFrameUniform(std::string uName, float uValue)
{
	m_floatMapFrame[uName] = uValue;
}

void ShaderManager::setFrameUniform(std::string uName, glm::vec3 uValue)
{
	m_vec3MapFrame[uName] = uValue;
}

void ShaderManager::setFrameUniform(std::string uName, glm::mat4 uValue)
{
	m_mat4MapFrame[uName] = uValue;
}

void ShaderManager::forwardUniformsDrawSpecific()
{
	for (auto uniform : m_boolMapDraw)
		RendererLocator::getRenderer()->setBool(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_intMapDraw)
		RendererLocator::getRenderer()->setInt(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_uintMapDraw)
		RendererLocator::getRenderer()->setuInt(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_floatMapDraw)
		RendererLocator::getRenderer()->setFloat(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_vec3MapDraw)
		RendererLocator::getRenderer()->setFloat3(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_mat4MapDraw)
		RendererLocator::getRenderer()->setMat4f(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
}

void ShaderManager::forwardFrameUniforms()
{
	for (auto uniform : m_boolMapFrame)
		RendererLocator::getRenderer()->setBool(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_intMapFrame)
		RendererLocator::getRenderer()->setInt(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_uintMapFrame)
		RendererLocator::getRenderer()->setuInt(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_floatMapFrame)
		RendererLocator::getRenderer()->setFloat(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_vec3MapFrame)
		RendererLocator::getRenderer()->setFloat3(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
	for (auto uniform : m_mat4MapFrame)
		RendererLocator::getRenderer()->setMat4f(m_currentShader->getId(), uniform.first.c_str(), uniform.second);
}

unsigned int ShaderManager::getTexSamplerId(Texture::TextureType type)
{
	return m_texSamplerMap.at(type);
}

void ShaderManager::hotReloadShader(std::string name)
{
	auto it = m_shaderMap.find(name);
	if (it != m_shaderMap.end())
		it->second->hotReload();
}
