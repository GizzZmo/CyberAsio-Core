# CyberASIO Core - Comprehensive Documentation

## Overview

CyberASIO Core is a futuristic cyberpunk-themed ASIO (Audio Stream Input/Output) driver simulation and control panel. It provides a sophisticated interface for managing low-latency audio settings, device control, and playback testing for generic sound devices on Windows systems.

![CyberAsio Core Interface](https://github.com/user-attachments/assets/438af5fe-ff33-40f6-a491-753b51e09b9a)

## Prototype Concept

### Vision
CyberASIO Core reimagines audio driver management through a cyberpunk aesthetic, transforming the traditionally mundane task of audio configuration into an engaging, futuristic experience. The application bridges the gap between professional audio engineering and modern UI/UX design.

### Core Philosophy
- **Low-latency Performance**: Simulates and manages ASIO driver functionality for professional audio applications
- **Cyberpunk Aesthetics**: Immersive visual experience with neon colors, futuristic fonts, and animated elements
- **Real-time Monitoring**: Live audio visualization and latency calculation
- **Cross-platform Compatibility**: Modern web technologies with C++ backend for performance-critical operations

## Current Implementation (Angular/TypeScript)

### Architecture Overview
```
┌─────────────────────────────────────────────────────────────┐
│                    Frontend (Angular)                      │
├─────────────────────────────────────────────────────────────┤
│  • Component-based architecture with signals               │
│  • Reactive state management                               │
│  • Cyberpunk UI with Tailwind CSS                         │
│  • Audio playback and visualization                        │
└─────────────────────────────────────────────────────────────┘
```

### Key Features

#### 1. Device Management
- **Supported Driver Types**: WDM, KS, WASAPI
- **Device States**: Active, Inactive, Disabled
- **Real-time Status Updates**: Visual indicators for device status
- **Device Selection**: Interactive device switching with status management

#### 2. Audio Configuration
- **Sample Rates**: 44.1kHz, 48kHz, 88.2kHz, 96kHz, 192kHz
- **Buffer Sizes**: 32-2048 samples with real-time latency calculation
- **Bit Depths**: 16-bit, 24-bit, 32-bit
- **Dynamic Latency Calculation**: Real-time input/output latency computation

#### 3. Audio Playback & Testing
- **File Upload Support**: Drag-and-drop audio file loading
- **Real-time Visualization**: 32-band spectrum analyzer simulation
- **Playback Controls**: Play/pause with visual feedback
- **Default Audio Sample**: T-Rex roar for immediate testing

#### 4. User Interface
- **Cyberpunk Theme**: Neon cyan and magenta color scheme
- **Responsive Design**: Mobile-first approach with grid layouts
- **Custom Components**: Styled sliders, buttons, and visualizers
- **Orbitron Font**: Futuristic typography for headers
- **Share Tech Mono**: Monospace font for technical data

### Technical Stack
- **Framework**: Angular 20.1.0 with standalone components
- **State Management**: Angular Signals for reactive programming
- **Styling**: Tailwind CSS with custom cyberpunk components
- **Build Tool**: Angular CLI with Vite
- **Browser APIs**: Web Audio API for audio processing

## Planned C++ Implementation

### Architecture Design
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

### Core Modules

#### 1. Web Server (`webserver.cpp`)
- **HTTP Server**: Serve static files and API endpoints
- **WebSocket Support**: Real-time communication for audio data
- **Port Configuration**: localhost:7788
- **CORS Support**: Enable frontend integration
- **JSON API**: RESTful endpoints for device management

#### 2. Audio Engine (`audio_engine.cpp`)
- **ASIO Integration**: Native ASIO driver interface
- **Buffer Management**: Low-latency audio buffer handling
- **Sample Rate Conversion**: Real-time audio format conversion
- **Latency Monitoring**: Precise latency measurement and reporting
- **File Playback**: Audio file loading and playback

#### 3. Device Manager (`device_manager.cpp`)
- **Device Enumeration**: Scan and list available audio devices
- **Driver Type Detection**: Identify WDM, KS, WASAPI devices
- **Status Monitoring**: Track device state changes
- **Configuration Management**: Store and apply device settings

#### 4. Configuration Manager (`config_manager.cpp`)
- **Settings Persistence**: Save/load user preferences
- **Audio Parameters**: Manage sample rate, buffer size, bit depth
- **Device Profiles**: Store device-specific configurations
- **Real-time Updates**: Apply settings without restart

### Technology Stack
- **HTTP Server**: cpp-httplib or Crow framework
- **JSON Processing**: nlohmann/json
- **Audio Processing**: ASIO SDK, PortAudio, or RtAudio
- **WebSocket**: libwebsockets or built-in framework support
- **Build System**: CMake with modern C++17/20
- **Platform Support**: Windows (primary), with cross-platform considerations

### API Specification

#### REST Endpoints
```
GET    /api/devices           - List available audio devices
GET    /api/devices/{id}      - Get device details
POST   /api/devices/{id}/activate - Activate device
GET    /api/config            - Get current configuration
PUT    /api/config            - Update configuration
GET    /api/status            - Get system status
POST   /api/audio/play        - Start audio playback
POST   /api/audio/stop        - Stop audio playback
POST   /api/audio/upload      - Upload audio file
```

#### WebSocket Events
```
audio_data     - Real-time audio visualization data
latency_update - Current system latency
device_change  - Device status change notification
config_change  - Configuration update notification
```

## Development Roadmap

### Phase 1: Foundation ✓
- [x] Angular prototype development
- [x] UI/UX design implementation
- [x] Basic audio simulation
- [x] Device management mockup

### Phase 2: Documentation & Architecture
- [ ] Comprehensive documentation
- [ ] C++ architecture design
- [ ] API specification
- [ ] Build system setup

### Phase 3: C++ Backend Implementation
- [ ] Web server foundation
- [ ] Audio engine core
- [ ] Device management system
- [ ] Configuration management

### Phase 4: Integration & Testing
- [ ] Frontend-backend integration
- [ ] Real audio hardware testing
- [ ] Performance optimization
- [ ] Cross-platform compatibility

### Phase 5: Advanced Features
- [ ] Plugin system architecture
- [ ] Advanced audio processing
- [ ] Professional DAW integration
- [ ] Remote configuration capabilities

## Installation & Setup

### Current Angular Version
```bash
# Prerequisites
Node.js 16+ required

# Installation
npm install

# Development
npm run dev

# Production
npm run build
```

### Planned C++ Version
```bash
# Prerequisites
C++17 compiler, CMake 3.16+, ASIO SDK

# Build
mkdir build && cd build
cmake ..
make

# Run
./cyberasio-core --port 7788
```

## Configuration Files

### Development Configuration
- `angular.json` - Angular CLI configuration
- `package.json` - Node.js dependencies
- `tsconfig.json` - TypeScript compiler options

### Planned C++ Configuration
- `CMakeLists.txt` - Build configuration
- `config.json` - Runtime configuration
- `devices.json` - Device profiles

## Contributing

### Code Style
- **Angular**: Follow Angular style guide
- **C++**: Google C++ Style Guide
- **CSS**: BEM methodology with Tailwind utilities
- **Documentation**: Markdown with clear examples

### Testing Strategy
- **Unit Tests**: Jest for Angular, Google Test for C++
- **Integration Tests**: End-to-end with Playwright
- **Performance Tests**: Audio latency benchmarks
- **Cross-platform Tests**: Windows, macOS, Linux validation

## License

MIT License - See LICENSE file for details.

## Contact

Project maintained by GizzZmo - Advanced audio engineering with cyberpunk aesthetics.