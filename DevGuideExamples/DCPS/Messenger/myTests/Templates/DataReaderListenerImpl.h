/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#ifndef DATAREADER_LISTENER_IMPL_H
#define DATAREADER_LISTENER_IMPL_H
//Changed from cpp to here to declare trigger_lv_event
#include "MessengerTypeSupportC.h"
#include "MessengerTypeSupportImpl.h"

#include <thread>
#include <condition_variable>
#include <chrono>
#include <dlfcn.h>
// For LV ref shared mem
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include <map>
#include <memory>
#include <mutex>
#include <cstring>
#include <memory>
#include <stdio.h>

#include <ace/Global_Macros.h>

#include <dds/DdsDcpsSubscriptionC.h>
#include <dds/DCPS/LocalObject.h>
#include <dds/DCPS/Definitions.h>


typedef int32_t MagicCookie;
typedef MagicCookie LVUserEventRef;
typedef int (*PostLVUserEvent_T)(LVUserEventRef ref, void* data);

class DataReaderListenerImpl
  : public virtual OpenDDS::DCPS::LocalObject<DDS::DataReaderListener> {
public:
  // std::condition_variable cv;
  // std::mutex cvMutex;
  //LVUserEventRef ref_from_LV;
  //int latestMessageCount;
  Messenger::Message message_;
  // Constructor that accepts *ref as an argument
  DataReaderListenerImpl();
  void share_message(Messenger::Message &message_to_transmit);
  void wait_for_event(int semid,int semaphore_nr);
  void send_ready_event(int semid,int semaphore_nr);
  void notify_no_more_data();
  // int initializeMember();
  // Sets the ref that was sent from LV, to be able to trigger events to LV 
  // LVUserEventRef set_LV_ref();
  // void shareMessage();
  //void setMessage(Messenger::Message &message);
  // void trigger_LV_event(Messenger::Message &message);
  // void clearAndRemoveSharedMemory(key_t shm_key);

  virtual void on_requested_deadline_missed(
    DDS::DataReader_ptr reader,
    const DDS::RequestedDeadlineMissedStatus& status);

  virtual void on_requested_incompatible_qos(
    DDS::DataReader_ptr reader,
    const DDS::RequestedIncompatibleQosStatus& status);

  virtual void on_sample_rejected(
    DDS::DataReader_ptr reader,
    const DDS::SampleRejectedStatus& status);

  virtual void on_liveliness_changed(
    DDS::DataReader_ptr reader,
    const DDS::LivelinessChangedStatus& status);

  virtual void on_data_available(
    DDS::DataReader_ptr reader);

  virtual void on_subscription_matched(
    DDS::DataReader_ptr reader,
    const DDS::SubscriptionMatchedStatus& status);

  virtual void on_sample_lost(
    DDS::DataReader_ptr reader,
    const DDS::SampleLostStatus& status);
};

#endif /* DATAREADER_LISTENER_IMPL_H */
