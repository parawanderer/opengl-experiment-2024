#ifndef POSTPROCESSOR_MINE_H
#define POSTPROCESSOR_MINE_H
#include "AnimatedEntity.h"
#include "Quad.h"
#include "Shader.h"

/**
 * \brief Checkout the full doc regarding how this works at (from the source code root) ./resources/doc/distance_field_postprocessor/README.md
 */
class DistanceFieldPostProcessor
{
public:
	DistanceFieldPostProcessor(Quad* quad, int currentWidth, int currentHeight);

	~DistanceFieldPostProcessor();

	void updateNewWidthHeight(int newWidth, int newHeight);
	void setOutlineSize(float outlineSize);
	void setOutlinePulsate(bool doPulsate);

	void computeAndRenderOverlay(const glm::mat4& projection, const glm::mat4& view, const std::vector<AnimatedEntity*>& objects, unsigned int overlayOutputToBufferId);

private:
	/**
	 * \brief 3D to 2D conversion Shader (works in 3D space). Creates a white on black silhouette
	 * like: https://miro.medium.com/v2/resize:fit:828/format:webp/1*QAk8u54-0-KErVNO-isSRw.png
	 */
	Shader _maskingShader;
	/**
	 * \brief 2D to 2D (works on 2D quad). Maps out the non-black pixels of a texture to the UV coordinates in the RG values of RGBA pixels
	 * like: https://miro.medium.com/v2/resize:fit:828/format:webp/1*vXogFpiOLmlcwRXSi__LXA.png
	 */
	Shader _UVMaskShader;
	/**
	 * \brief 2D to 2D (works on 2D quad). Maps out the output of _UVMaskShader to an approximate (unsigned) distance field.
	 * like: https://miro.medium.com/v2/resize:fit:640/format:webp/1*6nt7jdfoziuF6lGqcIyqlw.png
	 *
	 * Needs to run repeatedly to achieve the effect
	 * see:
	 * - https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9
	 * - https://itscai.us/blog/post/jfa/
	 * - https://www.youtube.com/watch?v=nKJgUsAU2d0
	 */
	Shader _jfaFloodingStepShader;
	/**
	 * \brief 2D to 2D (works on 2D quad). Uses the the distance field generated using _jfaFloodingStepShader to output a specific result/outline effect.
	 * This produces an overlay texture with transparency. The overlay texture can be applied on top of an original rendering of the image.
	 *
	 * 
	 */
	Shader _jfaDistanceFieldConvertorShader;
	/**
	 * \brief 2D to 2D (works on 2D quad). This shader just outputs the (single) input texture.
	 */
	Shader _justRenderThe2DTextureShader;

	Quad* _quad;

	int _currentWidth;
	int _currentHeight;

	float _outlineSize = 0.001f;
	bool _outlinePulsate = false;

	unsigned int _framebuffer1;
	unsigned int _textureColorbuffer1;
	unsigned int _rbo1;

	unsigned int _framebuffer2;
	unsigned int _textureColorbuffer2;
	unsigned int _rbo2;

	void setupShaders();
	void setupFrameBuffers();
};

#endif