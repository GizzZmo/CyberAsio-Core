#pragma once

#include "audio_engine.h"
#include "device_manager.h"
#include <string>
#include <map>
#include <memory>
#include <functional>

namespace CyberAsio {

struct SystemConfig {
    AudioConfig audio;
    int active_device_id = -1;
    std::string current_audio_file;
    bool auto_save = true;
    std::string config_file_path = "config.json";
};

/**
 * Configuration Manager for CyberAsio Core
 * Handles settings persistence, device profiles, and runtime configuration
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    // Configuration management
    bool loadConfig(const std::string& config_path = "");
    bool saveConfig(const std::string& config_path = "");
    void resetToDefaults();

    // System configuration
    SystemConfig getSystemConfig() const { return system_config_; }
    bool setSystemConfig(const SystemConfig& config);
    
    // Audio configuration
    AudioConfig getAudioConfig() const { return system_config_.audio; }
    bool setAudioConfig(const AudioConfig& config);
    
    // Device profiles
    bool saveDeviceProfile(int device_id, const AudioConfig& config);
    AudioConfig getDeviceProfile(int device_id) const;
    bool hasDeviceProfile(int device_id) const;
    void removeDeviceProfile(int device_id);
    
    // Runtime settings
    void setActiveDevice(int device_id);
    int getActiveDevice() const { return system_config_.active_device_id; }
    
    void setCurrentAudioFile(const std::string& filepath);
    std::string getCurrentAudioFile() const { return system_config_.current_audio_file; }
    
    // Change notifications
    void setConfigChangeCallback(std::function<void(const SystemConfig&)> callback);
    
    // Validation
    bool validateConfig(const SystemConfig& config) const;
    bool validateAudioConfig(const AudioConfig& config) const;
    
    // Export/Import
    std::string exportConfigToJSON() const;
    bool importConfigFromJSON(const std::string& json_data);

private:
    SystemConfig system_config_;
    std::map<int, AudioConfig> device_profiles_;
    std::function<void(const SystemConfig&)> config_change_callback_;
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // JSON serialization
    std::string systemConfigToJSON(const SystemConfig& config) const;
    SystemConfig systemConfigFromJSON(const std::string& json_data) const;
    std::string audioConfigToJSON(const AudioConfig& config) const;
    AudioConfig audioConfigFromJSON(const std::string& json_data) const;
    
    // Default values
    void initializeDefaults();
    SystemConfig getDefaultSystemConfig() const;
    AudioConfig getDefaultAudioConfig() const;
    
    // Validation helpers
    bool isValidSampleRate(int sample_rate) const;
    bool isValidBufferSize(int buffer_size) const;
    bool isValidBitDepth(int bit_depth) const;
    
    // Change notification
    void notifyConfigChanged();
};

} // namespace CyberAsio