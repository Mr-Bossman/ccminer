 #define _GNU_SOURCE
#include "miner.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include "portability.h"

struct work_restart* work_restart;
unsigned long *done;
size_t *id;
pthread_t *tid;


static void affine_to_cpu(int id) {
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(id, &set);
	pthread_setaffinity_np(tid[id], sizeof(&set), &set);
}
void* create(void* arg){
	int i = *(int*)arg;
	affine_to_cpu(i);
	struct work work;
	memset(&work, 0, sizeof(work));
	work_restart[i].restart = 0;
	work.target[7] = 0;
	scanhash_verus(i, &work, UINT32_MAX, done + i);
	return NULL;
}

int main(int argc, char *argv[])
{
	size_t count = 1;
	size_t time = 20;
	if (argc >= 2)
		if (atoi(argv[1]))
			count = atoi(argv[1]);
	if (argc == 3)
		if (atoi(argv[2]))
			time = atoi(argv[2]);
	work_restart = (struct work_restart*) malloc(sizeof(struct work_restart) * count);
	done = malloc(sizeof(unsigned long) * count);
	id = malloc(sizeof(size_t) * count);
	tid = malloc(sizeof(pthread_t) * count);
	for (size_t i = 0; i < count; i++){
		id[i] = i;
        	pthread_create(&tid[i], NULL, create, (void *)&id[i]);
	}
	int* ptr;
	for (size_t i = 1; i < time; i++){
		sleep(1);
		unsigned long tally = 0;
		for (size_t i = 0; i < count; i++)
			tally += done[i];
		double hashpsec = (double)tally/((double)i);
		printf("rounds: %lu in %lus\n%lfMH/s\n%lfMH/s per thread\n\n",tally,i,hashpsec/1000000,hashpsec/	(1000000*count));
	}
	for (size_t i = 0; i < count; i++)
		work_restart[i].restart = 1;
	for (size_t i = 0; i < count; i++)
		pthread_join(tid[i],(void**)&ptr);
	return 0;
}

void bn_store_hash_target_ratio(uint32_t* hash, uint32_t* target, struct work* work, int nonce){
	(void)hash;
	(void)target;
	(void)work;
	(void)nonce;
}
