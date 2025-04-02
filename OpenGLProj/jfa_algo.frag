#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // aka pixel coordinates

uniform sampler2D UVtexture;
uniform float stepSize;

// this is intially based on the algo shown here: https://www.youtube.com/watch?v=nKJgUsAU2d0 (along with https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9)
// but I encountered artifacting that I could not think of a fix for. The solution from the video is not direclty 
// applicable to OpenGL and it wasn't really the same effect. 
// After searching around for other implementations that might have run into the same artifacting I found this blog post: https://itscai.us/blog/post/jfa/
// where they actually encounter the same issue I saw. From this blog post I decided to adapt their fix 
// mentioned in "Step 3.5" to fix this artifacting. Their "naive" approach was equivalent to my approach
// referencing the Unity tutorial.
// I also liked their terminology more ("seed" for the source pixel for which we are computing right now, this is also what is used in the original JFA paper https://www.comp.nus.edu.sg/~tants/jfa/i3d06.pdf)
// This is also equivalent to the very original blog post's author's work: https://gist.github.com/bgolus/a18c1a3fc9af2d73cc19169a809eb195#file-hiddenjumpfloodoutline-shader-L219
void main()
{
	float bestDistance = 99999999999999.0;	// aka "closest to this pixel" as part of JFA algo (in 2D)
	vec2 bestUV = vec2(1.0, 1.0); // best solution so far ^

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			vec2 UVoffsetCoords = TexCoords + vec2(x, y) * stepSize;

			// sample coordinate is not out of bounds check (see: https://itscai.us/blog/post/jfa/ "Banding")
			// ensures we're checking pixels that are actually within the texture 
			// (all valid coordinates within the "screen"/texture are in range [0, 1])
			if (length(clamp(UVoffsetCoords, 0.0, 1.0) - UVoffsetCoords) <= .00001) { 

				vec2 seed = texture(UVtexture, UVoffsetCoords).rg;

				// scale + floor values purely for the distance comparison (see: https://itscai.us/blog/post/jfa/ "Interference Pattern")
				vec2 tcScaled = floor(TexCoords * textureSize(UVtexture, 0));
				vec2 sScaled = floor(seed * textureSize(UVtexture, 0));

				// float dist = length(sScaled - tcScaled);
				// ^ optimization, kind of copied from https://gist.github.com/bgolus/a18c1a3fc9af2d73cc19169a809eb195#file-hiddenjumpfloodoutline-shader-L234
				// (also mentioned in the blog post under “Optimizations?” https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9)
				// mainly because it's one I often use myself. When comparing distances where you don't 
				// actually care about the distance, but just choosing the min distance, you don't have to take the root of the distance but you can compare in squared form, 
				// because the < relationship is still going to remain proportional  (a^2 < b^2) if (a < b)
				vec2 diff = tcScaled - sScaled;
				float dist2 = dot(diff, diff); // this dot product just maths out to being the squared distance

				if (dist2 < bestDistance)
				{
					bestDistance = dist2;
					bestUV = seed;
				}
			}
		}
	}

	FragColor = vec4(bestUV, 0.0, 1.0);
}
