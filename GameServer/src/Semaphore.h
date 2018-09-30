#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <fcntl.h>
#include <semaphore.h>
#include <string>


using namespace std;

class Semaphore
{
	private:
		sem_t *semaphore;
		string semName;
	public:
		//Constructor Nombre semaforo create(true) or open(false), valor
		Semaphore(string , bool , int );
		int Lock();
		int UnLock();
		int TryLock();
		~Semaphore();
};

#endif