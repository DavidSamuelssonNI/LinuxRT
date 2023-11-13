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


void wait_for_event(int semid,int semaphore_nr);
void send_ready_event(int semid,int semaphore_nr);

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
    int semid = semget(sem_key, 4, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    //Set to known init state
    unsigned short sem_values[4] = {0, 0, 0, 0};
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 0;
    sb.sem_flg = 0;
    semctl(semid, 4, SETALL, sem_values);


    //for(int i = 0; i<10;i++){
      while(true){
        std::cout << "Send signal Ready"<<std::endl;
        //signal ready
        send_ready_event(semid,0);


        std::cout << "wait for memory"<<std::endl;
        // Wait for Program 1 to acknowledge
        wait_for_event(semid,1);

        std::cout << "finsihed waiting: "  <<std::endl;
            // Cast shareMem to Messenger::Message* since it's a struct
            Messenger::Message* receivedMessage = reinterpret_cast<Messenger::Message*>(shareMem);
            std::cout << "Shared mem value: " << receivedMessage->count << std::endl;
            int a = receivedMessage->count;
            if (libraryHandle) {
                PostLVUserEvent_T PostLVUserEvent = (PostLVUserEvent_T)dlsym(libraryHandle, "PostLVUserEvent");
                if(PostLVUserEvent){
                    std::cout << "Possible to reach PostLVUserEvent";
                    if (PostLVUserEvent) {
                        std::cout << "does this happen"<<std::endl;
                        int result = PostLVUserEvent(*ref, (void*)&a);
                        if(a==9){
                            break;
                        }
                    }
                }
            }
    
        send_ready_event(semid,2);
        //SKICKA TILLBAKA RESPONSE ATT DEN ÄR SATT INNAN NÄSTA LÄSES?
        std::cout<<"Slut"<<std::endl;
    }
    //Detach shared memory
    shmdt(shareMem);
    return 0;
}

void 
wait_for_event(int semid, int semaphore_nr)
{
    struct sembuf sb;
    sb.sem_num = semaphore_nr;
    sb.sem_op = -1; // Initialize semaphore value to 0
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);
}

void 
send_ready_event(int semid, int semaphore_nr)
{
    struct sembuf sb;
    sb.sem_num = semaphore_nr;
    sb.sem_op = 1; // Initialize semaphore value to 1
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);
}

// int data_available() {
//    key_t sem_key = 5679;
//     std::cout << "HÄNDER";
//     // Create or open a semaphore
//     int semid = semget(sem_key, 1, IPC_CREAT | 0666); // Use the correct number of semaphores (3)
//     if (semid == -1) {
//         perror("semget");
//         return 0; // Return 0 on failure
//     }

//     // Attempt to decrement the semaphore value for semaphore number 2
//     std::cout << "wait for 2"<< std::endl;
//     struct sembuf sb;
//     sb.sem_num = 0; // Use the correct semaphore number (2)
//     sb.sem_op = -1; // Decrement the semaphore value by 1
//     sb.sem_flg = IPC_NOWAIT; // Add IPC_NOWAIT flag

//     if (semop(semid, &sb, 1) == -1) {
//         // If semaphore is not available, return 0
//         if (errno == EAGAIN)
//             return 0;
//         perror("semop");
//         return 0; // Return 0 on other errors
//     }

//     return 1; // Return 1 if the semaphore operation is successful (a signal was available)
// }