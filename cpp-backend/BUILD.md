# C++ Backend Build Instructions

## Prerequisites

- **C++17 Compatible Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **CMake**: Version 3.16 or higher
- **Platform Dependencies**:
  - **Windows**: Windows 10+ with Windows SDK
  - **Linux**: GCC development tools
  - **macOS**: Xcode command line tools

## Quick Start

### 1. Build the Backend

```bash
cd cpp-backend
mkdir build && cd build
cmake ..
make
```

### 2. Run the Server

```bash
# Run on default port 7788
./cyberasio-core

# Or specify custom port
./cyberasio-core --port 8080

# Specify custom static files directory
./cyberasio-core --static-dir ../custom-static
```

### 3. Access the Web Interface

Open your browser and navigate to:
- **Default**: http://localhost:7788
- **Custom Port**: http://localhost:PORT

## Build Options

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Release Build
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Platform-Specific Instructions

#### Windows (Visual Studio)
```cmd
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

#### Windows (MinGW)
```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

#### Linux
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential cmake

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Build
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--port <port>` | Server port number | 7788 |
| `--static-dir <dir>` | Static files directory | static |
| `--help`, `-h` | Show help message | - |

## Configuration

The server automatically creates a `config.txt` file on first run. You can edit this file to customize default settings:

```
# CyberASIO Core Configuration
sample_rate=48000
buffer_size=256
bit_depth=24
channels=2
active_device_id=1
current_audio_file=T-Rex Roar (Default)
```

## Troubleshooting

### Build Issues

1. **CMake not found**: Install CMake 3.16+ from https://cmake.org/
2. **Compiler errors**: Ensure you have a C++17 compatible compiler
3. **Missing headers**: Install platform development tools

### Runtime Issues

1. **Port already in use**: Use `--port` to specify a different port
2. **Permission denied**: On Linux/macOS, ports below 1024 require sudo
3. **Static files not found**: Use `--static-dir` to specify correct path

### Network Issues

1. **Cannot connect from other machines**: The server binds to localhost only
2. **Firewall blocking**: Configure firewall to allow the specified port

## Architecture

```
CyberASIO Core C++ Backend
├── src/
│   ├── main.cpp           # Application entry point
│   ├── webserver.cpp      # HTTP server implementation
│   ├── audio_engine.cpp   # Audio processing engine
│   ├── device_manager.cpp # Audio device management
│   └── config_manager.cpp # Configuration management
├── include/
│   ├── webserver.h        # Web server interface
│   ├── audio_engine.h     # Audio engine interface
│   ├── device_manager.h   # Device manager interface
│   └── config_manager.h   # Configuration interface
├── static/
│   └── index.html         # Web interface
└── build/
    └── cyberasio-core     # Compiled executable
```

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web interface |
| `/api/status` | GET | System status |
| `/api/devices` | GET | Audio devices list |
| `/api/config` | GET | Current configuration |
| `/api/audio/play` | POST | Start audio playback |

## Development

### Adding New Features

1. **HTTP Endpoints**: Add routes in `webserver.cpp`
2. **Audio Processing**: Extend `audio_engine.cpp`
3. **Device Support**: Modify `device_manager.cpp`
4. **Configuration**: Update `config_manager.cpp`

### Testing

```bash
# Test the build
cd build
./cyberasio-core --help

# Test the web interface
curl http://localhost:7788/api/status
```

## Performance Notes

- **Memory Usage**: ~10MB baseline
- **CPU Usage**: <1% idle, ~5% during audio processing
- **Latency**: Simulated 10.67ms (configurable)
- **Concurrent Connections**: Supports multiple clients

## Known Limitations

- **Audio I/O**: Currently simulated (not real hardware)
- **Platform Audio**: Full ASIO/WASAPI integration requires additional SDKs
- **File Formats**: Limited audio file format support
- **Real-time Processing**: Basic implementation for demonstration

## Future Enhancements

- Real ASIO driver integration
- Advanced audio file format support
- WebSocket for real-time updates
- Multi-client audio streaming
- Professional DAW plugin compatibility