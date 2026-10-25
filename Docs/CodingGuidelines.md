# FarmSim coding guidelines

How code in this project should look and be organized. It builds on three sources:

1. **Epic's C++ Coding Standard** (https://dev.epicgames.com/documentation/unreal-engine/epic-cplusplus-coding-standard-for-unreal-engine). It is the baseline. Where this document says nothing, follow Epic.
2. **The Lyra Starter Game** sample. We borrow its architecture (small components, subsystems, data assets, events) but not its full machinery. See [section 4](#4-architecture-patterns-borrowed-from-lyra).
3. **Clean code**: small focused units, intention-revealing names, no duplication, no dead code.

The top priority is that the project stays **easy to read and easy to change** for one developer coming back to it after weeks away. When a rule here gets in the way of that, raise it and change the rule rather than quietly ignoring it.

Contents

1. [Ownership and separation of concerns](#1-ownership-and-separation-of-concerns)
2. [Naming](#2-naming)
3. [Formatting](#3-formatting)
4. [Architecture patterns borrowed from Lyra](#4-architecture-patterns-borrowed-from-lyra)
5. [Classes and headers](#5-classes-and-headers)
6. [UPROPERTY, UFUNCTION and Blueprint exposure](#6-uproperty-ufunction-and-blueprint-exposure)
7. [Memory, pointers and object lifetime](#7-memory-pointers-and-object-lifetime)
8. [Functions and control flow](#8-functions-and-control-flow)
9. [Modern C++ usage](#9-modern-c-usage)
10. [Errors, logging and assertions](#10-errors-logging-and-assertions)
11. [Comments and documentation](#11-comments-and-documentation)
12. [Performance and ticking](#12-performance-and-ticking)
13. [Project layout and assets](#13-project-layout-and-assets)
14. [Git workflow](#14-git-workflow)
15. [Known deviations to clean up](#15-known-deviations-to-clean-up)
16. [Review checklist](#16-review-checklist)

---

## 1. Ownership and separation of concerns

This is the most important rule in the project.

**Every actor, component and subsystem has one job. Logic, components and settings live on the class whose job they are, never on whichever class happens to be convenient.**

Current owners:

| Class | Kind | Owns | Does not own |
|---|---|---|---|
| `AFarmTile` | Actor | One tile's state (grass, tilled, planted, watered, growth) and its visuals | Time, input, saving |
| `AFarmGrid` | Actor | Spawning tiles, looking up a tile by location, advancing tiles on a new day | The clock, lighting, the day number |
| `AFarmDayCycle` | Actor | Day number, clock, sun and sky light, night exposure. Broadcasts `OnNewDay` then `OnNewDayFinished` | Tiles, saving, HUD |
| `UFarmingComponent` | Actor component | Current tool, seeds, harvest count, tile targeting, using a tool, sleeping and passing out | Drawing UI, input bindings, meshes |
| `AFarmerCharacter` | Character | Input bindings and the held tool mesh | Farming rules (delegates to `UFarmingComponent`) |
| `AFarmHUD` + `SFarmHudWidget` | HUD + Slate widget | Display only. Reads state from the day cycle and farming component | Any game state except fading messages |
| `UFarmSaveSubsystem` | World subsystem | Saving on `OnNewDayFinished`, loading on play start, starting a new game | Game rules |
| `UFarmSaveGame` | Save game | The saved data layout | Logic |

Before adding a component, property, timer or `Tick` logic to an existing class, ask: *is this that class's job?*

- If yes, add it there.
- If no, put it on the class that does own it, or create a new actor, component or subsystem for it. Say so explicitly in the commit message.

A real example from this project: night post-processing and sun control were first put on `AFarmGrid` because the grid was already in the level. They were moved to a dedicated `AFarmDayCycle`, which is auto-spawned if the level doesn't have one. That is the pattern to follow.

How to choose the right kind of owner:

| Need | Use |
|---|---|
| Something placed in the level, with a transform or editor-tweakable settings | `AActor` |
| A reusable behavior that attaches to an actor (the player, an NPC) | `UActorComponent` / `USceneComponent` |
| A single service per level that needs no transform (saving, lookups) | `UWorldSubsystem` |
| A service that must survive level loads (settings, profile, audio volume) | `UGameInstanceSubsystem` |
| Per-player UI, input or camera services | `ULocalPlayerSubsystem` |
| Static data describing content (a crop type, a tool, an item) | `UPrimaryDataAsset` (see [4.3](#43-data-assets-for-content)) |
| Plain data passed around or saved | `USTRUCT` |

Communication between owners goes **one way, through narrow interfaces**:

- Owners expose small public functions (`AFarmGrid::GetTileAtLocation`, `AFarmDayCycle::StartNextDay`) and read-only getters.
- Owners announce changes with delegates (`AFarmDayCycle::OnNewDay`, `UFarmingComponent::OnToolChanged`). Listeners subscribe. The broadcaster never knows who listens.
- Display code reads state; it never changes it. `AFarmHUD` reads from `UFarmingComponent` and `AFarmDayCycle`, but nothing in game logic depends on the HUD except to report messages.
- Never reach into another class to change its private state. If you need to, that class is missing a function.

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

The template classes (`AFarmSimCharacter`, `AFarmSimGameMode`, `AFarmSimPlayerController`) keep their generated names.

### 2.3 Case and wording

- **PascalCase for everything**: types, functions, members, locals, parameters, constants. No `snake_case`, no `m_` or `_` prefixes, no Hungarian notation beyond the prefixes above.
- Names are **descriptive and unabbreviated**: `ReachDistance`, not `RchDst`; `HarvestedCrops`, not `NumHarv`. Common, well-known short forms are fine (`HUD`, `UI`, `Max`, `Min`, `Num`).
- Functions that do something are **verbs**: `Till()`, `StartNextDay()`, `RefreshVisuals()`.
- Functions that return a `bool` ask a **question**: `IsRipe()`, `IsPastBedtime()`, `HasMessage()`.
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
- Keep lines under about 140 characters. Break long argument lists after a comma and indent one extra tab.
- Multi-line ternaries put the operator at the start of the next line, as in `AFarmTile::RefreshSoil`.
- One blank line between functions. Group related statements inside a function and separate groups with a blank line.

## 4. Architecture patterns borrowed from Lyra

Lyra is Epic's reference for a well-structured UE5 game. It is also built for a large team and multiplayer, so we take its ideas at the scale of a solo prototype. **Adopt a Lyra pattern when we have a concrete need for it, not before.**

### 4.1 Composition over inheritance

Lyra keeps actor classes thin and puts behavior in components (`ULyraHealthComponent`, `ULyraHeroComponent`). We do the same: the farmer is a template character plus a `UFarmingComponent`. New player abilities (fishing, an inventory, stamina) become new components rather than more code in `AFarmerCharacter`.

Keep inheritance trees shallow: engine base class, then at most one or two of our own levels.

### 4.2 Subsystems instead of singletons and managers

Lyra uses engine subsystems for global services. We use them instead of static globals, singletons or "manager" actors that only exist to be found. `UFarmSaveSubsystem` is the model: it is created automatically for every game world and found with `GetWorld()->GetSubsystem<UFarmSaveSubsystem>()`.

Use an actor instead when the thing needs a place in the level or settings a designer tweaks per level, like `AFarmDayCycle`.

### 4.3 Data assets for content

Lyra describes content in data assets (`ULyraPawnData`, `ULyraExperienceDefinition`) rather than hard-coding it. When we add more than one of something (crop types, tools, items, shop stock), describe each one in a `UPrimaryDataAsset` subclass such as `UFarmCropData` with its meshes, growth days and sell price. Code then works with any crop instead of having per-crop branches.

Until then, small tunables are `EditAnywhere` / `EditDefaultsOnly` properties on the owning class, as they are today.

### 4.4 Events over polling and direct calls

Lyra decouples systems with delegates and its gameplay message router. We use plain multicast delegates:

- `AFarmDayCycle` broadcasts `OnNewDay`; `AFarmGrid` listens and advances tiles.
- `OnNewDayFinished` fires after everything has reacted; `UFarmSaveSubsystem` listens and saves.
- `UFarmingComponent::OnToolChanged` lets `AFarmerCharacter` update the held mesh, including after a load.

Prefer non-dynamic delegates (`DECLARE_MULTICAST_DELEGATE_*`) for C++-only events. Use `DECLARE_DYNAMIC_MULTICAST_DELEGATE_*` only when Blueprints need to bind. Always unbind in `EndPlay` or `Deinitialize` when the broadcaster can outlive the listener.

### 4.5 Gameplay tags (later)

Lyra identifies almost everything with `FGameplayTag`. When we need open-ended categories that designers extend (item types, crop seasons, interaction types), switch from enums to gameplay tags. Small closed sets like `EFarmTool` stay enums.

### 4.6 Input

Lyra maps input actions to gameplay tags through data assets. For now, `AFarmerCharacter` creates its input actions in code so no assets are needed. When input grows, move to `UInputAction` and `UInputMappingContext` assets under `Content/Input`, and keep binding them in the character. Input handlers stay thin: they call into the owning component and contain no game rules.

### 4.7 What we deliberately do not adopt yet

The Gameplay Ability System, Experiences, Game Feature plugins, Common UI, the modular gameplay framework, and replication. Each adds a lot of setup that a single-player prototype doesn't need. Revisit when a concrete feature calls for one.

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
- New engine module dependencies go in `FarmSim.Build.cs`. Put them in `PrivateDependencyModuleNames` unless our public headers expose that module's types.

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

- Every `UObject*` member is a `UPROPERTY()` holding a `TObjectPtr<>`, so the garbage collector sees it. This applies even to private, non-edited members:

```cpp
UPROPERTY()
TObjectPtr<AFarmGrid> Grid;
```

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
- Expose to Blueprints (`BlueprintReadOnly`, `BlueprintCallable`, `BlueprintPure`) **only when a Blueprint actually needs it**. Everything exposed becomes an API we have to keep stable. Prefer `BlueprintReadOnly` to `BlueprintReadWrite`.
- Core rules live in C++. Blueprints are for content: picking meshes, tuning values, laying out levels, small visual-only behavior.
- Mark components created with `CreateDefaultSubobject` as `VisibleAnywhere`, and create them only in the constructor.

## 7. Memory, pointers and object lifetime

- Never use `new` / `delete` for `UObject`s. Create them with `NewObject`, `CreateDefaultSubobject` or `SpawnActor`.
- Members pointing at `UObject`s: `UPROPERTY() TObjectPtr<T>`.
- Non-owning references that may outlive the target and must not keep it alive: `TWeakObjectPtr<T>` (as `SFarmHudWidget` does with its HUD).
- References to assets that shouldn't load with the class: `TSoftObjectPtr<T>` / `TSoftClassPtr<T>`.
- Non-`UObject` heap data: `TUniquePtr`, `TSharedPtr`, `TSharedRef` (Slate widgets use `TSharedPtr`).
- Raw pointers are fine for locals and parameters that don't outlive the call.
- **Check pointers before using them** whenever they can legitimately be null: actors found in the level, `GetOwner()`, `GetWorld()`, `GetController()`, cast results. Use `IsValid()` for actors that may have been destroyed.
- Use `Cast<T>()` for `UObject` casts. Never `dynamic_cast` or C-style casts on `UObject`s. Use `CastChecked<T>()` only where a failure is a programming error.

## 8. Functions and control flow

- **Small functions that do one thing.** If you need a comment to separate the parts of a function, those parts probably want to be functions (`RefreshVisuals` calls `RefreshSoil` and `RefreshCrop`).
- Aim for functions that fit on one screen. Past about 40 lines, look for a split.
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

- Use UE types, not the standard library: `int32`, `uint8`, `float`, `double`, `FString`, `FName`, `FText`, `TArray`, `TMap`, `TSet`, `TOptional`, `TFunction`. Don't use `std::` containers or `int`/`long` where size matters.
- `FString` for runtime strings, `FName` for identifiers, `FText` for anything the player reads (so it can be localized later).
- Wrap string literals in `TEXT("...")`.
- `nullptr`, never `NULL` or `0`.
- `enum class` for every new enum. `UENUM(BlueprintType)` with `uint8` underlying type when it is used in properties.
- Range-based `for` over containers. Use an index loop only when you need the index.
- `auto`: only where the type is obvious from the right-hand side (`auto* Hud = Cast<AFarmHUD>(...)`), for iterators and for lambdas. Spell out the type otherwise. Write `auto*` and `const auto&` so pointers and references stay visible.
- Lambdas: capture explicitly (`[this]`, `[Tile]`). Never capture `this` in a lambda that can outlive the object; bind delegates with `AddUObject` / `AddWeakLambda` instead.
- `constexpr` for compile-time constants. `static constexpr` for class constants (`UFarmSaveGame::SlotName`).
- `FMath` instead of `<cmath>`.
- No exceptions and no RTTI; the engine builds without them.

## 10. Errors, logging and assertions

- Log with the project category `LogFarmSim` (declared in `FarmSim.h`):

```cpp
UE_LOG(LogFarmSim, Warning, TEXT("No FarmGrid in %s; farming is disabled."), *GetWorld()->GetMapName());
```

- Verbosity: `Error` for broken state, `Warning` for recoverable setup problems, `Log` for notable events, `Verbose` for detailed tracing.
- Never leave `GEngine->AddOnScreenDebugMessage` as a permanent feature. Player-facing feedback goes through `AFarmHUD::ShowMessage`.
- Assertions:
  - `check(Condition)`: a programmer error that makes continuing pointless. Compiled out in shipping builds, so never put side effects inside.
  - `ensure(Condition)` / `ensureMsgf`: something is wrong but the game can carry on. Logs a callstack once.
  - Plain `if` with a graceful fallback for things that can legitimately happen at runtime or through level setup (a missing grid, a missing save).
- Content mistakes a designer can make (a missing mesh, no grid in the level) must not crash. Fall back to a placeholder and warn, as `AFarmTile` does with its placeholder shapes.
- Failures the player should know about (a save that couldn't be written) are reported on the HUD.

## 11. Comments and documentation

- Every class gets a `/** */` comment saying **what it is for and what it owns**.
- Public functions and edited properties get a `/** */` comment when the name alone doesn't say everything: units, valid ranges, when it is called, what `nullptr` means.
- Inside functions, comment **why**, not what. Good: `// Static or stationary lights can't be turned at runtime.` Bad: `// Set the mobility to movable.`
- Write comments as full sentences in plain English.
- Don't leave commented-out code. Git remembers it.
- `TODO:` comments are allowed for known follow-ups but must say what is missing: `// TODO: Move tool meshes into UFarmToolData once there are more tools.`
- New files don't carry the Epic copyright line; the template's files keep theirs.

## 12. Performance and ticking

- Turn ticking **off by default** (`PrimaryActorTick.bCanEverTick = false`) and turn it on only for classes that really need per-frame work (`AFarmDayCycle` moves the sun).
- Prefer events, timers (`FTimerManager`) and delegates over checking something every frame.
- Don't search the level (`TActorIterator`, `GetAllActorsOfClass`) every frame. Find things once in `BeginPlay` and keep the reference.
- Don't load assets with `ConstructorHelpers` outside constructors, and don't load synchronously during gameplay.
- Optimize when something is measurably slow. Clear code first.

## 13. Project layout and assets

### 13.1 Source

```
Source/FarmSim/
    FarmSim.Build.cs, FarmSim.h/.cpp   Module, log category
    FarmSim*.h/.cpp                     Template classes (character, game mode, player controller)
    Farm*.h/.cpp                        Our gameplay code
    Variant_*/                          Unused template variants
```

The folder is flat while there are only a handful of files. **When it passes about 20 of our own files**, move to feature folders the way Lyra does (`Character/`, `UI/`, `System/`...):

```
Source/FarmSim/
    Farming/    Tiles, grid, crops, tools, farming component
    Time/       Day cycle
    Player/     Farmer character, player controller
    UI/         HUD and widgets
    Save/       Save game and save subsystem
    Core/       Game mode, module, shared types
```

Do that move in its own commit with no other changes, and add the folders to `PublicIncludePaths` in `FarmSim.Build.cs`.

### 13.2 Content and asset naming

Our assets live under `Content/Farm/`, split by type. Leave the template's folders (`Characters`, `ThirdPerson`, `LevelPrototyping`) alone.

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

## 14. Git workflow

- **Small, focused commits.** One logical change per commit. Refactors and moves go in their own commits, separate from behavior changes.
- **Commit messages**: an imperative subject line under about 70 characters (`Move lighting from FarmGrid to FarmDayCycle`), a blank line, then a short body saying *what* changed and *why*.
- **Author and committer**: `David Bennehag <david.bennehag@gmail.com>`.
- **No `Co-Authored-By:` or `Claude-Session:` trailers** or any other tool attribution in commit messages.
- **No pull requests.** Commits are made locally on `main` on David's machine. David reviews and pushes. Nobody else pushes.
- Binary files (`.uasset`, `.umap`, `.blend`, `.fbx`, `.png`) go through Git LFS, as set up in `.gitattributes`.
- Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/` or `.vs/`.
- Code must **compile and be play-tested** in the editor before it is pushed.

## 15. Known deviations to clean up

Existing code that doesn't yet match these guidelines. Fix them when touching the code nearby, each in a small commit. Don't do a sweeping cleanup in the middle of a feature.

- `UFarmingComponent::Seeds` and `HarvestedCrops` are public mutable properties, and `UFarmSaveSubsystem` writes them directly. Make them private with getters and a `RestoreInventory(...)` function.
- Tunable properties on `AFarmTile`, `AFarmGrid` and `AFarmDayCycle` are public. Move them to `protected` / `private` with `AllowPrivateAccess` and add getters where other classes read them.
- `TActorIterator` lookups for "the first grid in the level" appear in more than one class. Give `AFarmGrid` a static `Find(const UWorld*)` like `AFarmDayCycle::Find`.
- The `Variant_Combat`, `Variant_Platforming` and `Variant_SideScrolling` template folders (code and content) are unused. Decide whether to delete them; removing them also shortens `PublicIncludePaths` and drops the AI and StateTree module dependencies.

## 16. Review checklist

Before committing, check:

- [ ] Each change lives on the class that owns that responsibility ([section 1](#1-ownership-and-separation-of-concerns)).
- [ ] Names follow the prefixes, the `Farm` prefix and PascalCase, and nothing shadows a base class member.
- [ ] Braces on their own lines, always present, tabs for indentation.
- [ ] Headers forward declare; `.cpp` files include what they use, own header first.
- [ ] `UObject` members are `UPROPERTY() TObjectPtr<>`. Pointers that can be null are checked.
- [ ] Runtime state is private. Nothing new is exposed to Blueprints without a reason.
- [ ] No magic numbers, no duplicated logic, no commented-out code, no stray debug messages.
- [ ] Ticking is off unless needed. No per-frame searches of the level.
- [ ] Classes and non-obvious public members have `/** */` comments.
- [ ] It compiles and works in Play in Editor.
- [ ] The commit message is clear and has no tool attribution lines.
