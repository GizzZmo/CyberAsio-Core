#include "audio_engine.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <fstream>
#include <thread>
#include <chrono>

namespace CyberAsio {

AudioEngine::AudioEngine() = default;

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::initialize(const AudioConfig& config) {
    if (initialized_) {
        shutdown();
    }
    
    config_ = config;
    
    // Initialize audio buffer
    audio_buffer_.clear();
    audio_buffer_.resize(44100 * 2 * 10); // 10 seconds of stereo audio at 44.1kHz
    
    // Generate default test tone (T-Rex roar simulation)
    generateDefaultAudio();
    
    // Calculate initial latency
    calculateLatency();
    
    // Start audio processing thread
    should_stop_ = false;
    audio_thread_ = std::make_unique<std::thread>(&AudioEngine::audioThreadFunction, this);
    
    initialized_ = true;
    
    std::cout << "Audio Engine initialized successfully" << std::endl;
    std::cout << "Sample Rate: " << config_.sample_rate << " Hz" << std::endl;
    std::cout << "Buffer Size: " << config_.buffer_size << " samples" << std::endl;
    std::cout << "Bit Depth: " << config_.bit_depth << " bits" << std::endl;
    
    return true;
}

void AudioEngine::shutdown() {
    if (!initialized_) return;
    
    // Stop playback
    stop();
    
    // Stop audio thread
    should_stop_ = true;
    if (audio_thread_ && audio_thread_->joinable()) {
        audio_thread_->join();
    }
    audio_thread_.reset();
    
    initialized_ = false;
    std::cout << "Audio Engine shut down" << std::endl;
}

bool AudioEngine::setConfig(const AudioConfig& config) {
    if (!initialized_) return false;
    
    bool needs_restart = (config.sample_rate != config_.sample_rate || 
                         config.buffer_size != config_.buffer_size ||
                         config.bit_depth != config_.bit_depth);
    
    config_ = config;
    
    if (needs_restart) {
        // Recalculate latency with new settings
        calculateLatency();
        std::cout << "Audio configuration updated - Sample Rate: " << config_.sample_rate 
                  << " Hz, Buffer: " << config_.buffer_size << " samples" << std::endl;
    }
    
    return true;
}

bool AudioEngine::loadAudioFile(const std::string& filepath) {
    if (!initialized_) return false;
    
    // For now, just simulate loading
    std::cout << "Loading audio file: " << filepath << std::endl;
    
    // In a real implementation, this would use a library like libsndfile
    // to load various audio formats
    
    return true;
}

bool AudioEngine::loadAudioData(const std::vector<uint8_t>& data, const std::string& format) {
    if (!initialized_) return false;
    
    std::cout << "Loading audio data: " << data.size() << " bytes, format: " << format << std::endl;
    
    // Simulate audio loading
    return true;
}

void AudioEngine::play() {
    if (!initialized_) return;
    
    playing_ = true;
    playback_position_ = 0;
    
    std::cout << "Audio playback started" << std::endl;
}

void AudioEngine::pause() {
    playing_ = false;
    std::cout << "Audio playback paused" << std::endl;
}

void AudioEngine::stop() {
    playing_ = false;
    playback_position_ = 0;
    
    // Reset spectrum data
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_metrics_.spectrum_data.assign(32, 0.0f);
    current_metrics_.is_playing = false;
    
    std::cout << "Audio playback stopped" << std::endl;
}

AudioMetrics AudioEngine::getMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return current_metrics_;
}

void AudioEngine::setVisualizationCallback(std::function<void(const std::vector<float>&)> callback) {
    visualization_callback_ = callback;
}

bool AudioEngine::setActiveDevice(int device_id) {
    active_device_id_ = device_id;
    std::cout << "Active audio device set to ID: " << device_id << std::endl;
    return true;
}

void AudioEngine::audioThreadFunction() {
    auto last_update = std::chrono::steady_clock::now();
    
    while (!should_stop_) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update);
        
        if (elapsed.count() >= 50) { // Update every 50ms (20 FPS)
            processAudioBuffer();
            generateSpectrumData();
            last_update = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void AudioEngine::processAudioBuffer() {
    if (!playing_ || audio_buffer_.empty()) return;
    
    // Simulate audio processing
    size_t samples_per_update = (config_.sample_rate * 50) / 1000; // 50ms worth of samples
    playback_position_ += samples_per_update;
    
    if (playback_position_ >= audio_buffer_.size()) {
        playback_position_ = 0; // Loop
    }
    
    // Update metrics
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_metrics_.is_playing = true;
}

void AudioEngine::calculateLatency() {
    // Simulate latency calculation based on buffer size and sample rate
    double buffer_time_ms = (static_cast<double>(config_.buffer_size) / config_.sample_rate) * 1000.0;
    
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_metrics_.input_latency = buffer_time_ms;
    current_metrics_.output_latency = buffer_time_ms;
    current_metrics_.total_latency = current_metrics_.input_latency + current_metrics_.output_latency;
}

void AudioEngine::generateSpectrumData() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (playing_) {
        // Generate realistic-looking spectrum data
        current_metrics_.spectrum_data.resize(32);
        
        // Simulate spectrum analyzer data with some randomness
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(0.1f, 1.0f);
        
        for (size_t i = 0; i < 32; ++i) {
            // Lower frequencies typically have more energy
            float base_level = std::max(0.1f, 1.0f - (static_cast<float>(i) / 32.0f));
            float random_factor = dis(gen);
            
            // Add some time-based variation
            auto now = std::chrono::steady_clock::now();
            auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            float time_factor = 0.5f + 0.5f * std::sin((time_ms / 100.0f) + i * 0.5f);
            
            current_metrics_.spectrum_data[i] = base_level * random_factor * time_factor;
        }
        
        // Notify visualization callback
        if (visualization_callback_) {
            visualization_callback_(current_metrics_.spectrum_data);
        }
    } else {
        // Fade out spectrum data when not playing
        current_metrics_.spectrum_data.resize(32);
        std::fill(current_metrics_.spectrum_data.begin(), current_metrics_.spectrum_data.end(), 0.1f);
    }
}

void AudioEngine::generateDefaultAudio() {
    // Generate a simple test tone (simulating T-Rex roar)
    const float frequency = 220.0f; // A3 note
    const float sample_rate = 44100.0f;
    const float duration = 3.0f; // 3 seconds
    
    size_t num_samples = static_cast<size_t>(sample_rate * duration);
    audio_buffer_.resize(num_samples * 2); // Stereo
    
    for (size_t i = 0; i < num_samples; ++i) {
        float t = static_cast<float>(i) / sample_rate;
        
        // Generate a complex waveform that sounds more like a roar
        float base_wave = std::sin(2.0f * M_PI * frequency * t);
        float harmonic1 = 0.5f * std::sin(2.0f * M_PI * frequency * 2.0f * t);
        float harmonic2 = 0.25f * std::sin(2.0f * M_PI * frequency * 3.0f * t);
        float noise = 0.1f * ((rand() / static_cast<float>(RAND_MAX)) - 0.5f);
        
        // Apply envelope (fade in/out)
        float envelope = 1.0f;
        if (t < 0.1f) envelope = t / 0.1f;
        if (t > duration - 0.5f) envelope = (duration - t) / 0.5f;
        
        float sample = (base_wave + harmonic1 + harmonic2 + noise) * envelope * 0.3f;
        
        // Stereo (same signal on both channels)
        audio_buffer_[i * 2] = sample;
        audio_buffer_[i * 2 + 1] = sample;
    }
    
    std::cout << "Generated default audio (T-Rex roar simulation): " 
              << num_samples << " samples" << std::endl;
}

void AudioEngine::simulateAudioProcessing() {
    // This method can be used to simulate more complex audio processing
    // like effects, filtering, etc.
}

} // namespace CyberAsio