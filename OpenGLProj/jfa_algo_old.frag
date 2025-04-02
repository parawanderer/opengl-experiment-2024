#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // aka pixel coordinates

uniform sampler2D UVtexture;
uniform float stepSize;

void main()
{
	float bestDistance = 99999.0;
	vec2 bestUV = vec2(-1.0, -1.0);

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			vec2 UVoffsetCoords = TexCoords + vec2(x, y) * stepSize;

			vec2 tempValue = texture(UVtexture, UVoffsetCoords).rg;

			float dist = length(tempValue - TexCoords);

			if (tempValue.x >= 0 && tempValue.y >= 0 && dist < bestDistance)
			{
				bestDistance = dist;
				bestUV = tempValue;
			}
		}
	}

	FragColor = vec4(bestUV, 0.0, 0.0);
}