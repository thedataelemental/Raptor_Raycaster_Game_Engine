//
// Raptor Raycaster Game Engine
// Started 12/3/2022
// By Jackie P

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

// Player position
float player_x;
float player_y;

// Draw player in 2D top-down view
void drawPlayer()
{
	glColor3f(1,1,0); // Choose a color via RGB values
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(player_x, player_y);
	glEnd();
}

// Map dimensions
// 8x8 grid, 64 units per tile
int map_x_size = 8;
int map_y_size = 8;
int map_scale = 64;
int map[] = 
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,1,1,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void drawMap2D()
{
	int x;
	int y;
	int x_offset;
	int y_offset;

	for(y = 0; y < map_y_size; y++)
	{
		for(x = 0; x < map_x_size; x++)
		{
			if(map[y * map_x_size + x] == 1) // If this tile is a wall,
			{
				glColor3f(1,1,1); // draw it as a white square.
			}
			
			else
			{
				glColor3f(0,0,0); // Else, draw empty space as black.
			}
			
			x_offset = x * map_scale;
			y_offset = y * map_scale;
			
			glBegin(GL_QUADS);

			// Define four vertices of square to be drawn
			glVertex2i(x_offset, y_offset);
			glVertex2i(x_offset, y_offset + map_scale);
			glVertex2i(x_offset + map_scale, y_offset + map_scale);
			glVertex2i(x_offset + map_scale, y_offset);
			glEnd();
		}
	}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen
	drawMap2D();
	drawPlayer(); // Draw 2D player dot
	glutSwapBuffers(); // Render next frame
}

// Player movement
void buttons(unsigned char key, int x, int y)
{
	if(key == 'w')
	{
		player_y -= 5;
	}

	if(key == 'a')
	{
		player_x -= 5;
	}

	if(key == 's')
	{
		player_y += 5;
	}

	if(key == 'd')
	{
		player_x += 5;
	}
	
	glutPostRedisplay(); // Set flag for window to be redrawn
}

void init()
{
	glClearColor(0.3,0.3,0.3,0); // Set dark gray background
	gluOrtho2D(0,1024,512,0); // Make 1024 x 512 window w/ inverted Y coordinate
	player_x = 300; player_y = 300; // Initialize player position
}

void main(int argc, char* argv[])
{
	// Boilerplate OpenGL initializaion code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // Double buffered RGBA window
	glutInitWindowSize(1024, 512); // Window dimensions
	glutCreateWindow("Raycaster v2"); // Window name
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
}

