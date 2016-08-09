/*************************************************************************
    > File Name: writeShareMemory.c
    > Author: wht
    > Mail: vi_pzp@163.com 
    > Created Time: 2016年06月07日 星期二 14时52分35秒
 ************************************************************************/

#include<stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	int ret = -1;
	char *semString = "ftok.keyfile";
	char *shmString = "ftok.keyfile";

	key_t semKey = ftok(semString, 0);
	key_t shmKey = ftok(shmString, 0);
	int semid;
	int shmid;
	void *addr = NULL;

	printf(">> writeShareMemory\n");
	do
	{
		semid = semget(semKey, 2, 0666|IPC_CREAT);
		if (semid < 0)
		{
			printf("semget failed\n");
			ret = semid;
			break;
		}
		union semun {
			int val;
			struct semid_ds *buf;
			ushort *array;
		} sem_u;
		
		//sem_u.val = 0;
		//semctl(semid, 0, SETVAL, sem_u);
		sem_u.val = 1;
		semctl(semid, 1, SETVAL, sem_u);

		shmid = shmget(shmKey, 1024, 0666|IPC_CREAT);
		if (shmid < 0)
		{
			printf("shmget failed\n");
			ret = shmid;
			break;
		}

		addr = shmat(shmid, NULL, 0);
		//printf("%p\n", addr);
		if ((void *)(-1) == addr)
		{
			printf("shmat failed\n");
			break;
		}

		int i = 10;
		while(i--)
		{
			//p 1
			struct sembuf sem_p;
			sem_p.sem_num = 1;
			sem_p.sem_op = -1;
			semop(semid, &sem_p, 1);

			//strcpy(addr, "hello world");
			sleep(1);
			snprintf((char *)addr, 1024, "hello %d", i);
			printf("write:%s\n", (char *)addr);

			//v 0
			struct sembuf sem_v;
			sem_v.sem_num = 0;
			sem_v.sem_op = 1;
			semop(semid, &sem_v, 1);
		}

	} while(0);

	
	shmdt(addr);
	printf("<< writeShareMemory\n");
	return 0;
}
