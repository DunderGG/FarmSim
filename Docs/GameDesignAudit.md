# Game design document audit

**Reviewed:** 2026-10-26  
**Scope:** A critical review of [GameDesign.md](GameDesign.md) as a player-experience and production guide for FarmSim. It assesses clarity, internal consistency, player incentives, risk, scope and testability. It does not evaluate the game's actual fun through playtesting.

## Executive assessment

**Overall: 5/10.** The document has a distinctive premise and a good high-level identity: restorative farming in a robot-run post-apocalyptic world, with automation, relationships and a deliberately light touch. It usefully separates game design from lore, architecture and implementation planning.

The largest weakness is the distance between the breadth of the vision and the detail needed to make its systems cohere. The design names many compelling mechanics, but leaves important interactions, consequences and constraints open. Several systems reinforce each other in ways that could produce runaway difficulty or grind; others appear to conflict with the intended low-pressure play. The milestone section also overstates how much of the game's identity the proposed vertical slice demonstrates.

Scores use a 1–10 scale: 1 means unclear or risky; 5 means directionally useful but materially underspecified; 10 means coherent, testable and well matched to the project's intended experience.

| Area | Score | Assessment |
|---|---:|---|
| Premise and identity | 8/10 | Distinctive premise and strong thematic hooks; some pillar language is broader than the actual scope. |
| Core loop and player motivation | 5/10 | The farm-to-upgrade loop is understandable, but its short-term goals, rewards and decision pressure are not yet concrete. |
| System coherence | 4/10 | Water, radiation, land health, haze, solar power and weather create a promising web with insufficiently defined feedback controls. |
| Player experience and accessibility | 5/10 | “Your pace” and optional needs are good intentions, but late-night pressure, crop loss and exhaustion need guardrails. |
| Progression and economy | 4/10 | Many progression tracks are named, but prerequisites, pacing, overlap and the economy's role are unresolved. |
| Narrative and character design | 6/10 | Strong setting hooks and relationship premise; motivations, agency and story progression remain mostly placeholders. |
| Scope and production feasibility | 3/10 | Extremely broad for a solo project; the vertical slice is framed as proving more pillars than its content covers. |
| Modding and multiplayer goals | 4/10 | Good future aspirations, but “design every system for mods” and “multiplayer later” have substantial cost and are not bounded. |

## Findings

### G-01 — The vertical slice does not demonstrate every pillar as claimed

**Severity: High**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 13 and [Roadmap.md](Roadmap.md) now say the slice proves the farming core (three pillars in a small form) plus a first relationship with the caretaker robot, and name the pillars it does not test. Section 13 adds playtest criteria about the player's experience alongside the feature list.

At the time of the original review, section 13 said the one-season vertical slice should “show every pillar in a small form.” Its listed content includes a few crops, radiation/water, stamina, the caretaker robot, a sprinkler and selling. It does not include a rebuilt town facility, town events, a human discovery, a meaningful friendship/romance arc, exploration, or an implementation of the survival-needs toggle. Combat is explicitly deferred.

**Why it matters:** A milestone can be completed while the project still has not tested major pillars—especially town restoration and social relationships, which are described as core identity rather than optional extras. Calling the slice comprehensive risks giving a false sense of validation and delaying discovery of the hardest narrative and production problems.

**Recommended change:** Either narrow the claim to “validate the farming, resource and automation pillars plus an initial caretaker relationship,” or add one small, measurable proof for each pillar. Define acceptance criteria that test the intended player experience, not only feature presence.

### G-02 — The environmental feedback loop has no bounds or anti-runaway rules

**Severity: High**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 6 now bounds land health: 0 to 100 per map, a low-tech recovery path that needs no power, a 40% floor on solar output from haze, a cap on contamination per storm, and about one season from poor to fair with clean play. Storm frequency already no longer depends on land health (2026-10-27). [Roadmap.md](Roadmap.md) adds a best-case and worst-case simulation before energy is built.

At the time of the original review, sections 5–6 connected radiation, crop growth and mutation, land health, haze, solar output and the frequency of radiation storms. Healthy land improves solar power and reduces future storms; dirty shortcuts damage land and can reduce future power. The document does not specify caps, recovery rates, minimum energy availability, or whether storm frequency can create further land damage.

**Why it matters:** A positive feedback loop can become a runaway reward loop that makes recovery trivial, while a negative loop can become a softlock: low land health causes less power, insufficient power prevents purification, and more storms worsen the land. The design explicitly wants dirty choices to remain possible without forbidding them, which makes recovery paths particularly important.

**Recommended change:** Define the intended direction and limits of each relationship. Guarantee a viable low-tech recovery path, establish floors/ceilings and recovery times, and decide whether the system is primarily strategic feedback or a visual representation. Prototype a best-case and worst-case land-health trajectory before building all dependent systems.

### G-03 — “Your pace” conflicts with punitive late-night and seasonal consequences

**Severity: High**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 5 now states a gentle pressure level and adds guardrails: HUD warnings at 22:00 and 00:00, a single level of exhaustion that never stacks and clears after one early night, and no item or currency loss from night knock-outs until combat penalties are designed. Season-change loss is handled under G-12.

At the time of the original review, the fifth pillar promised no forced sleep and says the player decides how to spend their time. But the design also adds hostile robots after midnight, faster stamina drain, reduced stamina after late sleep, and crops dying at season change. The stated rule that enemies never damage property reduces one risk but does not resolve interrupted play, repeated player knockouts, or the loss of a nearly complete crop cycle.

**Why it matters:** These are overlapping time pressures. Players who explore, socialize or simply learn slowly may be punished both for staying up and for crossing a season boundary. That can make the game feel less like “your pace” and more like a hidden deadline system.

**Recommended change:** State the intended pressure level and provide guardrails: clear warnings, a grace period, an easy recovery from exhaustion, and a way to protect or recover valuable crops at season change. Playtest the worst case where the player ignores sleep and misses the last harvest; decide whether the resulting experience is acceptable.

### G-04 — Stamina at zero has no stated stopping condition

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 5 now says every action still works at zero stamina but takes twice as long and costs nothing more, stamina never goes below zero (so the faster late-night drain can't push it further), and sleep refills it fully, or to 75% when exhausted.

At the time of the original review, section 5 said the player can continue working at zero stamina, but more slowly. Section 13 also calls for a player to “run out of stamina” and “pay for staying up late.” No fatigue cap, action cost behavior, recovery rule, or relationship between stamina, exhaustion and work speed is described.

**Why it matters:** If actions remain repeatable indefinitely at zero stamina, stamina may be a time tax rather than a meaningful resource. If the slowdown is severe, it may instead become a soft lock or tedious wait. The document currently does not establish which experience is intended.

**Recommended change:** Specify what happens at zero, the minimum work rate, whether actions can still be performed, how stamina recovers, and whether late-night penalties stack with exhaustion and survival needs. Test whether the player can complete basic farm care without being trapped in a long action loop.

### G-05 — The optional survival setting is not defined across dependent systems

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 7 now defines the setting's contract: off removes the player's meters (hunger, thirst, radiation exposure, cold) with their HUD and penalties, farming systems (tile contamination, clean water for crops, storms) are the same in both modes, and the setting can change at any time, with meters starting full when switched back on.

At the time of the original review, section 7 said that with survival needs disabled, “nothing else should depend on needs being on.” Elsewhere, hunger, thirst and radiation sickness interact with food, clean water, stamina and radiation; section 5 adds cold and night radiation when needs are on. The document does not say whether disabling needs removes the systems, freezes them, or merely prevents penalties.

**Why it matters:** A setting that changes foundational resource rules can make crops, water, food and progression behave differently in ways that are difficult to balance or save consistently. An option intended to make play more casual may still leave players facing mandatory resource chores.

**Recommended change:** Define the option's exact contract: which meters, UI, penalties and dependencies are disabled; whether environmental radiation still affects tiles and crops; and whether saves can switch modes safely. Test a complete day and season in both modes and verify that progression remains possible.

### G-06 — Progression tracks overlap without an explicit hierarchy

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 9 now gives each track one job in a table: town restoration is the main story, the tech tree owns machines, automation, tool tiers and robot upgrades, relationships gate only that character's content, and skills give small bonuses but never gate. It also names the vertical slice's primary loop, its optional track and what the player has earned by the end of the season.

At the time of the original review, section 9 listed skills, tool tiers, a tech tree, town restoration and relationships as parallel progression tracks. The tech tree also unlocks tools, machines and robot upgrades; town facilities unlock shops, robots, areas and mechanics; relationships unlock recipes, upgrades and recruitment. Currency and materials are used across several of these.

**Why it matters:** The same reward may be gated by multiple tracks, or players may be required to grind an unintended track to access the main loop. Without a hierarchy, the tracks can become redundant menus rather than distinct kinds of progress.

**Recommended change:** Define the role of each track and one example of a meaningful dependency chain. For the vertical slice, identify which track is primary, which is optional, and what a player earns at the end of the season. Avoid making every system a prerequisite for every other system.

### G-07 — The economy has no early-game guarantee or meaningful trade-off specification

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 9 now sets first-season guarantees: starting seeds and a small purse, a cheap basic crop always for sale that grows in clean watered soil, a basic crop selling for at least twice its seed price, and town donations of fixed items kept separate from selling. A budget for an ordinary and a poor season comes before prices, as a [Roadmap.md](Roadmap.md) item.

At the time of the original review, the economy was described as “supporting,” but crops were also supposed to fund seeds, tools, tech unlocks and buildings, while the town restoration consumes player-produced crops, materials and machines. Dirty water, energy and materials add further resource demands. No starting supply, guaranteed income, price relationship or recovery from a failed harvest is described.

**Why it matters:** A poor first-season harvest could prevent buying seeds or restoring facilities, which in turn blocks the systems needed for a better harvest. Conversely, if crops can be sold without trade-offs, town donations and farming upgrades may compete only through arbitrary costs.

**Recommended change:** Define the minimum viable economy for the first season: starting stock, reliable seed access, sell-versus-donate trade-offs, and a way to recover from a bad crop outcome. Prototype a simple budget for one ordinary and one poor season before introducing the full economy.

### G-08 — The scope is not credible without an explicit content and production limit

**Severity: High**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 13 now defines a first release with explicit limits (maps, seasons, crops, characters, romance and systems), to revisit after the vertical slice, and names cut candidates kept out of the critical path: multiplayer, mod loading and a mod kit, animals, other camera angles, extra faction hubs and extra scavenging.

At the time of the original review, the long-term vision included farming simulation, four seasons, multiple resource networks, land health, building placement, automation, a town, schedules, dialogue, relationships, romance, companions, hostile AI, combat, survival needs, separate exploration maps, human characters, mod loading and multiplayer. The scope section defers many systems, but does not define a total target, a cut line, or which features are essential to a releasable game.

**Why it matters:** For a solo developer, this is enough breadth to risk years of partial systems and placeholder content. “Later” can read as a commitment even when the document says it is only a rough ordering. The realistic-art target and reliance on external assets add a separate content pipeline and integration risk.

**Recommended change:** Separate the product's minimum complete game from stretch goals. Set explicit limits for the first release (maps, crops, characters, relationship depth, systems) and name cut candidates. Keep multiplayer, mod loading and extensive romance/content out of the critical path until the core game has been proven and estimated.

### G-09 — “Design every system so a mod can add its content” is an unbounded requirement

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 2 now limits the goal to data-driven content discovered like base content, adds mod-specific extension points only for a concrete use, and says the game won't claim mod support until a supported scope and compatibility policy are decided.

At the time of the original review, section 2 said every system should be designed so mods can add its content. Section 13 later defers mod loading and a mod kit, while the architecture document describes mod support as a future capability rather than implemented functionality. Many systems—world zones, characters, recipes, tech trees, machine behavior—will require different extension surfaces and validation rules.

**Why it matters:** Treating mod extensibility as a universal acceptance criterion can create speculative abstractions and delay core gameplay, despite no working mod distribution or load pipeline. It also risks promising users compatibility that is not supported.

**Recommended change:** Reframe this as “avoid unnecessary hard-coding of content where data-driven design is already useful.” Add mod-specific extension points only after there is a concrete use case, and define a supported mod scope and compatibility policy before claiming mod support.

### G-10 — “Multiplayer later” is an aspiration that can quietly drive costly design

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 2 now makes single-player the design target, requires only ordinary separation of concerns, and defers authority, shared clock, sleep, farm ownership and saves to a separate co-op design milestone.

At the time of the original review, section 2 said systems should not make multiplayer impossible, while the decision log and [Architecture.md](Architecture.md) explicitly select single-player without replication. The open question correctly defers co-op decisions, but “not impossible” has no actionable boundary.

**Why it matters:** Contributors may introduce networking abstractions or restrictions preemptively, adding complexity to a single-player game without ensuring that the eventual time, sleep, farm ownership, save and NPC systems can actually support co-op.

**Recommended change:** Make single-player the current design target. Preserve ordinary separation of concerns and avoid gratuitous global assumptions, but do not require networking readiness. When co-op becomes funded work, decide its authoritative state, shared clock, save model and interaction rules as a separate design milestone.

### G-11 — Relationship progression is feature-rich but emotionally and mechanically underspecified

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 8 now requires visible reasons for every change in trust, makes romance an explicit opt-in that the character can decline, and sets one complete caretaker arc for the vertical slice. The caretaker starts loyal rather than sceptical, matching the lore. The arc (start, what changes trust, visible response, payoff) is in [Hob's character sheet](Spoilers/Characters/Hob.md#vertical-slice-arc), and [Roadmap.md](Roadmap.md) section 4.4 points to it.

At the time of the original review, section 8 proposed low or negative robot attitudes toward the player, gifts, requests, events, personal lore, recipes, upgrades, recruitment, romance and marriage. It does not say what distinguishes individual characters, how affection is communicated, whether negative opinions can be repaired, or how romance eligibility and consent are signaled.

**Why it matters:** The relationship system risks reducing characters to a checklist of gifts and unlocks, while the premise says each robot has a history and that earning trust should feel special. Starting at negative opinion can feel arbitrary unless the character's perspective and player's ability to respond are legible.

**Recommended change:** Define one relationship arc in the vertical slice: initial motivation, a player choice or action that changes trust, a visible response, and a payoff that is not only a resource unlock. Establish how boundaries and romance eligibility are communicated before expanding the cast.

### G-12 — Season transition rules can produce avoidable loss and unclear continuity

**Severity: Medium**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 4 now sets the timing (06:00 on the new season's first day) and makes the deadline visible: days to harvest on seeds, days left on the HUD, a warning when planting a crop that can't ripen in time, and a reminder from the caretaker robot. [Roadmap.md](Roadmap.md) section 4.2 lists these for the vertical slice.

At the time of the original review, the document said crops unsuited to a new season die, while multi-season and regrowing crops are deferred. It does not specify whether planted crops die at the exact transition, whether harvested crops remain available, how the player is warned, or whether each season belongs to a continuous calendar year.

**Why it matters:** The rule can turn a calendar transition into an opaque crop wipe, particularly when a crop's growth time is longer than the remaining season. This compounds the “your pace” issue and makes crop planning difficult without complete crop data.

**Recommended change:** Define transition timing and player notification, show expected days-to-maturity and the season boundary, and ensure crops offered early in a season can reach a reasonable harvest. Consider a grace or recovery mechanic before making seasonal crop loss a core source of challenge.

### G-13 — Several terms and mechanics need a shared rules vocabulary

**Severity: Low**

**Status: Resolved (2026-10-28).** [GameDesign.md](GameDesign.md) section 16 adds a mechanics glossary that names each gameplay value, where it lives, its lore alias, what changes it and what it affects, and states that tile contamination and player exposure are separate. Two relationships (how pollution is stored, and whether contaminated tiles expose the player) are marked open.

At the time of the original review, the document used “radiation,” “radiation sickness,” “night radiation,” “radiation storms,” “land health,” “pollution,” “Cinderium dust,” “haze,” “dirty machines,” “power cells” and “charge” without consistently separating fictional concepts from gameplay variables. For example, tile radiation affects growth and mutations, player radiation affects survival needs, and storms affect both the world and the player, but their relationships are not stated.

**Why it matters:** Different readers may interpret the same term as one shared meter or several systems. That ambiguity will spread into UI, tutorial messaging, content assets and balance.

**Recommended change:** Add a short mechanics glossary that distinguishes player exposure, tile contamination, map land health, pollution, weather effects and machine charge. State which are independent and which convert into one another; keep lore names as aliases for clear gameplay concepts.

## Positive foundations to preserve

- The premise and pillars provide a recognizable identity rather than a generic feature list.
- Separating lore hooks from the design rules keeps this document focused and lets the fiction evolve independently.
- The daily loop presents a useful high-level choice between tending the farm, socializing, engineering and exploring.
- The design explicitly avoids forced sleep, player death and enemies damaging crops/buildings, which are useful guardrails for a low-pressure game.
- The implementation order and roadmap connection help reduce the temptation to build every long-term system at once.
- The environmental-health concept ties theme, visuals and production together; it is worth prototyping, provided its feedback loops are bounded.

## Recommended correction order

1. [x] Narrow and define the vertical-slice promise (G-01; resolved 2026-10-28).
2. [x] Bound the land-health, radiation, storm and energy feedbacks before implementing them (G-02; resolved 2026-10-28).
3. [x] Decide how “your pace,” zero stamina, survival settings and season boundaries protect against softlocks and avoidable loss (G-03 through G-05, G-12; resolved 2026-10-28).
4. [x] Define the first-season progression and economy with a simple playable budget (G-06, G-07; resolved 2026-10-28).
5. [x] Establish a minimum complete game and defer unbounded scope (G-08). Bound the modding and multiplayer commitments (G-09, G-10). All resolved 2026-10-28.
6. [x] Prototype one character arc (G-11). Clarify the core mechanic vocabulary (G-13). All resolved 2026-10-28.

## Review limits

This is a design-document audit, not a playtest, market assessment or production estimate. Cross-checks were made against [Roadmap.md](Roadmap.md) and [Architecture.md](Architecture.md) to identify scope and intent mismatches. The findings identify risks to validate; they do not establish that any proposed mechanic will be unfun without player testing.
