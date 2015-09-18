#pragma once

#include <stdio.h>

class Master
{
public:
  Master(int num_slaves, int num_jobs, int job_size);
  ~Master();

  void printJobs();
  void mainLoop();

private:
  int** jobs;
  int* slaves;
  int num_jobs;
  int job_size;
  int next_job;
  int jobs_done;
  int num_slaves;
  int dead_slaves;

  bool hasWaitingJobs()
  {
    return next_job < num_jobs;
  }

  bool hasWorkingJobs()
  {
    return jobs_done < num_jobs;
  }
  bool hasSlavesAlive()
  {
    return dead_slaves < num_slaves;
  }
};