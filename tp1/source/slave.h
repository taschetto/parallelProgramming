#pragma once

#include <stdio.h>

class Slave
{
public:
  Slave(int job_size);
  ~Slave();
  void mainLoop();

private:
  int job_size; // tamanho de cada job
};