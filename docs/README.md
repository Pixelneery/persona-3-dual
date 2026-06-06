# Persona 3 Dual

A Nintendo DS demake of **Persona 3**, developed in C++ using devkitPro. Based on the **Persona 3** series of games (OG, FES, Portable, Reload), and inspired by the **Persona 3 Dual** online joke.

![Stars](https://img.shields.io/github/stars/p3d-project/persona-3-dual?style=flat-square&color=gold)
![Forks](https://img.shields.io/github/forks/p3d-project/persona-3-dual?style=flat-square&color=blue)
![Last Commit](https://img.shields.io/github/last-commit/p3d-project/persona-3-dual?style=flat-square&color=green)
![License](https://img.shields.io/github/license/p3d-project/persona-3-dual?style=flat-square)

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)
![Platform](https://img.shields.io/badge/platform-Nintendo%20DS-red?style=flat-square)
![Toolchain](https://img.shields.io/badge/toolchain-devkitPro-orange?style=flat-square)
![Emulator](https://img.shields.io/badge/emulator-melonDS-purple?style=flat-square)
[![Blender](https://img.shields.io/badge/Blender-%23F5792A.svg?logo=blender&logoColor=white)](#)
[![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?logo=github-actions&logoColor=white)](#)

[![Discord](https://img.shields.io/discord/1498850477545357482?label=Discord&logo=discord&style=flat-square&color=5865F2)](https://discord.gg/Xtmu76PveV)

### Want to help? Join the [Discord!](https://discord.gg/CQnkc5gS6a) Any help, big or small, would be greatly appreciated!


<img width="444" height="519" alt="20260417_231005" src="https://github.com/user-attachments/assets/8dd225c8-f3eb-4b80-8ec4-feb381e4b71d" />

---

## Media

### Pictures

| Home | Menu | Iwatodai Dorm |
|------|------|----------------------|
| ![Home](https://github.com/user-attachments/assets/6841906c-111e-4388-b549-1a99ad301ffb) | ![Menu](https://github.com/user-attachments/assets/cf379a87-7c03-4833-87c2-b918bdbd3929) | ![Dorm](https://github.com/user-attachments/assets/bef757fc-944c-4264-b167-ce7b34a3d187) |

### Videos
[![IMAGE ALT TEXT](http://img.youtube.com/vi/4RW8ppcPK6o/0.jpg)](http://www.youtube.com/watch?v=4RW8ppcPK6o "Persona 3 Dual (First Look)")

---

## Technical Details

| | |
|---|---|
| **Platform** | Nintendo DS |
| **CPU** | ARM9 (main) / ARM7 (sound/system) |
| **Toolchain** | devkitPro (devkitARM + libnds) |
| **Language** | C++, Python |
| **Emulator** | melonDS |

---

## Running the Game

The game requires FAT filesystem support to load assets at runtime. Currently, we only support:

- **[melonDS](https://melonds.kuribo64.net/downloads.php)** (multiplatform emulator)
- **Real DS / DSi / 3DS hardware** via [TWiLight Menu++](https://wiki.ds-homebrew.com/twilightmenu/)

> NOTE: Other flashcard launchers may work, but are untested

### melonDS (Emulator)

1. Download `persona-3-dual.nds` and `sdcard.img.gz` from the latest release, & decompress `sdcard.img.gz`
**Developers**: Build the project with `make` - this produces both `persona-3-dual.nds` and `sdcard.img`.
2. In melonDS, go to **Settings → DLDI** and enable DLDI.
3. Set the SD card image path to the generated `sdcard.img`.
> **Do NOT enable "Sync SD card to folder"**. This will wipe the contents of the folder.

Now, you can open melonDS and load the `persona-3-dual.nds` ROM!

<img width="316" height="300" alt="melonDS" src="https://github.com/user-attachments/assets/d34997e6-d13f-4428-a2b6-41b5272405d7" />

### Real Hardware (DS / DSi / 3DS)

Requires [TWiLight Menu++](https://wiki.ds-homebrew.com/twilightmenu/) with DLDI patching enabled.

1. Download `persona-3-dual.nds` and `data.zip` from the latest release, & decompress `data.zip`
**Developers**: Build the project with `make` - this produces the `persona-3-dual.nds` and the content in `/data`.
2. In TWiLight Menu++ settings, ensure **DLDI access** is set to **ARM9** & the **Game Loader** is set to **nds-bootstrap**
3. On your SD card, navigate to your `/roms/nds/` folder (or equivalent).
4. Copy `persona-3-dual.nds` and the entire `/data` folder into that directory:
   ```
   /roms/nds/
   ├── persona-3-dual.nds
   └── data/
       ├── music/
       ├── video/
       └── ...
   ```
5. Launch the game through TWiLight Menu++ as normal.

---

## Developer Setup

The team has opted to use **Docker** as the official dev solution, as it wraps the entire toolchain into a single image so you get an identical build environment regardless of your OS.

### 1. Install Docker

| Platform | Download |
|----------|----------|
| Windows / macOS | [Docker Desktop](https://www.docker.com/products/docker-desktop/) |
| Linux (Ubuntu/Debian) | [See Docker's Website for your Distro](https://docs.docker.com/desktop/setup/install/linux/) |

Verify the install:
```
docker --version
```

### 2. Clone the Repo

```bash
git clone https://github.com/p3d-project/persona-3-dual.git
cd persona-3-dual
```

### 3. Set Up Code Formatting Hooks

Install [pre-commit](https://pre-commit.com) and register the hooks once:

```bash
# macOS
brew install pre-commit

# Windows / Linux
pip install pre-commit
```

```bash
pre-commit install
```

The hooks run automatically on every `git commit` and keep C/C++, Python, and web files consistently formatted. See [CONTRIBUTING.md](CONTRIBUTING.md) for more details.

### 4. Build the Docker Image

Run this **once** (or again whenever `Dockerfile` or `tools/requirements.txt` changes). It downloads and caches all dependencies:

```bash
docker build -t p3d-dev .
```

> First build takes a few minutes while devkitARM downloads. Subsequent builds use the Docker layer cache and much quicker.

### 5. Compile the ROM

```bash
# Linux / macOS
docker run --rm -v "$(pwd)":/project p3d-dev make

# Windows (Command Prompt)
docker run --rm -v "%cd%":/project p3d-dev make

# Windows (PowerShell)
docker run --rm -v "${PWD}:/project" p3d-dev make
```

This produces `persona-3-dual.nds` and `sdcard.img` in your repo folder!

### 6. Interactive Shell (optional)

If you want to poke around, run commands manually, or debug the build:

```bash
# Linux / macOS
docker run --rm -it -v "$(pwd)":/project p3d-dev

# Windows PowerShell
docker run --rm -it -v "${PWD}:/project" p3d-dev
```

You're now inside the container at `/project` (your repo). Type `exit` to leave.

### Useful Docker Commands

| Command | What it does |
|---------|--------------|
| `docker build -t p3d-dev .` | (Re)build the dev image |
| `docker images` | List images on your machine |
| `docker rmi p3d-dev` | Delete the image (frees disk space) |
| `docker ps` | List running containers |

---

## Roadmap

See the [Project Board](https://github.com/orgs/p3d-project/projects/1) for current progress and open issues.

### ✅ Major Completed Features

- [x] Basic intro sequence
- [x] 3D environment rendering with UV-mapped textures
- [x] Character model + movement controls + animation
- [x] Music/SFX playback (PCM streaming via FAT, loop points, Maxmod SFX)
- [x] Video playback (8-bit, 15fps, interleaved audio+video, ring buffer)
- [x] Custom intro video (Persona 3 Dual logo) + classic intros (P3, P3 FES, P3P)
- [x] Automated asset conversion pipeline and tooling (integrated into `make`)
- [x] Dialogue system with branching and portraits
- [x] Basic battle system
- [x] Basic collision/mapping system
- [x] Basic menu system
- [x] Basic HUD with sprites
- [x] Basic battle system with console UI
- [x] Basic save system

### 🎯 Milestone 1: The Initial Demo

**Goal:** Lay the foundation of the game. Focused on implementing core engine features, basic systems, and getting our workflow documented. The grand finale of this phase is a polished promotional video showcasing our progress to try and recruit volunteers to the team.

**Key Deliverables:**

* **Environments & Models:** Getting characters moving around Iwatodai Dorm, Paulownia Mall, and other environment models, with optimized models. Initial implementation of the initial move-in intro sequence.
* **User Interface:** Building out the foundational UI, including the top-corner calendar display, dialogue boxes with custom backgrounds, and a clean console-style battle menu. We are also laying the groundwork for FEMC palette swaps.
* **Under the Hood:** Setting up dummy zones for battle and dialogue testing, adding basic save data functionality, implementing asset streaming, and finalizing the PersonaUniversalToolchain.

---

## Activity

![Alt](https://repobeats.axiom.co/api/embed/7e6123f89c4c8a46b04e80b52694693203c2cf9d.svg "Repobeats analytics image")

<a href="https://www.star-history.com/?repos=p3d-project%2Fpersona-3-dual&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=p3d-project/persona-3-dual&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=p3d-project/persona-3-dual&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=p3d-project/persona-3-dual&type=date&legend=top-left" />
 </picture>
</a>

---

*This is a fan project and is not affiliated with or endorsed by Atlus or Sega. We (the team & the project) will not accept any monetary donations or funding for this project, nor will we make money from this project. We (the team & the project) do not enable, condone, or endorce piracy.*
