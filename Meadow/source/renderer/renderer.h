#pragma once
#include "assets/submesh.h"
#include "assets/vertex2d.h"
#include <glm/mat4x4.hpp>
#include "windowmanager.h"
#include <array>
/*
* I only know OpenGL as I'm writing this class, so this may need to be
* modified to provide a more general Graphics api wrapper.
*/
class Renderer
{
public:

	/*
	* Testing funcs (depth, stencil testing)
	*/
	enum class TestingFuncs {
		NEVER = 0,
		LESS,
		LEQUAL,
		GREATER,
		GEQUAL,
		EQUAL,
		NOTEQUAL,
		ALWAYS
	};

	/*
	* Testing actions (depth, stencil testing)
	*/
	enum class TestingActions {
		KEEP = 0,
		ZERO,
		REPLACE,
		INCR,
		INCR_WRAP,
		DECR,
		DECR_WRAP,
		INVERT
	};

	virtual bool initialize(WindowManager* windowMan) = 0;
	virtual void setViewportSize(int width, int height) = 0;
	
	/*
	* Mesh buffer funcs
	*/
	// Use these to load mesh into GPU memory
	virtual void meshBuffersGenerate(int meshId) = 0;
	virtual void meshBuffersPushData(int meshId,
		const std::vector<Vertex> &vertices,
		const std::vector<unsigned int> &indices) = 0;
	virtual void mesh2DBuffersPushData(int meshId,
		const std::vector<Vertex2D>& vertices,
		const std::vector<unsigned int>& indices) = 0;

	// Use this to remove mesh from GPU memory
	virtual void meshBuffersDelete(int meshId) = 0;

	// Draw a loaded mesh with currently bound textures, shader, shader uniforms
	virtual void drawMesh(int meshId) = 0;

	/*
	* Shader program
	*/
	virtual void createShaderProgram(const unsigned int& id, const char* vertexShader, const char* fragShader) = 0;
	virtual void useShaderProgram(const unsigned int& id) = 0;
	virtual void deleteShaderProgram(const unsigned int& id) = 0;
	/*
	* Shader uniform setters
	*/
	virtual void setBool  (const unsigned int &sdrId, const char* name, bool value) = 0;
	virtual void setInt   (const unsigned int &sdrId, const char* name, int value) = 0;
	virtual void setuInt   (const unsigned int &sdrId, const char* name, unsigned int value) = 0;
	virtual void setFloat (const unsigned int &sdrId, const char* name, float value) = 0;
	virtual void setFloat3(const unsigned int &sdrId, const char* name, glm::vec3 value) = 0;
	virtual void setFloat4(const unsigned int &sdrId, const char* name, glm::vec4 value) = 0;
	virtual void setMat4f (const unsigned int &sdrId, const char* name, glm::mat4 value) = 0;

	/*
	* Textures
	*/
	enum class ImageFormat{RGB = 0, sRGB, RGB_HIGH_PRECISION, RGBA, sRGBA, R, DEPTH, DEPTH16};
	virtual void create2DTexture(const unsigned int& id, const unsigned int &width, const unsigned int &height,
		ImageFormat formatSrc, ImageFormat formatDestination, unsigned char* imgData, bool mipmap = true) = 0;
	virtual void create2DTextureMS(const unsigned int& id, const unsigned int& width, const unsigned int& height, ImageFormat format) = 0;
	virtual void bindTo2DSampler(const unsigned int& texId, const unsigned int& samplerId) = 0;
	virtual void bindTo2DSamplerMS(const unsigned int& texId, const unsigned int& samplerId) = 0;
	virtual void unbindTexture(const unsigned int& samplerId) = 0;
	virtual void deleteTexture(const unsigned int& id) = 0;

	/*
	* Buffer clearing
	*/
	virtual int getColorBuffBit() = 0;
	virtual int getStencilBuffBit() = 0;
	virtual int getDepthBuffBit() = 0;
	virtual void clearBuffer(int buffers) = 0;
	virtual void setClearColor(glm::vec4 color) = 0; // Set color buffer clearing color

	/*
	* Depth testing
	*/
	virtual void depthTesting(bool enable) = 0;
	virtual void depthMask(bool enable) = 0;


	/*
	* Stencil testing
	*/
	virtual void stencilTesting(bool enable) = 0;
	virtual void setStencilMask(unsigned int) = 0;
	virtual void setStencilFunc(TestingFuncs func, int ref, unsigned int mask) = 0;
	virtual void setStencilOp(TestingActions stencilFail, TestingActions depthFail, TestingActions depthPass) = 0;
	virtual void setStencilClearValue(unsigned int) = 0;

	/*
	* Blending
	*/
	virtual void blending(bool enable) = 0;

	/*
	* Face culling
	*/
	virtual void faceCulling(bool enable) = 0;
	virtual void cullFront() = 0;
	virtual void cullBack() = 0;
	virtual void cullFrontAndBack() = 0;

	/*
	* Framebuffers
	*/
	virtual void createFrameBuffer(int buffId, int texId, unsigned int width, unsigned int height) = 0;
	virtual void createFrameBufferDepthMapOnly(int buffId, int texId, unsigned int width, unsigned int height) = 0;
	virtual void createFrameBufferMultisample(int buffId, int texId, unsigned int width, unsigned int height) = 0;
	virtual void bindFrameBuffer(int buffId) = 0;
	virtual void bindFrameBufferRead(int buffId) = 0;
	virtual void bindFrameBufferDraw(int buffId) = 0;
	virtual void bindFrameBufferDefault() = 0;
	virtual void deleteFrameBuffer(int buffId) = 0;
	virtual bool checkFrameBufferStatus() = 0;
	virtual void getFrameBufferDimensions(int buffId, int& width, int& height) = 0;
	virtual void blitFramebuffer(int width, int height) = 0; //blits read to draw fb

	/*
	* Cubemaps
	*/
	virtual void createCubemap(int cmId) = 0;
	virtual void cubemapLoadTextures(int cmId, std::array<unsigned char*, 6> images, int width, int height) = 0;
	virtual void deleteCubemap(int cmId) = 0;
	virtual void bindCubemap(int cmId) = 0;

	/*
	* Drawing mode
	*/
	virtual void wireframe(bool enable) = 0;

	/*
	* Multisampling toggle
	*/
	virtual void multisampling(bool enable) = 0;

	/*
	* Disable/Enable drawing on the color buffer
	*/
	virtual void setColorMask(bool flag) = 0;

	/*
	* Benchmarking
	*/
	virtual void startTimer(unsigned int& q1, unsigned int& q2) = 0; //waits for jobs to finish, starts timer
	virtual void endTimer(unsigned int q1, unsigned int q2, float& elapsedTimer) = 0;

};

