<div align="center">

# CyberASIO Core

**A cyberpunk-inspired simulated ASIO control panel for low-latency audio on Windows 11**

[![CI](https://github.com/GizzZmo/CyberAsio-Core/actions/workflows/ci.yml/badge.svg)](https://github.com/GizzZmo/CyberAsio-Core/actions/workflows/ci.yml)
[![Screenshots](https://github.com/GizzZmo/CyberAsio-Core/actions/workflows/screenshots.yml/badge.svg)](https://github.com/GizzZmo/CyberAsio-Core/actions/workflows/screenshots.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-cyan.svg)](LICENSE)

[Live Demo (AI Studio)](https://ai.studio/apps/drive/16P4i7K46yrItL6SenVPDSkb-6CGLL2vq) · [Documentation](docs/DOCUMENTATION.md) · [Architecture](docs/ARCHITECTURE.md)

</div>

---

## Overview

**CyberASIO Core** is a high-fidelity simulated control panel for a futuristic ASIO-style audio driver.  
It features a neon cyberpunk UI that lets you manage low-latency audio settings, select virtual sound devices, adjust sample rates / buffer sizes / bit depths, and test local file playback with real-time visualizations (bars, spectrogram, waveform).

Built as an Angular 20 application with Web Audio API for the frontend and a C++ backend binary component for future native ASIO integration. Ideal for prototyping professional audio workflows, education, or creative tech experiments on Windows 11 (and modern browsers).

> **Current status**: Full interactive simulation via browser. The `cpp-backend` binary is included as a reference native component (source forthcoming).

## Features

- **Cyberpunk Control Panel** — Orbitron + Share Tech Mono typography, cyan/fuchsia neon aesthetic, glow effects
- **Virtual Sound Devices** — WDM / KS / WASAPI style devices with Active / Inactive / Disabled states
- **Core ASIO Settings**
  - Sample rates: 44.1 / 48 / 88.2 / 96 / 192 kHz
  - Buffer sizes: 32 → 2048 samples (with live latency calculation)
  - Bit depths: 16 / 24 / 32-bit
- **System Playback Test**
  - Load any local audio file
  - Play / Pause with Web Audio API
  - Three visualization modes: Frequency Bars, Spectrogram, Waveform
- **Real-time Latency Display** — calculated from buffer size + sample rate
- **Responsive Layout** — works on desktop and mobile viewports
- **CI-generated Screenshots & Artifacts** — automated visual assets and build outputs

## Quick Start

### Prerequisites

- Node.js 20+ (recommended)
- Modern browser with Web Audio API support

### Run Locally

```bash
# Clone
git clone https://github.com/GizzZmo/CyberAsio-Core.git
cd CyberAsio-Core

# Install
npm install

# (Optional) Gemini key if using AI Studio features
# echo "GEMINI_API_KEY=your_key" > .env.local

# Development server
npm run dev
```

Open http://localhost:4200 (or the port shown by `ng serve`).

### Production Build

```bash
npm run build
# Output in ./dist
```

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    CyberASIO Core                            │
├──────────────────────────┬──────────────────────────────────┤
│  Angular 20 Frontend     │  C++ Backend (native reference)  │
│  ────────────────────    │  ────────────────────────────    │
│  • Device management     │  • cyberasio-core binary         │
│  • Settings UI           │  • Future ASIO / PortAudio bridge│
│  • Web Audio visualizer  │  • Low-latency path (planned)    │
│  • Local file playback   │                                  │
└──────────────────────────┴──────────────────────────────────┘
```

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for full details.

## Documentation

| Document | Description |
|----------|-------------|
| [Documentation Guide](docs/DOCUMENTATION.md) | Complete user & developer guide |
| [Architecture](docs/ARCHITECTURE.md) | System design, components, data flow |
| [CI & Artifacts](docs/CI_AND_ARTIFACTS.md) | Workflows, screenshot generation, artifact handling |
| [Development](docs/DEVELOPMENT.md) | Local setup, coding standards, contribution |

## CI / Artifacts / Screenshots

This repository includes a full CI system:

- **ci.yml** — Builds the Angular app on every push/PR and uploads the `dist/` folder as a downloadable artifact.
- **screenshots.yml** — Spins up the app, captures high-quality screenshots of the control panel in different visualization modes, and publishes them as workflow artifacts (and optionally to the `docs/assets` path on main).

Artifacts are retained for 30 days and can be downloaded from the Actions tab.

See [docs/CI_AND_ARTIFACTS.md](docs/CI_AND_ARTIFACTS.md) for how to extend the pipeline (e.g. visual regression, multi-browser, or native binary packaging).

## Roadmap

- [ ] Publish full C++ source for the native backend
- [ ] Real ASIO / WASAPI exclusive mode bridge via Node-API or WebAssembly
- [ ] MIDI device support and routing matrix
- [ ] Preset save / load (localStorage + export)
- [ ] Dark / light theme toggle + more visualization modes
- [ ] Automated visual regression tests from the screenshot job

## License

MIT © GizzZmo / Jon-Arve Constantine Grønsberg-Ovesen

---

<div align="center">
Made with ❤️ and cyan neon for the audio community
</div>
