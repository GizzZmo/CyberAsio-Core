
import { Component, ChangeDetectionStrategy, signal, computed, effect, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';

interface SoundDevice {
  id: number;
  name: string;
  type: 'WDM' | 'KS' | 'WASAPI';
  status: 'Active' | 'Inactive' | 'Disabled';
}

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
  changeDetection: ChangeDetectionStrategy.OnPush,
  standalone: true,
  imports: [CommonModule],
})
export class AppComponent implements OnDestroy {
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
  
  private audio: HTMLAudioElement | null = null;
  private visualizerInterval: any;
  private objectUrl: string | null = null;

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
    // Simplified latency calculation for simulation
    const inputLatency = buffer / rate * 1000;
    const outputLatency = buffer / rate * 1000;
    return parseFloat((inputLatency + outputLatency).toFixed(2));
  });

  // --- UI & MOCK DATA ---
  visualizerBars = signal<number[]>(Array(32).fill(10));

  constructor() {
    this.bootSystem();
    if (typeof window !== 'undefined') {
        this.audio = new Audio('https://interactive-examples.mdn.mozilla.net/media/cc0-audio/t-rex-roar.mp3');
        this.audio.loop = true;
        this.audio.onplay = () => this.isPlaying.set(true);
        this.audio.onpause = () => this.isPlaying.set(false);
    }
  }
  
  ngOnDestroy() {
    if (this.audio) {
      this.audio.pause();
      this.audio = null;
    }
    clearInterval(this.visualizerInterval);
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

  setSampleRate(rate: number) {
    this.selectedSampleRate.set(rate);
  }

  setBufferSize(size: number) {
    this.selectedBufferSize.set(size);
  }

  setBitDepth(depth: number) {
    this.selectedBitDepth.set(depth);
  }
  
  onFileSelected(event: Event): void {
    const input = event.target as HTMLInputElement;
    if (!input.files || input.files.length === 0) {
      return;
    }

    const file = input.files[0];
    
    if (this.isPlaying()) {
      this.togglePlayback();
    }

    if (this.objectUrl) {
      URL.revokeObjectURL(this.objectUrl);
    }

    this.objectUrl = URL.createObjectURL(file);
    
    if (this.audio) {
        this.audio.src = this.objectUrl;
        this.currentFileName.set(file.name);
    }
  }

  togglePlayback() {
    if (!this.audio) return;

    if (this.isPlaying()) {
      this.audio.pause();
      clearInterval(this.visualizerInterval);
      this.visualizerBars.set(Array(32).fill(10));
    } else {
      this.audio.play().catch(e => console.error("Error playing audio:", e));
      this.visualizerInterval = setInterval(() => {
        const newBars = Array.from({ length: 32 }, () => Math.random() * 90 + 10);
        this.visualizerBars.set(newBars);
      }, 100);
    }
  }
  
  getBufferSizeLabel(size: number): string {
    return `${size} smp`;
  }
  
  getSampleRateLabel(rate: number): string {
    return `${(rate / 1000).toFixed(1)} kHz`;
  }
}
