//
// Raptor Raycaster Game Engine
// Started 12/3/2022
// By Jackie P

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.14159
#define PI_OVER_2 PI/2		
#define THREE_PI_OVER_2 3*PI/2

// Player position, velocity, angle, etc
float player_x;
float player_y;
float player_delta_x;
float player_delta_y;
float player_angle;

// Draw player in 2D top-down view
void drawPlayer()
{
	// Draw player location
	glColor3f(1,1,0); // Choose a color via RGB values
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(player_x, player_y);
	glEnd();

	// Draw player facing
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(player_x, player_y);
	glVertex2i(player_x + player_delta_x * 5, player_y + player_delta_y * 5);
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

// Draw 2D overhead map
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

			// Define four vertices of square to be drawn.
			// Plus or minus 1 to each coordinate to outline map tiles.
			glVertex2i(x_offset + 1, y_offset + 1);
			glVertex2i(x_offset + 1, y_offset + map_scale - 1);
			glVertex2i(x_offset + map_scale - 1, y_offset + map_scale - 1);
			glVertex2i(x_offset + map_scale - 1, y_offset + 1);
			glEnd();
		}
	}

}

// Calculate and draw rays
void drawRays3D()
{
	int ray;
	int ray_map_x_tile;
	int ray_map_y_tile;
	int ray_map_array_tile;
	int depth_of_field;

	// Variables for finding 1/64th of player's x and y position
	int player_x_over_64;
	int player_y_over_64;

	float ray_x_coordinate;
	float ray_y_coordinate;
	float ray_angle;
	float x_offset;
	float y_offset;

	ray_angle = player_angle;
	
	// Find first collision between rays and horizontal or vertical grid lines
	// aka find where each ray hits a wall
	for(ray = 0; ray < 1; ray++)
	{
		// Check for ray collision with horizontal lines

		depth_of_field = 0;
		float aTan = -1 / tan(ray_angle);
		
		// Ray looking down
		if(ray_angle > PI)
		{
			player_y_over_64 = (int) player_y;
			
			// Divide player y by 64 by bitshifting back and forth
			player_y_over_64 = player_y_over_64 >> 6;
			player_y_over_64 = player_y_over_64 << 6;

			ray_y_coordinate = player_y_over_64 - 0.0001;
			ray_x_coordinate = (player_y - ray_y_coordinate) * aTan + player_x;
			
			y_offset = -64;
			x_offset = -(y_offset) * aTan;
		}

		// Ray looking up
		if(ray_angle < PI)
		{
			player_y_over_64 = (int) player_y;

			// Divide player y by 64 by bitshifting back and forth
			player_y_over_64 = player_y_over_64 >> 6;
			player_y_over_64 = player_y_over_64 << 6;

			ray_y_coordinate = player_y_over_64 + 64;
			ray_x_coordinate = (player_y - ray_y_coordinate) * aTan + player_x;

			y_offset = 64;
			x_offset = -(y_offset) * aTan;
		}
		
		// Ray looking straight left or right
		if(ray_angle == 0 || ray_angle == PI)
		{
			ray_x_coordinate = player_x;
			ray_y_coordinate = player_y;
			depth_of_field = 8;
		}
		
		// Do actual horizontal ray collision checks
		while(depth_of_field < 8)
		{
			ray_map_x_tile = (int) (ray_x_coordinate) >> 6;
			ray_map_y_tile = (int) (ray_y_coordinate) >> 6;
			ray_map_array_tile = ray_map_y_tile * map_x_size + ray_map_x_tile;
			
			// Ray hit horizontal wall
			if(ray_map_array_tile > 0 && ray_map_array_tile < map_x_size * map_y_size && map[ray_map_array_tile] == 1)
			{
				depth_of_field = 8;
			}
			
			// Ray didn't hit horizontal wall
			else	
			{
				// Check next horizontal grid line for ray collision
				ray_x_coordinate += x_offset;
				ray_y_coordinate += y_offset;
				depth_of_field += 1;
			}
		}

		// Draw horizontal collision ray(s)
		glColor3f(1,0,0);
		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(player_x, player_y);
		glVertex2i(ray_x_coordinate, ray_y_coordinate);
		glEnd();
	

		// ------------------------------------------------------------------
		// Repeat process - check for ray collision with vertical lines

		depth_of_field = 0;
		float nTan = -tan(ray_angle);
		
		// Ray looking left
		if(ray_angle > PI_OVER_2 && ray_angle < THREE_PI_OVER_2)
		{
			player_x_over_64 = (int) player_x;
			
			// Divide player x by 64 by bitshifting back and forth
			player_x_over_64 = player_x_over_64 >> 6;
			player_x_over_64 = player_x_over_64 << 6;

			ray_x_coordinate = player_x_over_64 - 0.0001;
			ray_y_coordinate = (player_x - ray_x_coordinate) * nTan + player_y;
			
			x_offset = -64;
			y_offset = -(x_offset) * nTan;
		}

		// Ray looking right
		if(ray_angle < PI_OVER_2 || ray_angle > THREE_PI_OVER_2)
		{
			player_x_over_64 = (int) player_x;

			// Divide player x by 64 by bitshifting back and forth
			player_x_over_64 = player_x_over_64 >> 6;
			player_x_over_64 = player_x_over_64 << 6;

			ray_x_coordinate = player_x_over_64 + 64;
			ray_y_coordinate = (player_x - ray_x_coordinate) * nTan + player_y;

			x_offset = 64;
			y_offset = -(x_offset) * nTan;
		}
		
		// Ray looking straight up or down
		if(ray_angle == PI_OVER_2 || ray_angle == THREE_PI_OVER_2)
		{
			ray_x_coordinate = player_x;
			ray_y_coordinate = player_y;
			depth_of_field = 8;
		}
		
		// Do actual vertical line ray collision checks
		while(depth_of_field < 8)
		{
			ray_map_x_tile = (int) (ray_x_coordinate) >> 6;
			ray_map_y_tile = (int) (ray_y_coordinate) >> 6;
			ray_map_array_tile = ray_map_y_tile * map_x_size + ray_map_x_tile;
			
			// Ray hit horizontal wall
			if(ray_map_array_tile > 0 && ray_map_array_tile < map_x_size * map_y_size && map[ray_map_array_tile] == 1)
			{
				depth_of_field = 8;
			}
			
			// Ray didn't hit vetical wall
			else
			{
				// Check next vertical grid line for ray collision
				ray_x_coordinate += x_offset;
				ray_y_coordinate += y_offset;
				depth_of_field += 1;
			}
		}

		// Draw vertical collision ray(s)
		glColor3f(0,1,0);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(player_x, player_y);
		glVertex2i(ray_x_coordinate, ray_y_coordinate);
		glEnd();
		// ---------------------------------------------------------------
	}
}

// Primary display function given to OpenGL
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen
	drawMap2D();
	drawPlayer(); // Draw 2D player dot
	drawRays3D();
	glutSwapBuffers(); // Render next frame
}

// Player movement / user input
void buttons(unsigned char key, int x, int y)
{
	// Move player forward
	if(key == 'w')
	{
		player_x += player_delta_x;
		player_y += player_delta_y;
	}

	// Rotate player left
	if(key == 'a') 
	{
		player_angle -= 0.1;
		if (player_angle < 0)
		{
			player_angle += 2 * PI; // Reset player angle after full left rotation
		}
		
		// Update player velocity after left rotation
		// cos = adj/hyp = horizontal velocity
		// sin = opp/hyp = vertical velocity
		player_delta_x = cos(player_angle) * 5;
		player_delta_y = sin(player_angle) * 5;
	}

	// Move player back
	if(key == 's') 
	{
		player_x -= player_delta_x;
		player_y -= player_delta_y;
	}	
	
	// Rotate player right
	if(key == 'd')
	{
		player_angle += 0.1;
		if (player_angle > (2 * PI))
		{
			player_angle -= 2*PI; // Reset player angle after full right rotation
		}

		// Update player velocity after right rotation
		player_delta_x = cos(player_angle) * 5;
		player_delta_y = sin(player_angle) * 5;
	}
	
	glutPostRedisplay(); // Set flag for window to be redrawn
}

void init()
{
	glClearColor(0.3,0.3,0.3,0); // Set dark gray background
	gluOrtho2D(0,1024,512,0); // Make 1024 x 512 window w/ inverted Y coordinate
	
	// Initialize player position and velocity
	player_x = 300; player_y = 300;
	player_delta_x = cos(player_angle) * 5;
	player_delta_y = sin(player_angle) * 5;
}

void main(int argc, char* argv[])
{
	// Boilerplate OpenGL initializaion code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // Double buffered RGBA window
	glutInitWindowSize(1024, 512); // Window dimensions
	glutCreateWindow("Raptor Raycaster Engine"); // Window name
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
}

