#include "device_manager.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <mmdeviceapi.h>
    #include <functiondiscoverykeys_devpkey.h>
    #include <audioclient.h>
    #pragma comment(lib, "ole32.lib")
#elif __linux__
    // ALSA includes would go here if available
    // #include <alsa/asoundlib.h>
#elif __APPLE__
    #include <CoreAudio/CoreAudio.h>
    #include <AudioUnit/AudioUnit.h>
#endif

namespace CyberAsio {

DeviceManager::DeviceManager() {
    initializeSystemAudio();
    populateDefaultDevices();
}

DeviceManager::~DeviceManager() {
    shutdownSystemAudio();
}

bool DeviceManager::scanDevices() {
    devices_.clear();
    device_status_.clear();
    
    std::cout << "Scanning for audio devices..." << std::endl;
    
    // Platform-specific device scanning
#ifdef _WIN32
    scanWindowsDevices();
#elif __linux__
    scanLinuxDevices();
#elif __APPLE__
    scanMacDevices();
#else
    // Fallback to mock devices for unsupported platforms
    populateDefaultDevices();
#endif
    
    std::cout << "Found " << devices_.size() << " audio devices" << std::endl;
    return !devices_.empty();
}

std::vector<AudioDevice> DeviceManager::getDevices() const {
    return devices_;
}

std::shared_ptr<AudioDevice> DeviceManager::getDevice(int id) const {
    auto it = std::find_if(devices_.begin(), devices_.end(), 
                          [id](const AudioDevice& device) { return device.id == id; });
    
    if (it != devices_.end()) {
        return std::make_shared<AudioDevice>(*it);
    }
    return nullptr;
}

bool DeviceManager::activateDevice(int device_id) {
    auto device = getDevice(device_id);
    if (!device || device->status == DeviceStatus::Disabled) {
        return false;
    }
    
    // Deactivate currently active device
    if (active_device_id_ != -1) {
        updateDeviceStatus(active_device_id_, DeviceStatus::Inactive);
    }
    
    // Activate new device
    updateDeviceStatus(device_id, DeviceStatus::Active);
    active_device_id_ = device_id;
    
    std::cout << "Activated device: " << device->name << " (ID: " << device_id << ")" << std::endl;
    return true;
}

bool DeviceManager::deactivateDevice(int device_id) {
    if (active_device_id_ == device_id) {
        active_device_id_ = -1;
    }
    
    updateDeviceStatus(device_id, DeviceStatus::Inactive);
    
    auto device = getDevice(device_id);
    if (device) {
        std::cout << "Deactivated device: " << device->name << " (ID: " << device_id << ")" << std::endl;
    }
    
    return true;
}

bool DeviceManager::isDeviceActive(int device_id) const {
    return active_device_id_ == device_id;
}

void DeviceManager::updateDeviceStatus(int device_id, DeviceStatus status) {
    device_status_[device_id] = status;
    
    // Update the device in the devices vector
    auto it = std::find_if(devices_.begin(), devices_.end(),
                          [device_id](AudioDevice& device) { return device.id == device_id; });
    
    if (it != devices_.end()) {
        DeviceStatus old_status = it->status;
        it->status = status;
        notifyDeviceStatusChanged(device_id, old_status, status);
    }
}

DeviceStatus DeviceManager::getDeviceStatus(int device_id) const {
    auto it = device_status_.find(device_id);
    if (it != device_status_.end()) {
        return it->second;
    }
    return DeviceStatus::Inactive;
}

bool DeviceManager::setDeviceConfig(int device_id, int sample_rate, int buffer_size, int bit_depth) {
    auto device = getDevice(device_id);
    if (!device) return false;
    
    // Validate configuration
    bool valid_sample_rate = std::find(device->supported_sample_rates.begin(), 
                                      device->supported_sample_rates.end(), 
                                      sample_rate) != device->supported_sample_rates.end();
    
    bool valid_buffer_size = buffer_size >= device->min_buffer_size && 
                            buffer_size <= device->max_buffer_size;
    
    bool valid_bit_depth = std::find(device->supported_bit_depths.begin(),
                                    device->supported_bit_depths.end(),
                                    bit_depth) != device->supported_bit_depths.end();
    
    if (!valid_sample_rate || !valid_buffer_size || !valid_bit_depth) {
        std::cerr << "Invalid device configuration for device " << device_id << std::endl;
        return false;
    }
    
    std::cout << "Device " << device_id << " configured: " 
              << sample_rate << " Hz, " << buffer_size << " samples, " 
              << bit_depth << " bits" << std::endl;
    
    return true;
}

std::map<std::string, std::string> DeviceManager::getDeviceInfo(int device_id) const {
    std::map<std::string, std::string> info;
    
    auto device = getDevice(device_id);
    if (!device) return info;
    
    info["id"] = std::to_string(device->id);
    info["name"] = device->name;
    info["type"] = deviceTypeToString(device->type);
    info["status"] = deviceStatusToString(device->status);
    info["max_sample_rate"] = std::to_string(device->max_sample_rate);
    info["min_buffer_size"] = std::to_string(device->min_buffer_size);
    info["max_buffer_size"] = std::to_string(device->max_buffer_size);
    info["is_input"] = device->is_input ? "true" : "false";
    info["is_output"] = device->is_output ? "true" : "false";
    
    return info;
}

bool DeviceManager::initializeSystemAudio() {
    if (system_initialized_) return true;
    
#ifdef _WIN32
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM" << std::endl;
        return false;
    }
    
    system_initialized_ = initializeWASAPI();
#elif __linux__
    system_initialized_ = true; // ALSA doesn't require special initialization
#elif __APPLE__
    system_initialized_ = true; // Core Audio doesn't require special initialization
#else
    system_initialized_ = true; // Mock implementation
#endif
    
    if (system_initialized_) {
        std::cout << "System audio initialized successfully" << std::endl;
    }
    
    return system_initialized_;
}

void DeviceManager::shutdownSystemAudio() {
    if (!system_initialized_) return;
    
    cleanupSystemResources();
    
#ifdef _WIN32
    CoUninitialize();
#endif
    
    system_initialized_ = false;
    std::cout << "System audio shut down" << std::endl;
}

void DeviceManager::scanWindowsDevices() {
#ifdef _WIN32
    // This is a simplified implementation
    // In a real implementation, you would use IMMDeviceEnumerator to scan WASAPI devices
    populateDefaultDevices();
#endif
}

void DeviceManager::scanLinuxDevices() {
#ifdef __linux__
    // This is a simplified implementation
    // In a real implementation, you would use ALSA APIs to scan devices
    populateDefaultDevices();
#endif
}

void DeviceManager::scanMacDevices() {
#ifdef __APPLE__
    // This is a simplified implementation
    // In a real implementation, you would use Core Audio APIs to scan devices
    populateDefaultDevices();
#endif
}

AudioDevice DeviceManager::createMockDevice(int id, const std::string& name, DeviceType type) {
    AudioDevice device;
    device.id = id;
    device.name = name;
    device.type = type;
    device.status = (id == 1) ? DeviceStatus::Active : DeviceStatus::Inactive;
    device.max_sample_rate = 192000;
    device.min_buffer_size = 32;
    device.max_buffer_size = 2048;
    device.supported_sample_rates = {44100, 48000, 88200, 96000, 192000};
    device.supported_bit_depths = {16, 24, 32};
    device.is_input = true;
    device.is_output = true;
    
    // Some devices might be disabled for demo purposes
    if (id == 2) {
        device.status = DeviceStatus::Disabled;
    }
    
    return device;
}

void DeviceManager::populateDefaultDevices() {
    devices_ = {
        createMockDevice(1, "Generic HD Audio Device (WDM)", DeviceType::WDM),
        createMockDevice(2, "Realtek ASIO (KS)", DeviceType::KS),
        createMockDevice(3, "NVIDIA Broadcast (WASAPI)", DeviceType::WASAPI),
        createMockDevice(4, "Focusrite USB ASIO (WDM)", DeviceType::WDM)
    };
    
    // Set device statuses
    for (const auto& device : devices_) {
        device_status_[device.id] = device.status;
    }
    
    // Set first device as active
    if (!devices_.empty()) {
        active_device_id_ = 1;
    }
}

void DeviceManager::notifyDeviceStatusChanged(int device_id, DeviceStatus old_status, DeviceStatus new_status) {
    std::cout << "Device " << device_id << " status changed: " 
              << deviceStatusToString(old_status) << " -> " 
              << deviceStatusToString(new_status) << std::endl;
}

bool DeviceManager::initializeWASAPI() {
#ifdef _WIN32
    // Simplified WASAPI initialization
    // In a real implementation, you would create IMMDeviceEnumerator and check for devices
    return true;
#else
    return false;
#endif
}

bool DeviceManager::initializeASIO() {
    // ASIO SDK integration would go here
    // This requires the ASIO SDK from Steinberg
    return true;
}

void DeviceManager::cleanupSystemResources() {
    // Clean up any allocated resources
}

// Utility functions
std::string deviceTypeToString(DeviceType type) {
    switch (type) {
        case DeviceType::WDM: return "WDM";
        case DeviceType::KS: return "KS";
        case DeviceType::WASAPI: return "WASAPI";
        case DeviceType::ASIO: return "ASIO";
        default: return "Unknown";
    }
}

std::string deviceStatusToString(DeviceStatus status) {
    switch (status) {
        case DeviceStatus::Active: return "Active";
        case DeviceStatus::Inactive: return "Inactive";
        case DeviceStatus::Disabled: return "Disabled";
        case DeviceStatus::Error: return "Error";
        default: return "Unknown";
    }
}

DeviceType stringToDeviceType(const std::string& type_str) {
    if (type_str == "WDM") return DeviceType::WDM;
    if (type_str == "KS") return DeviceType::KS;
    if (type_str == "WASAPI") return DeviceType::WASAPI;
    if (type_str == "ASIO") return DeviceType::ASIO;
    return DeviceType::WDM; // Default
}

DeviceStatus stringToDeviceStatus(const std::string& status_str) {
    if (status_str == "Active") return DeviceStatus::Active;
    if (status_str == "Inactive") return DeviceStatus::Inactive;
    if (status_str == "Disabled") return DeviceStatus::Disabled;
    if (status_str == "Error") return DeviceStatus::Error;
    return DeviceStatus::Inactive; // Default
}

} // namespace CyberAsio