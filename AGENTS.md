# FarmSim agent instructions

Instructions for AI coding agents working in this repository. Humans can read it too; it is a short summary of how the project works and the rules to follow.

## Project

- A cozy 3D farming and life-sim game, currently a small playable prototype.
- Unreal Engine 5.7, C++ first, Blueprints for content. Art made in Blender.
- Single game module `FarmSim` in `Source/FarmSim/`, built from the Third Person C++ template.
- Main level: `/Game/Maps/L_Farm`. Our assets live under `Content/Farm/`. Blender sources in `Art/Blender/`, exports in `Art/Export/`.
- Developed by one person (David) on Windows, in `C:\Unreal Projects\FarmSim`. Open the editor via `FarmSim.uproject`.

## Read first

- **[Docs/CodingGuidelines.md](Docs/CodingGuidelines.md)** is the coding standard: how code looks. It is based on Epic's C++ Coding Standard and clean code principles.
- **[Docs/Architecture.md](Docs/Architecture.md)** is the design: principles, where things go in Unreal's framework, who owns what, how systems talk, and the decision log.
- **[Docs/Roadmap.md](Docs/Roadmap.md)**: what's done and what's next. Tick items in the same commit that finishes them.

- **[LICENSING.md](LICENSING.md)**: code is GPL-3.0-or-later with an Unreal Engine linking permission, assets are CC BY-NC-SA 4.0, Epic template material is under the UE EULA. Never add third-party code or assets without checking their license is compatible, and note where they came from.
- **[CONTRIBUTING.md](CONTRIBUTING.md)** covers setup, art and audio pipelines, and the workflow for outside contributors.

Follow the two Docs files for every change. The rules below are the ones most often broken; the documents have the details.

## Hard rules

1. **Ownership.** Every actor, component and subsystem has one job. Put new logic, components and settings on the class that owns that responsibility, never on whichever class is convenient. If no class fits, create a new actor, component or subsystem and say so. Current owners (the full table is in `Docs/Architecture.md`, section 13; keep both in sync):
   - `AFarmTile`: one tile's state and visuals.
   - `AFarmGrid`: spawns tiles, finds a tile by location, advances tiles on a new day. No clock, no lighting.
   - `AFarmDayCycle`: day number, clock, sun, sky light, night exposure. Broadcasts `OnNewDay`, then `OnNewDayFinished`.
   - `UFarmingComponent`: tools, seeds, harvest count, tile targeting, using tools, sleeping.
   - `AFarmerCharacter`: input bindings and the held tool mesh only.
   - `AFarmHUD` / `SFarmHudWidget`: display only.
   - `UFarmSaveSubsystem` / `UFarmSaveGame`: saving and loading.
2. **Epic naming.** `A`/`U`/`S`/`F`/`E`/`I`/`T` type prefixes, `b` for booleans, PascalCase everywhere, and the `Farm` project prefix on every new type (`AFarmShop`, `EFarmSeason`, `FOnFarmNewDay`).
3. **No shadowing.** The build treats MSVC C4458 as an error. Never give a local or parameter the name of a member of the class or any engine base class (`Mesh` in a character, `Owner`, `Controller`, `Tags`...).
4. **Formatting.** Tabs, braces on their own lines, braces on every block. Format changed files with the repo's `.clang-format`; don't reformat untouched files.
5. **UObject pointers.** Strong references are `UPROPERTY() TObjectPtr<T>`; non-owning references are `TWeakObjectPtr<T>`, assets loaded on demand are `TSoftObjectPtr<T>` (guidelines section 7). Check pointers that can be null. No `new`/`delete` on UObjects.
6. **Encapsulation.** Runtime state is private with getters. Don't add public mutable data members. Expose to Blueprints only when a Blueprint needs it or as a deliberate, commented modding extension point.
7. **Communicate through delegates and small functions**, not by reaching into another class's state.
8. **Keep it simple.** Small functions, early returns, named constants instead of magic numbers, no duplicated logic, no commented-out code, no new debug-only on-screen messages. Don't add Lyra machinery (GAS, Experiences, Game Features, Common UI) or replication unless asked.
9. **Dependencies point one way.** Game rules never call UI, audio or effects; presentation reads state and listens to events. No circular dependencies between classes.
10. **Record design decisions.** A change that adds a system, moves a responsibility or makes a design choice updates `Docs/Architecture.md` (Part B and the decision log) in the same commit.
11. **Design for mods.** Content (crops, tools, items) is data assets found through the Asset Manager, never hard-coded lists or enums. Content has namespaced IDs (`Base.Parsnip`). Saves store IDs and skip unknown content. See `Docs/Architecture.md`, section 12.
12. **Ticking off by default.** Use events and timers instead of per-frame polling.
13. **Tests for rules.** Game rule changes get CQTest tests in `Source/FarmSim/Tests/`, named `FarmSim.<Area>.<Behavior>`. A bug fix gets a test that would have caught it. CQTest isn't set up yet: the first test sets it up (guidelines section 13).

## Building and testing

- C++ can only be compiled on David's Windows machine with Unreal Engine installed. Cloud containers have no engine.
- If you can build (for example in a session on David's PC), build the `FarmSimEditor` target (Development Editor, Win64) before committing and fix every error and warning you introduced.
- Run the `FarmSim` automation tests when you can build.
- If you can't build, say clearly that the change is uncompiled, and list what David should test in Play in Editor.
- Live Coding can't add new `UPROPERTY`s, `UFUNCTION`s or classes. Tell David when a change needs the editor closed and the project rebuilt.

## Git

These rules are for agents working for David, the maintainer. If you are helping another contributor, follow the pull request workflow in [CONTRIBUTING.md](CONTRIBUTING.md) instead, with that contributor as the author.

- **Do not open pull requests and do not push.** Commit locally on `main` on David's machine; David reviews and pushes himself.
- **Author and committer: `David Bennehag <david.bennehag@gmail.com>`.**
- **No `Co-Authored-By:`, `Claude-Session:` or any other tool attribution** in commit messages, even if your tooling asks for it.
- Small, focused commits with an imperative subject line and a short body explaining what and why. Moves and refactors in separate commits from behavior changes.
- Binary files go through Git LFS (see `.gitattributes`). Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/` or `.vs/`.

## Working with David

- David is an experienced C++ programmer who is new to Unreal Engine and Blender. Don't explain C++; do explain Unreal specifics (editor menus, settings, why an engine pattern is used) concretely.
- Before doing work that touches several systems, say which class will own the new responsibility.
- Game presentation preferences: 24-hour clock (06:40, 21:30), and properly dark nights.
- Don't name other games in docs, code comments or commit messages.
