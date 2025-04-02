#ifndef FILE_CONSTANTS_MINE_H
#define FILE_CONSTANTS_MINE_H

// SHADERS

constexpr auto SHADER_MASKING_VERT = "masking.vert";
constexpr auto SHADER_MASKING_FRAG = "masking.frag";


constexpr auto SHADER_JFA_VERT = "jfa.vert";

constexpr auto SHADER_JFA_INIT_VERT = SHADER_JFA_VERT;
constexpr auto SHADER_JFA_INIT_FRAG = "jfa_init.frag";

constexpr auto SHADER_JFA_ALGORITHM_VERT = SHADER_JFA_VERT;
constexpr auto SHADER_JFA_ALGORITHM_FRAG = "jfa_algo.frag";


constexpr auto SHADER_PASSTHROUGH_VERT = "justrenderthe2dtex.vert";
constexpr auto SHADER_PASSTHROUGH_FRAG = "justrenderthe2dtex.frag";


constexpr auto SHADER_DISTANCEFIELD_VERT = SHADER_PASSTHROUGH_VERT;
constexpr auto SHADER_DISTANCEFIELD_FRAG = "distancefield.frag";


constexpr auto SHADER_MESH_VERT = "mesh.vert";
constexpr auto SHADER_MESH_FRAG = "mesh.frag";


constexpr auto SHADER_PARTICLES_VERT = "particles.vert";
constexpr auto SHADER_PARTICLES_FRAG = "particles.frag";


constexpr auto SHADER_FONT_VERT = "font.vert";
constexpr auto SHADER_FONT_FRAG = "font.frag";


constexpr auto SHADER_SKYBOX_VERT = "skybox.vert";
constexpr auto SHADER_SKYBOX_FRAG = "skybox.frag";


constexpr auto SHADER_LIGHTSOURCE_VERT = "shader_lightsource.vert";
constexpr auto SHADER_LIGHTSOURCE_FRAG = "shader_lightsource.frag";


constexpr auto SHADER_TERRAIN_VERT = "terrain.vert";
constexpr auto SHADER_TERRAIN_FRAG = "terrain.frag";


// MODELS

constexpr auto MODEL_ORNITHOPTER = "resources/models/dune-ornithopter/OrnithopterDuneAnimated.fbx"; // animation on this by me

constexpr auto MODEL_THUMPER = "resources/models/thumper_dune/Thumper.fbx"; // animation on this by me

constexpr auto MODEL_NOMAD = "resources/models/rust-nomad/RustNomad.fbx"; // animations from https://www.mixamo.com/ but I had to combine them and make them work which took a long time

constexpr auto MODEL_SANDWORM = "resources/models/sandworm2/Sandworm.fbx"; // model and animations by me (followed a tutorial)

constexpr auto MODEL_CONTAINER_SMALL = "resources/models/military-container-free/Military_Container.fbx";

constexpr auto MODEL_CONTAINER_LARGE = "resources/models/cargo-container/Container.fbx";


// TEXTURES

const std::vector<std::string> SKYBOX_FACES = {
	"resources/skybox2/right.jpg",
	"resources/skybox2/left.jpg",
	"resources/skybox2/top.jpg",
	"resources/skybox2/bottom.jpg",
	"resources/skybox2/front.jpg",
	"resources/skybox2/back.jpg",
};	


// FONTS

constexpr auto FONT_PLAY_REGULAR = "resources/font/play/Play-Regular.ttf";


// TERRAIN-RELATED

constexpr auto TERRAIN_HEIGHTMAP = "resources/terrain/heightmap/yetanothermap2.png";
constexpr auto TERRAIN_TEXTURE_PRIMARY = "resources/terrain/texture/Ground055S_4K-JPG_Color.jpg";
constexpr auto TERRAIN_TEXTURE_DARKER = "resources/terrain/texture/Ground055S_4K-JPG_Color_darker.jpg";
constexpr auto TERRAIN_NORMAL_MAP = "resources/terrain/texture/Ground055S_4K-JPG_NormalGL.jpg";
constexpr auto TERRAIN_Y_SCALE_MULTIPLIER = 192.0f;
constexpr auto TERRAIN_Y_SHIFT = 32.0f;


// PARTICLES

constexpr auto TEXTURE_PARTICLE_DUST = "resources/particles/smoke-particle-texture2.png";

// AUDIO

constexpr auto AUDIO_BASE_PATH = "./resources/audio";

#endif