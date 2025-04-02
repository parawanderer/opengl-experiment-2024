#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D mask;

// https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9
// https://www.youtube.com/watch?v=nKJgUsAU2d0
// also referenced: (for OpenGL post-processing quad texture): https://learnopengl.com/In-Practice/2D-Game/Postprocessing
// https://learnopengl.com/Advanced-OpenGL/Framebuffers
// (I did not read everything in the above, particularly not all the previous chapters, 
// only enough in my mind to move forward with implementing this outline algorithm)
//
// this init step accomplishes "Step 1: Collect Underpants" of the blog post https://bgolus.medium.com/the-quest-for-very-wide-outlines-ba82ed442cd9
// in my case I do not have anti-aliasing enabled so I don't need to treat anything in any special way. 
// I'm converting a white silhouette on a black background to one that fills the RB texture coordinates in the RB values.
void main()
{
    // **assume full-screen rendering**
    vec4 sampleAtPoint = texture(mask, TexCoords);
    if (sampleAtPoint.r > 0.0) { // not black
       
        FragColor = vec4(TexCoords.r, TexCoords.g, 0.0, 1.0);// output its UV coords;

    } else {
        FragColor = vec4(-1.0, -1.0, -1.0, -1.0); // "masked"
    }
}