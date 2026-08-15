# Architecture

## High-Level Overview

CyberASIO Core consists of two logical layers:

1. **Frontend (Angular 20 SPA)** – the interactive cyberpunk control panel and Web Audio engine.
2. **Native Backend (C++ reference)** – a compiled binary currently shipped for future integration; source not yet public.

```
┌──────────────────────────────────────────────────────────────────────┐
│                         User Browser                                  │
│  ┌────────────────────────────────────────────────────────────────┐  │
│  │  AppComponent (signals + effects)                               │  │
│  │  ├── Device list & selection                                    │  │
│  │  ├── Settings (sample rate / buffer / bit depth)                │  │
│  │  ├── Latency calculator (computed signal)                       │  │
│  │  ├── File loader → HTMLAudioElement                             │  │
│  │  └── Visualizer pipeline                                        │  │
│  │       ├── AudioContext + AnalyserNode                           │  │
│  │       ├── Bars / Spectrogram / Waveform drawers                 │  │
│  │       └── requestAnimationFrame loop                            │  │
│  └────────────────────────────────────────────────────────────────┘  │
│                           ▲                                           │
│                           │ (future IPC / WASM / N-API)               │
│  ┌────────────────────────────────────────────────────────────────┐  │
│  │  cpp-backend/cyberasio-core (native binary)                     │  │
│  │  Planned: real device enumeration, exclusive mode, low-latency  │  │
│  └────────────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────────────┘
```

## Frontend Component Model

- **Standalone** `AppComponent` (no NgModules).
- **OnPush** + **signals** + **zoneless** change detection for maximum performance.
- Template uses Angular control flow (`@for`, `@if`, `@switch`).
- Styling: Tailwind utility classes + a small set of global CSS customizations in `index.html` (scrollbar, range inputs, fonts).

### State Management

All mutable state lives in Angular signals:

| Signal                | Type                          | Purpose                          |
|-----------------------|-------------------------------|----------------------------------|
| `driverStatus`        | `'ONLINE' \| 'OFFLINE' \| 'ERROR'` | Simulated driver health         |
| `devices`             | `SoundDevice[]`               | Virtual device list              |
| `selectedDeviceId`    | `number \| null`              | Currently active device          |
| `selectedSampleRate`  | `number`                      | Hz                               |
| `selectedBufferSize`  | `number`                      | Samples                          |
| `selectedBitDepth`    | `number`                      | Bits                             |
| `isPlaying`           | `boolean`                     | Playback state                   |
| `visualizationType`   | `'BARS' \| 'SPECTROGRAM' \| 'WAVEFORM'` | Active visualizer          |
| `currentFileName`     | `string`                      | Loaded file display name         |

Derived state uses `computed()`:

- `selectedDevice`
- `latency` (ms)

Side effects (audio context setup, animation loop start/stop, canvas resize) are handled with `effect()`.

## Audio Pipeline

```
HTMLAudioElement ──► MediaElementAudioSourceNode ──► AnalyserNode ──► Destination
                                              │
                                              ├── getByteFrequencyData → Bars / Spectrogram
                                              └── getByteTimeDomainData → Waveform
```

- FFT size fixed at 1024.
- Bars use 32 bands with exponential smoothing.
- Spectrogram scrolls left and paints a new column of intensity-colored pixels.
- Waveform draws a continuous path.

## Native Backend (Current State)

The repository currently contains only the compiled binary under `cpp-backend/build/cyberasio-core` together with CMake build artifacts.  
This binary is a placeholder for a future native component that will:

- Enumerate real Windows audio endpoints (WASAPI / ASIO)
- Expose low-latency exclusive mode streams
- Communicate with the frontend via a local WebSocket, shared memory, or Node-API addon

Until the source is published, treat the binary as opaque reference material.

## Data Flow Summary

1. User selects device → signal update → UI reflects Active state.
2. User changes buffer/sample rate → `latency` recomputed instantly.
3. User loads file → object URL created → `HTMLAudioElement.src` set.
4. User presses Play → AudioContext resumed (if needed) → analyser connected → RAF loop starts.
5. RAF loop reads analyser data and paints the currently selected canvas.

## Future Extensions

- Replace simulated devices with real enumeration from the C++ backend.
- Add a routing matrix and multi-channel metering.
- Persist presets to localStorage or a small SQLite/JSON store.
- WebAssembly build of the C++ core for pure-browser low-latency experiments.
