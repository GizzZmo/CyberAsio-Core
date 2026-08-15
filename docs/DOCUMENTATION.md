# CyberASIO Core — Documentation Guide

> Complete reference for users, developers, and operators of the CyberASIO Core project.

## Table of Contents

1. [Introduction](#1-introduction)
2. [Getting Started](#2-getting-started)
3. [User Guide](#3-user-guide)
4. [Developer Guide](#4-developer-guide)
5. [Build & Deployment](#5-build--deployment)
6. [CI, Artifacts & Screenshots](#6-ci-artifacts--screenshots)
7. [Troubleshooting](#7-troubleshooting)
8. [FAQ](#8-faq)
9. [Contributing](#9-contributing)
10. [Changelog & Versioning](#10-changelog--versioning)

---

## 1. Introduction

**CyberASIO Core** is a simulated professional audio control panel that recreates the look and feel of a modern ASIO driver configuration utility, wrapped in a cyberpunk aesthetic.

### Goals

- Provide an interactive, educational, and visually striking prototype of low-latency audio settings management.
- Demonstrate real-time audio analysis and visualization using the Web Audio API.
- Serve as a foundation for a future native C++ ASIO / WASAPI backend.
- Generate reliable visual assets (screenshots) and build artifacts via continuous integration.

### Target Audience

- Audio engineers exploring UI concepts for driver panels
- Developers learning Angular + Web Audio
- Creative technologists building cyberpunk interfaces
- Anyone needing a polished local-file audio test player with visual feedback

---

## 2. Getting Started

### Requirements

| Component       | Version / Notes                  |
|-----------------|----------------------------------|
| Node.js         | ≥ 20.x (LTS recommended)         |
| npm             | Comes with Node                  |
| Browser         | Chrome / Edge / Firefox (latest) |
| OS              | Windows 11 preferred (any modern OS works for the web UI) |

### Installation

```bash
git clone https://github.com/GizzZmo/CyberAsio-Core.git
cd CyberAsio-Core
npm install
```

### First Run

```bash
npm run dev
```

The development server starts (default port 4200). Open the printed URL.

The panel boots in **ONLINE** state with four virtual devices. Select a device, adjust settings, load an audio file, and press **PLAY**.

---

## 3. User Guide

### Main Interface Regions

1. **Header** — Title, version, driver status indicator (green = ONLINE).
2. **Sound Devices** — List of virtual devices. Click to activate (disabled devices cannot be selected).
3. **Core Settings**
   - Sample Rate buttons
   - Buffer Size slider (updates live latency)
   - Bit Depth buttons
4. **Status Cards** — Latency (ms), Sample Rate, Buffer size, Bit Depth.
5. **System Playback Test**
   - Visualization mode switcher (Bars / Spectrogram / Waveform)
   - Canvas visualizer
   - LOAD FILE + PLAY / PAUSE controls

### Loading Audio

1. Click **LOAD FILE**.
2. Choose any supported audio file (mp3, wav, ogg, etc.).
3. The filename appears under “NOW PLAYING”.
4. Press **PLAY**. Visualizations update in real time.

### Understanding Latency

Latency is computed as:

```
latency_ms = (buffer_size / sample_rate) * 1000 * 2   // input + output
```

Lower buffer sizes produce lower latency but require more CPU and may underrun on slow systems.

### Visualization Modes

| Mode        | Description                                      |
|-------------|--------------------------------------------------|
| Bars        | 32 smoothed frequency bars with neon gradient    |
| Spectrogram | Scrolling frequency intensity over time          |
| Waveform    | Classic time-domain oscilloscope line            |

---

## 4. Developer Guide

See also [DEVELOPMENT.md](DEVELOPMENT.md) and [ARCHITECTURE.md](ARCHITECTURE.md).

### Project Structure

```
.
├── .github/workflows/     # CI & screenshot pipelines
├── cpp-backend/           # Native binary reference (source TBD)
├── docs/                  # This documentation set
├── src/
│   ├── app.component.ts   # Main logic, signals, Web Audio
│   ├── app.component.html # Template
│   └── app.component.css  # (mostly empty – Tailwind used)
├── index.html             # Global styles + Tailwind CDN + fonts
├── index.tsx              # Bootstrap
├── angular.json
├── package.json
└── tsconfig.json
```

### Key Technologies

- **Angular 20** (standalone components, signals, zoneless change detection)
- **Tailwind CSS** (via CDN in index.html for rapid styling)
- **Web Audio API** (AnalyserNode for visualizations)
- **Orbitron + Share Tech Mono** fonts (Google Fonts)

### Important Signals (Angular)

- `driverStatus`, `devices`, `selectedDeviceId`
- `selectedSampleRate`, `selectedBufferSize`, `selectedBitDepth`
- `isPlaying`, `visualizationType`, `currentFileName`
- Computed: `selectedDevice`, `latency`

### Extending the Visualizer

Add a new mode in `VisualizationType`, create a draw method, and wire it in the template `@switch` and the `runVisualizer` loop.

---

## 5. Build & Deployment

```bash
npm run build          # production → ./dist
npm run preview        # serve production build
```

The output is a static SPA that can be hosted on any static file server, GitHub Pages, Cloudflare Pages, etc.

AI Studio link remains available for the original hosted version.

---

## 6. CI, Artifacts & Screenshots

See the dedicated guide: [CI_AND_ARTIFACTS.md](CI_AND_ARTIFACTS.md).

Summary:

- Every push/PR builds the app and uploads `dist` as an artifact.
- A scheduled / manual / push-to-main workflow generates screenshots of the live UI and uploads them.
- Artifacts are downloadable from the Actions UI and can be promoted into `docs/assets/` if desired.

---

## 7. Troubleshooting

| Symptom                        | Possible Cause                     | Fix                                      |
|--------------------------------|------------------------------------|------------------------------------------|
| No audio / visualizer silent   | Browser autoplay policy            | Click PLAY after user gesture            |
| Canvas blank                   | Context not ready                  | Switch visualization mode once           |
| High latency shown             | Large buffer size                  | Lower buffer slider                      |
| `ng serve` fails               | Node version                       | Use Node 20+                             |
| CORS on external demo audio    | Cross-origin                       | Load a local file instead                |

---

## 8. FAQ

**Is this a real ASIO driver?**  
No. It is a high-quality simulation of the control panel and audio path using Web Audio. A native C++ backend is planned.

**Can I use it with real audio interfaces?**  
Currently the UI is simulated. Future versions will bridge to real devices via the C++ component.

**Does it work offline?**  
Yes, after `npm run build` the static files work offline (except external fonts if not cached).

**Why is there a binary in `cpp-backend/build`?**  
It is a reference native executable. Source will be published in a future release.

---

## 9. Contributing

1. Fork the repository.
2. Create a feature branch.
3. Follow the coding style in existing TypeScript (signals, standalone components).
4. Add or update documentation as needed.
5. Ensure CI passes.
6. Open a Pull Request.

See [DEVELOPMENT.md](DEVELOPMENT.md) for more.

---

## 10. Changelog & Versioning

Semantic versioning is used once the project leaves the 0.x series.

Current UI version shown in the header: **v1.2.0**.

Check the [Releases](https://github.com/GizzZmo/CyberAsio-Core/releases) page for tagged versions and notes.
