#include <iostream>
#include <cstdlib>
#include <cmath>
#include <pthread.h>
#include <time.h>
#include <assert.h>


using namespace std;

typedef struct thread_args{
	long long int LoopSize;
} thread_args;

long long int number_in_circle = 0; // shared variable
pthread_mutex_t mutex;

void *MonteCarlo(void* args)
{
	unsigned int seed = 333;

	thread_args *arg = (thread_args*) args;
	long long int LoopSize = arg->LoopSize;
	long long int TempSum = 0;
	for(long long int toss=0; toss<LoopSize; toss++){
		float x = ((float) rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
		float y = ((float) rand_r(&seed) / RAND_MAX) * 2.0 - 1.0;
		float distance_squared = x*x +y*y;
		if (distance_squared <=1.0) ++TempSum;
	}

	pthread_mutex_lock(&mutex);
	number_in_circle += TempSum;
	pthread_mutex_unlock(&mutex);

	pthread_exit((void *)0);
}

int main(int argc, char* argv[])
{
	if (argc != 3){
		cerr <<"Usage: "<<argv[0]<<" <number of threads> <number of tosses>"<<endl;
		return 1;
	}

	int number_of_thread = atoi(argv[1]);
	long long int number_of_tosses = atoll(argv[2]);
	
	
	pthread_t threads[number_of_thread];
	thread_args ARGS[number_of_thread];
	void *status;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	long long int LOOPSIZE = number_of_tosses/number_of_thread;
	for (int i=0; i<number_of_thread; i++) {
		ARGS[i].LoopSize = i == number_of_thread - 1 ? number_of_tosses - LOOPSIZE*(number_of_thread-1) : LOOPSIZE;
		//cout<<ARGS[i].LoopSize<<endl;
		pthread_create(&threads[i], &attr, MonteCarlo, (void *) &ARGS[i]);
	}
	
	pthread_attr_destroy(&attr);

	for(int i=0; i<number_of_thread; i++){
		int return_code = pthread_join(threads[i], &status);
		if(return_code){
			cerr<<"Error; return code from pthread_join() is "<<i<<endl;
			exit(-1);
		}
	}	

	float pi_estimate;
	pi_estimate = 4 * number_in_circle /(( double ) number_of_tosses);
	//cout<<pi_estimate<<endl;
	printf("%.7lf\n", pi_estimate);
	//assert(abs(pi_estimate-3.141) < 0.001);
	pthread_exit(NULL);
}
