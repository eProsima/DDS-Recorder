# Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

########################
# McapFileCreationTest #
########################

set(TEST_NAME McapFileCreationTest)

set(TEST_SOURCES
        McapFileCreationTest.cpp
        ${PROJECT_SOURCE_DIR}/test/blackbox/mcap/types/hello_world/HelloWorld.cxx
        ${PROJECT_SOURCE_DIR}/test/blackbox/mcap/types/hello_world/HelloWorldPubSubTypes.cxx
        ${PROJECT_SOURCE_DIR}/test/blackbox/mcap/types/hello_world/HelloWorldTypeObject.cxx
    )

set(TEST_LIST
        mcap_data_msgs
        mcap_data_topic
        mcap_data_num_msgs
        mcap_data_num_msgs_downsampling
        transition_paused_running
        transition_running_paused
        transition_paused_stopped
        transition_stopped_paused
        transition_running_stopped
        transition_stopped_running
        transition_running
        transition_paused
        transition_stopped
        transition_paused_event_less_window
        transition_paused_event_max_window
    )

set(TEST_NEEDED_SOURCES
    )

set(TEST_EXTRA_HEADERS
    ${PROJECT_SOURCE_DIR}/test/blackbox/mcap/types/hello_world
    ${PROJECT_SOURCE_DIR}/test/blackbox/mcap/types)

add_blackbox_executable(
    "${TEST_NAME}"
    "${TEST_SOURCES}"
    "${TEST_LIST}"
    "${TEST_NEEDED_SOURCES}"
    "${TEST_EXTRA_HEADERS}"
)
