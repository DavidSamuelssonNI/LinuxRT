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
