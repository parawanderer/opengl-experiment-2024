#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float outlinePlacementOffset;

void main()
{ 
    // this skips the intermediary step of actually rendering a "distance field" texture/map.
    // but if we wanted to render it out it would be as simple as this:
    //float distInv = 1.0 - dist;
    //FragColor = vec4(distInv, distInv, distInv, 1.0);
    // or don't invert to have an inverted colour scheme

    float smoothenedEdgeTransition = 0.0015;

    // compute the distance from each pixel to where they point to
    vec4 texSample = texture(screenTexture, TexCoords);
    float dist = length(texSample.rg - TexCoords);

    if (dist > 0.0) {
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/smoothstep.xhtml 
        // is going to interpolate from 0 to 1 when the distance is in range [outlinePlacementOffset, outlinePlacementOffset + smoothenedEdgeTransition] 
        // I got the idea from this video that discusses the usage for text rendering using distance fields: https://www.youtube.com/watch?v=d8cfgcJR9Tk
        // because I was looking for a smoothing effect like in this blogpost: https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9
        float opacity = 1.0 - smoothstep(outlinePlacementOffset, outlinePlacementOffset + smoothenedEdgeTransition, dist);
        //FragColor = vec4(opacity, opacity, opacity, 1.0);
        FragColor = vec4(1.0, 1.0, 1.0, opacity);
    } else {
        FragColor = vec4(0.0);
    }

    // not smoothened:
    //if (dist > 0.0 && dist <= outlinePlacementOffset) {
    //    FragColor = vec4(1.0);
    //} else {
    //    FragColor = vec4(0.0);
    //}
}

    