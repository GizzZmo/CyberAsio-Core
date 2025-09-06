#include "webserver.h"
#include "audio_engine.h"
#include "device_manager.h"
#include "config_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <regex>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

namespace CyberAsio {

WebServer::WebServer(int port) : port_(port), running_(false) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    setupRoutes();
}

WebServer::~WebServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool WebServer::start() {
    if (running_) return true;
    
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    
    // Set socket options
    int opt = 1;
#ifdef _WIN32
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);
    
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket to port " << port_ << std::endl;
        closesocket(server_socket);
        return false;
    }
    
    if (listen(server_socket, 10) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket" << std::endl;
        closesocket(server_socket);
        return false;
    }
    
    running_ = true;
    
    // Start server thread
    std::thread([this, server_socket]() {
        while (running_) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
            if (client_socket == INVALID_SOCKET) {
                if (running_) {
                    std::cerr << "Failed to accept client connection" << std::endl;
                }
                continue;
            }
            
            // Handle client in separate thread
            std::thread([this, client_socket]() {
                handleClient(client_socket);
                closesocket(client_socket);
            }).detach();
        }
        closesocket(server_socket);
    }).detach();
    
    return true;
}

void WebServer::stop() {
    running_ = false;
}

void WebServer::setupRoutes() {
    // API routes
    addRoute("GET", "/api/devices", [this](const std::map<std::string, std::string>& params) {
        return handleDevicesAPI(params);
    });
    
    addRoute("GET", "/api/config", [this](const std::map<std::string, std::string>& params) {
        return handleConfigAPI(params);
    });
    
    addRoute("GET", "/api/status", [this](const std::map<std::string, std::string>& params) {
        return handleStatusAPI(params);
    });
    
    addRoute("POST", "/api/audio/play", [this](const std::map<std::string, std::string>& params) {
        return handleAudioAPI(params);
    });
}

void WebServer::addRoute(const std::string& method, const std::string& path, 
                        std::function<std::string(const std::map<std::string, std::string>&)> handler) {
    routes_.push_back({method, path, handler});
}

void WebServer::setStaticDirectory(const std::string& path) {
    static_directory_ = path;
}

void WebServer::handleClient(SOCKET client_socket) {
    char buffer[4096];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received <= 0) return;
    
    buffer[bytes_received] = '\0';
    std::string request(buffer);
    
    // Parse HTTP request
    std::istringstream request_stream(request);
    std::string method, path, version;
    request_stream >> method >> path >> version;
    
    // Extract query parameters
    std::map<std::string, std::string> params;
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) {
        std::string query = path.substr(query_pos + 1);
        path = path.substr(0, query_pos);
        // Parse query parameters (simplified)
        std::regex param_regex(R"(([^&=]+)=([^&=]*))");
        std::sregex_iterator iter(query.begin(), query.end(), param_regex);
        std::sregex_iterator end;
        for (; iter != end; ++iter) {
            params[(*iter)[1]] = (*iter)[2];
        }
    }
    
    std::string response = handleRequest(method, path, params);
    
    // Send response
    send(client_socket, response.c_str(), response.length(), 0);
}

std::string WebServer::handleRequest(const std::string& method, const std::string& path, 
                                    const std::map<std::string, std::string>& params) {
    
    // Check API routes first
    for (const auto& route : routes_) {
        if (route.method == method && route.path == path) {
            std::string json_response = route.handler(params);
            return addCORSHeaders("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + json_response);
        }
    }
    
    // Serve static files
    if (method == "GET") {
        std::string file_path = path;
        if (file_path == "/") file_path = "/index.html";
        
        std::string content = serveStaticFile(file_path);
        if (!content.empty()) {
            std::string content_type = getContentType(file_path);
            return addCORSHeaders("HTTP/1.1 200 OK\r\nContent-Type: " + content_type + "\r\n\r\n" + content);
        }
    }
    
    // 404 Not Found
    return addCORSHeaders("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>");
}

std::string WebServer::serveStaticFile(const std::string& path) {
    std::string full_path = static_directory_ + path;
    std::ifstream file(full_path, std::ios::binary);
    
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string WebServer::getContentType(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) return "text/plain";
    
    std::string extension = path.substr(dot_pos);
    
    if (extension == ".html") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".json") return "application/json";
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".gif") return "image/gif";
    if (extension == ".svg") return "image/svg+xml";
    if (extension == ".ico") return "image/x-icon";
    if (extension == ".wav") return "audio/wav";
    if (extension == ".mp3") return "audio/mpeg";
    
    return "text/plain";
}

std::string WebServer::addCORSHeaders(const std::string& response) {
    std::string cors_headers = 
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    
    size_t header_end = response.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        return response.substr(0, header_end) + "\r\n" + cors_headers + response.substr(header_end);
    }
    return response;
}

std::string WebServer::handleDevicesAPI(const std::map<std::string, std::string>& params) {
    if (!device_manager_) {
        return R"({"error": "Device manager not available"})";
    }
    
    auto devices = device_manager_->getDevices();
    std::ostringstream json;
    json << R"({"devices": [)";
    
    for (size_t i = 0; i < devices.size(); ++i) {
        if (i > 0) json << ",";
        const auto& device = devices[i];
        json << R"({"id": )" << device.id 
             << R"(, "name": ")" << device.name << R"(")"
             << R"(, "type": ")" << deviceTypeToString(device.type) << R"(")"
             << R"(, "status": ")" << deviceStatusToString(device.status) << R"(")"
             << "}";
    }
    
    json << "]}";
    return json.str();
}

std::string WebServer::handleConfigAPI(const std::map<std::string, std::string>& params) {
    if (!config_manager_) {
        return R"({"error": "Config manager not available"})";
    }
    
    auto config = config_manager_->getAudioConfig();
    std::ostringstream json;
    json << R"({"config": {)"
         << R"("sample_rate": )" << config.sample_rate
         << R"(, "buffer_size": )" << config.buffer_size
         << R"(, "bit_depth": )" << config.bit_depth
         << R"(, "channels": )" << config.channels
         << "}}";
    
    return json.str();
}

std::string WebServer::handleStatusAPI(const std::map<std::string, std::string>& params) {
    std::ostringstream json;
    json << R"({"status": {)"
         << R"("server": "online")"
         << R"(, "audio_engine": ")" << (audio_engine_ && audio_engine_->isInitialized() ? "online" : "offline") << R"(")"
         << R"(, "device_manager": ")" << (device_manager_ ? "online" : "offline") << R"(")"
         << R"(, "config_manager": ")" << (config_manager_ ? "online" : "offline") << R"(")"
         << "}}";
    
    return json.str();
}

std::string WebServer::handleAudioAPI(const std::map<std::string, std::string>& params) {
    if (!audio_engine_) {
        return R"({"error": "Audio engine not available"})";
    }
    
    // For now, just return success
    return R"({"result": "success", "message": "Audio command processed"})";
}

} // namespace CyberAsio