#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <dlfcn.h>
#include <map>
#include <memory>
#include <mutex>
#include <cstring>
#include <memory>
#include <stdio.h>
#include <fstream>


#include "MessengerTypeSupportC.h"
#include "MessengerTypeSupportImpl.h"

typedef int32_t MagicCookie;
typedef MagicCookie LVUserEventRef;
typedef int (*PostLVUserEvent_T)(LVUserEventRef ref, void* data);

// Send LV ref value to shared memory
extern "C" int32_t pass_LV_ref_to_shared_mem(LVUserEventRef *ref){
    std::cout << "my so"<<std::endl;
    //Read LabVIEW lib 
    void* libraryHandle = dlopen("/usr/local/natinst/labview/liblvrt.so", RTLD_LAZY);
    
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

    std::cout << "Send signal Ready"<<std::endl;
    //signal ready
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1; // Initialize semaphore value to 1
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);


    std::cout << "wait for memory"<<std::endl;
    // Wait for Program 1 to acknowledge
    sb.sem_num = 1;
    sb.sem_op = -1; // Initialize semaphore value to 0
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);

    std::cout << "finsihed waiting"<<std::endl;
    // Cast shareMem to Messenger::Message* since it's a struct
    Messenger::Message* receivedMessage = reinterpret_cast<Messenger::Message*>(shareMem);
    std::cout << "Shared mem value: " << receivedMessage->count << std::endl;
    int a = receivedMessage->count;
    if (libraryHandle) {
        PostLVUserEvent_T PostLVUserEvent = (PostLVUserEvent_T)dlsym(libraryHandle, "PostLVUserEvent");
        if(PostLVUserEvent){
            std::cout << "Possible to reach PostLVUserEvent";
            if (PostLVUserEvent) {
                int result = PostLVUserEvent(*ref, (void*)&a);
            }
        }
    }
    //Detach shared memory
    shmdt(shareMem);
        return 0;
    
}