# Coding guidelines audit

**Reviewed:** 2026-10-26  
**Scope:** A critical review of [CodingGuidelines.md](CodingGuidelines.md) as practical C++ and Unreal Engine 5.7 guidance for FarmSim. This assesses correctness, consistency, clarity and fit for this project; it does not certify that the code follows the guide.

## Executive assessment

**Overall: 6/10.** The guide is well organized, concrete, and tailored to the project's Unreal workflow. Its emphasis on ownership, UObject lifetime, IWYU, formatting, tests and explicit error handling gives a solo developer a useful review checklist.

The main weakness is that several rules are absolute where Unreal and C++ need context, and some rules contradict either their own examples or [Architecture.md](Architecture.md). The biggest risks are in pointer ownership language, bool-returning action naming, test setup status, and guidance that could encourage presentation dependencies. The guide is a good local style baseline, but some rules should be clarified before treating it as a source of truth.

Scores use a 1–10 scale: 1 means misleading or absent; 5 means usable but has material gaps; 10 means accurate, specific and actionable for this project's scope.

| Area | Score | Assessment |
|---|---:|---|
| C++ and Unreal correctness | 6/10 | Many sound defaults, but pointer, loading and framework rules need qualifications. |
| Internal consistency | 5/10 | A few contradictions between naming rules and examples, plus a cross-document presentation mismatch. |
| Memory, reflection and lifetime guidance | 6/10 | Strong awareness of GC and weak references, but the blanket `TObjectPtr` rule obscures reference semantics. |
| Naming and formatting | 7/10 | Specific and mostly enforceable; some naming rules are too rigid for action APIs. |
| Build and module guidance | 5/10 | IWYU is good, but the proposed folder/include-path migration is over-prescriptive. |
| Testing and verification | 4/10 | Useful test intent, but CQTest is described as the standard before setup is complete. |
| Error handling and diagnostics | 6/10 | Good distinction between programmer errors and runtime failures; HUD advice needs to respect dependency direction. |
| Maintainability and usability | 7/10 | Strong layout and checklist; numeric thresholds and absolutes need rationale or softer language. |

## Findings

### C-01 — The bool naming rule conflicts with the `Till()` API

**Severity: Medium**

Section 2.3 says functions returning `bool` ask a question, with examples such as `IsRipe()` and `HasMessage()`. But the formatting example uses `AFarmTile::Till()` returning `bool` to indicate whether an action succeeded, and section 2.4 also uses the action-oriented `Till()` name.

**Why it matters:** The rule gives conflicting direction for common gameplay APIs that both perform an action and report success. A contributor could produce awkward names such as `DidTillSucceed()` or incorrectly change a useful result type.

**Recommended change:** Distinguish predicates from command results. Use question names for state queries (`IsRipe`); allow action names to return success, or prefer a consistent `TryTill` convention where the return value means the action may fail. Keep the documented choice consistent across examples and code.

### C-02 — “Every UObject member is TObjectPtr” blurs strong, weak and soft references

**Severity: High**

Section 6 says every `UObject*` member must be a `UPROPERTY()` holding `TObjectPtr<>`. Section 7 separately recommends `TWeakObjectPtr` for non-owning references and `TSoftObjectPtr`/`TSoftClassPtr` for deferred assets. These are different reference semantics, not interchangeable spelling choices.

**Why it matters:** `TObjectPtr` in a reflected property is a strong GC-visible reference. Applying it to a reference that should be weak can keep an object reachable longer than intended. Conversely, a soft asset reference has different loading behavior and should not be converted to a hard reference merely to satisfy the blanket rule.

**Recommended change:** Say that strong UObject references held by a UObject and intended to participate in GC reachability should normally be `UPROPERTY() TObjectPtr<T>`. Explicitly retain weak and soft references where their semantics are needed, with reflection annotations when appropriate. Describe ownership/reachability intent, not just pointer syntax.

### C-03 — CQTest is prescribed, but the project setup is still marked pending

**Severity: Medium**

Section 13 says to use CQTest for all new tests and gives setup instructions. The [Roadmap.md](Roadmap.md) checklist still marks “Set up CQTest in the project” as incomplete; the current [FarmSim.Build.cs](../Source/FarmSim/FarmSim.Build.cs) has no CQTest dependency, and [FarmSim.uproject](../FarmSim.uproject) does not enable the CQTest plugin.

**Why it matters:** The guide promises a test workflow that may not be available in a clean checkout. “Enable the plugin if the editor reports the module as missing” also leaves the required configuration dependent on an error rather than stating a verified setup.

**Recommended change:** Until setup and a sample test are verified, describe CQTest as the selected framework with setup pending. Once configured, record the exact plugin/module requirements and a command that has been run successfully. Keep the roadmap and guide synchronized.

### C-04 — Error feedback guidance can be read as coupling gameplay to the HUD

**Severity: Medium**

Section 10 says failures the player should know about, such as a failed save, “are reported on the HUD.” [Architecture.md](Architecture.md) says game rules must not depend on presentation and already lists direct `ShowMessage` calls as a known gap.

**Why it matters:** The coding guide could be read as authorizing exactly the dependency that the architecture document says should be removed. This also limits reuse of the save/gameplay code in contexts without the current HUD.

**Recommended change:** Require the failure to be reported to the player, but route it through a presentation-neutral event or feedback service. Let the HUD subscribe and display it; the save subsystem should log or return the failure independently of any widget.

### C-05 — The source-folder migration prescribes `PublicIncludePaths` too broadly

**Severity: Medium**

Section 14.1 says to add feature folders to `PublicIncludePaths` after the source is split into folders. Feature-folder paths are not automatically public API. Within a module, includes can use module-relative paths, and only headers intended for consumers of other modules should be exposed as public includes.

**Why it matters:** Publishing every feature directory can make private headers easy to include across module boundaries, weaken encapsulation and increase accidental compile-time coupling. The arbitrary “about 20 files” trigger also makes a structural change depend on file count rather than navigation pain or ownership boundaries.

**Recommended change:** Make folder organization a response to discoverability and feature boundaries, not a fixed file threshold. Use explicit module-relative includes or the appropriate private include paths for implementation-only headers. Expose only deliberate public headers to other modules.

### C-06 — “Use UE types, not the standard library” is broader than the stated reasons justify

**Severity: Medium**

Section 9 rules out `std::` containers categorically and says to use `FMath` instead of `<cmath>`. Unreal types are the right default for reflected properties, engine APIs, serialization and allocator-aware engine containers, but standard C++ library facilities remain useful and valid in non-reflected implementation details. The guide does not distinguish those cases.

**Why it matters:** An absolute ban may lead to inferior or less familiar implementations, or developers may assume standard-library utilities are unsupported by the toolchain. It also makes the guide more restrictive than necessary without identifying a concrete project risk.

**Recommended change:** Prefer Unreal types at Unreal API/reflection boundaries and for data that integrates with engine containers or serialization. Permit standard-library facilities in private implementation code when they are appropriate and supported by the project's compiler settings. Keep `FMath` as the normal choice for engine math types, not as a blanket prohibition on `<cmath>`.

### C-07 — The synchronous asset-load prohibition is too absolute

**Severity: Low**

Section 12 says not to load synchronously during gameplay. Avoiding blocking loads during active play is a good performance rule, but “during gameplay” can include a deliberate loading screen or initialization phase where synchronous loading may be an acceptable, bounded trade-off.

**Why it matters:** Read literally, the rule can force unnecessary async-loading machinery even when the player is not interacting and the load is intentionally blocking. It also does not state the actual concern: game-thread stalls and hitching.

**Recommended change:** Prohibit unbounded or latency-sensitive synchronous loads during interactive play. Prefer async loading for assets that may be large or unpredictably slow; allow explicit synchronous loads in controlled startup/loading flows when their cost and behavior are understood.

### C-08 — Several numeric rules are presented as universal thresholds

**Severity: Low**

Sections 8 and 14 prescribe a split at about 40 function lines and a folder restructure at about 20 project-owned files. These can be useful prompts, but neither number is a reliable quality threshold. A 41-line cohesive function may be clearer than a fragmented one, and 20 files may still be easy to navigate.

**Why it matters:** Numeric targets can encourage mechanical refactoring or create churn without improving comprehension. The guide's stated goal is readability, so the criteria should be based on responsibility and discoverability.

**Recommended change:** Retain these as review prompts rather than pass/fail rules: split when a function has multiple responsibilities or is hard to understand; organize folders when finding and changing related code becomes difficult.

## Positive foundations to preserve

- The ownership guidance and links to the architecture document make code placement a deliberate decision rather than a convenience.
- The warnings about UObject lifetime, `IsValid`, safe delegate binding and not capturing `this` in long-lived lambdas address common Unreal failure modes.
- The IWYU and forward-declaration guidance supports faster, more reliable builds when applied with Unreal Header Tool requirements in mind.
- The guide treats game-rule tests and bug-regression tests as normal work, rather than optional polish.
- The error-handling section distinguishes programmer invariants (`check`/`ensure`) from recoverable runtime conditions and designer mistakes.
- The review checklist and known-deviation list are practical tools for a small project, provided they stay synchronized with the actual build and architecture.

## Recommended correction order

1. Clarify strong, weak and soft UObject reference rules (C-02).
2. Resolve the bool naming contradiction and HUD feedback dependency (C-01, C-04).
3. Reconcile CQTest documentation with the actual project configuration (C-03).
4. Tighten module include-path guidance before source folders are reorganized (C-05).
5. Qualify the standard-library and synchronous-load rules; soften numeric thresholds (C-06 through C-08).

## Review limits

This is a documentation audit, not a compiler or static-analysis audit. Project configuration was checked for the CQTest setup claim, and the architecture document was checked for the HUD dependency rule. No C++ build, tests or formatter were run because no code changed. The findings assess the coding guide as guidance and do not claim that every rule is violated in the implementation.
