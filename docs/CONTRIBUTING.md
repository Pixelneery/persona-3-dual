# Contributing
### Want to help? Join the [Discord!](https://discord.gg/CQnkc5gS6a) Any help, big or small, would be greatly appreciated!
Check the [Project Board](https://github.com/orgs/p3d-project/projects/1) for open tasks, then jump into [Discord](https://discord.gg/CQnkc5gS6a) to coordinate with the team.

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

# Windows (PowerShell)
docker run --rm -v "${PWD}:/project" p3d-dev make
```

This produces `persona-3-dual.nds` and `sdcard.img` in your repo folder!

### 6. Interactive Shell (optional)

If you want to poke around, run commands manually, or debug the build:

```bash
# Linux / macOS
docker run --rm -it -v "$(pwd)":/project p3d-dev

# Windows (PowerShell)
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

## Code Formatting

This project uses [pre-commit](https://pre-commit.com) to auto-format all source files before every commit. It handles C/C++ (clang-format), Python (black + ruff), and web files (prettier).

**One-time setup after cloning:**

```bash
# macOS
brew install pre-commit

# Windows / Linux
pip install pre-commit
```

```bash
pre-commit install
```

That's it. From now on, hooks run automatically every time you run `git commit`.

**What happens during a commit:**

The hooks reformat your staged files in-place. If any file is changed, the commit is **aborted** so you can review the diff. Just re-stage and commit again.

**Run hooks manually (e.g. before opening a PR):**

```bash
pre-commit run --all-files
```

> **Windows note:** prettier requires Node.js to be installed. pre-commit downloads a local copy automatically the first time you run `pre-commit install` or `pre-commit run`.

---

## Pull Requests

- **Keep PRs focused** - try to keep one feature or fix per PR. Mixed concerns make review hard.
- **Keep PRs small** - smaller diffs get reviewed faster and merged sooner. If something is growing large, consider splitting it.
- **Write a clear description** - briefly explain what changed and why. Link to any related issue or task.
- **Test on emulator before opening** - make sure it builds and runs in melonDS.

Also, please try to have clear commit messages 🙂

---

## AI Usage Policy

AI tools are permitted under these conditions:

- **You designed the core idea.** AI can help you implement or refine it, not invent it for you.
- **You understand it.** If you can't explain what the code does, it doesn't belong in a PR.
- **No slop.** Generated code must be clean, logical, and intentional.
- **Exception — `/tools`, READMEs, Makefile.** The Python scripts in `/tools`, READMEs, and the Makefile are largely AI-generated to keep the build workflow fast and consistent. This is intentional and acceptable.

AI should help you build something, not build it for you.
