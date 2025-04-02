#include "DistanceFieldPostProcessor.h"

#include <iostream>
#include <GLFW/glfw3.h>

#include "Colors.h"
#include "ErrorUtils.h"
#include "FileConstants.h"

#define TEXTURE_INTERNAL_FORMAT GL_RGBA32F
#define TEXTURE_FORMAT GL_RGBA
#define TEXTURE_TYPE GL_FLOAT

DistanceFieldPostProcessor::DistanceFieldPostProcessor(
	Quad* quad,
	int currentWidth, 
	int currentHeight
) :
	_maskingShader(Shader::fromFiles(SHADER_MASKING_VERT, SHADER_MASKING_FRAG)),
	_UVMaskShader(Shader::fromFiles(SHADER_JFA_INIT_VERT, SHADER_JFA_INIT_FRAG)),
	_jfaFloodingStepShader(Shader::fromFiles(SHADER_JFA_ALGORITHM_VERT, SHADER_JFA_ALGORITHM_FRAG)),
	_jfaDistanceFieldConvertorShader(Shader::fromFiles(SHADER_DISTANCEFIELD_VERT, SHADER_DISTANCEFIELD_FRAG)),
	_justRenderThe2DTextureShader(Shader::fromFiles(SHADER_PASSTHROUGH_VERT, SHADER_PASSTHROUGH_FRAG)),
	_currentWidth(currentWidth),
	_currentHeight(currentHeight)
{
	// configure shaders
	this->setupShaders();

	// configure quad
	this->_quad = quad;

	// configure frame buffers
	this->setupFrameBuffers();
}

DistanceFieldPostProcessor::~DistanceFieldPostProcessor()
{
	glDeleteTextures(1, &this->_textureColorbuffer1);
	glDeleteTextures(1, &this->_textureColorbuffer2);

	glDeleteRenderbuffers(1, &this->_rbo1);
	glDeleteRenderbuffers(1, &this->_rbo2);

	glDeleteFramebuffers(1, &this->_framebuffer1);
	glDeleteFramebuffers(1, &this->_framebuffer2);
}

void DistanceFieldPostProcessor::setupShaders()
{
	this->_UVMaskShader.use();
	this->_UVMaskShader.setInt("mask", 0);

	this->_jfaFloodingStepShader.use();
	this->_jfaFloodingStepShader.setInt("UVtexture", 0);
	this->_jfaFloodingStepShader.setInt("textureWidth", this->_currentWidth);
	this->_jfaFloodingStepShader.setInt("textureHeight", this->_currentHeight);

	this->_jfaDistanceFieldConvertorShader.use();
	this->_jfaDistanceFieldConvertorShader.setInt("screenTexture", 0);

	this->_justRenderThe2DTextureShader.use();
	this->_justRenderThe2DTextureShader.setInt("screenTexture", 0);
}

void DistanceFieldPostProcessor::setupFrameBuffers()
{
	// https://learnopengl.com/Advanced-OpenGL/Framebuffers
	glGenFramebuffers(1, &this->_framebuffer1); // for "off-screen rendering"
	glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer1);

	// generate texture
	glGenTextures(1, &this->_textureColorbuffer1);
	glBindTexture(GL_TEXTURE_2D, this->_textureColorbuffer1);
	glTexImage2D(GL_TEXTURE_2D, 0, TEXTURE_INTERNAL_FORMAT, this->_currentWidth, this->_currentHeight, 0, TEXTURE_FORMAT, TEXTURE_TYPE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // must be GL_NEAREST to do point-sampling (i.e. don't interpolate between colours). Not doing point-sampling (=interpolating) messes up the Distance Field
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_textureColorbuffer1, 0);
	glCheckError();


	glGenRenderbuffers(1, &this->_rbo1);
	glBindRenderbuffer(GL_RENDERBUFFER, this->_rbo1);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->_currentWidth, this->_currentHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->_rbo1); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer 1 is not complete!" << std::endl;
		throw std::exception("Framebuffer 1 is not complete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// second one for multiple pass switching
	glGenFramebuffers(1, &this->_framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer2);

	// generate texture
	glGenTextures(1, &this->_textureColorbuffer2);
	glBindTexture(GL_TEXTURE_2D, this->_textureColorbuffer2);
	glTexImage2D(GL_TEXTURE_2D, 0, TEXTURE_INTERNAL_FORMAT, this->_currentWidth, this->_currentHeight, 0, TEXTURE_FORMAT, TEXTURE_TYPE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_textureColorbuffer2, 0);
	glCheckError();


	glGenRenderbuffers(1, &this->_rbo2);
	glBindRenderbuffer(GL_RENDERBUFFER, this->_rbo2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->_currentWidth, this->_currentHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->_rbo2);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer 2 is not complete!" << std::endl;
		throw std::exception("Framebuffer 2 is not complete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DistanceFieldPostProcessor::updateNewWidthHeight(int newWidth, int newHeight)
{
	this->_currentWidth = newWidth;
	this->_currentHeight = newHeight;
}

void DistanceFieldPostProcessor::setOutlineSize(float outlineSize)
{
	this->_outlineSize = outlineSize;
}

/**
 * 	trying to create an outline following https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9
 *	https://www.youtube.com/watch?v=nKJgUsAU2d0
 *	I chose to not follow this one: https://learnopengl.com/Advanced-OpenGL/Stencil-testing
 *	because I want a fixed-width outline like you can make in photoshop (same thing the author of that blog post wants)
 *	I also chose not to do anything using a vertex shader with displacement along the normal based on distance because
 *	I read in the same blog post that it is not a generic enough solution, and that that solution breaks with certain models
 *	The final rendering solution is going to be based on https://cdn.cloudflare.steamstatic.com/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
 * 	I suppose, which was linked in https://learnopengl.com/In-Practice/Text-Rendering too, which would look like this:
 *	https://www.youtube.com/watch?v=1b5hIMqz_wM
 *	
 *	this "Jump Flood Algorithm" also seemed pretty interesting with it's O(log n) time complexity
 *	https://en.wikipedia.org/wiki/Jump_flooding_algorithm
 *	I also looked up what "compute shaders" were here
 *
 *	I'll choose to not split this method off unto submethods per step due to potential performance benefits for now
 */
void DistanceFieldPostProcessor::computeAndRenderOverlay(const glm::mat4& projection, const glm::mat4& view, const std::vector<RenderableGameObject*>& objects, unsigned int overlayOutputToBufferId)
{
#pragma region STEP_1_MASKING

	// Step 1. masking outline
	glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer1);
	glEnable(GL_DEPTH_TEST);
	glClearColor(Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	this->_maskingShader.use();
	this->_maskingShader.setMat4("projection", projection);
	this->_maskingShader.setMat4("view", view);
	glCheckError();

	for (auto obj : objects)
	{
		obj->draw(this->_maskingShader);
	}
	glCheckError();
#pragma endregion

#pragma region STEP_2_JFA_INIT

	// Step 2. conversion to UV coords of white region of mask
	glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer2);
	glDisable(GL_DEPTH_TEST);
	glClearColor(Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	this->_UVMaskShader.use();
	this->_quad->draw(this->_textureColorbuffer1);

	glCheckError();
#pragma endregion

#pragma region STEP_3_RUN_JUMP_FLOOD_ALGORITHM

	// Step 3. run the jump flood algorithm based on the screen size
	// the runtime complexity is O(log n) based on n = 2^(ceil(log2(max(width, height)))-1) which is N/2 if max(width, height) = N is a power of 2
	// alternatively: n is max(width,height) rounded up to the next power of two (based on a grid of n x n pixels)
	// see: https://en.wikipedia.org/wiki/Jump_flooding_algorithm
	// main reference here: https://computergraphics.stackexchange.com/questions/2102/is-jump-flood-algorithm-separable


	unsigned int currentFrameBuffer = this->_framebuffer1;
	unsigned int currentTexture = this->_textureColorbuffer1;
	unsigned int lastFrameBuffer = this->_framebuffer2;
	unsigned int lastTexture = this->_textureColorbuffer2;

	this->_jfaFloodingStepShader.use();
	const int nrOfJFAPasses = (int)ceil(log2(std::max(this->_currentWidth, this->_currentHeight)));
	for (int i = 1; i <= nrOfJFAPasses; ++i)
	{
		const float stepSize = 1.0f / pow(2, i);
		this->_jfaFloodingStepShader.setFloat("stepSize", stepSize);

		glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer); // bind to the next buffer/texture
		glDisable(GL_DEPTH_TEST);
		glClearColor(Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, 1.0f); // clear out buffer
		glClear(GL_COLOR_BUFFER_BIT);
		glCheckError();

		// draw
		this->_quad->draw(lastTexture);// bind to the texture that was written to in the last run
		glCheckError();

		// switch them back up
		const unsigned int tmpBuff = currentFrameBuffer;
		currentFrameBuffer = lastFrameBuffer;
		lastFrameBuffer = tmpBuff;

		const unsigned int tmpTex = currentTexture;
		currentTexture = lastTexture;
		lastTexture = tmpTex;
	}
	glCheckError();

#pragma endregion

#pragma region STEP_4_DISTANCE_FIELD_EFFECT_COMPUTE

	// step 4. compute an effect overlay using the (unsigned) distance field we have generated in the previous step

	glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer); // back to default (output to screen)
	glDisable(GL_DEPTH_TEST);
	glClearColor(Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glCheckError();


	this->_jfaDistanceFieldConvertorShader.use();
	const float outlinePulse = sin(6.0f * glfwGetTime()) / 1000.0f;
	this->_jfaDistanceFieldConvertorShader.setFloat("outlinePlacementOffset", this->_outlineSize + outlinePulse);

	this->_quad->draw(lastTexture);
	glCheckError();

#pragma endregion

#pragma region STEP_5_APPLY_OVERLAY_ON_TOP_OF_RENDERED_IMAGE_SO_FAR

	// step 5. apply the rendered image

	glBindFramebuffer(GL_FRAMEBUFFER, overlayOutputToBufferId); // output to this buffer (usually buffer Id = 0 aka the default aka the screen)

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	this->_justRenderThe2DTextureShader.use();
	this->_quad->draw(currentTexture);

	glCheckError();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
#pragma endregion
}
