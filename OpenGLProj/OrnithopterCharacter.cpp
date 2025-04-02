#include "OrnithopterCharacter.h"

OrnithopterCharacter::OrnithopterCharacter(const WorldTimeManager* time, RenderableGameObject* ornithropterObject):
_time(time),
_model(ornithropterObject)
{}

void OrnithopterCharacter::onNewFrame()
{
	// ornithopter "fly effect" (it's not the best animation but it's interesting as a placeholder)
	const float t = this->_time->getCurrentTime();
	glm::mat4 orniModel = glm::mat4(1.0f);
	orniModel = glm::translate(orniModel, glm::vec3(sin(t) * -10.f, 400.0f + ((cos(t) - 1) * 25.0f), sin(-t / 3.0f) * 1500.f));
	orniModel = glm::rotate(orniModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	orniModel = glm::scale(orniModel, glm::vec3(2.0f));
	this->_model->setModelTransform(orniModel);
}

void OrnithopterCharacter::draw(Shader& shader)
{
	this->_model->draw(shader);
}
