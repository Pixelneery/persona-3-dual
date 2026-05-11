# Audio System

Audio is handled by `MusicController` (`source/controllers/MusicController.h`), a wrapper around **Maxmod** for SFX and a custom **PCM streaming** system for background music.

---

## Music

### Format

All music is stored as raw PCM in `nitrofiles/music/`. Source files are `.mp3` in `assets/music/` and are converted automatically by the Makefile via FFmpeg:

```
ffmpeg -i input.mp3 -f s16le -ar 32000 -ac 2 output.pcm
```

- **Sample rate:** 32,000 Hz
- **Channels:** Stereo (2)
- **Bit depth:** 16-bit signed

### Playing Music

```cpp
musicCtrl.init(filePath, loopStartSeconds, loopEndSeconds);
```

| Parameter | Description |
|-----------|-------------|
| `filePath` | Path to `.pcm` in NitroFS, e.g. `"nitro:/music/tightrope.pcm"` |
| `loopStartSeconds` | Time in seconds to loop back to when the loop end is reached |
| `loopEndSeconds` | Time in seconds to trigger the loop. Use `-1.0f` to play through without looping |

`musicCtrl.update()` must be called every frame (including inside fade loops) to keep the audio stream alive.

### Adding New Music

1. Drop the `.mp3` into `assets/music/`
2. Run `make`, it converts automatically to `nitrofiles/music/<name>.pcm`
3. Reference it in code as `"nitro:/music/<name>.pcm"`

To find loop points, open the file in Audacity and locate the loop region visually. For extended/full-play tracks, use `0.0f` as loop start and the full duration as loop end (get duration via `ffprobe -v quiet -show_entries format=duration -of csv=p=0 input.mp3`).

---

## SFX

SFX uses Maxmod directly. Effects are defined in `assets/sfx/` and compiled into `soundbank.bin`.

```cpp
musicCtrl.loadSFX(SFX_SELECT);                        // load effect
mm_sfxhand h = musicCtrl.playSFX(SFX_SELECT, 255, 128); // play (volume, panning)
musicCtrl.stopSFX(h);                                 // stop
```

---

## Random Music Selection

The random seed is set once at boot in `main.cpp` using the DS hardware timer:

```cpp
TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
srand(TIMER0_DATA);
```

This gives a different seed each run since `TIMER0_DATA` counts CPU cycles from power-on.

### Current Random Selections

#### Intro Screen (`IntroView`)
Tightrope plays on the title/intro screen with a **50/50** chance between two versions:

| Track | File | Chance | Loop Start | Loop End |
|-------|------|--------|------------|----------|
| Tightrope | `tightrope.pcm` | 50% | 17.962s | 66.082s |
| Tightrope (Floor Mix) | `tightrope_floor_mix.pcm` | 50% | 0.0s | 295.706s (full play) |

#### Intro Video (`IntroVideoView`)
A random intro video plays with equal probability each boot:

| Video | File | Chance |
|-------|------|--------|
| Persona 3 FES intro | `fes.vid` | 33.3% |
| Persona 3 base intro | `base.vid` | 33.3% |
| Persona 3 Portable intro | `portable.vid` | 33.3% |

Video audio is embedded in the `.vid` file and handled separately via `musicCtrl.initVideoAudio()`.

#### Iwatodai Dorm (`IwatodaiDormView`)
Normally plays the dorm theme, with a rare easter egg:

| Track | File | Chance | Loop Start | Loop End |
|-------|------|--------|------------|----------|
| Iwatodai Dorm | `iwatodai_dorm.pcm` | 66.6% | 0.0s | 920.973s (full play) |
| Tom's Diner (Suzanne Vega) | `toms_diner_suzanne_vega.pcm` | 33.3% | 0.0s | 234.568s (full play) |

### Adding a New Random Selection

```cpp
// equal chance between N tracks
const char* tracks[] = {"nitro:/music/a.pcm", "nitro:/music/b.pcm"};
musicCtrl.init(tracks[rand() % 2], loopStart, loopEnd);

// weighted (e.g. 1/3 chance of easter egg)
if (rand() % 3 == 0) {
    musicCtrl.init("nitro:/music/easter_egg.pcm", 0.0f, duration);
} else {
    musicCtrl.init("nitro:/music/normal.pcm", 0.0f, duration);
}

// per-track loop points
const char* tracks[] = {"nitro:/music/a.pcm", "nitro:/music/b.pcm"};
const float loopStarts[] = {17.962f, 0.0f};
const float loopEnds[]   = {66.082f, 295.706f};
int pick = rand() % 2;
musicCtrl.init(tracks[pick], loopStarts[pick], loopEnds[pick]);
```
