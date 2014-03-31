#include <sys/types.h>
#include <sys/sem.h>

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short int *array; 
};

int state=1;

int sem_init(int no_of_sems,int commom_initial_value)
{
	key_t key=ftok(".",state++);
	int sem_id=semget(key,no_of_sems,IPC_CREAT|0666);
	if(sem_id==-1)
	{
		printf("sem initialisation error\n");
		exit(1);
	}
	union semun tmp;
	tmp.val=commom_initial_value;
	int i;
	for(i=0;i<no_of_sems;i++)
	{
		semctl(sem_id,i,SETVAL,tmp);
	}
	return sem_id;
}

void sem_change(int sem_id,int sem_num,int amount)
{
	struct sembuf tmp;
	tmp.sem_num=sem_num;
	tmp.sem_op=amount;
	tmp.sem_flg=0;
	if(semop(sem_id,&tmp,1)==-1)
	{
		printf("Sem change error\n");
		exit(1);
	}
}

int sem_try_change(int sem_id,int sem_num,int amount)
{
	struct sembuf tmp;
	tmp.sem_num=sem_num;
	tmp.sem_op=amount;
	tmp.sem_flg=IPC_NOWAIT;
	return semop(sem_id,&tmp,1);
}
