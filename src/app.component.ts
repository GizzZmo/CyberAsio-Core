import { Component, ChangeDetectionStrategy, signal, computed, effect, OnDestroy, ViewChild, ElementRef } from '@angular/core';
import { CommonModule } from '@angular/common';

interface SoundDevice {
  id: number;
  name: string;
  type: 'WDM' | 'KS' | 'WASAPI';
  status: 'Active' | 'Inactive' | 'Disabled';
}

type VisualizationType = 'BARS' | 'SPECTROGRAM' | 'WAVEFORM';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
  changeDetection: ChangeDetectionStrategy.OnPush,
  standalone: true,
  imports: [CommonModule],
})
export class AppComponent implements OnDestroy {
  @ViewChild('spectrogramCanvas') spectrogramCanvas?: ElementRef<HTMLCanvasElement>;
  @ViewChild('waveformCanvas') waveformCanvas?: ElementRef<HTMLCanvasElement>;
  @ViewChild('barsCanvas') barsCanvas?: ElementRef<HTMLCanvasElement>;
  
  // --- STATE SIGNALS ---
  driverStatus = signal<'ONLINE' | 'OFFLINE' | 'ERROR'>('OFFLINE');
  devices = signal<SoundDevice[]>([]);
  selectedDeviceId = signal<number | null>(null);
  
  sampleRates = [44100, 48000, 88200, 96000, 192000];
  selectedSampleRate = signal(48000);

  bufferSizes = [32, 64, 128, 256, 512, 1024, 2048];
  selectedBufferSize = signal(256);

  bitDepths = [16, 24, 32];
  selectedBitDepth = signal(24);

  isPlaying = signal(false);
  currentFileName = signal('T-Rex Roar (Default)');
  
  visualizationType = signal<VisualizationType>('BARS');
  
  private audio: HTMLAudioElement | null = null;
  private objectUrl: string | null = null;

  // Web Audio API properties
  private audioContext: AudioContext | null = null;
  private analyser: AnalyserNode | null = null;
  private source: MediaElementAudioSourceNode | null = null;
  private frequencyDataArray: Uint8Array | null = null;
  private timeDomainDataArray: Uint8Array | null = null;
  private animationFrameId: number | null = null;
  
  // Canvas contexts
  private spectrogramCtx: CanvasRenderingContext2D | null = null;
  private waveformCtx: CanvasRenderingContext2D | null = null;
  private barsCtx: CanvasRenderingContext2D | null = null;
  private smoothedBarHeights: number[] = Array(32).fill(0);

  // --- DERIVED STATE (COMPUTED SIGNALS) ---
  selectedDevice = computed(() => {
    const id = this.selectedDeviceId();
    if (id === null) return null;
    return this.devices().find(d => d.id === id) || null;
  });

  latency = computed(() => {
    const buffer = this.selectedBufferSize();
    const rate = this.selectedSampleRate();
    if (this.driverStatus() !== 'ONLINE' || !this.selectedDevice()) {
      return 0;
    }
    const inputLatency = buffer / rate * 1000;
    const outputLatency = buffer / rate * 1000;
    return parseFloat((inputLatency + outputLatency).toFixed(2));
  });

  constructor() {
    this.bootSystem();
    if (typeof window !== 'undefined') {
        this.audio = new Audio('https://interactive-examples.mdn.mozilla.net/media/cc0-audio/t-rex-roar.mp3');
        this.audio.crossOrigin = 'anonymous';
        this.audio.loop = true;
        this.audio.onplay = () => this.isPlaying.set(true);
        this.audio.onpause = () => this.isPlaying.set(false);

        effect(() => {
          if (this.isPlaying()) {
            this.runVisualizer();
          } else {
            if (this.animationFrameId) {
              cancelAnimationFrame(this.animationFrameId);
              this.animationFrameId = null;
            }
            this.smoothedBarHeights.fill(0);
            this.clearCanvases();
          }
        });

        // This effect will run whenever the visualizationType changes,
        // ensuring the context is set up *after* the canvas element is in the DOM.
        effect(() => {
          const type = this.visualizationType();
          // Use a timeout to ensure the view has stabilized and clientWidth/clientHeight are correct.
          setTimeout(() => {
            if (type === 'BARS' && this.barsCanvas) {
              const canvas = this.barsCanvas.nativeElement;
              canvas.width = canvas.clientWidth;
              canvas.height = canvas.clientHeight;
              this.barsCtx = canvas.getContext('2d');
            }
            if (type === 'SPECTROGRAM' && this.spectrogramCanvas) {
              const canvas = this.spectrogramCanvas.nativeElement;
              canvas.width = canvas.clientWidth;
              canvas.height = canvas.clientHeight;
              this.spectrogramCtx = canvas.getContext('2d');
            }
            if (type === 'WAVEFORM' && this.waveformCanvas) {
              const canvas = this.waveformCanvas.nativeElement;
              canvas.width = canvas.clientWidth;
              canvas.height = canvas.clientHeight;
              this.waveformCtx = canvas.getContext('2d');
            }
          });
        });
    }
  }

  ngOnDestroy() {
    if (this.animationFrameId) {
      cancelAnimationFrame(this.animationFrameId);
    }
    if (this.audio) {
      this.audio.pause();
      this.audio = null;
    }
    this.source?.disconnect();
    this.analyser?.disconnect();
    this.audioContext?.close();
    if (this.objectUrl) {
      URL.revokeObjectURL(this.objectUrl);
    }
  }

  bootSystem() {
    this.driverStatus.set('ONLINE');
    this.devices.set([
      { id: 1, name: 'Generic HD Audio Device (WDM)', type: 'WDM', status: 'Inactive' },
      { id: 2, name: 'Realtek ASIO (KS)', type: 'KS', status: 'Disabled' },
      { id: 3, name: 'NVIDIA Broadcast (WASAPI)', type: 'WASAPI', status: 'Inactive' },
      { id: 4, name: 'Focusrite USB ASIO (WDM)', type: 'WDM', status: 'Inactive' },
    ]);
    this.selectDevice(1);
  }
  
  selectDevice(id: number) {
    if (this.driverStatus() !== 'ONLINE') return;
    this.devices.update(currentDevices => 
      currentDevices.map(device => 
        device.id === id 
          ? { ...device, status: 'Active' } 
          : { ...device, status: device.status === 'Disabled' ? 'Disabled' : 'Inactive' }
      )
    );
    this.selectedDeviceId.set(id);
  }

  setSampleRate(rate: number) { this.selectedSampleRate.set(rate); }
  setBufferSize(size: number) { this.selectedBufferSize.set(size); }
  setBitDepth(depth: number) { this.selectedBitDepth.set(depth); }
  setVisualizationType(type: VisualizationType) { this.visualizationType.set(type); }
  
  onFileSelected(event: Event): void {
    const input = event.target as HTMLInputElement;
    if (!input.files || input.files.length === 0) return;

    const file = input.files[0];
    if (this.isPlaying()) this.togglePlayback();
    if (this.objectUrl) URL.revokeObjectURL(this.objectUrl);

    this.objectUrl = URL.createObjectURL(file);
    if (this.audio) {
        this.audio.src = this.objectUrl;
        this.currentFileName.set(file.name);
    }
  }

  private setupAudioContext(): void {
    if (this.audioContext || !this.audio) return;
    this.audioContext = new AudioContext();
    this.source = this.audioContext.createMediaElementSource(this.audio);
    this.analyser = this.audioContext.createAnalyser();

    this.source.connect(this.analyser);
    this.analyser.connect(this.audioContext.destination);

    this.analyser.fftSize = 1024;
    const frequencyBinCount = this.analyser.frequencyBinCount;
    this.frequencyDataArray = new Uint8Array(frequencyBinCount);
    this.timeDomainDataArray = new Uint8Array(this.analyser.fftSize);
  }

  private runVisualizer(): void {
    const draw = () => {
      this.animationFrameId = requestAnimationFrame(draw);
      if (!this.analyser) return;

      switch(this.visualizationType()) {
          case 'BARS':
              this.drawBars();
              break;
          case 'SPECTROGRAM':
              this.drawSpectrogram();
              break;
          case 'WAVEFORM':
              this.drawWaveform();
              break;
      }
    };
    draw();
  }

  private drawBars(): void {
    if (!this.barsCtx || !this.analyser || !this.frequencyDataArray) return;
    
    const canvas = this.barsCtx.canvas;
    this.analyser.getByteFrequencyData(this.frequencyDataArray);
    
    this.barsCtx.clearRect(0, 0, canvas.width, canvas.height);
    
    const barCount = 32;
    // Spacing between bars is 15% of the total slot width for a bar.
    const barWidth = (canvas.width / barCount) * 0.85;
    const barSpacing = canvas.width / barCount - barWidth;
    const binsPerBar = Math.floor(this.analyser.frequencyBinCount / barCount);
    const smoothingFactor = 0.8;
    
    let x = 0;

    // Create gradient for the bars
    const gradient = this.barsCtx.createLinearGradient(0, canvas.height, 0, 0);
    gradient.addColorStop(0, '#d946ef'); // fuchsia-600
    gradient.addColorStop(1, '#22d3ee'); // cyan-400
    this.barsCtx.fillStyle = gradient;

    // Add glow effect
    this.barsCtx.shadowColor = '#d946ef';
    this.barsCtx.shadowBlur = 8;
    
    for (let i = 0; i < barCount; i++) {
        let sum = 0;
        for (let j = 0; j < binsPerBar; j++) {
            sum += this.frequencyDataArray[i * binsPerBar + j];
        }
        const avg = sum / binsPerBar;
        const targetHeight = (avg / 255) * canvas.height;
        
        // Apply smoothing
        this.smoothedBarHeights[i] = this.smoothedBarHeights[i] * smoothingFactor + targetHeight * (1 - smoothingFactor);
        const barHeight = Math.max(1, this.smoothedBarHeights[i]);
        
        const y = canvas.height - barHeight;
        
        this.barsCtx.fillRect(x, y, barWidth, barHeight);
        x += barWidth + barSpacing;
    }

    // Reset shadow
    this.barsCtx.shadowBlur = 0;
    this.barsCtx.shadowColor = 'transparent';
  }

  private drawWaveform(): void {
    if (!this.waveformCtx || !this.analyser || !this.timeDomainDataArray) return;

    const canvas = this.waveformCtx.canvas;
    this.analyser.getByteTimeDomainData(this.timeDomainDataArray);
    
    this.waveformCtx.clearRect(0, 0, canvas.width, canvas.height);
    this.waveformCtx.lineWidth = 2;
    this.waveformCtx.strokeStyle = 'rgb(0, 242, 234)';
    this.waveformCtx.beginPath();
    
    const sliceWidth = (canvas.width * 1.0) / this.analyser.fftSize;
    let x = 0;

    for (let i = 0; i < this.analyser.fftSize; i++) {
        const v = this.timeDomainDataArray[i] / 128.0;
        const y = (v * canvas.height) / 2;
        if (i === 0) {
            this.waveformCtx.moveTo(x, y);
        } else {
            this.waveformCtx.lineTo(x, y);
        }
        x += sliceWidth;
    }
    this.waveformCtx.lineTo(canvas.width, canvas.height / 2);
    this.waveformCtx.stroke();
  }

  private drawSpectrogram(): void {
      if (!this.spectrogramCtx || !this.analyser || !this.frequencyDataArray) return;

      const canvas = this.spectrogramCtx.canvas;
      const ctx = this.spectrogramCtx;
      this.analyser.getByteFrequencyData(this.frequencyDataArray);
      
      // Shift existing image left
      const imageData = ctx.getImageData(1, 0, canvas.width - 1, canvas.height);
      ctx.putImageData(imageData, 0, 0);

      // Draw new frequency data on the right edge
      for (let i = 0; i < this.frequencyDataArray.length; i++) {
          const value = this.frequencyDataArray[i];
          const y = canvas.height - (i / this.frequencyDataArray.length) * canvas.height;
          ctx.fillStyle = this.getColorForFrequencyValue(value);
          ctx.fillRect(canvas.width - 1, y, 1, 1);
      }
  }
  
  private getColorForFrequencyValue(value: number): string {
    const percent = value / 255;
    let r = 0, g = 0, b = 0;
    if (percent < 0.25) { // Dark blue to bright blue
        b = Math.round(percent * 4 * 255);
    } else if (percent < 0.5) { // Blue to cyan
        g = Math.round((percent - 0.25) * 4 * 255);
        b = 255;
    } else if (percent < 0.75) { // Cyan to green
        g = 255;
        b = 255 - Math.round((percent - 0.5) * 4 * 255);
    } else { // Green to yellow to red
        r = Math.round((percent - 0.75) * 4 * 255);
        g = 255;
    }
    return `rgb(${r}, ${g}, ${b})`;
  }

  private clearCanvases() {
    this.barsCtx?.clearRect(0, 0, this.barsCtx.canvas.width, this.barsCtx.canvas.height);
    this.spectrogramCtx?.clearRect(0, 0, this.spectrogramCtx.canvas.width, this.spectrogramCtx.canvas.height);
    this.waveformCtx?.clearRect(0, 0, this.waveformCtx.canvas.width, this.waveformCtx.canvas.height);
  }

  togglePlayback() {
    if (!this.audio) return;

    if (this.isPlaying()) {
      this.audio.pause();
    } else {
      if (!this.audioContext) this.setupAudioContext();
      if (this.audioContext && this.audioContext.state === 'suspended') {
        this.audioContext.resume();
      }
      this.audio.play().catch(e => console.error("Error playing audio:", e));
    }
  }
  
  getBufferSizeLabel(size: number): string { return `${size} smp`; }
  getSampleRateLabel(rate: number): string { return `${(rate / 1000).toFixed(1)} kHz`; }
}
