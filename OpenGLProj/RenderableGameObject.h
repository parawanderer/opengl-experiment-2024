#ifndef GAMEOBJECT_MINE_H
#define GAMEOBJECT_MINE_H

#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

#include "Model.h"
#include "Shader.h"

/**
 * \brief A RenderableGameObject holds world transformation related data for a model (and its meshes)
 */
class RenderableGameObject
{
public:
	/**
	 * \brief Initialise the object from the file path (does not reuse models)
	 */
	RenderableGameObject(const char* modelFilePath);

	/**
	 * \brief Allows reusing the model in multiple instances of a RenderableGameObject
	 */
	RenderableGameObject(Model* model);

	virtual ~RenderableGameObject();

	/**
	 * \brief Sets the World/Model transform matrix for rendering purposes.
	 */
	void setModelTransform(const glm::mat4& model);

	Model* getObjectModel() const;
	glm::mat4 getModelTransform() const;
	glm::mat3 getNormalMatrix() const;

	virtual void draw(Shader& shader);

protected:
	void fillShaderUnifs(Shader& shader);

private:
	Model* _model;
	bool _isModelExternal;

	glm::mat4 _modelTransform;
	glm::mat3 _normalMatrix;
};

#endif