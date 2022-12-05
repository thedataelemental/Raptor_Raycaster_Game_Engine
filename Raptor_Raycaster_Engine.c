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
#define ONE_DEGREE 0.0174533 // One degree in radians

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

// Use pythagorean theorem to find distance between player and ray's endpoint
// a^2 + b^2 = c^2
float find_ray_length(float player_x, float player_y, float ray_x, float ray_y, float angle)
{
	return (sqrt((ray_x-player_x)*(ray_x-player_x) + (ray_y-player_y)*(ray_y-player_y)));
}

// Calculate and draw rays
// This process repeats four times - left, right, up, and down (top-down view)
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
	float ray_length;
	
	// Define angle of first ray, at left side of player's view
	ray_angle = player_angle - ONE_DEGREE*30;
	
	// Define limits of ray angle
	if(ray_angle < 0)
	{
		ray_angle += 2*PI;
	}
	if(ray_angle > 2*PI)
	{
		ray_angle -= 2*PI;
	}
	
	// Find first collision between rays and horizontal or vertical grid lines
	// aka find where each ray hits a wall
	for(ray = 0; ray < 60; ray++)
	{
		// Check for ray collision with horizontal lines

		depth_of_field = 0;

		// Values for finding horizontal ray length
		float dist_to_horz_line = 1000000;
		float horizontal_x = player_x;
		float horizontal_y = player_y;

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
				// Find ray length
				horizontal_x = ray_x_coordinate;
				horizontal_y = ray_y_coordinate;
				dist_to_horz_line = find_ray_length(player_x, player_y, horizontal_x, horizontal_y, ray_angle);

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

		// ------------------------------------------------------------------
		// Repeat process - check for ray collision with vertical lines

		depth_of_field = 0;

		// Values for finding vertical ray length
		float dist_to_vert_line = 1000000;
		float vertical_x = player_x;
		float vertical_y = player_y;

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
			
			// Ray hit vetical wall
			if(ray_map_array_tile > 0 && ray_map_array_tile < map_x_size * map_y_size && map[ray_map_array_tile] == 1)
			{
				// Find ray length
				vertical_x = ray_x_coordinate;
				vertical_y = ray_y_coordinate;
				dist_to_vert_line = find_ray_length(player_x, player_y, vertical_x, vertical_y, ray_angle);

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

		// Set final ray to whichever is shortest - vertical hit or horizontal hit
		if(dist_to_vert_line < dist_to_horz_line)
		{
			ray_x_coordinate = vertical_x;
			ray_y_coordinate = vertical_y;
			ray_length = dist_to_vert_line;
			glColor3f(0,0,0.9);
		}
		if(dist_to_horz_line < dist_to_vert_line)
		{
			ray_x_coordinate = horizontal_x;
			ray_y_coordinate = horizontal_y;
			ray_length = dist_to_horz_line;
			glColor3f(0,0,0.7);
		}

		// Draw 2D rays
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(player_x, player_y);
		glVertex2i(ray_x_coordinate, ray_y_coordinate);
		glEnd();
		// ---------------------------------------------------------------
		
		// Draw 3D walls
		float correction_angle = player_angle - ray_angle; // Fix fisheye
		if (correction_angle > 2*PI)
		{
			correction_angle -= 2*PI;
		}
		if (correction_angle < 0)
		{
			correction_angle +- 2*PI;
		}
		ray_length = ray_length * cos(correction_angle);

		int screen_height = 320;
		float line_height = (map_scale * screen_height) / ray_length; // 320 = screen height
		if (line_height > screen_height)
		{
			line_height = screen_height;
		}
		float line_offset = 160 - line_height / 2; // Center wall lines on screen

		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(ray*8+530, line_offset);
		glVertex2i(ray*8+530, line_height+line_offset);
		glEnd();

		// Prep for next ray
		ray_angle += ONE_DEGREE;
		if(ray_angle < 0)
		{
			ray_angle += 2*PI;
		}
		if(ray_angle > 2*PI)
		{
			ray_angle -= 2*PI;
		}
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
	player_x = 364; player_y = 300;
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

