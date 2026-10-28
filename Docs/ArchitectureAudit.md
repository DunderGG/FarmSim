# Architecture document audit

**Reviewed:** 2026-10-26  
**Scope:** A critical review of [Architecture.md](Architecture.md) as guidance for a C++ / Unreal Engine 5.7 game project. This assesses the document's accuracy, completeness, internal consistency and usefulness as an implementation guide. It does not certify that the code follows the document.

## Executive assessment

**Overall: 6/10.** The document has a strong foundation for a small single-player prototype: ownership is explicit, lifecycle hazards are called out, presentation is meant to stay separate from game rules, and known gaps are not hidden. Those are meaningful strengths.

The score is held back by guidance that is too absolute or underspecified in areas where mistakes are costly: Unreal world and level lifetimes, save ownership and recovery, content identity, and the claims around future mod support. Some statements could lead a future contributor to implement behavior that the document itself does not intend. The architecture is therefore a useful direction-setting document, but it should not yet be treated as a reliable contract for persistence or mods.

Scores use a 1–10 scale: 1 means misleading or absent; 5 means usable but has material gaps; 10 means accurate, specific and actionable for this project's scope.

| Area | Score | Assessment |
|---|---:|---|
| Ownership and separation of concerns | 8/10 | Clear responsibilities and a useful ownership table. Some class/layer rules are stated as universal laws rather than design tests. |
| Unreal framework and lifecycle accuracy | 5/10 | Good warnings about initialization order, but “level” and “world” are conflated and actor auto-creation needs tighter lifecycle constraints. |
| Dependencies and event flow | 7/10 | Good direction-of-dependency rules. The day-completion guarantee needs a synchronous-work boundary. |
| Persistence and save safety | 4/10 | Good intent around owner-produced save data and versioning, but ownership, failure recovery and data-loss behavior are not sufficiently specified. |
| Content identity and mod extensibility | 5/10 | Data-driven direction is sound; stable IDs and plugin-based mod support are presented more confidently than the defined mechanisms justify. |
| Testability and verification | 5/10 | Testing principles exist in the coding guide, but the architecture decision log and roadmap disagree about whether CQTest is set up. |
| Performance and growth guidance | 6/10 | Event-driven defaults and measurement are good; the actor-count transition example is an arbitrary threshold and an overly direct optimization prescription. |
| Decision traceability and known gaps | 8/10 | The decision log and gap list are unusually useful. They would benefit from priority, risk and completion criteria. |

## Findings

### A-01 — Unreal world lifetime is described as level lifetime

**Severity: High**

**Status: Resolved (2026-10-28).** Sections 2 and 3 now use world lifetimes throughout, explain the difference between streaming a sublevel, opening a map and seamless travel, describe the game mode as authoritative and server-only in multiplayer, and state the current assumption: one map, no streaming, `OpenLevel` only for a new game.

Sections 2 and 3 repeatedly describe framework objects and `UWorldSubsystem` as lasting “one level,” then say level loads destroy the world. Unreal's lifecycle boundary is generally the `UWorld`, not an individual streamed level. Streaming a level does not, by itself, imply that the world and its subsystems are recreated; map travel and seamless travel have different lifecycle behavior. `AGameModeBase` is also authoritative game-mode state, not a client-visible world service.

**Why it matters:** A reader could attach state to the wrong lifetime, incorrectly assume a subsystem will be recreated, or lose state during travel. This becomes especially risky once the game uses streamed levels, transitions, or separate menus.

**Recommended change:** Use “world” consistently for world-scoped actors and subsystems. Distinguish streamed-level changes from world/map travel, and state which travel mode the project currently assumes. Keep the existing warning that world `BeginPlay` ordering is not guaranteed.

### A-02 — World-scoped save coordination has no stated travel boundary

**Severity: Medium**

**Status: Resolved (2026-10-28).** Section 6 and the ownership table state the current scope (one world, one save slot, a new game deletes the slot and reopens the map) and the split to make before save slots, menus or travel: slot choice in a game-instance subsystem, the world snapshot in the world subsystem. Logged as a decision and an expected change.

**Section 13** assigns saving and loading to `UFarmSaveSubsystem`, a `UWorldSubsystem`. Section 16 explicitly includes deleting the save as part of “delete save, reload level”; the concern is not that the document claims reloading alone deletes persistent data. Rather, the document does not say whether this world-scoped service is only responsible for the current world's snapshot or also owns active-slot/profile selection, nor what happens if a save/load is in progress during world travel.

**Why it matters:** The current single-map prototype can reasonably keep this service world-scoped. If the game later adds map travel, menus or multiple slots, however, it will need an explicit boundary between world snapshot work and state that must survive world replacement.

**Recommended change:** State the current assumption (one world and one active save slot) and that “New Game” explicitly deletes or selects a different slot before loading. Before adding travel or multiple slots, separate world-scoped snapshot collection/application from persistent slot/profile ownership; put only the latter in a game-instance-lifetime owner if it must survive world replacement.

### A-03 — Save recovery rules risk hiding or overwriting user data

**Severity: High**

**Status: Resolved (2026-10-28).** Section 6 defines the outcome for an empty slot, a current, migratable, corrupt or newer save, and unknown content; an unreadable save is never overwritten without the player starting a new game; a failed write keeps the last good save; the mid-day loss window and synchronous saving are stated as accepted. Save tests are listed. The current code's overwrite of an unreadable save is recorded as a known gap and a roadmap item.

Section 6 says an old save should migrate “or fall back to a clean start with a message.” It does not distinguish a missing save, a corrupt save, an unsupported newer save, or a failed migration. It also does not say whether a fallback may later overwrite the original file. “Save at clear points” describes when the game saves, not what happens when the write fails or the game exits before the next day boundary.

**Why it matters:** Treating an unreadable or newer save as a clean start can turn a recoverable compatibility problem into permanent data loss. Saving only at day boundaries also means all progress since the previous boundary can be lost; that may be an intentional prototype trade-off, but it should be explicit.

**Recommended change:** Define observable outcomes for missing, valid, migratable, corrupt and newer-version saves. Preserve the original on failed load/migration, report errors, and do not overwrite it with a clean-start save without an explicit reset decision. Document the accepted loss window and whether saves are synchronous or asynchronous. Add round-trip, migration, failure and unknown-content tests to the save plan.

### A-04 — “Stable ID” has no rename or collision policy

**Severity: Medium**

**Status: Resolved (2026-10-28).** Sections 5 and 12 define one canonical persisted ID (a typed-in `ContentId`, `<Namespace>.<Name>`, that also builds the `FPrimaryAssetId`), its format and validation, a no-rename rule with redirects, duplicate and unknown ID handling, and the tests needed. Logged as a decision.

Sections 5 and 12 call `FPrimaryAssetId`, `FName` and gameplay tags stable identifiers and prescribe namespaced content IDs. These types are useful identifiers, but the document does not define which representation is canonical or how IDs survive asset renames, package changes, duplicate mod IDs, removed mods or conflicting content. An asset-derived ID is not automatically an immutable save identifier.

**Why it matters:** Saves and mod content may become unresolvable after a rename or package reorganization. A collision can make the same saved ID resolve differently depending on what content is loaded.

**Recommended change:** Pick one canonical persisted ID format and define its namespace and validation rules. Treat IDs separately from asset paths and display names. Document how renames are migrated or redirected, how duplicate IDs fail, and how unknown IDs are reported/skipped. Add tests for rename migration and duplicate/unknown IDs before relying on IDs in player saves.

### A-05 — Mod-friendly architecture is described too close to guaranteed mod support

**Severity: Medium**

**Status: Resolved (2026-10-28).** Section 12 now states "mod-oriented design, not mod support", explains that Unreal doesn't load plugins into packaged games on its own and that Game Features need deliberate activation, and adds a readiness checklist (discovery, scanning, activation, load order and ID conflicts, versions, saves without the mod, trust and consent, a documented API).

Section 12 calls content and Blueprint mods what the project supports, while also deferring loading mod plugins from a packaged build, load order, conflict handling and a mod kit. Asset Manager discovery and primary asset types are useful building blocks, but do not on their own make third-party plugin content discoverable, cooked, loadable or safe to activate. Game Feature plugins also require deliberate runtime activation and lifecycle design; they are not a general-purpose mod loader that works automatically.

**Why it matters:** Contributors may build against an implied mod API or promise compatibility before packaging, discovery, activation, dependency and save-compatibility behavior exists. Blueprint mods also execute trusted game logic, as the document correctly notes, so extension surfaces need an explicit trust boundary.

**Recommended change:** Describe the current status as **mod-oriented design, not supported mod loading**. Keep the data-driven rules as goals, and add a readiness checklist before claiming support: packaged plugin discovery/cooking, Asset Manager scan configuration, activation/deactivation, load order and ID conflicts, version compatibility, save behavior when a mod is absent, and user trust/consent.

### A-06 — The day-finished event is not a general completion barrier

**Severity: Medium**

**Status: Resolved (2026-10-28).** Section 15 states the synchronous-completion contract for `OnNewDay` listeners and the explicit completion barrier that replaces `OnNewDayFinished` if asynchronous day work is ever needed. Logged as a decision.

Section 15 says `OnNewDayFinished` ensures saving runs after every system has reacted, regardless of subscription order. That is true only for work completed synchronously by listeners during the preceding `OnNewDay` broadcast. A listener that starts a timer, latent action, async task or deferred spawn has not necessarily finished when that broadcast returns. The document does not define whether day listeners are required to complete synchronously.

**Why it matters:** If day advancement gains asynchronous work, the save can capture a partially advanced world while still appearing to honor the documented event order.

**Recommended change:** State the synchronous-completion contract explicitly if it is intentional. If any system must complete asynchronously, replace the second broadcast as a presumed barrier with an explicit completion coordinator/countdown or another mechanism that only signals completion after all required work reports done. Test the save snapshot against that contract.

### A-07 — The auto-spawn recommendation needs world and uniqueness constraints

**Severity: Medium**

**Status: Resolved (2026-10-28).** Section 2 limits find-or-spawn to required placed actors and sets rules: world-scoped and game-thread only, spawning only in running game worlds, `Get` as the only creator, warnings when it spawns or finds duplicates, and subsystems for services. The current `AFarmDayCycle::Get` is recorded as a known gap and a roadmap item.

Sections 2 and 3 recommend that a required placed actor find-or-spawn itself through a static `Get(World)` call. This can be a practical choice for `AFarmDayCycle`, which controls world actors and lighting, but the guidance does not specify when spawning is allowed, how duplicate instances are prevented, or how editor/preview worlds and teardown are handled.

**Why it matters:** A convenience lookup can silently turn a missing level dependency into an implicit runtime side effect. With streaming, editor previews or future multiple-world contexts, “find-or-spawn” can produce surprising ownership or duplicate actors if its world scope is not explicit.

**Recommended change:** Keep `Get` explicitly world-scoped and game-thread-only; reject invalid, editor-preview or tearing-down worlds as appropriate. Define one creation authority and a uniqueness rule. For required actors, also provide validation or a clear failure path so a bad level setup is not silently masked. Do not generalize this pattern to services that belong in a subsystem.

### A-08 — The CQTest decision is ahead of the project setup

**Severity: Medium**

**Status: Resolved (2026-10-28).** The decision log entry now reads "Adopt CQTest for game-rule tests; setup is pending" and points to the roadmap. The roadmap's save tests now include the tile save and restore ownership boundary.

Section 16 says “Tests use CQTest,” but the [Roadmap.md](Roadmap.md) Tests checklist still has “Set up CQTest in the project” unchecked. The [CodingGuidelines.md](CodingGuidelines.md) describes setup as work to do with the first test. This is a documentation-state mismatch, not a criticism of choosing CQTest.

**Why it matters:** The architecture decision log reads like a current implementation fact, while the roadmap says the framework is not yet installed/configured. Contributors may assume tests can be added and run now.

**Recommended change:** Rephrase the decision as “Adopt CQTest for game-rule tests; setup is pending,” then update the roadmap when the module/plugin dependency is actually configured and the first test runs. Include a test that exercises the key ownership boundary, such as tile state save/restore, not only isolated transitions.

### A-09 — The scale example prescribes a structural rewrite by actor count

**Severity: Low**

**Status: Resolved (2026-10-28).** Section 10 now keeps one actor per tile until profiling shows a cost, and presents grid-owned state arrays, instanced meshes and merged collision as options to weigh against the measured bottleneck, not a step triggered by tile count.

Section 10 contrasts an 8×8 field of tile actors with “fields of thousands” represented by an array of structs and instanced static meshes. The section also correctly says to measure before optimizing, but the specific threshold suggests actor count alone decides when to switch. Rendering, collision, ticking, component count, interaction frequency and target hardware all affect the trade-off; instanced meshes replace rendering representation, not automatically tile behavior or collision.

**Why it matters:** A premature rewrite could make tile rules and interaction harder to maintain without fixing the actual bottleneck.

**Recommended change:** Keep the current actor design until profiling demonstrates a bottleneck. Present data-oriented storage and instancing as options to evaluate against measured CPU, memory, collision and rendering costs, not as a threshold-based next step.

## Positive foundations to preserve

- The ownership table and “ask the owner to change its state” rule are strong; preserve them as systems are added.
- The initialization section correctly warns against relying on cross-actor `BeginPlay` order and discourages constructor-time world access.
- The distinction between game rules and presentation is sound, and the known-gap section clearly records current violations.
- The save-data direction—plain values/IDs, owner-provided serialization and restore without replaying gameplay—is a good target once the ownership boundary is implemented.
- The decision log and explicit “not adopted yet” entries prevent unnecessary framework adoption. Keep documenting choices, but distinguish decisions, current implementation and future intent.

## Recommended correction order

1. [x] Correct world-versus-level lifetime wording and the save subsystem/new-game description (A-01, A-02).
2. [x] Define save load/write failure behavior and the accepted save-loss window (A-03).
3. [x] Choose a canonical persisted content ID and rename/collision policy before adding more content or mods (A-04).
4. [x] Recast mod support as a future capability with explicit readiness requirements (A-05).
5. [x] Specify the synchronous day-event contract and constrain find-or-spawn behavior (A-06, A-07).
6. [x] Align the CQTest decision log with the roadmap, then revisit the actor-scale example (A-08, A-09).

## Review limits

This is a document audit, not a code audit or a certification against every Epic recommendation. The codebase was not traced to confirm each implementation statement. The CQTest status was cross-checked against the project roadmap and coding guidelines; other findings evaluate the claims and omissions in the architecture document itself. Revisit the audit when save migration, packaged mod loading, or world travel becomes an implemented feature.
