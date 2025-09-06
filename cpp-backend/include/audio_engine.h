#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>

namespace CyberAsio {

struct AudioConfig {
    int sample_rate = 48000;
    int buffer_size = 256;
    int bit_depth = 24;
    int channels = 2;
};

struct AudioMetrics {
    double input_latency = 0.0;
    double output_latency = 0.0;
    double total_latency = 0.0;
    std::vector<float> spectrum_data;
    bool is_playing = false;
};

/**
 * Audio Engine for CyberAsio Core
 * Handles audio processing, playback, and ASIO driver simulation
 */
class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    // Engine control
    bool initialize(const AudioConfig& config);
    void shutdown();
    bool isInitialized() const { return initialized_; }

    // Configuration
    bool setConfig(const AudioConfig& config);
    AudioConfig getConfig() const { return config_; }

    // Playback control
    bool loadAudioFile(const std::string& filepath);
    bool loadAudioData(const std::vector<uint8_t>& data, const std::string& format);
    void play();
    void pause();
    void stop();
    bool isPlaying() const { return playing_; }

    // Metrics and monitoring
    AudioMetrics getMetrics() const;
    void setVisualizationCallback(std::function<void(const std::vector<float>&)> callback);

    // Device interface
    bool setActiveDevice(int device_id);
    int getActiveDevice() const { return active_device_id_; }

private:
    AudioConfig config_;
    std::atomic<bool> initialized_{false};
    std::atomic<bool> playing_{false};
    int active_device_id_ = -1;

    // Audio data
    std::vector<float> audio_buffer_;
    size_t playback_position_ = 0;
    
    // Processing thread
    std::unique_ptr<std::thread> audio_thread_;
    std::atomic<bool> should_stop_{false};
    
    // Metrics
    mutable std::mutex metrics_mutex_;
    AudioMetrics current_metrics_;
    
    // Callbacks
    std::function<void(const std::vector<float>&)> visualization_callback_;

    // Internal methods
    void audioThreadFunction();
    void processAudioBuffer();
    void calculateLatency();
    void generateSpectrumData();
    void simulateAudioProcessing();
    void generateDefaultAudio();
    
    // File loading helpers
    bool loadWavFile(const std::string& filepath);
    bool loadAudioFromBytes(const std::vector<uint8_t>& data);
};

} // namespace CyberAsio