#include <kernel/shell.h>

#include <math.h>

#include <string.h>
#include <kernel/drivers/tty.h>
#include <kernel/drivers/keyboard.h>

const char const possibleChars[] =
"&#W@KOJos?+^':. "/**/;
int possibleCharsCount;

const uint16_t floorCharacter = '.' + 0xf00;
const uint16_t cellingCharacter = ' ' + 0xf00;

#define isWall(x) ((x) >= 48)

const double MOVE_SPEED = 0.7;

typedef struct {

	double x, y;

	double directionX, directionY;

	double planeX, planeY;
} Camera;

typedef struct {
	char* mapChars;
	uint mapWidth;
	uint mapHeight;
} Map;

Camera createCamera(double x, double y, double directionX, double directionY, double planeX, double planeY) {
	Camera camS;

	camS.x = x;
	camS.y = y;
	camS.directionX = directionX;
	camS.directionY = directionY;

	camS.planeX = planeX;
	camS.planeY = planeY;

	return camS;
}

void rotate(Camera* cam, double angle) {
	double planeX = cam->planeX, planeY = cam->planeY;
	double directionX = cam->directionX, directionY = cam->directionY;

	double oldxDir = directionX;
	cam->directionX = directionX * cos(angle) - directionY * sin(angle);
	cam->directionY = oldxDir * sin(angle) + directionY * cos(angle);

	double oldxPlane = planeX;
	cam->planeX = planeX * cos(angle) - planeY * sin(angle);
	cam->planeY = oldxPlane * sin(angle) + planeY * cos(angle);
}

void move(Camera* cam, bool forward, Map* map) {
	if (forward) {
		if (!isWall(map->mapChars[(int)(cam->x + cam->directionX * MOVE_SPEED) + (int)(cam->y) * map->mapWidth])) {
			cam->x += cam->directionX*MOVE_SPEED;
		}

		if (!isWall(map->mapChars[(int)cam->x + (int)(cam->y + cam->directionY * MOVE_SPEED) * map->mapWidth])) {
			cam->y += cam->directionY*MOVE_SPEED;
		}
	} else {
		if (!isWall(map->mapChars[(int)(cam->x - cam->directionX * MOVE_SPEED) + (int)(cam->y) * map->mapWidth])) {
			cam->x -= cam->directionX * MOVE_SPEED;
		} if (!isWall(map->mapChars[(int)cam->x + (int)(cam->y - cam->directionY * MOVE_SPEED) * map->mapWidth])) {
			cam->y -= cam->directionY * MOVE_SPEED;
		}
	}
}

void draw(Camera* camera, const Map* map);

#define drawChar(x, y, ch, col) tty_set_data((x), (y), (ch), (col));

int counter = 0;

Camera cam;
Map map;

void test_3d() {
	tty_clear();

	possibleCharsCount = strlen(possibleChars);

	cam = createCamera(2, 2, 1, 0, 0, -0.8);

	const uint mapWidth = 16;
	const uint mapHeight = 16;

	char* mapChars =
		"2222222222222222"
		"2              2"
		"2              2"
		"2    4         2"
		"2    5         2"
		"2    6         2"
		"211111         2"
		"2              2"
		"2              2"
		"2              2"
		"2              2"
		"2              2"
		"2              2"
		"2              2"
		"2              2"
		"2222222222222222";

	Map _map = {mapChars, mapWidth, mapHeight};
	map = _map;

	//tty_disable_cursor();
	char ch;

	do {
		ch = keyboard_read_input_buffer().symbol;
		
		switch (ch) {
			case 'a': rotate(&cam, 0.1); break;
			case 'd': rotate(&cam, -0.1); break;
			case 'w': move(&cam, 1, &map); break;
			case 's': move(&cam, 0, &map); break;
			case 'p': ++counter; break;
			default: break;
		}

		draw(&cam, &map);

	} while(ch != 27);

	tty_gotoxy(0, 0);
}

void draw(Camera* camera, const Map* mapI) {
	const int mapWidth = mapI->mapWidth, mapHeight = mapI->mapHeight;
	const char* const map = mapI->mapChars;

	const double width = 80;
	const double height = 24;

	for (int x = 0; x < width; ++x) {
		double cameraX = 2 * x / (double)(width)-1;
		double rayDirX = camera->directionX + camera->planeX * cameraX;
		double rayDirY = camera->directionY + camera->planeY * cameraX;
		//Map position
		//System.out.println(camera.xPlane);

		int mapX = (int)camera->x;
		int mapY = (int)camera->y;
		//length of ray from current position to next x or y-side
		double sideDistX;
		double sideDistY;
		//Length of ray from one side to next in map
		double deltaDistX = sqrt(1 + (rayDirY*rayDirY) / (rayDirX*rayDirX));

		double deltaDistY = sqrt(1 + (rayDirX*rayDirX) / (rayDirY*rayDirY));
		double perpWallDist;
		//Direction to go in x and y
		int stepX, stepY;

		int side = 0;//was the wall vertical or horizontal
					 //Figure out the step direction and initial distance to a side

		if(rayDirX < 0) {
			stepX = -1;
			sideDistX = (camera->x - mapX) * deltaDistX;
		} else {
			stepX = 1;
			sideDistX = (mapX + 1.0 - camera->x) * deltaDistX;
		}
		if(rayDirY < 0) {
			stepY = -1;
			sideDistY = (camera->y - mapY) * deltaDistY;
		} else {
			stepY = 1;
			sideDistY = (mapY + 1.0 - camera->y) * deltaDistY;
		}

		char wall;

		//Loop to find where the ray hits a wall
		while (1) {
			//Jump to next square
			if (sideDistX < sideDistY) {
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			} else {
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}

			if (mapX >= mapWidth || mapY >= mapHeight) {
				wall = '0';

				break;
			}

			if((wall = map[mapX + mapY * mapHeight]) != ' ') break;
		}



		// double xx = (double)(mapX - camera->x) / (double)4;
		// double yy = (double)(mapY - camera->y) / (double)4;

		if (side == 0) {
			perpWallDist = fabs((mapX - camera->x + (1 - stepX) / 2) / rayDirX);
		} else {
			perpWallDist = fabs((mapY - camera->y + (1 - stepY) / 2) / rayDirY);
		}
		int lineHeight;

		if (perpWallDist > 0) {
			lineHeight = fabs((int)(height / perpWallDist));
		}

		else lineHeight = height;

		int drawStart = -lineHeight / 2 + height / 2;

		if(drawStart < 0) {
			drawStart = 0;
		}

		int drawEnd = lineHeight / 2 + height / 2;

		if(drawEnd >= height) {
			drawEnd = height - 1;
		}

		// double distance;

		// {
		// 	double dx = camera->x - mapX + 0.5;
		// 	double dy = camera->y - mapY + 0.5;

		// 	distance = sqrt(dx * dx + dy * dy);
		// }

		double distWall;

		//Calculate distance
		if (side) {
			//If its a y-axis wall
			distWall = abs((mapY - camera->y + (1 - stepY) / 2) / rayDirY);
		} else {
			//If its a x-axis wall
			distWall = abs((mapX - camera->x + (1 - stepX) / 2) / rayDirX);
		}

		// double wallX;
		// if (side == 1) {
		// 	wallX = (camera->x + ((mapY - camera->y + (1 - stepY) / 2) / rayDirY) * rayDirX);
		// } else {
		// 	wallX = (camera->y + ((mapX - camera->x + (1 - stepX) / 2) / rayDirX) * rayDirY);
		// }

		//wallX -= floor(wallX);

		int outCharId = (int)((distWall) * 4) - 2;

		if (outCharId >= possibleCharsCount) {
			outCharId = possibleCharsCount - 1;
		} else if (outCharId < 0) {
			outCharId = 0;
		}

		char tx = possibleChars[outCharId];

		if (wall == '0') {
			tx = '0';
		}

		uint color = (wall - ' ') & 0xf;
		
		uint16_t data = tx + (color << 8);
		
		for(int y = drawStart; y < drawEnd; y++) {
			drawChar(x, y, data, color);
		}

		for (int y = 0; y < drawStart; ++y) {
			drawChar(x, y, cellingCharacter, 0x0F);
		}

		for (int y = drawEnd; y < height; ++y) {
			drawChar(x, y, floorCharacter, 0x0F);
		}

	// nextCollum:;
	}
}
