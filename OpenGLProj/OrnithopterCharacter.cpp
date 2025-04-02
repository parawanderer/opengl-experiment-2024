#include "OrnithopterCharacter.h"

// this does not loop perfectly but I edited/combined this sound to sound more like an atypical helicopter
const std::string ORNITHOPTER_TRACK = "helicopter_dualblade_mix.mp3";

const float ORNITHOPTER_SOUND_MIN_DISTANCE = 8.0f;

// I also animated these little things myself on the model
const std::string STATIONARY_ANIM = "stationary";
const std::string FLYING_ANIM = "flying";


const float ORNITHOPTER_MODEL_SCALE = 3.0f;


OrnithopterCharacter::OrnithopterCharacter(const WorldTimeManager* time, SoundManager* sound, RenderableGameObject* ornithropterObject, AnimationSet* animations):
_time(time),
_sound(sound),
_model(ornithropterObject),
_animator(animations)
{
	this->_animator.playAnimation(FLYING_ANIM);

	this->_currentSound = AudioPlayer(this->_sound->playTracked3D(ORNITHOPTER_TRACK, true, glm::vec3(1000, 300, 1000)));
	this->_currentSound.setMinimumDistance(ORNITHOPTER_SOUND_MIN_DISTANCE);
}

void OrnithopterCharacter::onNewFrame()
{
	// ornithopter "fly effect" (it's not the best animation but it's interesting as a placeholder)
	const float t = this->_time->getCurrentTime();
	glm::vec3 newPosition = glm::vec3(sin(t / 2.0f) * -10.f, 400.0f + ((cos(t / 2.0f) - 1) * 25.0f), sin(-t / 6.0f) * 3000.f);


	glm::mat4 orniModel = glm::mat4(1.0f);
	orniModel = glm::translate(orniModel, newPosition);
	// kind of a hacky way to do rotation to get an interesting animation
	if (newPosition.z <= this->_currentPos.z) orniModel = glm::rotate(orniModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	orniModel = glm::scale(orniModel, glm::vec3(ORNITHOPTER_MODEL_SCALE));
	this->_model->setModelTransform(orniModel);

	this->_currentPos = newPosition;
	this->_animator.updateAnimation(this->_time->getDeltaTime());
	this->_currentSound.setPosition(newPosition);
}

void OrnithopterCharacter::draw(Shader& shader)
{
	const std::vector<glm::mat4> transforms = this->_animator.getFinalBoneMatrices();
	this->setupEntityShaderForAnim(shader, transforms);

	this->_model->draw(shader);
	this->clearEntityShaderForAnim(shader);
}
