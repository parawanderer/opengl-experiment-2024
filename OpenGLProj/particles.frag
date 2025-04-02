#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;

out vec4 Color;

uniform sampler2D sprite;

void main() {
	vec4 pixelColor = (texture(sprite, TexCoords) * ParticleColor);
	
	if (pixelColor.a < 0.1) discard; // solving this issue -> https://gamedev.stackexchange.com/questions/146616/modern-opengl-billboarding-alpha-transparency
	// the "ideal" look is achieved when disregarding depth enirely but I don't think that's a good solution.
	// I also happened to have had a look at this video that has a possibly related solution that might work for my thing here too
	// https://youtu.be/4QOcCGI6xOU?si=h6oS3hDgom3dvMCo&t=317
	// but this is complex to implement (similar to my postprocessing effect that I've already done that was pretty annoying to set up)
	// and because the clouds/dust that I achieve aren't particularly "good" looking in the first place, even in no depth test mode,
	// I honestly don't feel like it's worthwhile to try setting up this second approach and seeing if that solves my problem or not,
	// because even if it does the visual result would be extremely underwhelming (no better than the look I get with the depth test disabled).

	Color = pixelColor;
	Color = pixelColor;
}