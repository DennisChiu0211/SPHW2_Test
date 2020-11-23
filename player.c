#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int player_id = 0;
	player_id = atoi(argv[1]);

	int bit_list[21] = {20, 18, 5, 21, 8, 7, 2, 19, 14, 13, 9, 1, 6, 10, 16, 11, 4, 12, 15, 17, 3};
	for (int i = 1; i <= 10; i++){
		int bid = 0;
		bid = bit_list[player_id + i - 2] * 100;
		printf("%d %d\n", player_id, bid);
		fflush(stdout);
	}
	exit(0);
}