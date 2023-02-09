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

/**
 * @file YamlReaderConfiguration.cpp
 *
 */

#include <ddsrouter_core/participants/participant/configuration/SimpleParticipantConfiguration.hpp>
#include <ddsrouter_core/participants/participant/configuration/ParticipantConfiguration.hpp>
#include <ddsrouter_core/types/topic/dds/DdsTopic.hpp>
#include <ddsrouter_core/types/topic/filter/DdsFilterTopic.hpp>
#include <ddsrouter_core/types/topic/filter/WildcardDdsFilterTopic.hpp>

#include <ddsrouter_yaml/yaml_configuration_tags.hpp>
#include <ddsrouter_yaml/Yaml.hpp>
#include <ddsrouter_yaml/YamlManager.hpp>
#include <ddsrouter_yaml/YamlReader.hpp>

#include <foxgloveweb_yaml/yaml_configuration_tags.hpp>

#include <foxgloveweb_yaml/YamlReaderConfiguration.hpp>

namespace eprosima {
namespace foxgloveweb {
namespace yaml {

using namespace eprosima::ddsrouter::core;
using namespace eprosima::ddsrouter::participants;
using namespace eprosima::ddsrouter::yaml;

Configuration::Configuration(const Yaml& yml)
{
    load_ddsrouter_configuration_(yml);
}

Configuration::Configuration(const std::string& file_path)
{
    load_ddsrouter_configuration_from_file_(file_path);
}

void Configuration::load_ddsrouter_configuration_(
        const Yaml& yml)
{
    try
    {
        YamlReaderVersion version = LATEST;

        /////
        // Get optional allowlist
        if (YamlReader::is_tag_present(yml, ALLOWLIST_TAG))
        {
            configuration.allowlist = utils::convert_set_to_shared<types::DdsFilterTopic, types::WildcardDdsFilterTopic>(
                YamlReader::get_set<types::WildcardDdsFilterTopic>(yml, ALLOWLIST_TAG, version));

            // Add to allowlist always the type object topic
            configuration.allowlist.insert(
                std::make_shared<types::WildcardDdsFilterTopic>("*__internal__*"));
        }

        /////
        // Get optional blocklist
        if (YamlReader::is_tag_present(yml, BLOCKLIST_TAG))
        {
            configuration.blocklist = utils::convert_set_to_shared<types::DdsFilterTopic, types::WildcardDdsFilterTopic>(
                YamlReader::get_set<types::WildcardDdsFilterTopic>(yml, BLOCKLIST_TAG, version));
        }

        /////
        // Create Simple Participant Configuration
        simple_configuration = std::make_shared<SimpleParticipantConfiguration>(
                "SimpleFoxgloveParticipant",
                false,
                0u);

        // Set the domain in Simple Participant Configuration
        if (YamlReader::is_tag_present(yml, DOMAIN_ID_TAG))
        {
            simple_configuration->domain = YamlReader::get<types::DomainId>(yml, DOMAIN_ID_TAG, version);
        }

        /////
        // Create Foxglove Websocket Participant Configuration
        foxglove_ws_configuration = std::make_shared<ParticipantConfiguration>(
                "FoxgloveWebsocketParticipant",
                false);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error loading DDS Router configuration from yaml:\n " << e.what());
    }
}

void Configuration::load_ddsrouter_configuration_from_file_(
        const std::string& file_path)
{
    Yaml yml;

    // Load file
    try
    {
        yml = YamlManager::load_file(file_path);
    }
    catch (const std::exception& e)
    {
        throw eprosima::utils::ConfigurationException(
                  utils::Formatter() << "Error loading DDSRouter configuration from file: <" << file_path <<
                      "> :\n " << e.what());
    }

    Configuration::load_ddsrouter_configuration_(yml);
}

} /* namespace yaml */
} /* namespace foxgloveweb */
} /* namespace eprosima */
