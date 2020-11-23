#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

typedef struct PlayerInfo{
	int id;
	int value;
	int rank;
}player;

player Player_list[8];

int compare_value(const void *a, const void *b)
{
    player *c = (player *)a;
    player *d = (player *)b;
    if (c->value != d->value) return d->value - c->value;
    else return c->id - d->id;                   
}

int compare_id(const void *a, const void *b)
{
    player *c = (player *)a;
    player *d = (player *)b;
    if (c->id != d->id) return c->id - d->id;
    else return d->value - c->value;                                
}

int main(int argc, char *argv[]){
	int host_id = atoi(argv[1]);
	int key = atoi(argv[2]);
	int depth = atoi(argv[3]);
	int p1[2][2], p2[2][2];
	FILE *F[2][2];
	if (depth == 0){
		for (int i = 0; i < 2; i++){
			pipe(p1[i]);
			pipe(p2[i]);
			int pid = fork();
			if (pid) {
				//parent, i = 0 left child, i = 1 right child
				close(p1[i][0]);
				close(p2[i][1]);
				F[i][0] = fdopen(p2[i][0], "r");
				F[i][1] = fdopen(p1[i][1], "w");
				continue;
			}
			else {
				//child
				close(p1[i][1]);
				close(p2[i][0]);
				dup2(p1[i][0], 0);
				dup2(p2[i][1], 1);
				close(p1[i][0]);
				close(p2[i][1]);
				char tmp[100];
				sprintf(tmp, "%d", depth+1);
				execlp("./host", "./host", argv[1], argv[2], tmp, NULL);
			}
		}
		char fifo[2][30];
		//F2[0]: store fp of player_id, F2[1]: send something back
		FILE *F2[2];
		sprintf(fifo[0], "fifo_%d.tmp", host_id);
		sprintf(fifo[1], "fifo_0.tmp");
		F2[0] = fopen(fifo[0], "r");
		F2[1] = fopen(fifo[1], "w");

		int player_id[8];
		while(1){
			for (int i = 0; i < 8; i++){
				fscanf(F2[0], "%d", &player_id[i]);
				Player_list[i].id = player_id[i];
			}
			if (player_id[0] == -1) exit(0);

			fprintf(F[0][1], "%d %d %d %d\n", player_id[0], player_id[1], player_id[2], player_id[3]);
			fflush(F[0][1]);
			fsync(fileno(F[0][1]));
			fprintf(F[1][1], "%d %d %d %d\n", player_id[4], player_id[5], player_id[6], player_id[7]);
			fflush(F[1][1]);
			fsync(fileno(F[1][1]));
			
			for (int i = 0; i < 10; i++){
				int player_id[2], player_score[2];
				for (int j = 0; j < 2; j++){
					fscanf(F[j][0], "%d%d", &player_id[j], &player_score[j]);
				}
				if (player_score[0] > player_score[1]){
					for (int k = 0; k < 8; k++){
						if (Player_list[k].id == player_id[0]){
							Player_list[k].value += 1;
						}
					}
				}
				else if  (player_score[1] > player_score[0]){
					for (int l = 0; l < 8; l++){
						if (Player_list[l].id == player_id[1]){
							Player_list[l].value += 1;
						}
					}
				}
				qsort(Player_list, 8, sizeof(player), compare_value);
				int num = 1;
				for (int j = 0; j < 8; j++){
					if (j == 0){
						Player_list[j].rank = 1;
					}
					else {
						if (Player_list[j].value == Player_list[j-1].value){
							Player_list[j].rank = Player_list[j-1].rank;
							num++;
						}
						else {
							Player_list[j].rank = (Player_list[j-1].rank + num);
							num = 1;
						}
					}
				}
				qsort(Player_list, 8, sizeof(player), compare_id);	
			}
			fprintf(F2[1], "%d\n", key);
			for (int j = 0; j < 8; j++){
				fprintf(F2[1], "%d %d\n", Player_list[j].id, Player_list[j].rank);
			}
			fflush(F2[1]);
			fsync(fileno(F2[1]));
		}
		wait(NULL);
		wait(NULL);
		fclose(F2[0]);
		fclose(F2[1]);
		fclose(F[0][0]);
		fclose(F[0][1]);
		fclose(F[1][0]);
		fclose(F[1][1]);
	}
	else if (depth == 1){
		for (int i = 0; i < 2; i++){
			pipe(p1[i]);
			pipe(p2[i]);
			int pid = fork();
			if (pid) {
				//parent, i = 0 left child, i = 1 right child
				close(p1[i][0]);
				close(p2[i][1]);
				F[i][0] = fdopen(p2[i][0], "r");
				F[i][1] = fdopen(p1[i][1], "w");
				continue;
			}
			close(p1[i][1]);
			close(p2[i][0]);
			dup2(p1[i][0], 0);
			dup2(p2[i][1], 1);
			close(p1[i][0]);
			close(p2[i][1]);
			char tmp[100];
			sprintf(tmp, "%d", depth+1);
			execlp("./host", "./host", argv[1], argv[2], tmp, NULL);
		}
		int player_id[4];
		while(1){
			for (int i = 0; i < 4; i++){
				scanf("%d", &player_id[i]);
			}
			if (player_id[0] == -1) exit(0);
			fprintf(F[0][1], "%d %d\n", player_id[0], player_id[1]);
			fflush(F[0][1]);
			fsync(fileno(F[0][1]));
			fprintf(F[1][1], "%d %d\n", player_id[2], player_id[3]);
			fflush(F[1][1]);
			fsync(fileno(F[1][1]));	
			for (int i = 0; i < 10; i++){
				int player_id[2], player_score[2];
				for (int j = 0; j < 2; j++){
					fscanf(F[j][0], "%d%d", &player_id[j], &player_score[j]);
				}
				if (player_score[0] > player_score[1]){
					printf("%d %d\n", player_id[0], player_score[0]);
				}
				else {
					printf("%d %d\n", player_id[1], player_score[1]);
				}
				fflush(stdout);
				fsync(1);
			}
		}
		wait(NULL);
		wait(NULL);
		fclose(F[0][0]);
		fclose(F[0][1]);
		fclose(F[1][0]);
		fclose(F[1][1]);
	}
	else {
		int player_id[2];
		while(1){
			for (int i = 0; i < 2; i++){
				scanf("%d", &player_id[i]);				
			}
			if (player_id[0] == -1) exit(0);
			for (int i=0; i<2; i++){
				pipe(p1[i]);
				pipe(p2[i]);
				int pid = fork();
				if (pid){
					close(p1[i][0]);
					close(p2[i][1]);
					F[i][0] = fdopen(p2[i][0], "r");
					F[i][1] = fdopen(p1[i][1], "w");
					continue;
				}
				close(p1[i][1]);
				close(p2[i][0]);
				dup2(p1[i][0], 0);
				dup2(p2[i][1], 1);
				close(p1[i][0]);
				close(p2[i][1]);
				char tmp[10];
				sprintf(tmp, "%d", player_id[i]);
				execlp("./player", "./player", tmp, NULL);
			}
			for (int i = 0; i < 10; i++){
				int player_id[2], player_score[2];
				for (int j = 0; j < 2; j++){
					fscanf(F[j][0], "%d%d", &player_id[j], &player_score[j]);
				}
				if (player_score[0] > player_score[1]){
					printf("%d %d\n", player_id[0], player_score[0]);
				}
				else {
					printf("%d %d\n", player_id[1], player_score[1]);
				}
				fflush(stdout);
				fsync(1);
			}
			wait(NULL);
			wait(NULL);
			fclose(F[0][0]);
			fclose(F[0][1]);
			fclose(F[1][0]);
			fclose(F[1][1]);
		}
	}
}