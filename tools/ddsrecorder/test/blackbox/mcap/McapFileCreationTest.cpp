// Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define MCAP_IMPLEMENTATION

#include <cpp_utils/testing/gtest_aux.hpp>
#include <gtest/gtest.h>

#include <ddspipe_core/core/DdsPipe.hpp>
#include <ddspipe_core/efficiency/payload/FastPayloadPool.hpp>

#include <ddspipe_participants/participant/dynamic_types/DynTypesParticipant.hpp>
#include <ddspipe_participants/participant/dynamic_types/SchemaParticipant.hpp>

#include <ddsrecorder_participants/mcap/McapHandler.hpp>
#include <ddsrecorder_yaml/YamlReaderConfiguration.hpp>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/types/DynamicDataPtr.h>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/TypeObjectFactory.h>

#include <mcap/reader.hpp>

#include "command_receiver/CommandReceiver.hpp"

#include "types/hello_world/HelloWorldTypeObject.h"

// #include <google/protobuf/descriptor.pb.h>
// #include <google/protobuf/descriptor_database.h>
// #include <google/protobuf/dynamic_message.h>

using namespace eprosima::fastdds::dds;
// using namespace eprosima::fastdds::rtps;
// using namespace eprosima::fastrtps::rtps;
// using namespace eprosima::fastrtps;
// using namespace eprosima::fastrtps::types;

// using namespace eprosima;
using namespace eprosima::ddspipe;
using namespace eprosima::ddsrecorder;
// using namespace eprosima::ddspipe::participants;
// using namespace eprosima::ddsrecorder::participants;

// #include <nlohmann/json.hpp>

enum class DataTypeKind
{
    HELLO_WORLD,
};

namespace test {
unsigned int domain = 100;

std::string topic_pub = "TypeIntrospectionTopic";
std::string topic_pub_name = "HelloWorld_TypeIntrospectionExample";

unsigned int n_msgs = 11;
std::string send_message = "Hello World";
unsigned int index = 6;

// Publisher
eprosima::fastdds::dds::DataWriter* writer_;
eprosima::fastrtps::types::DynamicType_ptr dynamic_type_;

} // test

void create_recorder()
{
    eprosima::ddsrecorder::yaml::Configuration configuration("/home/irenebm/annapurna/DDS-Recorder/src/ddsrecorder/conf-recorder.yaml");

    // Create allowed topics list
    auto allowed_topics = std::make_shared<core::AllowedTopicList>(
        configuration.allowlist,
        configuration.blocklist);
    // Create Discovery Database
    std::shared_ptr<core::DiscoveryDatabase> discovery_database =
        std::make_shared<core::DiscoveryDatabase>();
    // Create Payload Pool
    std::shared_ptr<core::PayloadPool> payload_pool =
        std::make_shared<core::FastPayloadPool>();
    // Create Thread Pool
    std::shared_ptr<eprosima::utils::SlotThreadPool> thread_pool =
        std::make_shared<eprosima::utils::SlotThreadPool>(configuration.n_threads);

    // Create MCAP Handler
    // std::string file_name = "/home/irenebm/annapurna/DDS-Recorder/my_output.mcap";
    std::string file_name = configuration.recorder_output_file + "_" + eprosima::utils::timestamp_to_string(eprosima::utils::now()) + ".mcap";
    std::shared_ptr<eprosima::ddsrecorder::participants::McapHandler> mcap_handler =
    std::make_shared<eprosima::ddsrecorder::participants::McapHandler>(
        file_name.c_str(),
        payload_pool,
        configuration.max_pending_samples,
        configuration.buffer_size,
        configuration.downsampling,
        configuration.event_window);

    // Create DynTypes Participant
    auto dyn_participant = std::make_shared<eprosima::ddspipe::participants::DynTypesParticipant>(
        configuration.simple_configuration,
        payload_pool,
        discovery_database);
    dyn_participant->init();

    // Create Recorder Participant
    auto recorder_participant = std::make_shared<eprosima::ddspipe::participants::SchemaParticipant>(
        configuration.recorder_configuration,
        payload_pool,
        discovery_database,
        mcap_handler);

    // Create and populate Participant Database
    std::shared_ptr<core::ParticipantsDatabase> participant_database =
        std::make_shared<core::ParticipantsDatabase>();

    // Populate Participant Database
    participant_database->add_participant(
        dyn_participant->id(),
        dyn_participant
    );
    participant_database->add_participant(
        recorder_participant->id(),
        recorder_participant
    );

    std::make_unique<core::DdsPipe>(
        allowed_topics,
        discovery_database,
        payload_pool,
        participant_database,
        thread_pool,
        configuration.builtin_topics,
        true
    );
}

void create_publisher(
        std::string topic_name,
        uint32_t domain,
        DataTypeKind data_type_kind)
{
    eprosima::fastdds::dds::DomainParticipantQos pqos;
    pqos.name("TypeIntrospectionExample_Participant_Publisher");
    pqos.wire_protocol().builtin.typelookup_config.use_client = false;
    pqos.wire_protocol().builtin.typelookup_config.use_server = true;

    // Create the Participant
    eprosima::fastdds::dds::DomainParticipant* participant_ = DomainParticipantFactory::get_instance()->create_participant(domain, pqos);

    // Register the type
    registerHelloWorldTypes();
    std::string data_type_name_ = "HelloWorld";
    test::dynamic_type_ = eprosima::fastrtps::types::TypeObjectFactory::get_instance()->build_dynamic_type(
            data_type_name_,
            GetHelloWorldIdentifier(true),
            GetHelloWorldObject(true));

    TypeSupport type(new eprosima::fastrtps::types::DynamicPubSubType(test::dynamic_type_));
    // Set type so introspection info is sent
    type->auto_fill_type_information(true);
    type->auto_fill_type_object(false);
    // Register the type in the Participant
    participant_->register_type(type);

    // Create the Publisher
    eprosima::fastdds::dds::Publisher* publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

    // Create the DDS Topic
    eprosima::fastdds::dds::Topic* topic_ = participant_->create_topic(topic_name, data_type_name_, TOPIC_QOS_DEFAULT);

    // Create the DDS DataWriter
    test::writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, nullptr);
}

void send_sample(uint32_t index) {
    // Create and initialize new dynamic data
    eprosima::fastrtps::types::DynamicData_ptr dynamic_data_;
    dynamic_data_ = eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(test::dynamic_type_);

    // Set index
    dynamic_data_->set_uint32_value(index, 0);
    // Set message
    dynamic_data_->set_string_value(test::send_message, 1);
    test::writer_->write(dynamic_data_.get());

    usleep(1000000);   // microsecond intervals - dont change

    logUser(DDSRECORDER_EXECUTION, "Message published.");
}

TEST(McapFileCreationTest, mcap_data_msgs)
{
    // DDS Recorder Initialization
    logUser(DDSRECORDER_EXECUTION, "Starting DDS Recorder execution.");

    // Create Recorder
    create_recorder();

    // Create Publisher
    create_publisher(
        test::topic_pub,
        test::domain,
        DataTypeKind::HELLO_WORLD);

    // Send data
    send_sample(static_cast<uint32_t>(test::index));

    // Read MCAP file
    mcap::McapReader mcap_reader_;
    auto status = mcap_reader_.open("/home/irenebm/annapurna/DDS-Recorder/my_output_2023-03-01_10-47-01.mcap");
    if (!status.ok()) {
        std::cerr << "! " << status.message << "\n";
        return;
    }

    auto messageView = mcap_reader_.readMessages();
    std::string received_message;
    for (auto it = messageView.begin(); it != messageView.end(); it++) {
        std::string data(reinterpret_cast<char const*>(it->message.data),  it->message.dataSize-1);
        received_message = data.substr(it->message.dataSize/2);
    }
    mcap_reader_.close();

    // Test data
    ASSERT_EQ(received_message, test::send_message);

    logUser(DDSRECORDER_EXECUTION, "Finishing DDS Recorder execution correctly.");
}

TEST(McapFileCreationTest, mcap_data_topic)
{
    // DDS Recorder Initialization
    logUser(DDSRECORDER_EXECUTION, "Starting DDS Recorder execution.");

    create_recorder();

    // Create Publisher
    create_publisher(
        test::topic_pub,
        test::domain,
        DataTypeKind::HELLO_WORLD);

    // Send data
    send_sample(static_cast<uint32_t>(test::index));

    // Read MCAP file
    //! MCAP reader
    mcap::McapReader mcap_reader_;
    auto status = mcap_reader_.open("/home/irenebm/annapurna/DDS-Recorder/my_output_2023-03-01_10-47-01.mcap");

    if (!status.ok()) {
        std::cerr << "! " << status.message << "\n";
        return;
    }

    auto messages = mcap_reader_.readMessages();

    std::string received_topic;
    std::string received_topic_name;

    for (auto it = messages.begin(); it != messages.end(); it++) {
        received_topic = it->channel->topic;
        received_topic_name = it->schema->name;
    }
    mcap_reader_.close();

    // Test data
    ASSERT_EQ(received_topic, test::topic_pub);
    ASSERT_EQ(received_topic_name, test::topic_pub_name);

    logUser(DDSRECORDER_EXECUTION, "Finishing DDS Recorder execution correctly.");
}

TEST(McapFileCreationTest, mcap_data_num_msgs)
{
    // DDS Recorder Initialization
    logUser(DDSRECORDER_EXECUTION, "Starting DDS Recorder execution.");

    create_recorder();

    // Create Publisher
    create_publisher(
        test::topic_pub,
        test::domain,
        DataTypeKind::HELLO_WORLD);

    // Send data
    for(int i = 0; i < test::n_msgs; i++) {
        send_sample(static_cast<uint32_t>(test::index));
    }

    // Read MCAP file
    //! MCAP reader
    mcap::McapReader mcap_reader_;
    auto status = mcap_reader_.open("/home/irenebm/annapurna/DDS-Recorder/my_output_2023-03-01_10-47-01.mcap");

    if (!status.ok()) {
        std::cerr << "! " << status.message << "\n";
        return;
    }

    auto messages = mcap_reader_.readMessages();

    unsigned int n_received_msgs = 0;
    for (auto it = messages.begin(); it != messages.end(); it++) {
        n_received_msgs++;
    }
    mcap_reader_.close();

    // Test data
    ASSERT_EQ(test::n_msgs, n_received_msgs);

    logUser(DDSRECORDER_EXECUTION, "Finishing DDS Recorder execution correctly.");
}


int main(
        int argc,
        char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
