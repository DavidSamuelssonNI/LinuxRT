/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl.h"
// #include "MessengerTypeSupportC.h"
// #include "MessengerTypeSupportImpl.h"

#include <iostream>

// void
// DataReaderListenerImpl::SenderThreadFunction() {
//     void* libraryHandle = dlopen("/usr/local/natinst/labview/liblvrt.so", RTLD_LAZY);
//     PostLVUserEvent_T PostLVUserEvent = (PostLVUserEvent_T)dlsym(libraryHandle, "PostLVUserEvent");
//     if (libraryHandle) {
//         while (true) {
//             // Replace the following sleep with your desired interval mechanism
//             std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second (adjust as needed)

//             // Check if a valid last available sample exists
//             // if (lastAvailableMessage.text.in() != nullptr) {
//                 // Send the last available sample using PostLVUserEvent
//                 PostLVUserEvent(*ref, (void*)&latestMessageCount);
//             // }

//             // Notify the condition variable to wake up the main thread
//             cv.notify_one();
//         }
//     }
// }

void
DataReaderListenerImpl::on_requested_deadline_missed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_requested_incompatible_qos(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_rejected(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleRejectedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_liveliness_changed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::LivelinessChangedStatus& /*status*/)
{
}

DataReaderListenerImpl::DataReaderListenerImpl(){
  key_t shm_key = 1234;  // Use the same specific key as in Program 2
  key_t sem_key = 5678;
  //clearAndRemoveSharedMemory(shm_key);
  //ref_from_LV = set_LV_ref();
  //std::cout << "SET LV REF" << ref_from_LV;

}

// void DataReaderListenerImpl::clearAndRemoveSharedMemory(key_t shm_key) {
//     // Create or open shared memory
//     int shmid = shmget(shm_key, sizeof(unsigned int), IPC_CREAT | 0666);
//     if (shmid == -1) {
//         perror("shmget");
//         std::cerr << "shmid error" << std::endl;
//         return;
//     }

//     // Attach shared memory to process
//     unsigned int* ref = (unsigned int*)shmat(shmid, NULL, 0);

//     // Clear the existing data in shared memory
//     std::memset(ref, 0, sizeof(unsigned int));

//     // Detach shared memory
//     shmdt(ref);

//     // Check if other processes are attached
//     struct shmid_ds shmid_ds_info;
//     if (shmctl(shmid, IPC_STAT, &shmid_ds_info) == -1) {
//         perror("shmctl");
//         std::cerr << "Failed to get shared memory information" << std::endl;
//         return;
//     }

//     if (shmid_ds_info.shm_nattch == 0) {
//         // No other processes are attached, so it's safe to remove the shared memory
//         if (shmctl(shmid, IPC_RMID, NULL) == -1) {
//             perror("shmctl");
//             std::cerr << "Failed to remove shared memory" << std::endl;
//             return;
//         }
//     }
// }

// int DataReaderListenerImpl::initializeMember() {
//     // You can call a function here to initialize memberVariable
//     return set_LV_ref();
// }

// Sets the ref that was sent from LV, to be able to trigger events to LV 
//LVUserEventRef
/*
void
DataReaderListenerImpl::set_LV_ref()
{

    key_t shm_key = 1234;  // Use the same specific key as in Program 1
    key_t sem_key = 5678;  // Use the same specific key as in Program 1
    typedef int32_t MagicCookie;
    typedef MagicCookie LVUserEventRef;
    typedef int (*PostLVUserEvent_T)(LVUserEventRef ref, void* data);
    // Create or open shared memory
    int shmid = shmget(shm_key, sizeof(unsigned int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        std::cerr << "shmid error" << std::endl;
        //return 1;
    }

    // Attach shared memory to process
    LVUserEventRef* reff = (LVUserEventRef*)shmat(shmid, NULL, 0);

    // Create or open a semaphore
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        std::cerr << "semid error" << std::endl;
        //return 1;
    }

    // Wait for Program 1 to signal
    std::cout << "Wait for Program 1" << *reff<< std::endl;
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);

    while(true){
    std::cout << *reff<< std::endl;
    if(*reff < 0){
      break;
    }
    
    }
    LVUserEventRef result = *reff;
    
    std::cout << "Signal Program 1" << std::endl;
    sb.sem_op = 1;  // Increment the semaphore value
    semop(semid, &sb, 1);

    // Cleanup and detach shared memory
    shmdt(reff);

    return result;

}
*/

void
DataReaderListenerImpl::notify_no_more_data(){
  key_t sem_key = 5679;
  std::cout << "IS notifu_no_more_data sent?" << std::endl;
    // Create or open a semaphore
  int semid2 = semget(sem_key, 1, IPC_CREAT | 0666);
  if (semid2 == -1) {
      perror("semget");
      //return 1;
  }

     //signal ready
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1; // Initialize semaphore value to 1
    sb.sem_flg = SEM_UNDO;
    semop(semid2, &sb, 1);

}

void 
DataReaderListenerImpl::wait_for_event(int semid, int semaphore_nr)
{
    struct sembuf sb;
    sb.sem_num = semaphore_nr;
    sb.sem_op = -1; // Initialize semaphore value to 0
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);
}

void 
DataReaderListenerImpl::send_ready_event(int semid, int semaphore_nr)
{
    struct sembuf sb;
    sb.sem_num = semaphore_nr;
    sb.sem_op = 1; // Initialize semaphore value to 1
    sb.sem_flg = SEM_UNDO;
    semop(semid, &sb, 1);
}

void
DataReaderListenerImpl::share_message(Messenger::Message &message_to_transmit){

    key_t shm_key = 1234;
    key_t sem_key = 5678;

    // Create or open shared memory
    int shmid = shmget(shm_key, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        //return 1;
    }

    // Attach shared memory to the process
    Messenger::Message* shareMem = (Messenger::Message*)shmat(shmid, NULL, 0);
    
    // Create or open a semaphore
    int semid = semget(sem_key, 4, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        //return 1;
    }

        //Set to known init state
    unsigned short sem_values[4] = {0, 0, 0, 0};
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 0;
    sb.sem_flg = 0;
    semctl(semid, 4, SETALL, sem_values);

    std::cout << "Receiver: Waiting for the semaphore signal..." << std::endl;

    // Decrement the semaphore value (wait for signal)
    wait_for_event(semid, 0);

    std::cout << "Receiver: Semaphore signal received. Proceeding." << std::endl;
    Messenger::Message a;
    a.count = message_to_transmit.count;
     memcpy(shareMem, &a, sizeof(Messenger::Message));
    // You can perform your desired actions here after receiving the signal

    std::cout << "Send back memory Ready"<<std::endl;
    //signal ready
    send_ready_event(semid, 1);

    //Wait for LV to handle the event
    wait_for_event(semid,2);


    //Detach shared memory
    shmdt(shareMem);

}

// void
// DataReaderListenerImpl::setMessage(Messenger::Message &message){
//   DataReaderListenerImpl::message_ = message;
// }

/*
void
DataReaderListenerImpl::trigger_LV_event(Messenger::Message &message){
  std::cout << "Trigger " << ref_from_LV << "message counnt" << message.count<< std::endl;
  
  void* libraryHandle = dlopen("/usr/local/natinst/labview/liblvrt.so", RTLD_LAZY);
  //int apa = message.count;
  if (libraryHandle) {
    std::cout << "libhand: " << std::endl;
    typedef int32_t MagicCookie;
    typedef MagicCookie LVUserEventRef;
    typedef int (*PostLVUserEvent_T)(LVUserEventRef ref, void* data);

    PostLVUserEvent_T PostLVUserEvent = (PostLVUserEvent_T)dlsym(libraryHandle, "PostLVUserEvent");
    
        if (PostLVUserEvent) {
          std::cout << "Postlvuser: " << std::endl;
            // You have successfully obtained the function pointer
            // You can call the function with appropriate arguments
            // int ref = 10;

                
                int i =55 ;
                int result = PostLVUserEvent((unsigned int)ref_from_LV, (void*)&i);
                std::cout << "POSTLV: " << result <<std::endl;
                // i++;
                usleep(1000000);

                // Handle the result as needed
                // return result;

            //return 0;
            } else {
            //return 1;
            // Handle the case where PostLVUserEvent is not found
          }
          dlclose(libraryHandle);
  }
    // dlclose(libraryHandle); //GÖR NÅGOT ÅT
}
*/
void
DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
  // set_LV_ref();
  Messenger::MessageDataReader_var reader_i =
  Messenger::MessageDataReader::_narrow(reader);

  if (!reader_i) {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" _narrow failed!\n")));
    ACE_OS::exit(1);
  }

  Messenger::Message message;
  DDS::SampleInfo info;

  const DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);
  //Ready to produce more data
  // key_t sem_key = 5678;
  // int semid = semget(sem_key, 4, IPC_CREAT | 0666);
  // send_ready_event(semid,3);

  share_message(message);
  if (error == DDS::RETCODE_OK) {
    std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
    std::cout << "SampleInfo.instance_state = " << OpenDDS::DCPS::InstanceState::instance_state_mask_string(info.instance_state) << std::endl;
    
    if (info.valid_data) {
      std::cout << "Message: subject    = " << message.subject.in() << std::endl
                << "         subject_id = " << message.subject_id   << std::endl
                << "         from       = " << message.from.in()    << std::endl
                << "         count      = " << message.count        << std::endl
                << "         text       = " << message.text.in()    << std::endl;
      
     
      // message_ = message;
      //shareMessage();
      //trigger_LV_event(message);
    }
    

  } else {

    ACE_ERROR((LM_ERROR,
               ACE_TEXT("ERROR: %N:%l: on_data_available() -")
               ACE_TEXT(" take_next_sample failed!\n")));
  }
  if(info.valid_data){
    //notify_no_more_data();
    //latestMessageCount = message.count;
  }
  
  // cv.notify_one();
}

void
DataReaderListenerImpl::on_subscription_matched(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_sample_lost(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleLostStatus& /*status*/)
{
}
