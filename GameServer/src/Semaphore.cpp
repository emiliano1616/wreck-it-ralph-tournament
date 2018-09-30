#include "Semaphore.h"
#include <iostream>

Semaphore::Semaphore(string nombre, bool create, int valorInicial)
{
	this->semName = nombre;
	if (create)
		this->semaphore = sem_open(semName.c_str(), O_CREAT, 0644, valorInicial);
	else
		this->semaphore = sem_open(semName.c_str(), O_CREAT | O_EXCL, 0644, valorInicial);
}

int Semaphore::Lock()
{
	return sem_wait(this->semaphore);
}

int Semaphore::UnLock()
{
	return sem_post(this->semaphore);
}

int Semaphore::TryLock()
{
	return sem_trywait(this->semaphore);
}

Semaphore::~Semaphore()
{
	cout<<"llamando al destructor de semaphore"<<endl;
	//sem_close(this->semaphore);
	sem_close(this->semaphore);
	sem_unlink(this->semName.c_str());
	cout<<"despues de destroy"<<endl;
}