/* 
This program will numerically compute the integral of
                  4/(1+x*x)				  
from 0 to 1.  The value of this integral is pi. 
It uses the timer from the OpenMP runtime library
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NUM_THREADS 2
double compute_pi(double step);
double compute_pi_meth1(double step);
double compute_pi_meth2(double step);
double compute_pi_meth3(double step);
double compute_pi_meth4(double step);
double compute_pi_parallel_reduction(double step);
void usage(char prog_name[]);
static long num_steps = 1000000;


int main (int argc, char **argv)
{
	double start_time, run_time=0, pi, step;
	double pi_meth1,start_time_meth1,run_time_meth1=0;
	double pi_meth2,start_time_meth2,run_time_meth2=0;
	double pi_meth3,start_time_meth3,run_time_meth3=0;
	double pi_meth4,start_time_meth4,run_time_meth4=0;
	double pi_meth5,start_time_meth5,run_time_meth5=0;
	int iter;

	if (argc != 2) {
		usage(argv[0]);
		exit (-1);
	}
	
	iter=atoi(argv[1]);
	step = 1.0/(double)num_steps;
	for(int i=0; i<iter; i++){
        /* Record the time for staring the computation of pi */
		start_time = omp_get_wtime();
		pi=compute_pi(step);
        /* Record the time for ending of the computation of pi and
		find the elapsed time for the computation of pi
		Note we are running the same process for multiple time and taking the average here */
		run_time += omp_get_wtime() - start_time;
		// doing the same for method1
		start_time_meth1 = omp_get_wtime();
		pi_meth1=compute_pi_meth1(step);
		run_time_meth1 += omp_get_wtime() - start_time_meth1;
		// doing the same again for method 2:
		start_time_meth2 = omp_get_wtime();
		pi_meth2=compute_pi_meth2(step);
		run_time_meth2 += omp_get_wtime() - start_time_meth2;
		//meth3
		start_time_meth3 = omp_get_wtime();
		pi_meth3=compute_pi_meth3(step);
		run_time_meth3 += omp_get_wtime() - start_time_meth3;
		//meth4
		start_time_meth4 = omp_get_wtime();
		pi_meth4=compute_pi_meth4(step);
		run_time_meth4 += omp_get_wtime() - start_time_meth4;
		//another one
		start_time_meth5 = omp_get_wtime();
		pi_meth5=compute_pi_parallel_reduction(step);
		run_time_meth5 += omp_get_wtime() - start_time_meth5;
	}
	printf("\nsequential: pi with %ld steps is %f in %f seconds\n",num_steps,pi,run_time/iter);
	printf("\nparallel with false sharing: pi with %ld steps is %f in %f seconds\n",num_steps,pi_meth1,run_time_meth1/iter);
	printf("\nparallel with race condition: pi with %ld steps is %f in %f seconds\n",num_steps,pi_meth2,run_time_meth2/iter);
	printf("\nparallel without false sharing: pi with %ld steps is %f in %f seconds\n",num_steps,pi_meth3,run_time_meth3/iter);
	printf("\nparallel with no race but serialized: pi with %ld steps is %f in %f seconds\n",num_steps,pi_meth4,run_time_meth4/iter);
	printf("\nparallel with no race and parallel: pi with %ld steps is %f in %f seconds\n",num_steps,pi_meth5,run_time_meth5/iter);
    return EXIT_SUCCESS; 
}

/*--------------------------------------------------------------------
 * Function:    compute_pi
 * Purpose:     Compute number pi in serial
 * In arg:      step
 */  
double compute_pi(double step){
	double pi, x, sum=0.0;
	for (int i=1;i<= num_steps; i++){
		x = (i-0.5)*step;
		sum = sum + 4.0/(1.0+x*x);
	}
	pi = step * sum;
	return pi;
} /* compute_pi */
//Method 1: False sharing
double compute_pi_meth1(double step){
	int nthreads ;
	double pi , sum [ NUM_THREADS ];
	omp_set_num_threads ( NUM_THREADS ) ;
	# pragma omp parallel
	{
		int i , id , tthreads ; double x ;
		tthreads = omp_get_num_threads () ;
		id = omp_get_thread_num () ;
		if ( id ==0) nthreads = tthreads ;
		for ( i = id , sum [ id ]=0.0; i < num_steps ; i = i + tthreads ) {
			x = ( i +0.5) * step ;
			sum [ id ] = sum [ id ] + 4.0/(1.0+ x * x ) ;
		}
	}
	pi = 0.0;
	for (int i =0 ; i < nthreads ; i ++){
		pi += step * sum [ i ];
	}
	return pi;
}
//Method2:race condition:
double compute_pi_meth2(double step){
	int i , tthreads , id ;
	double pi = 0.0 , sum = 0.0 , x ;
	# pragma omp parallel
	{
		tthreads = omp_get_num_threads () ;
		id = omp_get_thread_num () ;
		for ( i = id ; i < num_steps ; i += tthreads ) {
			x = ( i +0.5) * step ;
			sum = sum + 4.0/(1.0+ x * x ) ;
		}
	}
	pi += step * sum ;
	return pi;	

}
//mthe3
double compute_pi_meth3(double step){
	int nthreads ;
	double pi , sum [ NUM_THREADS ];
	omp_set_num_threads ( NUM_THREADS ) ;
	# pragma omp parallel
	{
		int i , id , tthreads ; double x,local_sum = 0.0 ;
		tthreads = omp_get_num_threads () ;
		id = omp_get_thread_num () ;
		if ( id ==0) nthreads = tthreads ;
		for ( i = id /*, sum [ id ]=0.0*/; i < num_steps ; i = i + tthreads ) {
			x = ( i +0.5) * step ;
			//sum [ id ] = sum [ id ] + 4.0/(1.0+ x * x ) ;
			local_sum += 4.0/(1.0+ x * x );
		}
	
			sum[id] = local_sum;
		
	}
	pi = 0.0;
	for (int i =0 ; i < nthreads ; i ++){
		pi += step * sum [ i ];
	}
	return pi;
}
// serialized:
// Method3: Parallel but serialized using critical (no race condition)
double compute_pi_meth4(double step){
    int i, id, tthreads;
    double pi = 0.0, sum = 0.0, x;

    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel private(i, id, tthreads, x)
    {
        double local_sum = 0.0;

        tthreads = omp_get_num_threads();
        id = omp_get_thread_num();

        for (i = id; i < num_steps; i += tthreads) {
            x = (i + 0.5) * step;
            local_sum += 4.0/(1.0 + x*x);
        }

        // serialize the update
        #pragma omp critical
        sum += local_sum;
    }

    pi = step * sum;
    return pi;
}
//another one:
double compute_pi_parallel_reduction(double step){
    double pi = 0.0, x;

    #pragma omp parallel for reduction(+:pi)
    for(int i = 1; i < num_steps; i++){
        x = (i + 0.5) * step;
        pi += 4.0 / (1.0 + x*x);
    }

    pi *= step;
    return pi;
}


/*--------------------------------------------------------------------
 * Function:    usage
 * Purpose:     Print command line for function
 * In arg:      prog_name
 */
void usage(char prog_name[]) {
   fprintf(stderr, "usage:  %s <number of times to run>\n", prog_name);
} /* usage */


