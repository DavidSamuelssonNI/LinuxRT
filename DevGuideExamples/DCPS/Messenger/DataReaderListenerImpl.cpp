#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl.h"

#include "MessengerTypeSupportC.h"
#include "MessengerTypeSupportImpl.h"

#include "ApaModuleTypeSupportC.h"
#include "ApaModuleTypeSupportImpl.h"

#include <iostream>

void DataReaderListenerImpl::on_requested_deadline_missed(DDS::DataReader_ptr /*reader*/, const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void DataReaderListenerImpl::on_requested_incompatible_qos(DDS::DataReader_ptr /*reader*/, const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void DataReaderListenerImpl::on_sample_rejected(DDS::DataReader_ptr /*reader*/, const DDS::SampleRejectedStatus& /*status*/)
{
}

void DataReaderListenerImpl::on_liveliness_changed(DDS::DataReader_ptr /*reader*/, const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
        Messenger::MessageDataReader_var message_reader =
      Messenger::MessageDataReader::_narrow(reader);

    if (!message_reader) {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                 ACE_TEXT(" _narrow failed!\n")));
      ACE_OS::exit(1);
    }

    Messenger::Message message;
    DDS::SampleInfo info;

    const DDS::ReturnCode_t error = message_reader->take_next_sample(message,info);

    if (error == DDS::RETCODE_OK) {
      std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
      std::cout << "SampleInfo.instance_state = " << OpenDDS::DCPS::InstanceState::instance_state_mask_string(info.instance_state) << std::endl;

      if (info.valid_data) {
        std::cout << "Message: subject    = " << message.subject.in() << std::endl
                  << "         subject_id = " << message.subject_id   << std::endl
                  << "         from       = " << message.from.in()    << std::endl
                  << "         count      = " << message.count        << std::endl
                  << "         text       = " << message.text.in()    << std::endl;
      }
    } else {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                 ACE_TEXT(" take_next_sample failed!\n")));
    }
    ApaModule::ApaStructDataReader_var apaStruct_reader =
      ApaModule::ApaStructDataReader::_narrow(reader);

    if (!apaStruct_reader) {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                 ACE_TEXT(" _narrow failed!\n")));
      ACE_OS::exit(1);
    }

    ApaModule::ApaStruct apaStruct;
    DDS::SampleInfo info;

    const DDS::ReturnCode_t error = apaStruct_reader->take_next_sample(apaStruct,info);

    if (error == DDS::RETCODE_OK) {
      std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
      std::cout << "SampleInfo.instance_state = " << OpenDDS::DCPS::InstanceState::instance_state_mask_string(info.instance_state) << std::endl;

      if (info.valid_data) {
        std::cout << "Message: subject    = " << apaStruct.subject.in() << std::endl
                  << "         subject_id = " << apaStruct.subject_id   << std::endl
                  << "         from       = " << apaStruct.from.in()    << std::endl
                  << "         count      = " << apaStruct.count        << std::endl
                  << "         text       = " << apaStruct.text.in()    << std::endl;
      }
    } else {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                 ACE_TEXT(" take_next_sample failed!\n")));
    }
}

void DataReaderListenerImpl::on_subscription_matched(DDS::DataReader_ptr /*reader*/, const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void DataReaderListenerImpl::on_sample_lost(DDS::DataReader_ptr /*reader*/, const DDS::SampleLostStatus & /*status*/)
{
}

