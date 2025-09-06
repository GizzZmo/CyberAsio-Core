#include "webserver.h"
#include "audio_engine.h"
#include "device_manager.h"
#include "config_manager.h"
#include <iostream>
#include <memory>
#include <signal.h>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace CyberAsio;

// Global server instance for signal handling
std::unique_ptr<WebServer> g_server;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down gracefully..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    std::cout << "=== CyberASIO Core C++ Backend v1.1.0 ===" << std::endl;
    std::cout << "Cyberpunk ASIO Driver Simulation Server" << std::endl;
    std::cout << "========================================" << std::endl;

    // Parse command line arguments
    int port = 7788;
    std::string static_dir = "static";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = std::atoi(argv[++i]);
        } else if (arg == "--static-dir" && i + 1 < argc) {
            static_dir = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --port <port>         Server port (default: 7788)" << std::endl;
            std::cout << "  --static-dir <dir>    Static files directory (default: static)" << std::endl;
            std::cout << "  --help, -h            Show this help message" << std::endl;
            return 0;
        }
    }

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        // Initialize core components
        std::cout << "Initializing CyberASIO Core components..." << std::endl;
        
        auto audio_engine = std::make_shared<AudioEngine>();
        auto device_manager = std::make_shared<DeviceManager>();
        auto config_manager = std::make_shared<ConfigManager>();
        
        // Load configuration
        std::cout << "Loading configuration..." << std::endl;
        if (!config_manager->loadConfig()) {
            std::cout << "Warning: Could not load config, using defaults" << std::endl;
        }
        
        // Initialize device manager
        std::cout << "Scanning audio devices..." << std::endl;
        if (!device_manager->scanDevices()) {
            std::cout << "Warning: Device scanning failed, using mock devices" << std::endl;
        }
        
        // Initialize audio engine with config
        std::cout << "Initializing audio engine..." << std::endl;
        AudioConfig audio_config = config_manager->getAudioConfig();
        if (!audio_engine->initialize(audio_config)) {
            std::cout << "Warning: Audio engine initialization failed, running in simulation mode" << std::endl;
        }
        
        // Create and configure web server
        std::cout << "Starting web server on localhost:" << port << "..." << std::endl;
        g_server = std::make_unique<WebServer>(port);
        g_server->setStaticDirectory(static_dir);
        g_server->setAudioEngine(audio_engine);
        g_server->setDeviceManager(device_manager);
        g_server->setConfigManager(config_manager);
        
        if (!g_server->start()) {
            std::cerr << "Failed to start web server on port " << port << std::endl;
            return 1;
        }
        
        std::cout << std::endl;
        std::cout << "🚀 CyberASIO Core is now running!" << std::endl;
        std::cout << "📡 Web interface: http://localhost:" << port << std::endl;
        std::cout << "🎵 Audio engine: " << (audio_engine->isInitialized() ? "Ready" : "Simulation mode") << std::endl;
        std::cout << "🔊 Devices found: " << device_manager->getDevices().size() << std::endl;
        std::cout << std::endl;
        std::cout << "Press Ctrl+C to stop the server..." << std::endl;
        
        // Keep the main thread alive
        while (g_server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "CyberASIO Core shut down successfully." << std::endl;
    return 0;
}