#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

// y, x 좌표가 갈수 있는 길인지 체크
bool is_valid_path(const int* maze, int maze_size, const int* path_tried, int y, int x)
{
	// 배열 좌표내의 좌표가 아닌경우
	if(y < 0 || y >= maze_size) return false;
	if(x < 0 || x >= maze_size) return false;

	// check blocked area
	if(maze[maze_size*y + x] == 1) return false;

	// check already tried area
	if(path_tried[maze_size*y + x] == 1) return false;

	return true;
}

// 좌표를 상하좌우 방향으로 이동하기 위한 상대좌표값
const int direction[4][3] =
{
//   row col mark
	{ 0,  1, 'r'}, // right
	{ 1,  0, 'd'}, // down
	{ 0, -1, 'l'}, // left
	{-1,  0, 'u'}  // up
};

bool find_path(int* maze, int maze_size, int* path_tried, int y, int x)
{
	// check exit door
	if(y == maze_size-1 && x == maze_size-1)
	{
		return true;
	}

	// 갔던 길 표시
	path_tried[maze_size*y + x] = 1;

	// try 4 directions
	for(int i=0; i<4; i++)
	{
		int yy = y + direction[i][0];
		int xx = x + direction[i][1];
		if(is_valid_path(maze, maze_size, path_tried, yy, xx))
		{
			// recursive call
			if(find_path(maze, maze_size, path_tried, yy, xx))
			{
				// mark moved direction
				maze[maze_size*yy + xx] = direction[i][2];
				return true;
			}
		}
	}

	return false;
}

bool input_maze(int* maze, int maze_size)
{
	printf("Please input contents of maze row by row. 1 for barrier and 0 for free passage.\n\n");

	for(int y=0; y<maze_size; y++)
	{
		for(int x=0; x<maze_size; x++)
		{
			scanf("%d", &maze[y*maze_size + x]);
			int v = maze[y*maze_size + x];
			if(v != 0 && v != 1) // 잘못된 입력일 경우
			{
				printf("**Error** 0 or 1 is allowed!\n");
				return false;
			}
		}
	}

	if(maze[0] == 1) // 입구가 막힌경우
	{
		printf("**Error** entrance to maze is blocked!\n");
		return false;
	}

	return true;
}

bool input_maze_size(int* maze_size)
{
	printf("Please input size of maze (a number between 4 and 20 is expected) -> ");
	scanf("%d", maze_size);

	if(*maze_size < 4 || *maze_size > 20)
	{
		printf("**Error** maze size not in range!\n");
		return false;
	}

	return true;
}

void print_map(int* maze, int size, const char* desc)
{
	printf("%s\n", desc);

	for(int y=-1; y<=size; y++)
	{
		if(y == -1 || y == size)
		{
			printf("+");
			for(int x=0; x<size; x++)
			{
				printf("-");
			}
			printf("+");
		}
		else
		{
			if(y == 0) printf(" ");
			else printf("|");
			for(int x=0; x<size; x++)
			{
				char c = maze[size*y + x];
				if(c == 0) c = ' ';
				if(c == 1) c = 'x'; // barrier
				printf("%c", c);
			}

			if(y != size-1) printf("|");
		}

		printf("\n");
	}
}

int main()
{
	while(true)
	{
		int maze_size = 0;
		if(!input_maze_size(&maze_size))
		{
			continue;
		}

		const int MN = maze_size*maze_size; // total size(M x N)
		// memory alloc
		int* maze = new int[MN];
		int* path_tried = new int[MN];

		// initialize memory
		memset(maze, 0, sizeof(int)*MN);
		memset(path_tried, 0, sizeof(int)*MN);

		if(!input_maze(maze, maze_size))
		{
			delete [] maze;
			delete [] path_tried;
			break;
		}

		int success = find_path(maze, maze_size, path_tried, 0, 0);
		if(!success) printf("**Warning** no path from entrance to exit!\n");

		maze[0] = 's'; // starting point
		print_map(maze, maze_size, "The maze and the path:");

		// 메모리 dealloc
		delete [] maze;
		delete [] path_tried;
		break;
	}

	return 0;
}

