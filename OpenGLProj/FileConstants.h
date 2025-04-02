#ifndef CONSTANTS_MINE_H
#define CONSTANTS_MINE_H


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


#endif