#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CyberAsio {

enum class DeviceType {
    WDM,
    KS,
    WASAPI,
    ASIO
};

enum class DeviceStatus {
    Active,
    Inactive,
    Disabled,
    Error
};

struct AudioDevice {
    int id;
    std::string name;
    DeviceType type;
    DeviceStatus status;
    int max_sample_rate;
    int min_buffer_size;
    int max_buffer_size;
    std::vector<int> supported_sample_rates;
    std::vector<int> supported_bit_depths;
    bool is_input;
    bool is_output;
};

/**
 * Device Manager for CyberAsio Core
 * Handles audio device enumeration, management, and status monitoring
 */
class DeviceManager {
public:
    DeviceManager();
    ~DeviceManager();

    // Device enumeration
    bool scanDevices();
    std::vector<AudioDevice> getDevices() const;
    std::shared_ptr<AudioDevice> getDevice(int id) const;
    
    // Device management
    bool activateDevice(int device_id);
    bool deactivateDevice(int device_id);
    bool isDeviceActive(int device_id) const;
    
    // Status monitoring
    void updateDeviceStatus(int device_id, DeviceStatus status);
    DeviceStatus getDeviceStatus(int device_id) const;
    
    // Configuration
    bool setDeviceConfig(int device_id, int sample_rate, int buffer_size, int bit_depth);
    std::map<std::string, std::string> getDeviceInfo(int device_id) const;
    
    // System integration
    bool initializeSystemAudio();
    void shutdownSystemAudio();

private:
    std::vector<AudioDevice> devices_;
    std::map<int, DeviceStatus> device_status_;
    int active_device_id_ = -1;
    bool system_initialized_ = false;

    // Platform-specific implementations
    void scanWindowsDevices();
    void scanLinuxDevices();
    void scanMacDevices();
    
    // Device creation helpers
    AudioDevice createMockDevice(int id, const std::string& name, DeviceType type);
    void populateDefaultDevices();
    
    // Status update helpers
    void notifyDeviceStatusChanged(int device_id, DeviceStatus old_status, DeviceStatus new_status);
    
    // System API wrappers
    bool initializeWASAPI();
    bool initializeASIO();
    void cleanupSystemResources();
};

// Utility functions
std::string deviceTypeToString(DeviceType type);
std::string deviceStatusToString(DeviceStatus status);
DeviceType stringToDeviceType(const std::string& type_str);
DeviceStatus stringToDeviceStatus(const std::string& status_str);

} // namespace CyberAsio