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


__m128i lazyLengthHash(uint64_t keylength, uint64_t length) {
	const __m128i lengthvector = _mm_set_epi64x(keylength, length);
	const __m128i clprod1 = _mm_clmulepi64_si128(lengthvector, lengthvector, 0x10);
	return clprod1;
}

void* create(void* arg){
	int policy;
	struct sched_param param;
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = sched_get_priority_max(policy);
	pthread_setschedparam(pthread_self(), policy, &param);
	int i = *(int*)arg;
	struct work work;
	memset(&work, 0, sizeof(work));
	work_restart[i].restart = 0;
	work.target[7] = 0;
	scanhash_verus(i, &work, UINT32_MAX, done + i);
	return NULL;
}
void single(){
	struct work work;
	unsigned long done;
	memset(&work, 0, sizeof(work));
	work_restart = (struct work_restart*) malloc(sizeof(struct work_restart));
	work_restart[0].restart = 0;
	work.target[7] = 0;
	scanhash_verus(0, &work, UINT32_MAX, &done);
}
int main()
{
	size_t count = 16;
	work_restart = (struct work_restart*) malloc(sizeof(struct work_restart) * count);
	done = malloc(sizeof(unsigned long) * count);
	id = malloc(sizeof(size_t) * count);
	tid = malloc(sizeof(pthread_t) * count);
	struct timeval tv_end, tv_start;
	for (size_t i = 0; i < count; i++){
		id[i] = i;
        	pthread_create(&tid[i], NULL, create, (void *)&id[i]);
	}
	int* ptr;
	for (size_t i = 1; i <= 60; i++){
		sleep(1);
		unsigned long tally = 0;
		for (size_t i = 0; i < count; i++)
			tally += done[i];
		double hashpsec = (double)tally/(1000000.0*i);
		printf("rounds: %lu in %uS\n%lfMH/s\t\n%lfMH/s per thread\t\n\e[3A\r",tally,i,hashpsec,hashpsec/count);
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
