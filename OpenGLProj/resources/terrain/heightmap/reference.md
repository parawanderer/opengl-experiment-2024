# Source model for desert/dunes/terrain: 

Model Information:
* title:	Desert Landscape
* source:	https://sketchfab.com/3d-models/desert-landscape-220c14d161e44e83be64f30f2034cf4b
* author:	Šimon Ustal (https://sketchfab.com/simonustal)

Model License:
* license type:	CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/)
* requirements:	Author must be credited. Commercial use is allowed.

This work is based on "Desert Landscape" (https://sketchfab.com/3d-models/desert-landscape-220c14d161e44e83be64f30f2034cf4b) by Šimon Ustal (https://sketchfab.com/simonustal) licensed under CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/)

# Heightmap conversions (custom):

Using Blender:
- https://www.youtube.com/watch?v=arvhK4tvYuY&t=436s

This still did not give something that was smooth enough (triangles were still clearly visible) which I think is not a blender issue but a model issue. 
Trying to apply a subdivision modifier to the model did not work on my laptop, just ate up 120 GB memory (well I guess technically 32 GB as that's the actual memory capacity I have, I guess it switched to using the SSD after getting some of that 32 GB that was still available), started swapping, and then windows killed Blender. I guess because the model was already quite large.
So I tried variations of exporting to Grayscale 16-bit PNG and then to the recommended file format from the video "exr" at 32 bits (never heard of this before the video, but I read up on this format apparently using floating point to store colour values). 

Then I imported this "exr" file in photoshop, and in the final result I used photoshop to import this exr file, convert it to grayscale in photoshop, applied box blur filter at 3 pixels (I knew this would achieve a smoothening effect on the pixels regardless of the file format. 3 was after seeing 1 and 2 not being very smooth, and when rendering the terrain as a mesh seeing that the problem was still large flat triangles made up of smaller triangles). Then I converted it to 16 bit in photoshop (presumably this was a loss in percision but I don't think the library we use as recommended by LearnOpenGL supports 32 bit images), and saved it as a PNG. This yields a 16-bit grayscale png height map.

Now I really liked how this looked so I went with this result, but it's still missing the kind of "sharp edges" that I think desert dunes have at the top, and I'm not entirely sure how to correct for that.
This particular map is also just not very sharp-cornered, as I can see in Blender. However I like the overall shape of the map (I like the placement of the dunes and that it has this "sagging" effect in the middle, that makes it possible to get a nice view on your surroundings depending on where you stand in first person mode) and its size, and I couldn't find a better *free* map on Sketchfab.
The overall shapeliness also seems quite close to some stock footage of the Sahara desert: https://www.pexels.com/video/famous-desert-huacachina-of-peru-also-known-as-oasis-of-america-3571195/
If need be, I can also correct the "sags" in blender and redo the export, or attempt to create my own sharp corners.

So for now I'm choosing to switch to this map and use it moving forward. But I'm sure it is possible to get something even better.

This pack right here seems like it has some really nice looking dunes that have sharp corners, but it's a paid pack so I'm not going to go with that as of right now
https://sketchfab.com/3d-models/dunes-5b05a2e6f4fa4bc9a69aee0e0813589a


There's also https://www.world-machine.com/ which seems like you can use to generate some really nice stuff for free, but I don't think I have enough time to learn how to use that at the moment.