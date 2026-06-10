# P3D Project — Onboarding Guide

Welcome to the Persona 3 Dual team! This guide will get you set up and oriented regardless of your role.

---

## 1. Join the GitHub Organization

We use GitHub to track tasks and coordinate across all teams. **Everyone on the project needs to join the org.**

**Steps:**
1. Create a [GitHub account](https://github.com) if you don't have one.
2. Message **@thebosst** on Discord with:
   - Your team (Dev, Art, Website, Video)
   - Your GitHub username
   - Your current task (if any)
3. You'll receive an email invitation to join the org — accept it.
> **Example message:**
> `Hey! I just joined the Art team, and need access to the GitHub Organizaion. My GitHub username is [username]. I'm currently working on [task].`

---

## 2. Using the Project Board

The project board is how we track progress across all teams.

**Columns:**
| Column | Meaning |
|---|---|
| Backlog | Not yet started, no current priority |
| Next Milestone | Targeted for the upcoming milestone |
| Ready | Current milestone, ready to be picked up |
| In Progress | Actively being worked on |
| In Review | Submitted, awaiting review |
| Done | Completed |
| Suspended | Paused, not actively exploring anymore |

**Guidelines:**
- Drag your issue to **In Progress** when you start, and **Done** when finished.
- **Assign yourself** to any issue you're working on.
- **Comment regularly** on your issue to keep the team updated on progress.
- You can filter by label (e.g. `graphics`, `bug`, `development`, `polish`) or milestone.
### Creating a New Issue

Use the following format when creating issues:

```
## Summary
[Brief description of what needs to be done and why.]

## Requirements
- [Requirement 1]
- [Requirement 2]
- [Requirement 3]

## Examples / References
[Screenshots, mockups, links, or other reference material.]
```

---

## 3. Dev Team Setup

The team uses **Docker** as the official development environment. It wraps the entire toolchain into a single image so everyone gets an identical build environment regardless of OS.

### Step 1 — Install Docker

| Platform | Instructions |
|---|---|
| Windows / macOS | [Docker Desktop](https://www.docker.com/products/docker-desktop/) |
| Linux (Ubuntu/Debian) | [Docker's install guide](https://docs.docker.com/desktop/setup/install/linux/) |

Verify the install:
```bash
docker --version
```

### Step 2 — Clone the Repo

```bash
git clone https://github.com/p3d-project/persona-3-dual.git
cd persona-3-dual
```

### Step 3 — Set Up Code Formatting Hooks

Install [pre-commit](https://pre-commit.com):

```bash
# macOS
brew install pre-commit

# Windows / Linux
pip install pre-commit
```

Then register the hooks:
```bash
pre-commit install
```

Hooks run automatically on every `git commit` and keep C/C++, Python, and web files consistently formatted. See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

> **Windows note:** prettier (used for web files) requires Node.js. pre-commit will download a local copy automatically on first run.

### Step 4 — Build the Docker Image

Run this **once** (or again whenever `Dockerfile` or `tools/requirements.txt` changes):

```bash
docker build -t p3d-dev .
```

> The first build takes a few minutes while devkitARM downloads. Subsequent builds use the Docker layer cache and are much faster.

### Step 5 — Compile the ROM

```bash
# Linux / macOS
docker run --rm -v "$(pwd)":/project p3d-dev make

# Windows (PowerShell)
docker run --rm -v "${PWD}:/project" p3d-dev make
```

This produces `persona-3-dual.nds` and `sdcard.img` in your repo folder.

### Optional — Interactive Shell

If you want to run commands manually or debug the build:

```bash
# Linux / macOS
docker run --rm -it -v "$(pwd)":/project p3d-dev

# Windows (PowerShell)
docker run --rm -it -v "${PWD}:/project" p3d-dev
```

You'll be inside the container at `/project` (your repo). Type `exit` to leave.

### Useful Docker Commands

| Command | What it does |
|---|---|
| `docker build -t p3d-dev .` | (Re)build the dev image |
| `docker images` | List images on your machine |
| `docker rmi p3d-dev` | Delete the image (frees disk space) |
| `docker ps` | List running containers |

---

## 4. Website Team Setup

The website is an [Astro](https://astro.build) project.

### Step 1 — Install Dependencies

Ensure you have [Node.js](https://nodejs.org) installed, then:

```bash
npm install
```

### Step 2 — Start the Dev Server

```bash
npm run dev
```

The site will be available at `http://localhost:4321` by default.

### Step 3 — Build for Production

```bash
npm run build
```

---

## 5. Art Team Setup

### Tools

| Role | Tools |
|---|---|
| Graphic Designer | [GIMP](https://www.gimp.org), [Aseprite](https://www.aseprite.org) |
| Sprite Work | [GIMP](https://www.gimp.org), [Aseprite](https://www.aseprite.org) |
| Modeling | [Blender](https://www.blender.org) |
| Ripper | Noesis |

### Workflow

- **Upload all completed assets and their editable source files to [Google Drive](#).**
- Track your work on the project board. Move issues through the columns as you progress.
- When uploading assets and closing an issue, leave a comment on the related issue with a link to the Drive folder, and upload viewable images to the issue.
---

## 6. Video Team Setup

The video team produces **promo videos** and **devlog videos**, paired with each project milestone.

### Tools
- TBD
### Workflow
- Each milestone gets a promo video and an accompanying devlog.
- Coordinate with other teams to gather footage, screenshots, and updates before each milestone.
- **Upload all completed assets and their editable source files to [Google Drive](#)** and link them in the relevant issue.
---

*Questions? Reach out on Discord.*
