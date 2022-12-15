//
// Raptor Raycaster Game Engine
// Started 12/3/2022
// By Jackie P

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#include "all_textures.ppm"

#define PI 3.14159
#define PI_OVER_2 PI/2		
#define THREE_PI_OVER_2 3*PI/2		
#define ONE_DEGREE 0.0174533 // One degree in radians

// Button states for player input / movement
typedef struct
{
	int w, a, s, d;
} ButtonKeys;
ButtonKeys Keys;

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
	glVertex2i(player_x/4, player_y/4);
	glEnd();

	// Draw player facing
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(player_x/4, player_y/4);
	glVertex2i(player_x/4 + player_delta_x/4 * 5, player_y/4 + player_delta_y/4 * 5);
	glEnd();
}

// Map dimensions
// 8x8 grid, 64 units per tile
int map_x_size = 32;
int map_y_size = 32;
int map_scale = 64;

// Actual map tile layout
int map_walls[] = 
{
	1,1,2,1,3,1,2,1,3,1,2,1,1,2,1,1,1,2,1,1,2,1,2,1,1,1,2,1,1,2,1,1,
	4,0,0,0,7,0,0,0,0,0,0,1,0,0,0,1,0,0,0,4,0,0,0,1,1,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,0,0,0,0,7,0,0,0,4,0,0,0,1,0,0,0,4,1,0,0,0,0,0,0,4,
	4,1,2,1,1,0,0,1,0,0,0,1,0,0,1,1,0,0,0,7,0,0,0,1,4,0,0,1,1,0,0,1,
	1,1,1,1,1,0,0,4,1,2,1,1,0,0,0,4,1,0,0,1,0,0,1,1,1,0,0,0,4,0,0,4,
	4,0,0,0,4,0,0,1,0,0,0,4,0,0,1,1,0,0,0,4,0,0,0,0,4,0,0,0,1,0,0,1,
	1,0,0,0,1,0,0,4,1,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1,4,0,0,4,
	4,1,7,1,1,0,0,1,0,0,0,4,0,0,0,0,0,0,1,1,1,1,0,0,7,0,0,0,1,1,0,1,
	1,0,0,0,1,1,7,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,4,0,0,4,
	4,0,0,0,0,0,0,0,0,0,1,1,1,2,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,1,1,2,1,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,4,
	4,1,0,0,1,1,1,2,1,2,1,1,0,0,0,1,0,0,1,1,7,1,1,1,1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,1,1,1,4,
	4,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,7,0,0,1,0,0,0,1,1,1,1,
	1,0,1,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,4,
	4,1,1,3,1,1,1,1,7,1,2,1,1,3,1,1,2,1,1,0,0,1,1,2,1,1,0,0,7,0,0,1,
	1,1,1,1,0,4,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,4,
	4,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,7,0,0,0,1,0,0,1,1,0,1,
	1,1,0,0,0,4,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,4,
	4,1,0,0,0,1,1,2,1,1,2,1,1,1,2,1,1,2,1,2,1,1,1,2,1,1,0,0,4,0,0,1,
	1,1,0,0,0,0,0,0,1,1,0,0,4,0,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1,0,0,4,
	4,1,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,4,0,0,7,0,0,0,0,4,0,0,1,
	1,1,1,7,1,2,1,2,1,1,0,0,4,1,0,0,0,0,0,0,1,0,0,1,2,1,2,1,1,3,1,4,
	4,0,0,0,0,0,0,0,7,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,4,1,1,1,
	1,0,0,0,0,0,0,0,1,1,0,0,4,1,0,0,0,0,0,0,4,0,0,0,0,0,0,0,1,0,0,4,
	4,1,0,0,0,0,0,0,4,0,0,0,1,0,0,0,0,0,1,2,1,0,1,2,1,0,0,0,7,0,0,1,
	1,1,2,1,2,7,1,2,1,1,1,0,4,0,0,1,3,1,1,0,0,0,0,1,0,0,0,0,1,0,0,4,
	4,0,0,0,0,0,0,0,0,0,4,0,1,1,7,1,0,0,0,0,0,0,0,4,0,0,0,0,4,0,0,1,
	1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,2,1,2,1,0,0,0,0,1,0,0,4,
	4,1,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,1,0,0,0,1,1,2,1,1,4,0,0,1,
	1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,3,1,2,1,2,1,3,1,2,1,2,1,3,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,
};

// Layout of textures assigned to floor in each map tile
int map_floors[] = 
{
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
};

// Layout of textures assigned to ceiling in each map tile
int map_ceilings[] = 
{
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
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
			if(map_walls[y * map_x_size + x] == 7) // If a tile is a door,
			{
				glColor3f(1,0,0); // draw it as a red square.
			}

			else if(map_walls[y * map_x_size + x] > 0) // If this tile is a wall,
			{
				glColor3f(1,1,1); // draw it as a white square.
			}

			else
			{
				glColor3f(0,0,0); // Else, draw empty space as black.
			}
			
			x_offset = x * 16;
			y_offset = y * 16;
					
			glBegin(GL_QUADS);

			// Define four vertices of square to be drawn.
			// Plus or minus 1 to each coordinate to outline map tiles.
			glVertex2i(x_offset + 1, y_offset + 1);
			glVertex2i(x_offset + 1, y_offset + 16 - 1);
			glVertex2i(x_offset + 16 - 1, y_offset + 16 - 1);
			glVertex2i(x_offset + 16 - 1, y_offset + 1);
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

	// Variables for rounding player's x and y to 1/64th precision
	int rounded_player_x;
	int rounded_player_y;

	float ray_x_coordinate;
	float ray_y_coordinate;
	float ray_angle;
	float x_offset;
	float y_offset;
	float ray_length;

	// Debug values
	float x_intersect;
	float y_intersect;
	int saved_rounded_y;
	
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
		// What texture did the rays hit?
		int vertical_map_texture = 0;
		int horizontal_map_texture = 0;

		// Check for ray collision with horizontal lines

		depth_of_field = 0;

		// Values for finding ray distance to horizontal grid wall line
		float dist_to_horz_line = 1000000;
		float x_distance_to_horizontal_wall_line = player_x;
		float y_distance_to_horizontal_wall_line = player_y;

		//------------ WARNING - PLAYER ROTATION  / RAY ANGLE --------
		// UNIT CIRCLE IS INVERTED (GOES CLOCKWISE)					
		// PI/2 IS DOWN. 3PI/2 IS UP.								
		//------------------------------------------------------------
		
		// Ray looking up - Accounting for inverse unit circle 
		if(ray_angle > PI)
		{
			rounded_player_y = (int) player_y;
			
			// Divide player y by 64 by bitshifting back and forth
			// This value lands on the grid line directly above the player
			rounded_player_y = rounded_player_y >> 6;
			rounded_player_y = rounded_player_y << 6;

			// Find point of collision with first horizontal grid line
			ray_y_coordinate = rounded_player_y - 0.0001; // Y coordinate of grid line just above player
			ray_x_coordinate = (ray_y_coordinate - player_y) * 1/tan(ray_angle) /* Horizontal distance between player and intersection point */ + player_x;
			
			y_offset = -64;								// Vert distance to next line
			x_offset = y_offset * 1/tan(ray_angle);		// Horz distance to next line
		}

		// Ray looking down - Accounting for inverse unit circle 
		if(ray_angle < PI)
		{
			rounded_player_y = (int) player_y;

			// Divide player y by 64 by bitshifting back and forth=
			// This value lands on the grid line directly above the player
			rounded_player_y = rounded_player_y >> 6;
			rounded_player_y = rounded_player_y << 6;

			// Find point of collision with first horizontal grid line
			ray_y_coordinate = rounded_player_y + 64; // Y coordinate of grid line just below player
			ray_x_coordinate = (ray_y_coordinate - player_y) * 1/tan(ray_angle) /* Horizontal distance between player and intersection point */ + player_x;

		// Debug code - print values / save values to draw intersect point later
//			if (ray == 30)
//			{
//				x_intersect = ray_x_coordinate;
//				y_intersect = ray_y_coordinate;
//				saved_rounded_y = rounded_player_y;
//			
//				printf("player x: %f\n", player_x);
//				printf("player y: %f\n", player_y);
//				printf("ray x: %f\n", ray_x_coordinate);
//				printf("ray y: %f\n", ray_y_coordinate);
//				printf("ray angle: %f\n", ray_angle);
//				printf("rounded player y: %d\n)", saved_rounded_y);
//			}
			
			y_offset = 64; 							// Vert distance to next line
			x_offset = y_offset * 1/tan(ray_angle); // Horz distance to next line
		}
		
		// Ray looking straight left or right
		if(ray_angle == 0 || ray_angle == PI)
		{
			ray_x_coordinate = player_x;
			ray_y_coordinate = player_y;
			depth_of_field = 32;
		}
		
		// Do actual horizontal ray collision checks
		while(depth_of_field < 32)
		{
			ray_map_x_tile = (int) (ray_x_coordinate) >> 6;
			ray_map_y_tile = (int) (ray_y_coordinate) >> 6;
			ray_map_array_tile = ray_map_y_tile * map_x_size + ray_map_x_tile;
			
			// Ray hit horizontal wall
			if(ray_map_array_tile > 0 && ray_map_array_tile < map_x_size * map_y_size && map_walls[ray_map_array_tile] > 0)
			{
				horizontal_map_texture = map_walls[ray_map_array_tile] - 1; // Subtract 1 to start at beginning of textures array

				// Find ray length
				x_distance_to_horizontal_wall_line = ray_x_coordinate;
				y_distance_to_horizontal_wall_line = ray_y_coordinate;
				dist_to_horz_line = find_ray_length(player_x, player_y, x_distance_to_horizontal_wall_line, y_distance_to_horizontal_wall_line, ray_angle);

				depth_of_field = 32;
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
		// ------------------------------------------------------------------

		depth_of_field = 0;

		// Values for finding ray distance to vertical grid wall line
		float dist_to_vert_line = 1000000;
		float x_distance_to_vertical_wall_line = player_x;
		float y_distance_to_vertical_wall_line = player_y;
		
		// Ray looking left
		if(ray_angle > PI_OVER_2 && ray_angle < THREE_PI_OVER_2)
		{
			rounded_player_x = (int) player_x;
			
			// Divide player y by 64 by bitshifting back and forth
			// This value lands on the grid line directly left of the player
			rounded_player_x = rounded_player_x >> 6;
			rounded_player_x = rounded_player_x << 6;

			ray_x_coordinate = rounded_player_x - 0.0001; // Grid line left of player
			ray_y_coordinate = (ray_x_coordinate - player_x) * tan(ray_angle) /* Vertical distance between player and intersection point */ + player_y;
			
			x_offset = -64;							// Horz distance to next line
			y_offset = x_offset * tan(ray_angle);	// Vert distance to next line
		}

		// Ray looking right
		if(ray_angle < PI_OVER_2 || ray_angle > THREE_PI_OVER_2)
		{
			rounded_player_x = (int) player_x;

			// Divide player y by 64 by bitshifting back and forth
			// This value lands on the grid line directly left of the player
			rounded_player_x = rounded_player_x >> 6;
			rounded_player_x = rounded_player_x << 6;

			ray_x_coordinate = rounded_player_x + 64; // Grid line right of player
			ray_y_coordinate = (ray_x_coordinate - player_x) * tan(ray_angle) /* Vertical distance between player and intersection point */ + player_y;

			x_offset = 64; 							// Horz distance to next line
			y_offset = x_offset * tan(ray_angle);	// Vert distance to next line
		}
		
		// Ray looking straight up or down
		if(ray_angle == PI_OVER_2 || ray_angle == THREE_PI_OVER_2)
		{
			ray_x_coordinate = player_x;
			ray_y_coordinate = player_y;
			depth_of_field = 32;
		}
		
		// Do actual vertical line ray collision checks
		while(depth_of_field < 32)
		{
			ray_map_x_tile = (int) (ray_x_coordinate) >> 6;
			ray_map_y_tile = (int) (ray_y_coordinate) >> 6;
			ray_map_array_tile = ray_map_y_tile * map_x_size + ray_map_x_tile;
			
			// Ray hit vetical wall
			if(ray_map_array_tile > 0 && ray_map_array_tile < map_x_size * map_y_size && map_walls[ray_map_array_tile] > 0)
			{
				vertical_map_texture = map_walls[ray_map_array_tile] - 1; // Subtract 1 to start at beginning of textures array

				// Find ray length
				x_distance_to_vertical_wall_line = ray_x_coordinate;
				y_distance_to_vertical_wall_line = ray_y_coordinate;
				dist_to_vert_line = find_ray_length(player_x, player_y, x_distance_to_vertical_wall_line, y_distance_to_vertical_wall_line, ray_angle);

				depth_of_field = 32;
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
		float shading = 1;
		float map_texture; // Which texture did the array hit?

		if(dist_to_vert_line < dist_to_horz_line) // Ray hit vertical wall line first
		{
			map_texture = vertical_map_texture;
			shading = 0.5;
			ray_x_coordinate = x_distance_to_vertical_wall_line;
			ray_y_coordinate = y_distance_to_vertical_wall_line;
			ray_length = dist_to_vert_line;
			glColor3f(0,0,0.9);
		}
		if(dist_to_horz_line < dist_to_vert_line) // Ray hit horizontal wall line first
		{
			map_texture = horizontal_map_texture;
			ray_x_coordinate = x_distance_to_horizontal_wall_line;
			ray_y_coordinate = y_distance_to_horizontal_wall_line;
			ray_length = dist_to_horz_line;
			glColor3f(0,0,0.7);
		}

		// Draw 2D rays
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(player_x/4, player_y/4); // divide by 4 because maps are now 4 times larger as of 12/14/22
		glVertex2i(ray_x_coordinate/4, ray_y_coordinate/4);
		glEnd();

		// --------------------------------------------------------------
		// Draw 3D scene
		// --------------------------------------------------------------
		float correction_angle = player_angle - ray_angle; // Fisheye correction angle

		// Angle limits for fisheye fix
		if (correction_angle > 2*PI)
		{
			correction_angle -= 2*PI;
		}
		if (correction_angle < 0)
		{
			correction_angle +- 2*PI;
		}

		ray_length = ray_length * cos(correction_angle); // Apply fisheye fix

		int screen_height = 320;
		float line_height = (map_scale * screen_height) / ray_length;
		float texture_y_step = 32.0 / (float) line_height;
		float texture_y_offset = 0;
		
		// Fix texture distortion when too close to walls
		if (line_height > screen_height)
		{
			texture_y_offset = (line_height - screen_height) / 2.0;
			line_height = screen_height;
		}

		// Prepare to start drawing walls
		float line_offset = 160 - line_height / 2; // Center wall lines on screen
		int current_pixel;
		float texture_y = texture_y_offset * texture_y_step; // + map_texture * 32;
		float texture_x;

		// Do wall texture mapping
		if (shading == 1) // Rendering horizontal grid line
		{
			texture_x = (int) (ray_x_coordinate / 2.0) % 32;
			if (ray_angle < PI)
			{
				// Flip north textures horizontally
				texture_x = 31 - texture_x; // note - textures are 32 pixels across
			}
		}
		else // Rendering vertical grid line
		{
			texture_x = (int) (ray_y_coordinate / 2.0) % 32;
			if (ray_angle > PI/2 && ray_angle < 3*PI/2)
			{
				// Flip west textures horizontally
				texture_x = 31 - texture_x;
			}
		}	

 		// Drawing wall
		for (current_pixel = 0; current_pixel < line_height; current_pixel++)
		{
			// Get texture pixel to draw
			int texture_pixel = ((int) texture_y * 32 + (int) texture_x) * 3; // Find specific pixel of texture
			texture_pixel += (map_texture * 32 * 32 * 3); // Change wall texture depending on numbers in floor array
			int red = all_textures[texture_pixel+0] * shading; // Get RGB values of pixel
			int green = all_textures[texture_pixel+1] * shading;
			int blue = all_textures[texture_pixel+2] * shading;
			
			// Draw wall pixel as texture pixel
			glPointSize(8);
			glColor3ub(red, green, blue);
			glBegin(GL_POINTS);
			glVertex2i(ray * 8 + 530, current_pixel + line_offset);
			glEnd();

			texture_y += texture_y_step;
		}

		//--------------------------------
		// Draw floors and ceilings
		//--------------------------------
		for(current_pixel = line_offset + line_height; current_pixel < screen_height; current_pixel++) // Draw from bottom of wall to screen's edge
		{
			// Draw floor
			float y_distance = current_pixel - (screen_height / 2.0); // Vertical distance between current pixel and center of the screen / horizon
			float floor_angle = player_angle - ray_angle; 

			// Limits for angle rotation
			if(floor_angle > 2*PI) {floor_angle -= 2*PI;}
			if(floor_angle < 0) {floor_angle += 2*PI;}

			// Floor texture mapping	
			// Find texture x and y:
			// Start by dividing player x and y by 2, because the map scale is   64x64, but the textures are 32x32.
			// Use cosine (adjacent / hypotenuse) to find horizontal distance, and sine (opposite / hypotenuse) to find vertical distance.
			// The rest of the tx and ty formulas are still weird to me, but I know it has to do with projecting a ray onto the floor with trigonometry.
			texture_x = (player_x / 2)  + cos(ray_angle) * 158 * 32 / y_distance / cos(floor_angle);
			texture_y = (player_y / 2) + sin(ray_angle) * 158 * 32 / y_distance / cos(floor_angle);
			int map_position = map_floors[(int) (texture_y / 32.0) * map_x_size + (int) (texture_x / 32.0)]*32*32; // Which tile of the floor (which entry of map_floors array) are we currently rendering?
//			float pixel_color = all_textures[((int) (texture_y) & 31) * 32 +  ((int) (texture_x) & 31) + map_position] * 0.7; // What is the color of the pixel we are currently rendering?

			// Get floor texture pixel to draw
			int texture_pixel = (((int) (texture_y) & 31) * 32 +  ((int) (texture_x) & 31)) * 3 + map_position * 3; // Find specific pixel of texture
			int red = all_textures[texture_pixel+0] * 0.7; // Get RGB values of pixel
			int green = all_textures[texture_pixel+1] * 0.7;
			int blue = all_textures[texture_pixel+2] * 0.7;
			
			// Actually drawing floor
			glPointSize(8);
			glColor3ub(red, green, blue);
			glBegin(GL_POINTS);
			glVertex2i(ray*8+530, current_pixel); // X-coord is multiplied by 8 because that is the point size. Then add 530 to render on right size of screen (left of the top down view)
			glEnd();

			// Draw ceiling - same as floor, only a different map array
			map_position = map_ceilings[(int) (texture_y / 32.0) * map_x_size + (int) (texture_x / 32.0)]*32*32; // Which tile of the ceiling are we currently rendering?
//			pixel_color = all_textures[((int) (texture_y) & 31) * 32 +  ((int) (texture_x) & 31) + map_position] * 0.7; // What is the color of the pixel we are currently rendering?
			
			// Actually drawing ceiling - same as floor, mostly
//			glColor3f(red, green, blue);
//			glPointSize(8);
//			glBegin(GL_POINTS);
//			glVertex2i(ray*8+530, screen_height - current_pixel); // Difference from floor rendering is here: draw from top of wall to top of screen. This chunk of code is otherwise the same.
//			glEnd();

			texture_pixel = (((int) (texture_y) & 31) * 32 +  ((int) (texture_x) & 31)) * 3 + map_position * 3; // Find specific pixel of texture
			red = all_textures[texture_pixel+0] * 0.7; // Get RGB values of pixel
			green = all_textures[texture_pixel+1] * 0.7;
			blue = all_textures[texture_pixel+2] * 0.7;
			
			// Actually drawing floor
			glPointSize(8);
			glColor3ub(red, green, blue);
			glBegin(GL_POINTS);
			glVertex2i(ray*8+530, screen_height - current_pixel); // X-coord is multiplied by 8 because that is the point size. Then add 530 to render on right size of screen (left of the top down view)
			glEnd();
		}

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

	// Debug code - draw point of intersection with first horizontal grid line
//	glColor3f(1,0,0);
//	glPointSize(8);
//	glBegin(GL_POINTS);
//	glVertex2i(x_intersect, y_intersect);
//	glEnd();
	
}

// Primary display function given to OpenGL
void display()
{
	//---------------------------------------------
	// PLAYER MOVEMENT
	//---------------------------------------------

	// Collision checks

	// Probes to check for collision before letting player move
	int horizontal_collision_probe = 0;
	int vertical_collision_probe = 0;
	
	if(player_delta_x < 0) // If player is facing left,
	{
		horizontal_collision_probe = -20; // Check left of player.
	}
	else
	{
		horizontal_collision_probe = 20; // Else, check right of player.
	}

	if(player_delta_y < 0) // If player is facing up (note - y axis is inverted),
	{
		vertical_collision_probe = -20; // Check above player.
	}
	else
	{
		vertical_collision_probe = 20; // Else, check below player.
	}

	// Player's x and y map tiles
	int player_map_x_tile = (int) player_x / 64.0;
	int player_map_y_tile = (int) player_y / 64.0;

	// Player's intended x and y map tiles (if walking forward)
	int horizontal_probe_map_tile = (int) ((player_x + horizontal_collision_probe) / 64.0);
	int vertical_probe_map_tile = (int) ((player_y + vertical_collision_probe) / 64.0);

	// Player's intended x and y map tiles (if walking backward)
	int reverse_horz_probe_map_tile = (int) ((player_x - horizontal_collision_probe) / 64.0);
	int reverse_vert_probe_map_tile = (int) ((player_y - vertical_collision_probe) / 64.0);

	// Locate player's intended map tiles, as located in actual map array
	int player_intended_x_tile = 			map_walls[((map_x_size * player_map_y_tile) + horizontal_probe_map_tile)]; 		// If I walk forward, will my x coordinate be in a wall?
	int player_intended_y_tile = 			map_walls[((map_x_size * vertical_probe_map_tile) + player_map_x_tile)]; 		// If I walk forward, will my y coordinate be in a wall?
	int player_intended_reverse_x_tile = 	map_walls[((map_x_size * player_map_y_tile) + reverse_horz_probe_map_tile)]; 	// If I walk backward, will my x coordinate be in a wall?
	int player_intended_reverse_y_tile = 	map_walls[((map_x_size * reverse_vert_probe_map_tile) + player_map_x_tile)]; 	// If I walk backward, will my y coordinate be in a wall?

	printf("player x tile: %d\n",   player_map_x_tile);
	printf("player y tile: %d\n\n", player_map_y_tile);
	

	// Move player forward
	if(Keys.w == 1)
	{
		if (player_intended_x_tile == 0)
		{
			player_x += player_delta_x*0.75; // Multiply by 0.75 to achieve slower speed
		}

		if (player_intended_y_tile == 0)
		{
			player_y += player_delta_y*0.75;
		}
	}

	// Rotate player left
	if(Keys.a == 1) 
	{
		//---------------- WARNING - PLAYER ROTATION -----------------
		// UNIT CIRCLE IS INVERTED (GOES CLOCKWISE)					
		// PI/2 IS DOWN. 3PI/2 IS UP.								
		//------------------------------------------------------------
	
		player_angle -= 0.075;
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
	if(Keys.s == 1) 
	{
		if (player_intended_reverse_x_tile == 0)
		{
			player_x -= player_delta_x/2;
		}

		if (player_intended_reverse_y_tile == 0)
		{
			player_y -= player_delta_y/2;
		}
	}	
	
	// Rotate player right
	if(Keys.d == 1)
	{
		player_angle += 0.075;
		if (player_angle > (2 * PI))
		{
			player_angle -= 2*PI; // Reset player angle after full right rotation
		}

		// Update player velocity after right rotation
		player_delta_x = cos(player_angle) * 5;
		player_delta_y = sin(player_angle) * 5;
	}
	
	glutPostRedisplay(); // Set flag for window to be redrawn

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen
	drawMap2D();
	drawPlayer(); // Draw 2D player dot
	drawRays3D();

	glutSwapBuffers(); // Render next frame
}

void init()
{
	glClearColor(0.3,0.3,0.3,0); // Set dark gray background
	gluOrtho2D(0,1024,512,0); // Make 1024 x 512 window w/ inverted Y coordinate
	
	// Initialize player position and velocity
	player_x = 128; player_y = 128;
	player_delta_x = cos(player_angle) * 5;
	player_delta_y = sin(player_angle) * 5;
}

// Detect keyboard butttons being pressed
void ButtonDown(unsigned char key, int x, int y)
{
	if (key=='w')
	{
		Keys.w = 1;
	}

	if (key=='a')
	{
		Keys.a = 1;
	}
	
	if (key=='s')
	{
		Keys.s = 1;
	}

	if (key=='d')
	{
		Keys.d = 1;
	}

	if (key=='e')
	{
		// Open door

		// Probes to check if player is facing a door
		// (Same system as player wall collision)
		int x_probe = 0;
		int y_probe = 0;

		// Position probes depending on player's facing
		if(player_delta_x < 0)
		{
			x_probe = -25;
		}
		else
		{
			x_probe = 25;
		}

		if(player_delta_y < 0)
		{
			y_probe = -25;
		}
		else
		{
			y_probe = 25;
		}

		// X and Y map tiles of player and probes
		int player_x_tile = player_x / 64.0;
		int player_y_tile = player_y / 64.0;

		int x_probe_tile = (player_x + x_probe) / 64.0;
		int y_probe_tile = (player_y + y_probe) / 64.0;

		// If the tile in front of the player is a door, delete it
		if(map_walls[y_probe_tile * map_x_size + x_probe_tile] == 7)
		{
			map_walls[y_probe_tile * map_x_size + x_probe_tile] = 0;
		}
	}
}

// Detect keyboard buttons being released
void ButtonUp(unsigned char key, int x, int y)
{
	if (key=='w')
	{
		Keys.w = 0;
	}

	if (key=='a')
	{
		Keys.a = 0;
	}
	
	if (key=='s')
	{
		Keys.s = 0;
	}

	if (key=='d')
	{
		Keys.d = 0;
	}
}

// Stop user from resizing window - maintains graphics
void resize()
{
	glutReshapeWindow(1024, 512);
}

void main(int argc, char* argv[])
{
	// Boilerplate OpenGL initializaion code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // Double buffered RGBA window
	glutInitWindowSize(1024, 512); // Window dimensions
	glutInitWindowPosition(200,400);
	glutCreateWindow("Raptor Raycaster Engine"); // Window name
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(ButtonDown);
	glutKeyboardUpFunc(ButtonUp);
	glutMainLoop();
}

