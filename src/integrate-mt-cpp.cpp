#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <new>

#include <thread>
#include <mutex>

typedef double real_function_t(double);

double numeric = 0; // shared resource
std::recursive_mutex numeric_mutex; // corresponding mutex

//struct numeric_guard {
//  numeric_gaurd() { numeric_mutex.lock(); }
//  ~numeric_gaurd() { numeric_mutex.unlock(); }  
//};

struct numeric_guard
  : std::lock_guard < std::recursive_mutex > {
  numeric_guard() : std::lock_guard < std::recursive_mutex> (numeric_mutex) {}
};

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

void _usleep(int micros) {
  if (rand() % 100 == 0) {
    printf("pthread_exit\n");
    pthread_exit(NULL);
  }
  usleep(micros);
}


struct Job {
  real_function_t *integrand;
  double a;
  double b;
  int n;
  double *A;

  std::thread *thread;

  Job() {
    thread = 0;
  }

  void work() {
    double area = integrate(integrand,a,b,n);
    {
      numeric_guard guard;
      double total = *A;
      _usleep(rand()%10000);
      total = total + area;
      *A = total;
    }
  }
  
  void start() {
    if (thread == 0) {
      thread = new std::thread(&Job::work,this);
    }
  }

  void join() {
    if (thread != 0) {
      thread->join();
      delete thread;
      thread = 0;
    }
  }
  
  ~Job() {
    join();
  }
};

int main(int argc, char *argv[]) {
  double a = 0.0;
  double b = 1.0;
  int n = 100;
  int njobs = 4;
  const char *name="x^2";
  real_function_t *integrand = &f;
  real_function_t *integral = &F;
  
  srand(time(0));
  
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
    printf("usage: integrate-mt-cpp <a=0.0> <b=1.0> <n=100> <njobs=4> <f=x^2|sin(x)>\n");
    exit(0);
  }
  
  Job *jobs = new Job[njobs];

  for (int i=0; i<njobs; ++i) {
    jobs[i].a = a + (b-a)*((double) i/((double) njobs));
    jobs[i].b = a + (b-a)*((double) (i+1)/((double) njobs));
    int j0 = (int) (n*((double) i/((double) njobs)));
    int j1 = (int) (n*((double)(i+1)/((double) njobs)));    
    jobs[i].n = j1-j0;
    jobs[i].integrand = integrand;
    jobs[i].A = &numeric;
  }

  // critical section start (uses numeric)
  {
    numeric_guard guard;
    numeric = 0;
  }
  // critical section end (uses numeric)  
  for (int i=0; i<njobs; ++i) {
    jobs[i].start();
  }

  for (int i=0; i<njobs; ++i) {
    jobs[i].join();
  }

  delete [] jobs;

  double exact;
  double abserr;
  double relerr;
  // critical section start (uses numeric)
  {
    numeric_guard guard;
    exact   = integral(b)-integral(a);
    abserr  = fabs(numeric-exact);
    relerr  = fabs(numeric-exact)/fabs(exact);
  }
  // critical section start (uses numeric)  

  printf("f=%s a=%lf b=%lf n=%d njobs=%d numeric=%lf exact=%lg abserr=%lg relerr=%lg\n",
	 name,a,b,n,njobs,numeric,exact,abserr,relerr);

  return 0;
}
