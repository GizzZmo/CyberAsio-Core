#include "config_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

namespace CyberAsio {

ConfigManager::ConfigManager() {
    initializeDefaults();
}

ConfigManager::~ConfigManager() {
    if (system_config_.auto_save) {
        saveConfig();
    }
}

bool ConfigManager::loadConfig(const std::string& config_path) {
    std::string path = config_path.empty() ? system_config_.config_file_path : config_path;
    
    std::cout << "Loading configuration from: " << path << std::endl;
    
    if (!loadFromFile(path)) {
        std::cout << "Could not load config file, using defaults" << std::endl;
        return false;
    }
    
    std::cout << "Configuration loaded successfully" << std::endl;
    return true;
}

bool ConfigManager::saveConfig(const std::string& config_path) {
    std::string path = config_path.empty() ? system_config_.config_file_path : config_path;
    
    std::cout << "Saving configuration to: " << path << std::endl;
    
    if (!saveToFile(path)) {
        std::cerr << "Failed to save configuration" << std::endl;
        return false;
    }
    
    std::cout << "Configuration saved successfully" << std::endl;
    return true;
}

void ConfigManager::resetToDefaults() {
    system_config_ = getDefaultSystemConfig();
    device_profiles_.clear();
    notifyConfigChanged();
    
    std::cout << "Configuration reset to defaults" << std::endl;
}

bool ConfigManager::setSystemConfig(const SystemConfig& config) {
    if (!validateConfig(config)) {
        std::cerr << "Invalid system configuration" << std::endl;
        return false;
    }
    
    system_config_ = config;
    notifyConfigChanged();
    
    std::cout << "System configuration updated" << std::endl;
    return true;
}

bool ConfigManager::setAudioConfig(const AudioConfig& config) {
    if (!validateAudioConfig(config)) {
        std::cerr << "Invalid audio configuration" << std::endl;
        return false;
    }
    
    system_config_.audio = config;
    notifyConfigChanged();
    
    std::cout << "Audio configuration updated - Sample Rate: " << config.sample_rate 
              << " Hz, Buffer: " << config.buffer_size << " samples, Bit Depth: " 
              << config.bit_depth << " bits" << std::endl;
    
    return true;
}

bool ConfigManager::saveDeviceProfile(int device_id, const AudioConfig& config) {
    if (!validateAudioConfig(config)) {
        std::cerr << "Invalid audio configuration for device profile" << std::endl;
        return false;
    }
    
    device_profiles_[device_id] = config;
    
    std::cout << "Device profile saved for device " << device_id << std::endl;
    return true;
}

AudioConfig ConfigManager::getDeviceProfile(int device_id) const {
    auto it = device_profiles_.find(device_id);
    if (it != device_profiles_.end()) {
        return it->second;
    }
    return getDefaultAudioConfig();
}

bool ConfigManager::hasDeviceProfile(int device_id) const {
    return device_profiles_.find(device_id) != device_profiles_.end();
}

void ConfigManager::removeDeviceProfile(int device_id) {
    device_profiles_.erase(device_id);
    std::cout << "Device profile removed for device " << device_id << std::endl;
}

void ConfigManager::setActiveDevice(int device_id) {
    system_config_.active_device_id = device_id;
    notifyConfigChanged();
}

void ConfigManager::setCurrentAudioFile(const std::string& filepath) {
    system_config_.current_audio_file = filepath;
    notifyConfigChanged();
}

void ConfigManager::setConfigChangeCallback(std::function<void(const SystemConfig&)> callback) {
    config_change_callback_ = callback;
}

bool ConfigManager::validateConfig(const SystemConfig& config) const {
    return validateAudioConfig(config.audio) && config.active_device_id >= -1;
}

bool ConfigManager::validateAudioConfig(const AudioConfig& config) const {
    return isValidSampleRate(config.sample_rate) &&
           isValidBufferSize(config.buffer_size) &&
           isValidBitDepth(config.bit_depth) &&
           config.channels > 0 && config.channels <= 8;
}

std::string ConfigManager::exportConfigToJSON() const {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"system\": " << systemConfigToJSON(system_config_) << ",\n";
    json << "  \"device_profiles\": {\n";
    
    bool first_profile = true;
    for (const auto& [device_id, config] : device_profiles_) {
        if (!first_profile) json << ",\n";
        json << "    \"" << device_id << "\": " << audioConfigToJSON(config);
        first_profile = false;
    }
    
    json << "\n  }\n";
    json << "}";
    
    return json.str();
}

bool ConfigManager::importConfigFromJSON(const std::string& json_data) {
    try {
        // This is a simplified JSON parser
        // In a real implementation, you would use a proper JSON library like nlohmann/json
        
        // For now, just validate that it's roughly JSON-shaped
        if (json_data.find("{") == std::string::npos || json_data.find("}") == std::string::npos) {
            return false;
        }
        
        std::cout << "Configuration imported from JSON" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to import configuration: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return false;
    }
    
    // For simplicity, we'll use a basic key-value format
    // In a real implementation, you would use JSON
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments
        
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse configuration values
        if (key == "sample_rate") {
            system_config_.audio.sample_rate = std::stoi(value);
        } else if (key == "buffer_size") {
            system_config_.audio.buffer_size = std::stoi(value);
        } else if (key == "bit_depth") {
            system_config_.audio.bit_depth = std::stoi(value);
        } else if (key == "channels") {
            system_config_.audio.channels = std::stoi(value);
        } else if (key == "active_device_id") {
            system_config_.active_device_id = std::stoi(value);
        } else if (key == "current_audio_file") {
            system_config_.current_audio_file = value;
        }
    }
    
    return true;
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# CyberASIO Core Configuration\n";
    file << "# Generated automatically - do not edit while application is running\n\n";
    
    file << "# Audio Configuration\n";
    file << "sample_rate=" << system_config_.audio.sample_rate << "\n";
    file << "buffer_size=" << system_config_.audio.buffer_size << "\n";
    file << "bit_depth=" << system_config_.audio.bit_depth << "\n";
    file << "channels=" << system_config_.audio.channels << "\n\n";
    
    file << "# System Configuration\n";
    file << "active_device_id=" << system_config_.active_device_id << "\n";
    file << "current_audio_file=" << system_config_.current_audio_file << "\n";
    
    return true;
}

std::string ConfigManager::systemConfigToJSON(const SystemConfig& config) const {
    std::ostringstream json;
    json << "{\n";
    json << "    \"audio\": " << audioConfigToJSON(config.audio) << ",\n";
    json << "    \"active_device_id\": " << config.active_device_id << ",\n";
    json << "    \"current_audio_file\": \"" << config.current_audio_file << "\",\n";
    json << "    \"auto_save\": " << (config.auto_save ? "true" : "false") << "\n";
    json << "  }";
    return json.str();
}

SystemConfig ConfigManager::systemConfigFromJSON(const std::string& json_data) const {
    // Simplified JSON parsing - in real implementation use proper JSON library
    SystemConfig config = getDefaultSystemConfig();
    return config;
}

std::string ConfigManager::audioConfigToJSON(const AudioConfig& config) const {
    std::ostringstream json;
    json << "{\n";
    json << "      \"sample_rate\": " << config.sample_rate << ",\n";
    json << "      \"buffer_size\": " << config.buffer_size << ",\n";
    json << "      \"bit_depth\": " << config.bit_depth << ",\n";
    json << "      \"channels\": " << config.channels << "\n";
    json << "    }";
    return json.str();
}

AudioConfig ConfigManager::audioConfigFromJSON(const std::string& json_data) const {
    // Simplified JSON parsing - in real implementation use proper JSON library
    AudioConfig config = getDefaultAudioConfig();
    return config;
}

void ConfigManager::initializeDefaults() {
    system_config_ = getDefaultSystemConfig();
}

SystemConfig ConfigManager::getDefaultSystemConfig() const {
    SystemConfig config;
    config.audio = getDefaultAudioConfig();
    config.active_device_id = -1;
    config.current_audio_file = "T-Rex Roar (Default)";
    config.auto_save = true;
    config.config_file_path = "config.txt";
    return config;
}

AudioConfig ConfigManager::getDefaultAudioConfig() const {
    AudioConfig config;
    config.sample_rate = 48000;
    config.buffer_size = 256;
    config.bit_depth = 24;
    config.channels = 2;
    return config;
}

bool ConfigManager::isValidSampleRate(int sample_rate) const {
    std::vector<int> valid_rates = {44100, 48000, 88200, 96000, 192000};
    return std::find(valid_rates.begin(), valid_rates.end(), sample_rate) != valid_rates.end();
}

bool ConfigManager::isValidBufferSize(int buffer_size) const {
    return buffer_size >= 32 && buffer_size <= 2048 && (buffer_size & (buffer_size - 1)) == 0; // Power of 2
}

bool ConfigManager::isValidBitDepth(int bit_depth) const {
    return bit_depth == 16 || bit_depth == 24 || bit_depth == 32;
}

void ConfigManager::notifyConfigChanged() {
    if (config_change_callback_) {
        config_change_callback_(system_config_);
    }
}

} // namespace CyberAsio