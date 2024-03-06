#include "DT5742.h"
#include <pthread.h>
#include <iostream>
#include <chrono>
#include <thread>

void *send_job(void *t) {
  DT5742 *dt = (DT5742*) t;
  dt->Capture();
  pthread_exit(NULL);
}


int main(int argn, char** argv) {
  int nev = atoi(argv[1]);
  int nspill0 = atoi(argv[2]);
  int nspill1 = atoi(argv[3]);
  bool spill0 = false;
  bool spill1 = false;
  if(nspill0==1) spill0=true;
  if(nspill1==1) spill1=true;
  std::cout << "Aiming at " << nev << std::endl;
  
  DT5742 *dt0 = new DT5742(0,nev,spill0);
  dt0->Setup("input0.txt");
  //dt0->Print();
  std::cout << std::endl;

  DT5742 *dt1 = new DT5742(1,nev,spill1);
  dt1->Setup("input1.txt");
  //dt1->Print();
  std::cout << std::endl;

  
  //setting up jobs
  int rc;
  pthread_t threads[2];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  //sending jobs
  rc = pthread_create(&threads[0], &attr, send_job, (void *)dt0 );
  if (rc) {
     std::cout << "Error:unable to create thread," << rc << std::endl;
     exit(-1);
  }
  rc = pthread_create(&threads[1], &attr, send_job, (void *)dt1 );
  if (rc) {
     std::cout << "Error:unable to create thread," << rc << std::endl;
     exit(-1);
  }

  long ev0, ev0t;
  long ev1, ev1t;
  long tg0 = dt0->EventsTarget();
  long tg1 = dt1->EventsTarget();
  for( long counter=0; (ev0<tg0) || (ev1<tg1); ++counter ) {
    ev0t = dt0->EventsInDisk();
    ev1t = dt1->EventsInDisk();
    std::this_thread::sleep_for( std::chrono::milliseconds(800) );
    ev0 = dt0->EventsInDisk();
    ev1 = dt1->EventsInDisk();
    double instRate0 = (ev0-ev0t)/0.8;
    double instRate1 = (ev1-ev1t)/0.8;
    std::cout << "EVENTS: [0] => " << ev0 << "(" << ev0*100./tg0 << "%)";
    std::cout << " || [1] => "     << ev1 << "(" << ev1*100./tg1 << "%)";
    std::cout << " ||| => [" << instRate0 << "Hz, " << instRate1 << "Hz]";
    std::cout << "\r";
    std::cout << std::flush;
    if(counter%5==0) std::cout << std::endl;
  }
  
  // free attribute and wait for the other threads
  void *status;
  pthread_attr_destroy(&attr);
  rc = pthread_join(threads[0], &status);
  if (rc) {
      std::cout << "Error:unable to join," << rc << std::endl;
      exit(-1);
  }
  std::cout << "Job 0 completed with status :" << status << std::endl;

  rc = pthread_join(threads[1], &status);
  if (rc) {
      std::cout << "Error:unable to join," << rc << std::endl;
      exit(-1);
  }
  std::cout << "Job 1 completed with status :" << status << std::endl;

  std::cout << std::endl;
  dt0->Summary();
  dt1->Summary();
  std::cout << std::endl;
  std::cout << "[ALL DONE]" << std::endl;

  pthread_exit(NULL);
  
  //dt0.Print();
  //dt1.Print();
  //dt0->Capture(100);
  //dt1->Capture(100);

  return 0;
}
