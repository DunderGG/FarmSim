# Project Vestige coding guidelines

How code in this project should look. How the systems fit together, who owns what and why, is in [Architecture.md](Architecture.md).

This document builds on three sources:

1. **Epic's C++ Coding Standard** (https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine). It is the baseline. Where this document says nothing, follow Epic.
2. **The Lyra Starter Game** sample. We borrow its structure and patterns but not its full machinery. See [Architecture.md](Architecture.md#11-patterns-borrowed-from-lyra).
3. **Clean code**: small focused units, intention-revealing names, no duplication, no dead code.

Other official references are listed under [Further reading](#18-further-reading).

The top priority is that the project stays **easy to read and easy to change** for one developer coming back to it after weeks away. When a rule here gets in the way of that, raise it and change the rule rather than quietly ignoring it.

Contents

1. [Ownership and separation of concerns](#1-ownership-and-separation-of-concerns)
2. [Naming](#2-naming)
3. [Formatting](#3-formatting)
4. [Architecture](#4-architecture)
5. [Classes and headers](#5-classes-and-headers)
6. [UPROPERTY, UFUNCTION and Blueprint exposure](#6-uproperty-ufunction-and-blueprint-exposure)
7. [Memory, pointers and object lifetime](#7-memory-pointers-and-object-lifetime)
8. [Functions and control flow](#8-functions-and-control-flow)
9. [Modern C++ usage](#9-modern-c-usage)
10. [Errors, logging and assertions](#10-errors-logging-and-assertions)
11. [Comments and documentation](#11-comments-and-documentation)
12. [Performance and ticking](#12-performance-and-ticking)
13. [Testing](#13-testing)
14. [Project layout and assets](#14-project-layout-and-assets)
15. [Git workflow](#15-git-workflow)
16. [Known deviations to clean up](#16-known-deviations-to-clean-up)
17. [Review checklist](#17-review-checklist)
18. [Further reading](#18-further-reading)

---

## 1. Ownership and separation of concerns

This is the most important rule in the project.

**Every actor, component and subsystem has one job. Logic, components and settings live on the class whose job they are, never on whichever class happens to be convenient.**

Before adding a component, property, timer or `Tick` logic to an existing class, ask: *is this that class's job?*

- If yes, add it there.
- If no, put it on the class that does own it, or create a new actor, component or subsystem for it. Say so explicitly in the commit message and add it to the ownership table.

A real example from this project: night post-processing and sun control were first put on `AFarmGrid` because the grid was already in the level. They were moved to a dedicated `AFarmDayCycle`, which is auto-spawned if the level doesn't have one. That is the pattern to follow.

The current ownership table is in [Architecture.md, section 13](Architecture.md#13-systems-and-ownership). How to choose the right kind of owner (actor, component, subsystem, data asset) is in [section 2](Architecture.md#2-unreals-gameplay-framework-where-things-go) and how owners communicate is in [section 4](Architecture.md#4-how-systems-talk-to-each-other).

## 2. Naming

Follow Epic's naming rules. The `.editorconfig` in the repo enforces the prefixes in Visual Studio and Rider.

### 2.1 Type prefixes

| Prefix | For | Example |
|---|---|---|
| `A` | Classes deriving from `AActor` | `AFarmTile` |
| `U` | Classes deriving from `UObject` (components, subsystems, data assets) | `UFarmingComponent` |
| `S` | Slate widgets | `SFarmHudWidget` |
| `F` | Structs and most other classes | `FFarmTileSaveData` |
| `E` | Enums | `EFarmTool` |
| `I` | Interfaces | `IFarmInteractable` |
| `T` | Templates | `TFarmPool` |
| `b` | Booleans (variables, members, parameters) | `bWatered`, `bHasCrop` |

### 2.2 The `Farm` project prefix

Like Lyra prefixes its classes with `Lyra` (`ALyraCharacter`, `ULyraHealthComponent`), every class, struct, enum, delegate and log category we add starts with **`Farm`** after the type prefix: `AFarmGrid`, `EFarmTileState`, `FOnFarmNewDay`. This keeps our types apart from engine types in autocomplete and in the editor's class pickers.

The template classes (`AVestigeCharacter`, `AVestigeGameMode`, `AVestigePlayerController`) keep the template's naming: the module name plus the class role.

### 2.3 Case and wording

- **PascalCase for everything**: types, functions, members, locals, parameters, constants. No `snake_case`, no `m_` or `_` prefixes, no Hungarian notation beyond the prefixes above.
- Names are **descriptive and unabbreviated**: `ReachDistance`, not `RchDst`; `HarvestedCrops`, not `NumHarv`. Common, well-known short forms are fine (`HUD`, `UI`, `Max`, `Min`, `Num`).
- Functions that do something are **verbs**: `Till()`, `StartNextDay()`, `RefreshVisuals()`.
- Functions that only **query state** and return a `bool` ask a **question**: `IsRipe()`, `IsPastBedtime()`, `HasMessage()`.
- Functions that **do something that can fail** keep their verb name and return `bool` for success: `AFarmTile::Till()` returns `false` when the tile isn't grass. Say in the `/** */` comment what `false` means. Don't rename them as questions (`DidTillSucceed()`) or add a `Try` prefix.
- Getters use `Get`: `GetCurrentDay()`. `Find` returns `nullptr` when nothing exists; `Get` may create it (see `AFarmDayCycle::Find` vs `AFarmDayCycle::Get`).
- Delegates: the type is `FOnFarm<Event>`, the member is `On<Event>`: `FOnFarmNewDay OnNewDay`.
- Event handlers are named for what they do, not when they run: `AdvanceTiles(int32 NewDay)`, `SaveFarm(int32 NewDay)`, not `HandleOnNewDay`.
- Units go in the name or the comment when not obvious: `RealSecondsPerGameHour`, `TileSize` with `/** ...in cm */`.
- Out parameters start with `Out`: `void GetBounds(FVector& OutOrigin, FVector& OutExtent)`.
- A parameter that would clash with a member gets the `In` prefix (`InWorld`), or a different name. Never reuse a name from a base class.

### 2.4 Never shadow

The build treats MSVC warning **C4458 (declaration hides class member)** as an error. A local or parameter must never have the same name as a member of the class or any base class. Watch out especially for names inherited from engine classes:

```cpp
// Wrong: ACharacter already has a member called Mesh.
UStaticMesh* Mesh = ToolMeshes.FindRef(Tool);

// Right
UStaticMesh* ToolMesh = ToolMeshes.FindRef(Tool);
```

Other common traps in actors and pawns: `Controller`, `Owner`, `Tags`, `Instigator`, `RootComponent`, `Children`. In Slate widgets: `ChildSlot`.

## 3. Formatting

The repo has a **`.clang-format`** file that encodes these rules, so formatting is automatic rather than something to review.

- In Visual Studio, check *Tools > Options > Text Editor > C/C++ > Code Style > Formatting > Enable ClangFormat support*. VS then uses the repo's file. Format a document with *Ctrl+K, Ctrl+D*.
- Format the files you change. Don't reformat files you aren't otherwise touching, and don't mix a first-time reformat of a file with behavior changes in the same commit.
- clang-format can't lay out Slate argument blocks. Wrap `SLATE_BEGIN_ARGS` ... `SLATE_END_ARGS()` in `// clang-format off` and `// clang-format on`.
- The rules below still apply where the formatter has no opinion (blank lines between groups, where to break long expressions).

- **Tabs for indentation**, 4 spaces wide. No trailing whitespace.
- **Braces on their own line** (Allman style), for every block:

```cpp
bool AFarmTile::Till()
{
	if (State != EFarmTileState::Grass)
	{
		return false;
	}
	State = EFarmTileState::Tilled;
	RefreshVisuals();
	return true;
}
```

- **Always use braces**, even for one-line `if`, `for` and `while` bodies.
- One-line function bodies are fine in headers for trivial getters: `int32 GetCurrentDay() const { return CurrentDay; }`.
- `else if` stays on one line. Don't write `else` after a branch that returns.
- A space after `if`, `for`, `while`, `switch` and around binary operators. No space between a function name and its `(`.
- Pointer and reference symbols attach to the type: `AFarmTile* Tile`, `const FVector& WorldLocation`.
- Keep lines under 140 characters. Break long argument lists after a comma and indent one extra tab.
- When a ternary breaks over lines, the `?` and `:` start the new lines, as in `AFarmTile::RefreshSoil`.
- Function bodies in `.cpp` files always go on their own lines, even one-liners.
- One blank line between functions. Group related statements inside a function and separate groups with a blank line.

## 4. Architecture

Architecture lives in its own document, [Architecture.md](Architecture.md): design principles, where things go in Unreal's gameplay framework, lifetimes and initialization order, communication between systems, data, saving, presentation, the C++/Blueprint split, the Lyra patterns we use, Project Vestige's current design and the decision log.

Code that changes the design must follow it, and update its Part B and decision log in the same commit.

## 5. Classes and headers

### 5.1 One class per file pair

Each class lives in `ClassName.h` and `ClassName.cpp`, named without the type prefix: `AFarmTile` in `FarmTile.h`. Small helper types tied to one class (an enum, a delegate, a save struct) can share its header.

### 5.2 Header layout

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmDayCycle.generated.h"   // Always the last include

class ADirectionalLight;              // Forward declarations instead of includes
class UPostProcessComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFarmNewDay, int32 /* Day */);

/** What this class is for, in one or two sentences. */
UCLASS()
class AFarmDayCycle : public AActor
{
	GENERATED_BODY()

public:
	// Constructor, then public functions, then public delegates and properties.

protected:
	// Engine overrides (BeginPlay, EndPlay, Tick) and anything subclasses need.

private:
	// Helpers, owned components, runtime state.
};
```

- Order sections `public`, `protected`, `private`.
- Inside each section: constructor, static functions, functions, delegates, properties.
- Mark every override with `override`. Mark classes and functions `final` when they must not be extended further.
- Keep engine overrides `protected` unless callers outside the class need them.

### 5.3 Includes

- **Include what you use** (IWYU). Each file includes the headers for what it uses directly. Do not rely on another header to bring something in.
- In headers, **forward declare** pointer and reference types instead of including their headers. Include headers in the `.cpp`.
- In a `.cpp`, include its own header first, then a blank line, then the rest in **alphabetical order**:

```cpp
#include "FarmHUD.h"

#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "FarmDayCycle.h"
#include "FarmHudWidget.h"
```

- Never include `Engine.h` or `EngineMinimal.h`.
- New engine module dependencies go in `Vestige.Build.cs`. Put them in `PrivateDependencyModuleNames` unless our public headers expose that module's types.

### 5.4 Const correctness

- Getters and anything that doesn't change the object are `const`.
- Pass non-trivial types (`FString`, `FVector`, `TArray`) by `const&`. Pass small types (`int32`, `float`, `bool`, enums, pointers) by value.
- Use `const` for locals that never change after initialization.
- Return `const` references to member containers rather than copies: `const TArray<TObjectPtr<AFarmTile>>& GetTiles() const`.
- Do not return `const` values by value (`const FString GetName()` blocks moves).

### 5.5 Encapsulation

- **Runtime state is private.** Expose it through getters, and change it only through functions that keep the object consistent (`AFarmTile::Till` updates the state *and* the visuals).
- **Tunables** that designers edit are `UPROPERTY(EditAnywhere / EditDefaultsOnly)`. Prefer making them `protected` or `private` with `meta = (AllowPrivateAccess = "true")`, as Lyra does, and add a getter when other classes need to read them.
- Public mutable data members are a smell. If another class needs to set a value, add a function with a name that says why (`RestoreState`, `RestoreClock`).

## 6. UPROPERTY, UFUNCTION and Blueprint exposure

- A **strong** reference from a `UObject` to another `UObject` is a `UPROPERTY()` holding a `TObjectPtr<>`, so the garbage collector sees it and keeps the target alive. This applies even to private, non-edited members:

```cpp
UPROPERTY()
TObjectPtr<AFarmGrid> Grid;
```

- Not every reference should be strong. Weak and soft references have different meanings and stay weak or soft; see [section 7](#7-memory-pointers-and-object-lifetime) for which to pick. Don't turn one into a `TObjectPtr` just to satisfy the rule above.

- Pick the narrowest specifier:

| Specifier | Use for |
|---|---|
| `EditDefaultsOnly` | Settings that are the same for every instance, set in the class or Blueprint defaults (meshes, materials) |
| `EditAnywhere` | Settings that differ per placed instance (grid size) |
| `VisibleAnywhere` | Components created in the constructor |
| `VisibleInstanceOnly` | Runtime state you want to inspect while debugging in the editor |
| no edit specifier | Internal references the editor shouldn't show |

- Always give edited properties a `Category`. Use `"Farming"` and subcategories with `|`: `"Farming|Time"`, `"Farming|Lighting"`, `"Farming|Visuals"`.
- Add `ClampMin` / `ClampMax` meta where a value has a sensible range.
- Expose to Blueprints (`BlueprintReadOnly`, `BlueprintCallable`, `BlueprintPure`) **only when a Blueprint actually needs it, or as a deliberate modding extension point** marked `/** Extension point: ... */` (see [Architecture.md, Designing for mods](Architecture.md#12-designing-for-mods)). Everything exposed becomes an API we have to keep stable. Prefer `BlueprintReadOnly` to `BlueprintReadWrite`.
- Core rules live in C++. Blueprints are for content: picking meshes, tuning values, laying out levels, small visual-only behavior.
- Mark components created with `CreateDefaultSubobject` as `VisibleAnywhere`, and create them only in the constructor.

## 7. Memory, pointers and object lifetime

- Never use `new` / `delete` for `UObject`s. Create them with `NewObject`, `CreateDefaultSubobject` or `SpawnActor`.

Choose a member's pointer type by what the reference **means**, not by habit:

| The reference... | Use | Effect |
|---|---|---|
| should keep the target alive (components we create, assets we always need, actors this object depends on) | `UPROPERTY() TObjectPtr<T>` | Strong: the garbage collector keeps the target while we point at it |
| points at something we don't own and must not keep alive (another actor that may be destroyed, the HUD from a Slate widget) | `TWeakObjectPtr<T>` | Weak: becomes null when the target is destroyed. Check with `IsValid()` or `Get()` before use. Add `UPROPERTY()` when the editor or serialization needs to see it |
| names an asset that shouldn't load with the class (crop meshes, data assets for content not in use) | `UPROPERTY() TSoftObjectPtr<T>` / `TSoftClassPtr<T>` | Soft: stores the asset's path; load it when needed |

`SFarmHudWidget` holding its HUD in a `TWeakObjectPtr` is an example of the second row.

- Non-`UObject` heap data: `TUniquePtr`, `TSharedPtr`, `TSharedRef` (Slate widgets use `TSharedPtr`).
- Raw pointers are fine for locals and parameters that don't outlive the call.
- **Check pointers before using them** whenever they can legitimately be null: actors found in the level, `GetOwner()`, `GetWorld()`, `GetController()`, cast results. Use `IsValid()` for actors that may have been destroyed.
- Use `Cast<T>()` for `UObject` casts. Never `dynamic_cast` or C-style casts on `UObject`s. Use `CastChecked<T>()` only where a failure is a programming error.

## 8. Functions and control flow

- **Small functions that do one thing.** If you need a comment to separate the parts of a function, those parts probably want to be functions (`RefreshVisuals` calls `RefreshSoil` and `RefreshCrop`).
- Aim for functions that fit on one screen. Length is a prompt, not a limit: split a function when it does more than one thing or is hard to follow, not because it crossed a line count. A long but single-purpose function is fine.
- **Return early** to keep the happy path unindented:

```cpp
void UFarmSaveSubsystem::LoadFarm()
{
	UFarmingComponent* Farming = GetPlayerFarming();
	if (!Farming || !UGameplayStatics::DoesSaveGameExist(UFarmSaveGame::SlotName, UFarmSaveGame::UserIndex))
	{
		return;
	}
	...
}
```

- Use `if (T* X = ...)` to scope a pointer to the block that uses it.
- Few parameters: three or fewer is the goal. Group more into a struct.
- No boolean flag parameters that switch behavior (`DoThing(true)`). Write two functions or use an enum.
- `switch` on enums lists every value and has no `default`, so the compiler warns when a value is added.
- **No magic numbers.** Name constants in a namespace at the top of the `.cpp` (`FarmTileColors`, `FarmTilePlaceholder`) or make them tunable properties.
- **Don't repeat yourself.** When the same few lines appear a second time, extract a function. When they appear in two classes, ask which class owns that logic.
- Prefer composition of small, obvious steps over clever one-liners.

## 9. Modern C++ usage

- **Default to UE types**: `int32`, `uint8`, `float`, `double`, `FString`, `FName`, `FText`, `TArray`, `TMap`, `TSet`, `TOptional`, `TFunction`. They are required for anything reflected (`UPROPERTY`, `UFUNCTION`), saved, passed to engine APIs or shown in the editor, and they're what readers of Unreal code expect. Don't use `int`/`long` where size matters.
- The standard library is allowed in private implementation code when it does something the UE types don't do as well (for example `<algorithm>` helpers or `std::array` inside a `.cpp`). Keep `std::` types out of headers, reflected members and save data.
- `FString` for runtime strings, `FName` for identifiers, `FText` for anything the player reads (so it can be localized later).
- Wrap string literals in `TEXT("...")`.
- `nullptr`, never `NULL` or `0`.
- `enum class` for every new enum. `UENUM(BlueprintType)` with `uint8` underlying type when it is used in properties.
- Range-based `for` over containers. Use an index loop only when you need the index.
- `auto`: only where the type is obvious from the right-hand side (`auto* Hud = Cast<AFarmHUD>(...)`), for iterators and for lambdas. Spell out the type otherwise. Write `auto*` and `const auto&` so pointers and references stay visible.
- Lambdas: capture explicitly (`[this]`, `[Tile]`). Never capture `this` in a lambda that can outlive the object; bind delegates with `AddUObject` / `AddWeakLambda` instead.
- `constexpr` for compile-time constants. `static constexpr` for class constants (`UFarmSaveGame::SlotName`).
- `FMath` is the normal choice for math, especially on engine types (`FVector`, `FRotator`) and for helpers like `FMath::Clamp` and `FMath::Lerp`. Use `<cmath>` only for something `FMath` lacks.
- No exceptions and no RTTI; the engine builds without them.

## 10. Errors, logging and assertions

- Log with the project category `LogVestige` (declared in `Vestige.h`):

```cpp
UE_LOG(LogVestige, Warning, TEXT("No FarmGrid in %s; farming is disabled."), *GetWorld()->GetMapName());
```

- Verbosity: `Error` for broken state, `Warning` for recoverable setup problems, `Log` for notable events, `Verbose` for detailed tracing.
- Never leave `GEngine->AddOnScreenDebugMessage` as a permanent feature.
- Assertions:
  - `check(Condition)`: a programmer error that makes continuing pointless. Compiled out in shipping builds, so never put side effects inside.
  - `ensure(Condition)` / `ensureMsgf`: something is wrong but the game can carry on. Logs a callstack once.
  - Plain `if` with a graceful fallback for things that can legitimately happen at runtime or through level setup (a missing grid, a missing save).
- Content mistakes a designer can make (a missing mesh, no grid in the level) must not crash. Fall back to a placeholder and warn, as `AFarmTile` does with its placeholder shapes.
- Failures the player should know about (a save that couldn't be written) must reach the player, but game code never calls the HUD to do it. The code that failed logs the failure, returns or broadcasts it, and raises a presentation-neutral feedback event; the HUD listens and displays it (see [Architecture.md, section 7](Architecture.md#7-presentation-ui-audio-and-effects)). Today `UFarmingComponent` and `UFarmSaveSubsystem` still call `AFarmHUD::ShowMessage` directly; that is a [known gap](Architecture.md#17-known-gaps), not the pattern to copy.

## 11. Comments and documentation

- Every class gets a `/** */` comment saying **what it is for and what it owns**.
- Public functions and edited properties get a `/** */` comment when the name alone doesn't say everything: units, valid ranges, when it is called, what `nullptr` means.
- Inside functions, comment **why**, not what. Good: `// Static or stationary lights can't be turned at runtime.` Bad: `// Set the mobility to movable.`
- Write comments as full sentences in plain English.
- Don't leave commented-out code. Git remembers it.
- `TODO:` comments are allowed for known follow-ups but must say what is missing: `// TODO: Move tool meshes into UFarmToolData once there are more tools.`
- New source files start with our copyright and license header (see [LICENSING.md](../LICENSING.md#license-notices-in-files)), then a blank line:

  ```cpp
  // Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
  // SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception
  ```

  They don't carry the Epic copyright line; the template's files keep theirs.

## 12. Performance and ticking

- Turn ticking **off by default** (`PrimaryActorTick.bCanEverTick = false`) and turn it on only for classes that really need per-frame work (`AFarmDayCycle` moves the sun).
- Prefer events, timers (`FTimerManager`) and delegates over checking something every frame.
- Don't search the level (`TActorIterator`, `GetAllActorsOfClass`) every frame. Find things once in `BeginPlay` and keep the reference.
- Don't load assets with `ConstructorHelpers` outside constructors.
- Don't block the game thread with a synchronous load while the player is playing; it causes a visible hitch. Load assets that may be large or slow asynchronously (`FStreamableManager` / the Asset Manager). Synchronous loads are fine during startup or behind a loading screen, where nobody is waiting on the next frame and the cost is known.
- Optimize when something is measurably slow. Clear code first.

## 13. Testing

Game rules get automated tests. They run in seconds without playing the game, and they catch a broken rule the moment it breaks instead of three play sessions later.

- **Framework: CQTest**, Epic's newest C++ test framework (https://dev.epicgames.com/documentation/unreal-engine/cqtest-test-framework-for-unreal-engine). It wraps the engine's automation system with `TEST_CLASS` / `TEST` macros and automatic setup and teardown. It is the chosen framework for all new tests. Don't mix in the older Automation Spec or `IMPLEMENT_SIMPLE_AUTOMATION_TEST` styles.
- **Status: setup pending.** The project has no tests yet; `Vestige.Build.cs` doesn't depend on CQTest and `Vestige.uproject` doesn't enable it ([Roadmap.md](Roadmap.md), Tests). Whoever adds the first test also sets CQTest up, confirms the test runs, and then replaces this bullet with the exact module and plugin entries and the command that was used, and ticks the roadmap item, in the same commit. Until then, the expected starting point is `"CQTest"` in `PrivateDependencyModuleNames` and, if the build can't find the module, the CQTest plugin enabled in `Vestige.uproject`.
- **What to test**: rules and data, not visuals. Tile transitions (grass, tilled, planted, ripe), growth only when watered, harvesting, tool use with no seeds left, the day clock rolling over, and save data round-tripping.
- **Where**: `Source/Vestige/Tests/<ClassName>Tests.cpp`, wrapped in `#if WITH_DEV_AUTOMATION_TESTS` so tests never ship. Name tests `Vestige.<Area>.<Behavior>`, for example `Vestige.Tile.WateredCropGrowsOneStage`.
- **Running**: from the editor's Test Automation window (Tools menu), filtered on `Vestige`, or from the command line with `-ExecCmds="Automation RunTests Vestige"`.
- **Design for testability**: keep rules in code that doesn't need a running level. When rule code is tangled with visuals or actor setup, that's a sign to separate them (see [Architecture.md, Known gaps](Architecture.md#17-known-gaps)).
- Run the tests before committing a change to game rules. A bug fix gets a test that would have caught it.

## 14. Project layout and assets

### 14.1 Source

```
Source/Vestige/
    Vestige.Build.cs, Vestige.h/.cpp   Module, log category
    Vestige*.h/.cpp                     Template classes (character, game mode, player controller)
    Farm*.h/.cpp                        Our gameplay code
```

The folder is flat while it is easy to find things in. **When finding or changing related code gets awkward** (related files are hard to spot, or a feature's files are scattered between others), move to feature folders the way Lyra does (`Character/`, `UI/`, `System/`...). File count is only a hint; somewhere around 20 of our own files is a good time to ask the question:

```
Source/Vestige/
    Farming/    Tiles, grid, crops, tools, farming component
    Time/       Day cycle
    Player/     Farmer character, player controller
    UI/         HUD and widgets
    Save/       Save game and save subsystem
    Core/       Game mode, module, shared types
```

Do that move in its own commit with no other changes. Include by path from the module's source root (`#include "Farming/FarmTile.h"`). The template's `PublicIncludePaths` entry `"Vestige"` already puts that root on the include path, so nothing needs to change in `Vestige.Build.cs`. Don't add each feature folder to `PublicIncludePaths`: bare includes like `"FarmTile.h"` hide where a file lives, and every added path is exposed to any other module that depends on ours. The game is a single module today; if a second module ever needs our headers, give the module a `Public/` and `Private/` split and move only the headers meant for other modules into `Public/`.

### 14.2 Content and asset naming

Follow Epic's **Recommended Asset Naming Conventions** (https://dev.epicgames.com/documentation/en-us/unreal-engine/recommended-asset-naming-conventions-in-unreal-engine-projects): `Prefix_BaseAssetName_Variant_Suffix`. The table below lists the prefixes we use most; for any other asset type, use the prefix from Epic's page.

Our assets live under `Content/Farm/`, split by type. Leave the template's folders (`Characters`, `ThirdPerson`, `Input`) alone.

| Prefix | Asset type | Example |
|---|---|---|
| `L_` | Level / map | `L_Farm` |
| `BP_` | Blueprint class | `BP_FarmTile` |
| `WBP_` | Widget Blueprint | `WBP_Inventory` |
| `ABP_` | Animation Blueprint | `ABP_Farmer` |
| `DA_` | Data asset | `DA_Crop_Parsnip` |
| `SM_` | Static mesh | `SM_Crop_Ripe` |
| `SK_` | Skeletal mesh | `SK_Farmer` |
| `M_` / `MI_` | Material / material instance | `MI_Soil_Watered` |
| `T_` | Texture (with suffix `_D`, `_N`, `_ORM`) | `T_Soil_D` |
| `IA_` / `IMC_` | Input action / mapping context | `IA_UseTool` |
| `S_` / `SC_` | Sound wave / sound cue | `S_Hoe` |
| `NS_` | Niagara system | `NS_WaterSplash` |

Variants go after the base name with underscores: `SM_Crop_Stage0`, `SM_Crop_Stage1`.

Blender sources go in `Art/Blender/`, exported FBX files in `Art/Export/`, with the same names as the imported assets.

## 15. Git workflow

- **Small, focused commits.** One logical change per commit. Refactors and moves go in their own commits, separate from behavior changes.
- **Commit messages**: an imperative subject line under about 70 characters (`Move lighting from FarmGrid to FarmDayCycle`), a blank line, then a short body saying *what* changed and *why*.
- **Author and committer**: `David Bennehag <david.bennehag@gmail.com>`.
- **No `Co-Authored-By:` or `Claude-Session:` trailers** or any other tool attribution in commit messages.
- **No pull requests.** Commits are made locally on `main` on David's machine. David reviews and pushes. Nobody else pushes.
- Binary files (`.uasset`, `.umap`, `.blend`, `.fbx`, `.png`) go through Git LFS, as set up in `.gitattributes`.
- Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/` or `.vs/`.
- Code must **compile and be play-tested** in the editor before it is pushed.

## 16. Known deviations to clean up

Existing code that doesn't yet match these guidelines. Fix them when touching the code nearby, each in a small commit. Don't do a sweeping cleanup in the middle of a feature. Design-level gaps are listed in [Architecture.md, Known gaps](Architecture.md#17-known-gaps).

- `UFarmingComponent::Seeds` and `HarvestedCrops` are public mutable properties, and `UFarmSaveSubsystem` writes them directly. Make them private with getters and a `RestoreInventory(...)` function.
- Tunable properties on `AFarmTile`, `AFarmGrid` and `AFarmDayCycle` are public. Move them to `protected` / `private` with `AllowPrivateAccess` and add getters where other classes read them.
- `FarmHudWidget.h` needs `// clang-format off` / `on` around its Slate argument block.

## 17. Review checklist

Before committing, check:

- [ ] Each change lives on the class that owns that responsibility ([section 1](#1-ownership-and-separation-of-concerns)).
- [ ] Design changes are reflected in [Architecture.md](Architecture.md) (ownership table, decision log).
- [ ] Names follow the prefixes, the `Farm` prefix and PascalCase, and nothing shadows a base class member.
- [ ] Changed files are formatted with clang-format.
- [ ] New source files start with the copyright and SPDX license header.
- [ ] Headers forward declare; `.cpp` files include what they use, own header first.
- [ ] Strong `UObject` references are `UPROPERTY() TObjectPtr<>`; non-owning ones are weak, deferred assets are soft. Pointers that can be null are checked.
- [ ] Runtime state is private. Nothing new is exposed to Blueprints without a reason.
- [ ] No magic numbers, no duplicated logic, no commented-out code, no stray debug messages.
- [ ] Ticking is off unless needed. No per-frame searches of the level.
- [ ] Classes and non-obvious public members have `/** */` comments.
- [ ] Changed game rules have tests, and the `Vestige` tests pass.
- [ ] It compiles and works in Play in Editor.
- [ ] The commit message is clear and has no tool attribution lines.

## 18. Further reading

Official Epic references, checked against UE5:

- [Epic C++ Coding Standard](https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine): the baseline for this document.
- [Recommended Asset Naming Conventions](https://dev.epicgames.com/documentation/en-us/unreal-engine/recommended-asset-naming-conventions-in-unreal-engine-projects): asset prefixes and naming.
- [Class Specifiers](https://dev.epicgames.com/documentation/en-us/unreal-engine/class-specifiers), [UProperties](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-uproperties), [UFunctions](https://dev.epicgames.com/documentation/en-us/unreal-engine/ufunctions-in-unreal-engine) and [Metadata Specifiers](https://dev.epicgames.com/documentation/en-us/unreal-engine/metadata-specifiers-in-unreal-engine): what every `UCLASS`, `UPROPERTY`, `UFUNCTION` and `meta=` option does.
- [CQTest](https://dev.epicgames.com/documentation/unreal-engine/cqtest-test-framework-for-unreal-engine) and the [Automation Test Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine): writing and running tests.
- The Lyra Starter Game documentation and source: architecture reference (see [Architecture.md](Architecture.md#11-patterns-borrowed-from-lyra)).

Beyond Epic:

- [Tom Looman](https://www.tomlooman.com/): practical Unreal C++ articles on the gameplay framework, subsystems and performance, kept up to date with recent engine versions.
- [Game Programming Patterns](https://gameprogrammingpatterns.com/) by Robert Nystrom: not Unreal-specific, but its chapters on State, Observer, Component and Update Method describe patterns this project already uses. Older, but the patterns haven't aged.
