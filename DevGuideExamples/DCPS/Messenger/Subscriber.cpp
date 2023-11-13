/*
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */
/*
#include <thread>
#include <condition_variable>
#include <chrono>

// ... Other code ...

// Declare a condition variable and mutex
std::condition_variable cv;
std::mutex cvMutex;

// Start a separate thread for periodic sending
std::thread senderThread([&] {
    while (true) {
        // Replace the following sleep with your desired interval mechanism
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second (adjust as needed)

        // Check if a valid last available sample exists
        if (lastAvailableMessage.text.in() != nullptr) {
            // Send the last available sample using PostLVUserEvent
            PostLVUserEvent(*ref, (void*)lastAvailableMessage.text.in());
        }

        // Notify the condition variable to wake up the main thread
        cv.notify_one();
    }
});

// ... Other code ...

// Inside your on_data_available method
void DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader) {
    // ... Existing code to process data ...

    // Notify the condition variable to wake up the senderThread
    cv.notify_one();
}

// ... Other code ...

// Join the senderThread when the program exits
senderThread.join();
*/


#include "DataReaderListenerImpl.h"
#include "MessengerTypeSupportImpl.h"

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/StaticIncludes.h>
#if OPENDDS_DO_MANUAL_STATIC_INCLUDES
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsSubscriptionC.h>

#include <ace/Log_Msg.h>

//int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
int ACE_TMAIN()
{
  try {
    //std::cout << "argc " << argc <<" hej "<<  argv[0] <<" hej "<< argv[1]<<" hej"<< argv[2] << std::endl;
    int argc = 3;
    char* argv[] = { (char*)"./subscriber", (char*)"-DCPSConfigFile", (char*)"./rtps.ini" };
    // Initialize DomainParticipantFactory
    DDS::DomainParticipantFactory_var dpf =
      TheParticipantFactoryWithArgs(argc, argv);

    // Create DomainParticipant
    DDS::DomainParticipant_var participant =
      dpf->create_participant(42,
                              PARTICIPANT_QOS_DEFAULT,
                              0,
                              OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!participant) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_participant failed!\n")),
                       1);
    }

    // Register Type (Messenger::Message)
    Messenger::MessageTypeSupport_var ts =
      new Messenger::MessageTypeSupportImpl;

    if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" register_type failed!\n")),
                       1);
    }

    // Create Topic (Movie Discussion List)
    CORBA::String_var type_name = ts->get_type_name();
    DDS::Topic_var topic =
      participant->create_topic("Movie Discussion List",
                                type_name,
                                TOPIC_QOS_DEFAULT,
                                0,
                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!topic) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_topic failed!\n")),
                       1);
    }

    // Create Subscriber
    DDS::Subscriber_var subscriber =
      participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                     0,
                                     OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!subscriber) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_subscriber failed!\n")),
                       1);
    }

    // Create DataReader
    DDS::DataReaderListener_var listener(new DataReaderListenerImpl());

    DDS::DataReaderQos reader_qos;
    subscriber->get_default_datareader_qos(reader_qos);
    reader_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

    DDS::DataReader_var reader =
      subscriber->create_datareader(topic,
                                    reader_qos,
                                    listener,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!reader) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_datareader failed!\n")),
                       1);
    }

    Messenger::MessageDataReader_var reader_i =
      Messenger::MessageDataReader::_narrow(reader);

    if (!reader_i) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" _narrow failed!\n")),
                       1);
    }

    // Block until Publisher completes
    DDS::StatusCondition_var condition = reader->get_statuscondition();
    condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);

    DDS::WaitSet_var ws = new DDS::WaitSet;
    ws->attach_condition(condition);

    while (true) {
      DDS::SubscriptionMatchedStatus matches;
      if (reader->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" get_subscription_matched_status failed!\n")),
                         1);
      }

      if (matches.current_count == 0 && matches.total_count > 0) {
        break;
      }

      DDS::ConditionSeq conditions;
      DDS::Duration_t timeout = { 60, 0 };
      if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" wait failed!\n")),
                         1);
      }
    }

    ws->detach_condition(condition);

    // Clean-up!
    participant->delete_contained_entities();
    dpf->delete_participant(participant);

    TheServiceParticipant->shutdown();

  } catch (const CORBA::Exception& e) {
    e._tao_print_exception("Exception caught in main():");
    return 1;
  }

  return 0;
}
