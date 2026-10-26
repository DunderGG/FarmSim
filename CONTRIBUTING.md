# Contributing to FarmSim

Thanks for wanting to help. FarmSim is a cozy 3D farming and life-sim game, built with Unreal Engine 5 and Blender. It is an early prototype made by one developer, so there is plenty to do and plenty of room to shape it.

This guide covers how to get the project running, how it is organized, and how to contribute code, art, audio and ideas.

> **Licensing in short:** code is GPL-3.0, assets are CC BY-NC-SA 4.0, and the maintainer (David, [@DunderGG](https://github.com/DunderGG)) sells official builds of the game. Contributors are credited. Read [Licensing and credit](#11-licensing-and-credit) before your first contribution.

Contents

1. [Ways to help](#1-ways-to-help)
2. [Getting set up](#2-getting-set-up)
3. [Finding your way around](#3-finding-your-way-around)
4. [Contributing code](#4-contributing-code)
5. [Contributing art](#5-contributing-art)
6. [Contributing audio](#6-contributing-audio)
7. [Adding content the data-driven way](#7-adding-content-the-data-driven-way)
8. [Ideas, bugs and feedback](#8-ideas-bugs-and-feedback)
9. [Git workflow and pull requests](#9-git-workflow-and-pull-requests)
10. [Working with AI tools](#10-working-with-ai-tools)
11. [Licensing and credit](#11-licensing-and-credit)
12. [Be kind](#12-be-kind)

---

## 1. Ways to help

| Area | Examples | Skills |
|---|---|---|
| **Gameplay code** | New tools, crops that need special rules, an inventory, shops, NPCs | C++, Unreal Engine |
| **3D art** | Crops at each growth stage, tools, buildings, fences, trees, props | Blender, low-poly modeling |
| **Materials and textures** | Soil, grass, wood, stylized materials | Unreal materials, texturing |
| **Animation** | Tool swings, watering, harvesting | Blender or Unreal animation |
| **UI** | HUD, inventory, menus, icons | Unreal UMG/Slate, 2D art |
| **Audio** | Footsteps, tool sounds, ambience, music | Sound design, composing |
| **Content and balancing** | New crops and items defined as data, prices, growth times | Unreal editor, no code needed |
| **Testing** | Playing builds and reporting bugs, writing automated tests | Patience, C++ for tests |
| **Documentation** | Fixing unclear docs, writing how-tos | Writing |
| **Mods** | Building content as a mod, to test that the game is moddable | Unreal editor, Blueprints |

Not sure where you fit? Open an issue and say what you'd like to do.

## 2. Getting set up

### What you need

| Tool | Version | Notes |
|---|---|---|
| Windows | 10 or 11 | The project is developed and built on Windows |
| Unreal Engine | **5.7** | Install through the Epic Games Launcher. Other versions will ask to convert the project; don't |
| Visual Studio | 2022 | Only for C++ work. Opening the project prompts you to install the components listed in `.vsconfig` (the "Game development with C++" workload) |
| Git and **Git LFS** | Recent | All binary files (`.uasset`, `.umap`, `.fbx`, `.blend`, textures, audio) are stored in Git LFS |
| Blender | Current release | Only for 3D art |

### First run

1. Install Git LFS once: `git lfs install`.
2. Clone the repository. LFS files download with it; if you see small text files where assets should be, run `git lfs pull`.
3. Double-click **`FarmSim.uproject`**. The first time, Unreal offers to build the C++ modules; say yes. This takes a few minutes.
   - If you will write C++, right-click `FarmSim.uproject` and pick *Generate Visual Studio project files* first, then open `FarmSim.sln` and build the **FarmSimEditor** target in **Development Editor / Win64**.
   - Always open the project through `FarmSim.uproject` or the launcher's *My Projects* list. Opening the editor on its own and letting it auto-load the project can show a misleading "modules were built with a different engine version" error.
4. Open the level **`Content/Maps/L_Farm`** and press **Play**.

### Controls

| Key | Action |
|---|---|
| WASD, mouse | Move, look |
| Space | Jump |
| 1 / 2 / 3 / 4 | Hoe / seeds / watering can / hand |
| E | Use the current tool on the tile in front of you |
| N | Sleep to the next day (the game saves) |
| F9 | Delete the save and start a new game |

Till the grass, plant seeds, water them each day, sleep, and harvest when the crop is ripe.

## 3. Finding your way around

```
FarmSim/
    FarmSim.uproject        The Unreal project
    Source/FarmSim/         Game C++ code (one module)
    Content/                Unreal assets
        Farm/               Our game assets (meshes, materials, Blueprints)
        Maps/               Levels (L_Farm is the main one)
        Characters/, ThirdPerson/, Input/
                            From Epic's Third Person template
    Art/
        Blender/            Blender source files and scripts
        Export/             FBX files exported from Blender, ready to import
    Config/                 Engine and project settings
    Docs/                   Project documentation
```

Read these before making larger changes:

- **[Docs/Architecture.md](Docs/Architecture.md)**: how the game is structured, which class owns what, the design rules, and why decisions were made. Its *Known gaps* and *Expected changes* sections are a good list of things that need doing.
- **[Docs/CodingGuidelines.md](Docs/CodingGuidelines.md)**: how code should look, naming for code and assets, testing.

## 4. Contributing code

1. **Talk first for anything bigger than a small fix.** Open an issue that describes the change and which class or system would own it. It saves you from building something that doesn't fit.
2. **Follow the architecture.** The most important rule: every actor, component and subsystem has one job, and new logic goes on the class whose job it is. If nothing fits, propose a new class. See [Architecture.md](Docs/Architecture.md).
3. **Follow the coding guidelines.** Epic's naming (`AFarm...`, `UFarm...`, `bIsSomething`), tabs and braces on their own lines, small functions, no magic numbers. The repo's `.clang-format` handles formatting: enable *ClangFormat support* in Visual Studio and format the files you change.
4. **Build for mods.** Content (crops, tools, items) is data, not hard-coded lists or enums. See [Designing for mods](Docs/Architecture.md#12-designing-for-mods).
5. **Test it.** Game rule changes come with automated tests (CQTest, in `Source/FarmSim/Tests/`). Run the `FarmSim` tests from the editor's Test Automation window, and play the change in the editor.
6. **No new warnings.** The build treats some warnings as errors, for example a local variable that hides a member (`Mesh` inside a character class).
7. **Update the docs.** If your change adds a system, moves a responsibility or makes a design choice, update `Docs/Architecture.md` (the ownership table and the decision log) in the same pull request.

## 5. Contributing art

### Style

Low-poly and stylized, with simple shapes and flat or lightly textured colors. Look at the existing crops and the watering can in `Content/Farm/Meshes` for reference. Readability from the game camera matters more than detail.

### From Blender to Unreal

1. **Scale:** Blender in metric with unit scale 1.0, so 1 Blender meter is 1 Unreal meter (100 Unreal units). A farm tile is **1 m × 1 m**.
2. **Orientation:** Z up, and the front of the object facing **+X**.
3. **Origins (pivots)** are where the object touches the world or is held:
   - Soil and ground pieces: bottom center.
   - Crops: center of the base, on the soil surface.
   - Held tools: the point where the hand grips.
4. **Apply transforms** (rotation and scale) before exporting.
5. **Export FBX** to `Art/Export/` with the same name the asset will have in Unreal, using *Forward: X*, *Up: Z* and *Apply Scalings: FBX Units Scale*.
6. **Save the `.blend` source** in `Art/Blender/`. Every imported mesh should have its source in the repo so others can change it.
7. **Import** into `Content/Farm/` in the folder for its type (`Meshes`, `Materials`, `Textures`...).

`Art/Blender/make_farm_assets.py` generates the current crop and tool meshes from a script and is a working example of these settings.

### Naming

Follow Epic's [recommended asset naming](https://dev.epicgames.com/documentation/en-us/unreal-engine/recommended-asset-naming-conventions-in-unreal-engine-projects): `SM_` static mesh, `SK_` skeletal mesh, `M_` material, `MI_` material instance, `T_` texture, `BP_` Blueprint and so on. Variants go at the end: `SM_Crop_Stage0`, `SM_Crop_Ripe`. The full table is in [CodingGuidelines.md](Docs/CodingGuidelines.md#142-content-and-asset-naming).

### Budgets

Keep things light so big farms stay fast: a crop or small prop in the hundreds of triangles, a tool under about 2,000, a building under about 10,000. Textures 1024 × 1024 or smaller unless there's a reason. These are guidelines, not hard limits.

## 6. Contributing audio

- Deliver **WAV**, 16-bit or 24-bit, 44.1 or 48 kHz. Mono for sounds placed in the world, stereo for music and ambience.
- Name sound waves `S_<Name>` and sound cues `SC_<Name>`, and put them in `Content/Farm/Audio/`.
- Keep the source project files (DAW sessions, raw recordings) out of the repo unless they're needed to edit the sound later. Ask first if they are large.

## 7. Adding content the data-driven way

The goal is that adding a crop, tool or item means adding data, not changing C++. That's also how mods will add content.

The data asset types for this (`UFarmCropData` and friends) are planned but not built yet; see [Expected changes](Docs/Architecture.md#18-expected-changes). Building them is a good first contribution for a C++ developer. Once they exist, this section will explain step by step how to add a crop.

## 8. Ideas, bugs and feedback

- **Bugs:** open an issue with what you did, what you expected, what happened, and the engine version. Attach the log from `Saved/Logs/FarmSim.log` and a screenshot if it helps.
- **Ideas:** open an issue describing the idea and why it would make the game better. Small, concrete ideas are easiest to act on.
- **Questions:** open an issue too. If something in the docs confused you, that's a bug in the docs.

## 9. Git workflow and pull requests

1. Fork the repository, or create a branch if you have write access. Name the branch after the change: `add-fence-meshes`, `fix-watering-twice`.
2. Make **small, focused commits** with clear messages: an imperative subject line under about 70 characters ("Add fence meshes"), a blank line, then what changed and why.
3. Keep moves and renames in their own commits, separate from changes in behavior.
4. **Never commit** `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/` or generated `.sln` files. `.gitignore` handles this; don't force-add them.
5. **Binary files go through Git LFS.** `.gitattributes` covers the common types. If you add a new binary file type, add it to `.gitattributes` in the same commit.
6. **Avoid editing the same binary asset as someone else.** `.uasset` and `.umap` files can't be merged. Say in your issue which assets and levels you'll be changing, especially `L_Farm`.
7. Open a **pull request** against `main`. Describe what it changes and how you tested it, and include screenshots or a short clip for anything visual.
8. The maintainer reviews every pull request. Expect questions and requests for changes; they're about keeping the project consistent, not about you.

Before opening a pull request, check:

- [ ] It builds, and the `FarmSim` tests pass.
- [ ] You played it in the editor and it works.
- [ ] Code follows the [coding guidelines](Docs/CodingGuidelines.md#17-review-checklist) and the [architecture](Docs/Architecture.md).
- [ ] Assets are named correctly, and their Blender sources are included.
- [ ] Docs are updated if the design changed.

## 10. Working with AI tools

You're welcome to use AI coding assistants. [AGENTS.md](AGENTS.md) (and `CLAUDE.md`, which loads it) gives them the project's rules. You are still responsible for what you submit: read and understand every change, build it and test it before opening a pull request.

## 11. Licensing and credit

FarmSim's code is licensed under the **GPL-3.0** (with a permission that allows linking with Unreal Engine) and its assets under **CC BY-NC-SA 4.0**. The maintainer sells official builds of the game. The full picture is in [LICENSING.md](LICENSING.md).

**Contributor terms.** By submitting a pull request, a file or any other contribution to this repository, you agree that:

1. The contribution is your own work, or you have the right to submit it, and you have said where any third-party parts come from and what license they are under.
2. Your contribution is licensed under the license for the part of the repository it goes into: GPL-3.0 (with the Unreal Engine permission) for code, CC BY-NC-SA 4.0 for assets and everything else.
3. You also grant David Bennehag, the maintainer, a perpetual, worldwide, non-exclusive, royalty-free, irrevocable license to use, modify, distribute and sublicense your contribution as part of FarmSim, including in commercial builds sold on game stores and consoles, and under other license terms if the project needs them (for example for a console release).
4. You keep the copyright to your work and may use it elsewhere however you like.

Add a line `I agree to the contributor terms in CONTRIBUTING.md` to your first pull request.

**Credit.** Add yourself to [CREDITS.md](CREDITS.md) in your first pull request. Contributors are also listed in the game's credits.

**Third-party material.** Only submit assets and code you made yourself or that are under a license compatible with ours (for example CC0 or CC BY for assets, MIT or BSD for code). Say where they came from in the pull request. Never submit assets ripped from other games, or anything generated from someone else's protected work.

## 12. Be kind

Be respectful and patient with everyone. People of every skill level are welcome. Critique the work, not the person.
