#include "AnimatedEntity.h"


void AnimatedEntity::setupEntityShaderForAnim(Shader& shader, const std::vector<glm::mat4>& transforms)
{
	shader.setBool("doAnimate", true);
	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBoneMatrices[" + std::to_string(i) + "]", transforms[i]);
		glGetError();
	}
}

void AnimatedEntity::clearEntityShaderForAnim(Shader& shader)
{
	shader.setBool("doAnimate", false);
}
