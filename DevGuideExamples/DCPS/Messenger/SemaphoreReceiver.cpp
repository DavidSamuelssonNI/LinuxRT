#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <dlfcn.h>

#include "MessengerTypeSupportC.h"
#include "MessengerTypeSupportImpl.h"

int main()
{

    key_t shm_key = 1234;
    key_t sem_key = 5678;

    // Create or open shared memory
    int shmid = shmget(shm_key, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    // Attach shared memory to the process
    Messenger::Message* shareMem = (Messenger::Message*)shmat(shmid, NULL, 0);
    // Create or open a semaphore
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    std::cout << "Receiver: Waiting for the semaphore signal..." << std::endl;

    // Decrement the semaphore value (wait for signal)
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1; // Decrement the semaphore value by 1
    sb.sem_flg = 0;
    semop(semid, &sb, 1);

    std::cout << "Receiver: Semaphore signal received. Proceeding." << std::endl;
    Messenger::Message a;
    a.count = 5;
     memcpy(shareMem, &a, sizeof(Messenger::Message));
    // You can perform your desired actions here after receiving the signal

    std::cout << "Send back memory Ready"<<std::endl;
    //signal ready
    sb.sem_num = 1;
    sb.sem_op = 1; // Initialize semaphore value to 1
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);

    //Detach shared memory
    shmdt(shareMem);
    return 0;
}


