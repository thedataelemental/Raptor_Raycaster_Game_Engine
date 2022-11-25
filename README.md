# raycaster_game_engine
First person "3D" raycaster game engine. Based on youtube tutorials by 3DSage.

This is my first experiment with building a 3D engine / renderer. The code is written in C, and rendered with function calls to OpenGL.
The engine is a 'raycaster'. This means that the rendering is done by casting rays from the player to the nearest walls, noting the distance, 
and drawing vertical pixel columns of varying height according to that distance. Distance measurement and player movement are done via simple trigonometry. 
This is an implementation based on tutorials by 3DSage, who in turn was re-implementing work done by John Carmack for Doom 1 & 2 and Wolfenstein 3D in the 90s.
