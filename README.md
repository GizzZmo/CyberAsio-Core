# CyberASIO Core

<div align="center">
<img width="1200" height="475" alt="GHBanner" src="https://github.com/user-attachments/assets/0aa67016-6eaf-458a-adb2-6e31a0763ed6" />
</div>

**A futuristic cyberpunk-themed ASIO driver simulation with C++ backend and web interface**

[![C++ Backend](https://img.shields.io/badge/Backend-C++17-blue.svg)](cpp-backend/)
[![Web Interface](https://img.shields.io/badge/Frontend-JavaScript-yellow.svg)](static/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

## 🚀 Features

- **Cyberpunk UI**: Futuristic neon-themed web interface
- **C++ Backend**: High-performance server with audio simulation
- **Device Management**: WDM, KS, WASAPI, and ASIO device support
- **Real-time Monitoring**: Live latency calculation and audio visualization
- **Cross-platform**: Windows, Linux, and macOS support
- **Professional Audio**: Configurable sample rates, buffer sizes, and bit depths

## 📸 Screenshots

### Original Angular Implementation
![Angular Interface](https://github.com/user-attachments/assets/438af5fe-ff33-40f6-a491-753b51e09b9a)

### New C++ Backend Implementation
![C++ Backend Interface](https://github.com/user-attachments/assets/1a42ece9-5839-4961-8d1d-6692cb918664)

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                  Frontend (Web-based)                      │
├─────────────────────────────────────────────────────────────┤
│                  HTTP/WebSocket Layer                      │
├─────────────────────────────────────────────────────────────┤
│                   C++ Backend Server                       │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │   Audio     │   Device    │   Config    │     Web     │  │
│  │  Engine     │  Manager    │  Manager    │   Server    │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                System Audio APIs (ASIO/WASAPI)             │
└─────────────────────────────────────────────────────────────┘
```

## 🚀 Quick Start

### Prerequisites
- **C++17 compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.16+**
- **Modern web browser**

### Build and Run

```bash
# Clone the repository
git clone https://github.com/GizzZmo/CyberAsio-Core.git
cd CyberAsio-Core

# Build the C++ backend
cd cpp-backend
mkdir build && cd build
cmake ..
make

# Run the server on localhost:7788
./cyberasio-core --port 7788
```

### Access the Interface

Open your browser and navigate to: **http://localhost:7788**

## 📚 Documentation

- **[Comprehensive Documentation](docs/README.md)** - Complete project overview
- **[C++ Build Guide](cpp-backend/BUILD.md)** - Detailed build instructions
- **[API Reference](docs/API.md)** - REST API documentation
- **[Development Guide](docs/DEVELOPMENT.md)** - Contributing guidelines

## 🎵 Audio Features

### Supported Configurations
- **Sample Rates**: 44.1kHz, 48kHz, 88.2kHz, 96kHz, 192kHz
- **Buffer Sizes**: 32 - 2048 samples
- **Bit Depths**: 16-bit, 24-bit, 32-bit
- **Channels**: Stereo/Multi-channel support

### Device Types
- **WDM** (Windows Driver Model)
- **KS** (Kernel Streaming)
- **WASAPI** (Windows Audio Session API)
- **ASIO** (Audio Stream Input/Output)

## 🛠️ Development

### Project Structure
```
CyberAsio-Core/
├── cpp-backend/          # C++ server implementation
│   ├── src/             # Source files
│   ├── include/         # Header files
│   ├── static/          # Web interface files
│   └── BUILD.md         # Build instructions
├── docs/                # Comprehensive documentation
├── src/                 # Original Angular components
└── README.md            # This file
```

### Technology Stack
- **Backend**: C++17, CMake, Native sockets
- **Frontend**: HTML5, CSS3, JavaScript (ES6+)
- **Styling**: Tailwind CSS with cyberpunk theme
- **Audio**: Simulated ASIO/WASAPI integration

## 🔧 Configuration

The server creates a `config.txt` file automatically:

```
# CyberASIO Core Configuration
sample_rate=48000
buffer_size=256
bit_depth=24
channels=2
active_device_id=1
current_audio_file=T-Rex Roar (Default)
```

## 📡 API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web interface |
| `/api/status` | GET | System status |
| `/api/devices` | GET | Audio devices |
| `/api/config` | GET | Configuration |

## 🌟 Legacy Support

The original Angular implementation is still available for reference:

```bash
# Run the Angular version (Node.js required)
npm install
npm run dev
```

## 🚧 Roadmap

- [ ] Real ASIO driver integration
- [ ] WebSocket real-time updates
- [ ] Advanced audio file support
- [ ] Multi-client streaming
- [ ] Plugin system architecture
- [ ] Professional DAW integration

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **ASIO SDK** by Steinberg for audio driver standards
- **Tailwind CSS** for the cyberpunk styling framework
- **Modern C++** community for best practices and standards

---

<div align="center">
<strong>Transforming audio driver management with cyberpunk aesthetics</strong><br>
Made with ❤️ by <a href="https://github.com/GizzZmo">GizzZmo</a>
</div>
