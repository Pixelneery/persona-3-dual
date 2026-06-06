# Contributing
### Want to help? Join the [Discord!](https://discord.gg/CQnkc5gS6a) Any help, big or small, would be greatly appreciated!
Check the [Project Board](https://github.com/orgs/p3d-project/projects/1) for open tasks, then jump into [Discord](https://discord.gg/CQnkc5gS6a) to coordinate with the team.

### Code Formatting

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

### Pull Requests

- **Keep PRs focused** — try to keep one feature or fix per PR. Mixed concerns make review hard.
- **Keep PRs small** — smaller diffs get reviewed faster and merged sooner. If something is growing large, consider splitting it.
- **Write a clear description** — briefly explain what changed and why. Link to any related issue or task.
- **Test on emulator before opening** — make sure it builds and runs in melonDS.

Also, please try to have clear commit messages 🙂

### AI Usage Policy

AI tools are permitted under these conditions:

- **You designed the core idea.** AI can help you implement or refine it, not invent it for you.
- **You understand it.** If you can't explain what the code does, it doesn't belong in a PR.
- **No slop.** Generated code must be clean, logical, and intentional.
- **Exception — `/tools`, READMEs, Makefile.** The Python scripts in `/tools`, READMEs, and the Makefile are largely AI-generated to keep the build workflow fast and consistent. This is intentional and acceptable.

AI should help you build something, not build it for you.
