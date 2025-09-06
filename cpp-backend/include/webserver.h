#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace CyberAsio {

// Forward declarations
class AudioEngine;
class DeviceManager;
class ConfigManager;

/**
 * HTTP Web Server for CyberAsio Core
 * Handles static file serving and REST API endpoints
 */
class WebServer {
public:
    struct Route {
        std::string method;
        std::string path;
        std::function<std::string(const std::map<std::string, std::string>&)> handler;
    };

    WebServer(int port = 7788);
    ~WebServer();

    // Server control
    bool start();
    void stop();
    bool isRunning() const { return running_; }

    // Route management
    void addRoute(const std::string& method, const std::string& path, 
                  std::function<std::string(const std::map<std::string, std::string>&)> handler);
    
    // Static file serving
    void setStaticDirectory(const std::string& path);

    // Component injection
    void setAudioEngine(std::shared_ptr<AudioEngine> engine) { audio_engine_ = engine; }
    void setDeviceManager(std::shared_ptr<DeviceManager> manager) { device_manager_ = manager; }
    void setConfigManager(std::shared_ptr<ConfigManager> manager) { config_manager_ = manager; }

private:
    int port_;
    bool running_;
    std::string static_directory_;
    std::vector<Route> routes_;
    
    // Component references
    std::shared_ptr<AudioEngine> audio_engine_;
    std::shared_ptr<DeviceManager> device_manager_;
    std::shared_ptr<ConfigManager> config_manager_;

    // Internal methods
    void setupRoutes();
    void handleClient(int client_socket);
    std::string handleRequest(const std::string& method, const std::string& path, 
                             const std::map<std::string, std::string>& params);
    std::string serveStaticFile(const std::string& path);
    std::string getContentType(const std::string& extension);
    
    // API handlers
    std::string handleDevicesAPI(const std::map<std::string, std::string>& params);
    std::string handleConfigAPI(const std::map<std::string, std::string>& params);
    std::string handleStatusAPI(const std::map<std::string, std::string>& params);
    std::string handleAudioAPI(const std::map<std::string, std::string>& params);
    
    // CORS headers
    std::string addCORSHeaders(const std::string& response);
};

} // namespace CyberAsio