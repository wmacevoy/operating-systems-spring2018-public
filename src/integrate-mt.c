#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef double real_function_t(double);

double f(double x) {
  double y = pow(x,2);
  return y;
}

double F(double x) {
  double y = pow(x,3)/3.0;
  return y;
}

double g(double x) {
  double y = sin(x);
  return y;
}

double G(double x) {
  double y = -cos(x);
  return y;
}

double integrate(real_function_t *f, double a, double b, int n) {
  double ans = 0;
  double h = (b-a)/n;
  for (int i=0; i<n; ++i) {
    double x = a+h*(i+0.5);
    ans = ans + h*f(x);
  }
  return ans;
}

struct job_struct {
  real_function_t *integrand;
  double a;
  double b;
  int n;
  double A;
  pthread_t thread;
};
typedef struct job_struct job_t;

void* worker(void *arg) {
  job_t *job = (job_t*) arg;
  job->A = integrate(job->integrand,job->a,job->b,job->n);
  return 0;
}

int main(int argc, char *argv[]) {
  double a = 0.0;
  double b = 1.0;
  int n = 100;
  int njobs = 4;
  char *name="x^2";
  real_function_t *integrand = &f;
  real_function_t *integral = &F;

  for (int i=1; i<argc; ++i) {
    if (strncmp(argv[i],"a=",2)==0) { a=atof(argv[i]+2); continue; }
    if (strncmp(argv[i],"b=",2)==0) { b=atof(argv[i]+2); continue; }
    if (strncmp(argv[i],"n=",2)==0) { n=atoi(argv[i]+2); continue; }
    if (strncmp(argv[i],"njobs=",6)==0) { njobs=atoi(argv[i]+6); continue; }    
    if (strncmp(argv[i],"f=",2)==0) {
      name = argv[i]+2;
      if (strcmp(name,"x^2")==0) { integrand=&f; integral=&F; continue; }
      if (strcmp(name,"sin(x)")==0) { integrand=&g; integral=&G; continue; }
      printf("unknown function\n");
      exit(1);
    }
    printf("usage: integrate <a=0.0> <b=1.0> <n=100> <f=x^2|sin(x)>\n");
    exit(0);
  }
  
  job_t *jobs = (job_t*) malloc(sizeof(job_t)*njobs);

  for (int i=0; i<njobs; ++i) {
    jobs[i].a = a + (b-a)*((double) i/((double) njobs));
    jobs[i].b = a + (b-a)*((double) (i+1)/((double) njobs));
    jobs[i].n = n/njobs;
    jobs[i].integrand = integrand;
    jobs[i].A = 0;
  }

  for (int i=0; i<njobs; ++i) {
    pthread_create(&jobs[i].thread, 0, worker, &jobs[i]);
  }

  for (int i=0; i<njobs; ++i) {
    pthread_join(jobs[i].thread, 0);
  }

  double numeric = 0;
  for (int i=0; i<njobs; ++i) {
    numeric = numeric + jobs[i].A;
  }

  double exact   = integral(b)-integral(a);
  double abserr  = fabs(numeric-exact);
  double relerr  = fabs(numeric-exact)/fabs(exact);

  printf("f=%s a=%lf b=%lf n=%d njobs=%d numeric=%lf exact=%lg abserr=%lg relerr=%lg\n",
	 name,a,b,n,njobs,numeric,exact,abserr,relerr);
  
  return 0;
}
