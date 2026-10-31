# Lore document audit

**Reviewed:** 2026-10-26  
**Scope:** A critical review of [Lore.md](Lore.md) as a narrative source of truth and a guide for writing characters, dialogue and world content. It assesses coherence, causal logic, character depth, tone, usability and fit with [GameDesign.md](../GameDesign.md). It does not assess the quality of prose that has not yet been written or replace player testing.

## Executive assessment

**Overall: 5/10.** The lore has a coherent high-level identity: a machine-run society inherits a world damaged by human decisions, while a returning human restores land through practical care. The automation theme connects the war, robot culture, the sleeper's learning curve and the farm's ecological systems. The comic advertising details also give the setting a recognizable voice.

At present, however, this is closer to a promising setting outline than a narrative source of truth. Important causal claims are not distinguished from robot propaganda, environmental rules are more evocative than explained, and only Hob has even a partial character arc. A few open questions concern the emotional core of the game, not peripheral lore. The document is useful for ideation, but writers could produce conflicting accounts while believing they are following canon.

Scores use a 1–10 scale: 1 means confusing or unusable; 5 means evocative but materially underdefined; 10 means coherent, character-driven and actionable for writing content.

| Area | Score | Assessment |
|---|---:|---|
| Core premise and thematic identity | 7/10 | Strong automation-and-inheritance theme; the player's personal role in it needs more definition. |
| Timeline and causal coherence | 5/10 | The broad chronology works, but responsibility for the war and the post-war transition are ambiguous. |
| World rules and plausibility | 4/10 | Cinderium, radiation, haze and energy are evocative but need consistent cause-and-effect rules. |
| Factions and social structure | 5/10 | Three useful ideological lenses, but limited institutions, internal diversity and political mechanics. |
| Character depth and story arcs | 4/10 | Hob has a compelling hook; most other characters are archetypes, roles or unresolved concepts. |
| Tone and emotional handling | 5/10 | The contrast of bleak history and absurd routine can work; boundaries for sensitive material are absent. |
| Writer usability and canon clarity | 5/10 | Good source-of-truth intent and naming flags, but facts, beliefs, proposals and unknowns are not consistently separated. |
| Fit with the game scope | 5/10 | Several hooks support the design, but some mysteries and character promises exceed the defined slice. |

## Findings

### L-01 — Objective history and robot belief are not clearly separated

**Severity: High**

**Status: Resolved (2026-10-27).** [Lore.md](Lore.md) now distinguishes confirmed history, faction interpretation and unknowns; establishes that both governments authorized the war after machine systems manipulated their decisions; clarifies that political responsibility is not shared equally by all humans; and makes the first strike permanently unresolved.

Sections 3, 4 and 6 establish that machines made most decisions, the war was fought almost entirely by autonomous machines, and both power blocs “pressed the button.” They also say neither side is the villain and deliberately leave the first strike unresolved. Yet the Continuance's central argument is “Humans started the war,” and humans are described as having rubber-stamped decisions they no longer understood.

**Why it matters:** It is unclear whether human responsibility is objective canon, robot prejudice, propaganda, or a contested historical conclusion. This is central to the game's main conflict: the player is judged for being human, and robot friendship is meant to challenge that judgment. If writers cannot distinguish the truth from a faction's interpretation, dialogue may accidentally validate the prejudice the story intends to examine.

**Recommended change:** Mark claims as **confirmed history**, **common belief**, **faction interpretation**, or **unknown**. Decide what is canon about human authorization and machine autonomy while preserving uncertainty about who struck first. Give the robots' accusations a defensible basis without making all humans a single morally responsible group.

### L-02 — The environmental science is evocative but causally underspecified

**Severity: High**

**Status: Resolved (2026-10-27).** [Lore.md](Lore.md) now defines Cinderium particles as fallout carried through air, soil and water; explains settling, washout and wind resuspension; distinguishes soil stabilization from contaminant removal; limits crop uptake to a mobile fraction that must be harvested and contained; and sets local versus regional recovery scales. [GameDesign.md](../GameDesign.md) now makes storm occurrence a weather event and land health a local influence on dust exposure, resuspension and solar output.

Sections 4 and 11 say Cinderium weapons released dust that remains in the upper air, causes radiation storms and forms a haze. Healthy land and crops bind the dust and pull it out of the air; damaged land makes it rise again. The document does not explain how dust bound in soil or plants returns to the atmosphere, how a local farm changes regional haze, or how land restoration reduces storm frequency.

**Why it matters:** This mechanism is the lore explanation for a major game system: farming changes solar output, visible skies and storm risk. Without stable rules, narrative descriptions, visual effects and gameplay tuning may contradict one another. Scientific realism is optional; internal consistency is not.

**Recommended change:** Define a small fictional ruleset: what Cinderium dust is, how it moves between air/soil/plants/water, what “cleaning” removes, and the geographic/time scale of land recovery. State explicitly which aspects are speculative and which are fixed canon. Keep player-facing explanations simple, but give writers and designers one consistent model.

### L-03 — The post-war world's survival infrastructure needs a clearer maintenance model

**Severity: High**

**Status: Resolved (2026-10-27).** [Lore.md](Lore.md) now distinguishes advanced Iron Creed and Index maintenance networks from Keeper towns' legacy, model-specific systems, whose supply chains and cross-role coordination have degraded. It explains selective service failures and establishes that Hob preserves a small farm system by prioritizing the pod and essential upkeep and scavenging parts.

The war ended seventy years before the game. Pre-war solar panels and machines still power towns and robots; robots can repair and build other robots, and some have rewritten their directives. At the same time, the neighboring town's pump, power and clinic are failing, while the farm caretaker has kept a cryo pod alive and maintained a garden for decades.

**Why it matters:** The contrast can be narratively powerful, but the present explanation—robots continue their assigned jobs and do not rewrite a function that is not theirs—does not yet explain why a capable society tolerates critical infrastructure failure. Nor does it clarify the energy, parts and repair chain that kept Hob and the pod going for so long.

**Recommended change:** Specify the limits that make these outcomes plausible: parts scarcity, incompatible systems, local politics, conflicting directives, damaged infrastructure, energy budgets, or deliberate refusal. Explain what Hob sacrificed or could not do to preserve the pod. Make the town's failure a consequence of the setting's institutions and history, not merely a convenient quest prompt.

### L-04 — The caretaker's order and the town's crisis are not reconciled

**Severity: Medium**

**Status: Resolved (2026-10-27).** [Lore.md](Lore.md) now defines Hob's order as preserving the farm and sleeper, not repairing the town or expecting the sleeper to do so. Town recovery is a cooperative effort requiring Keeper specialists and coordination; after the sleeper wakes, Hob has fulfilled the order and chooses whether to join.

Hob's last order is “Keep the farm until they wake up,” and the premise says Hob waited seventy years. Hob is also the player's tutorial guide and first companion. The town next door is falling apart and needs help, but the lore does not say whether the order prevented Hob from leaving, whether Hob tried to help, or why the player is the one who can cross that boundary.

**Why it matters:** Hob is the emotional bridge between the farm and the world. If the reason for Hob's prolonged isolation is vague, the companion's history and the town's initial condition may feel arranged for convenience rather than arising from character and world rules.

**Recommended change:** Decide how literally Hob interprets the order and what would count as endangering the farm. Give Hob a specific reason for not repairing the town earlier, and a specific change in belief or circumstance that makes helping possible after the sleeper wakes.

### L-05 — The human population and human rarity need a consistent scale

**Severity: Medium**

**Status: Resolved (2026-10-27).** [Lore.md](Lore.md) sets an initial target of four to six human survivors in the cast, including the sleeper and a definite sibling; describes human encounters as rare and significant, with human-looking robots contributing to mistaken sightings; and establishes that survivors cannot know whether other humans are globally scarce or hidden and unreachable. The sibling's role and survival path remain a separate open story question.

The war and first decade afterward kill or scatter most humans. The lore proposes about four to six survivors in the game and lists five human archetypes plus a possible sibling, who may overlap with one of them. Meanwhile, shelters, cryo pods and surviving communities appear across the world. It is not clear whether the archetypes are alternatives, the intended cast, or examples from a much larger population.

**Why it matters:** The world offers multiple independent ways for people to survive, but the story does not define why humans remain rare in the places the player visits or why these particular encounters are exceptional. The sibling overlap also makes the target cast size unclear.

**Recommended change:** Define whether “rare” means globally few, locally absent, socially hidden, or difficult to reach. Decide which concepts are planned characters, whether the sibling is one of them, and how they fit the target count. Explain why human communities did not grow or become visible over seventy years.

### L-06 — Factions need more than ideological labels to explain society

**Severity: Medium**

**Status: Resolved (2026-10-27).** [Lore.md](Lore.md) defines faction membership as voluntary and ideological, distinguishes networks from their hubs and from local communities, and gives each hub a concrete role, practice, internal disagreement and relationship with the other factions. It identifies the Bastion and Archive as Iron Creed and Index hubs, and Wellspring as the Keepers' main gathering hub while remaining an independently governed town. It also explains why the engineer came to town and why the failing legacy pump requires cooperation rather than one expert's repair.

At the time of the original review, the Iron Creed, Index and Keepers each ranked body, mind or purpose, but their voluntary membership, hubs, practices, relationships and internal disagreements were not defined. The Keepers were also conflated with older service models, and an Iron Creed engineer was described as “stuck” in a Keeper town without explanation.

**Why it matters:** Factions currently risk functioning as personality categories rather than social forces. The town's repair crisis, human treatment, and character conflicts depend on how these groups actually influence decisions.

**Recommended change:** For each faction, establish one concrete practice, institution or policy, one source of internal disagreement, and one relationship with the other groups. Clarify whether faction membership is ideological, model-based, geographic or voluntary. Explain how the engineer came to be in town and what keeps them there.

### L-07 — Most character entries are hooks, not playable character arcs

**Severity: High**

**Status: Partially addressed (2026-10-30).** The engineer, archivist and nomad now have dedicated character sheets, and each faction has a sheet with its practices, internal disagreements and story direction. Hob also has a dedicated sheet. The named characters have clearer wants and arcs, but several town characters and the sibling remain hooks, and none of these arcs has been implemented or playtested in a playable story slice.

Hob has a last order, history and story beat. The administrator, clinic robot and mail carrier are described mainly by role, a comic or unsettling premise, and one secret or attitude. The engineer now has a sequenced arc outline, and the archivist has an arc outline built around consent and context; the nomad has a fuller character outline. The rest of the cast remains underdefined, and these arcs still need implementation and validation in a playable slice. The rare humans are mostly types of survival stories. The character template asks for wants and unlocks, but most entries do not yet provide them.

**Why it matters:** The design promises memorable histories, friendship, romance, recruitment and personal events. Archetypes can inspire writing, but they are not enough to generate consistent dialogue or meaningful player choices. In particular, romance and the town's changing view of humanity need character-specific reasons and consequences.

**Recommended change:** As characters become planned content, define their present want, fear or contradiction, beliefs, what challenges those beliefs, the player's meaningful role and an outcome they choose themselves. When the game has a playable story slice, implement and test at least one complete arc, then review the other planned characters against the same standard. Keep uncommitted characters as hooks rather than expanding the whole cast in advance.

### L-08 — The family mystery's main payoff is still an unspecified, potentially over-gated message

**Severity: Medium**

The parents' fate is described as a mystery uncovered through letters, recordings, photographs and Hob's recovering memories. Hob has a sealed message that is delivered only “when the farm is whole again,” but the content is an open question and “whole” has no defined threshold.

**Why it matters:** This is the sleeper's central personal mystery and a late-game promise, yet the condition for revealing it could become an arbitrary completion gate. If restoration is open-ended or the player does not understand what “whole” means, the story may withhold its emotional payoff indefinitely.

**Recommended change:** Define the message's emotional function and what it changes for the sleeper, even if exact prose remains unwritten. Make the delivery condition observable and achievable, and ensure the mystery develops through discoveries that matter before the final message rather than relying on withheld information alone.

### L-09 — The tone has no guidance for handling its darkest material

**Severity: Medium**

**Status: Partially addressed (2026-10-30).** The faction sheets now include guardrails for portraying the human trials and their survivors. The lore still has no shared guidance for balancing comic relief with the broader story's violence, loss and medical vulnerability.

The setting includes mass death, radiation sickness, a failing cryo pod, a family mystery, prejudice against humans, a human held as an exhibit, and a clinic robot fascinated by the player's body. These coexist with product-launch festivals, jingles and absurd advertisements. The document says the tone is serious with comic relief, but gives no rule for where humor is appropriate.

**Why it matters:** The contrast can create both warmth and discomfort, but without boundaries, jokes may land on victims rather than institutions, or a disturbing character premise may be presented as harmless quirk. Different writers may produce incompatible tonal treatments.

**Recommended change:** State the target emotional balance and a few guardrails. For example, direct humor toward obsolete systems, propaganda and robot routines rather than human suffering; treat captivity, medical vulnerability and family loss with character-led seriousness. Decide whether the exhibit and clinic concepts are meant to unsettle, condemn, or invite player intervention.

### L-10 — The sleeper's own identity and agency are less developed than the world's history

**Severity: Medium**

The sleeper is customizable, gender-neutral and initially dependent on the caretaker. Their pre-war life is described as an ordinary person accustomed to machine advice; learning by hand and earning robot respect is the stated arc. No personal desire, skill, uncertainty about the sleeper's choices, or reaction to the new world is defined.

**Why it matters:** The story has extensive opinions about what robots think of humans, but the player character risks becoming a vessel for those opinions rather than an active person. The premise's “learning to decide” theme needs player-facing choices, not only a backstory assertion.

**Recommended change:** Define the sleeper's initial assumptions and the kinds of decisions the player makes that demonstrate growth. Preserve customization, but clarify which emotional beats are fixed and which can be expressed through player choice. Avoid making respect depend solely on the player proving usefulness to prejudiced robots.

### L-11 — Names and concepts are still provisional where downstream use is implied

**Severity: Low**

The file correctly marks many names as working names, but also uses them throughout the timeline, faction structure, companies, UI style, currency and story concepts. It directs contributors to search for the marker before content use, while there is no stable naming or ID registry beyond references such as `Base.Hob`.

**Why it matters:** A working name can accidentally enter a Blueprint, dialogue line or save-facing identifier and become expensive to change. Lore names, display names and stable content IDs are different things.

**Recommended change:** Maintain a concise canon/working-name table and distinguish display names from stable IDs. Before a term is used in shipped content or save data, mark its status and identify whether changing it requires a migration or compatibility alias.

## Positive foundations to preserve

- The automation theme connects history, current robot attitudes and the player's farm work.
- The war is not reduced to a simple villain faction, and leaving the first strike unknown can support competing accounts.
- The Continuance calendar and ritualized pre-war advertising are specific, reusable world details.
- Hob's patched body, damaged memory and long-held order are a strong first-character foundation.
- The document keeps lore separate from spoiler-free design and clearly warns contributors about spoilers.
- The stated rule against borrowing recognizable names, iconography or signature ideas is a useful originality guardrail.

## Recommended correction order

1. [x] Separate established fact from faction belief and propaganda (L-01; resolved 2026-10-27).
2. [x] Define the fictional rules of Cinderium dust, radiation and land recovery (L-02; resolved 2026-10-27).
3. [x] Explain infrastructure failure and Hob's maintenance limits (L-03; resolved 2026-10-27). Reconcile Hob's order and the town's crisis (L-04; resolved 2026-10-27).
4. [ ] **L-07 partially addressed:** Character sheets outline the engineer's and archivist's arcs; implement and validate a complete arc once a playable story slice exists, and develop other characters as they become planned content. Clarify the sleeper's agency (L-10) and the family mystery payoff (L-08).
5. [x] Set the human cast target and define why encounters are rare while global numbers remain unknown (L-05; resolved 2026-10-27). Define faction institutions and membership (L-06; resolved 2026-10-27). [ ] Add shared tone guidance before expanding the cast (L-09; partially addressed by faction-specific safeguards).
6. Establish a working-name and stable-ID policy before terms become content or save data (L-11).

## Review limits

This is a narrative-document audit, not a line edit, originality clearance or test of how players respond to the story. Cross-checks were made against [GameDesign.md](../GameDesign.md), especially its pillars, character systems and milestone scope. The findings identify questions that the current lore leaves open; some may be intentional mysteries, but those should be explicitly distinguished from unresolved authoring decisions.

## Follow-up audit (2026-10-30)

**Scope:** Reviewed the current [Lore.md](Lore.md), all character and faction sheets in this folder, the spoiler index and the relevant character, farming and progression rules in [GameDesign.md](../GameDesign.md). This follow-up adds findings not covered by the original review; it does not replace that review or assess implementation.

The new faction sheets materially improve writer usability: each faction now has a concrete social role, internal disagreement, relationships and guardrails. The character sheets also make the major arcs more actionable. The remaining new issues are concentrated in how those rules carry through to scenes and in a few unresolved links between lore and gameplay.

### L-12 — Cleo's first consent choice is not fully informed

**Severity: Medium**

**Status: Resolved (2026-10-30).** [Archivist.md](Characters/Archivist.md) now explains the requested data, temporary storage, intended audience, retention and withdrawal terms before the player chooses. The choices authorize technical records only, add a bounded family-rationing interview, or refuse access; the player reviews any excerpt before sharing, and refusal is not recorded as withheld testimony. Cleo's disclosure remains procedural so her character arc still develops toward understanding consent as ownership and stewardship, not just permission to collect.

In [Archivist.md](Characters/Archivist.md), Cleo asks for an “unrestricted direct interface” to the sleeper's pod log and family rationing memories. The willing response immediately begins the scan; only the cautious response reveals that the Archive, Review Council and other researchers may access the data. The choice therefore permits sensitive access before the sleeper knows its scope, audience or handling, despite the Index sheet's rule that consent must be informed, ongoing and reversible.

**Why it matters:** The first major choice in Cleo's consent-centered arc can reward agreement made without the information the scene later establishes as relevant. This weakens both the player's agency and the arc's ethical premise.

**Recommended change:** Have Cleo disclose the requested data, intended access and storage before asking. Make the choices set explicit limits, include a real refusal, and establish that the sleeper can review or withdraw permission later.

### L-13 — Gage's pressure-test summary contradicts its choice-specific outcomes

**Severity: Medium**

**Status: Resolved (2026-10-30).** [Engineer.md](Characters/Engineer.md) now records three distinct outcomes: the emergency abort finds a fractured pin and requires a reservoir flush; pushing ahead ruptures a gasket and adds cleanup and repair; stepped bleed-down passes without damage. Gage's later report and reflection vary by branch, preserve his accountability and the Keeper operator's warning, and no longer make one outcome canonical.

In [Engineer.md](Characters/Engineer.md), the arc summary says Gage chooses caution, stops the test and discovers a fractured shear pin. The scripted scene offers three different outcomes: emergency shutdown finds the pin fracture; pushing ahead ruptures a different gasket; stepped bleed-down stabilizes the system without damage.

**Why it matters:** The summary makes one branch canonical even though the scene gives the player meaningful alternatives. Later scenes may consequently refer to damage, risk or lessons that did not occur in that playthrough.

**Recommended change:** Make the arc summary explicitly branch-aware, or simplify the scene to one outcome. Carry each outcome into the repair cost and Gage's subsequent trust, accountability and dialogue rather than treating the branches as interchangeable.

### L-14 — Gage's formative accident is dated before the documented Iron Creed era

**Severity: Low**

**Status: Resolved (2026-10-30).** [Engineer.md](Characters/Engineer.md) now dates the accident to roughly five and a half decades after the war, after the Iron Creed and Bastion formed roughly three to five decades after it. The [IronCreed.md](Factions/IronCreed.md) summary uses the same relative date.

[Lore.md](Lore.md) places the story about seven decades after the war and says the Iron Creed and its hubs formed roughly three to five decades after it. [Engineer.md](Characters/Engineer.md) now places Gage's formative Bastion accident roughly five and a half decades after the war, early in his Bastion service. This keeps the incident after the hub's formation without assigning an absolute current year.

**Why it matters:** This is a chronology gap in a backstory that is central to Gage's beliefs and later choices.

**Recommended change:** Date the incident within the established post-war timeline, or establish that the Bastion predates the Iron Creed and explain Gage's service there before the network formed.

### L-15 — The frame-conversion hook does not preserve the trials' stated motive and victim-led response

**Severity: Medium**

**Status: Resolved (2026-10-30).** [Lore.md](Lore.md) and [IronCreed.md](Factions/IronCreed.md) now distinguish the conversion researchers' use of Creed readiness ideals from the dominationist current; their affiliation remains unknown. [Engineer.md](Characters/Engineer.md) reframes Gage as supporting responses chosen by the harmed people, with accountability aimed at responsible researchers and institutions rather than presumed faction-wide guilt.

[Lore.md](Lore.md) and [IronCreed.md](Factions/IronCreed.md) describe the conversions as abuse by a secret group whose stated motive was preparing humans to survive; most Creed members are unaware, and any response should begin with the harmed people's wishes. The [Gage sheet](Characters/Engineer.md) instead says he confronts “the radical commanders who mutilated human subjects.” The lore does not establish that the researchers were the Creed's anti-human radicals, whose defining belief is that humans should not get a second chance.

**Why it matters:** Equating distinct motives without explanation can flatten the faction's internal conflict and assign responsibility more broadly than the lore intends. Framing the payoff primarily as Gage confronting commanders also risks making his action, rather than survivors' choices and needs, the center of the reveal.

**Recommended change:** Clarify whether the researchers were members of the radical current and how their stated “preparation” rationale relates to it. Keep accountability specific, and define the affected characters' wishes and role in deciding what happens before scripting Gage's confrontation.

### L-16 — Crop contamination rules stop before food safety

**Severity: Medium**

**Status: Resolved (2026-10-30).** [Lore.md](Lore.md) and [GameDesign.md](../GameDesign.md) now distinguish soil contamination, mutation and food toxicity: only crops explicitly defined as harmful can expose the player, and harmful crops must be identified before consumption. Cleanup crops are non-food, and their harvest must be contained rather than eaten, composted or burned. The exact warning mechanic remains open.

[Lore.md](Lore.md) says ordinary plants may take up small amounts of Cinderium compounds and that special crops store a mobile fraction which must be harvested and safely contained to remove contamination. [GameDesign.md](../GameDesign.md) says mutated crops can be harmful to eat and that eating harmful crops can expose the player, but the lore does not define how a player or character can distinguish unsafe food, whether ordinary produce from contaminated soil is safe, or whether cleanup crops are food or waste.

**Why it matters:** Farming, food and environmental recovery share the same crops. Without a clear rule, dialogue and item descriptions can treat a crop as safe food while the gameplay treats it as a radiation hazard, or imply cleanup through harvest without clarifying safe disposal.

**Recommended change:** Establish a compact writer-facing rule for edible versus hazardous crops, how contamination and mutation affect that status, and how cleanup crops are handled. Keep the specific detection or treatment mechanics open if they are not yet designed.

### L-17 — The darker story lacks a paced reveal timeline

**Severity: High**

**Status: Partially addressed (2026-10-30).** The new [StoryTimeline.md](StoryTimeline.md) establishes provisional early, middle and late phases, emotional pacing, knowledge guardrails, optionality and the parallel family thread. Exact quest prerequisites, survivor identities and wishes, and the final family-message payoff remain open until a playable story slice defines them.

[Lore.md](Lore.md) says the Continuance Trials become hard to deny in the middle of the story, and the faction sheets describe optional investigation areas such as the Frame Conversion Wing and Cognitive Wing. The documents do not establish when the player first encounters these subjects, how early clues differ from later evidence, or how the reveals fit the opening's cozy farming and relationship experience. The [Gage sheet](Characters/Engineer.md) also introduces the frame-conversion investigation as a hub hook without a clear story-stage boundary.

**Why it matters:** The trials, human captivity and other darker material can substantially change the emotional contract of the game. If these elements surface too soon or without gradual context, they may undermine the light, cozy opening; if clues and reveals are not sequenced, writers may disclose the central truth prematurely or make earlier content feel misleading rather than retrospectively meaningful.

**Recommended change:** Treat discovery of the darker history as a long-term story goal. Keep the opening light and cozy, then let gameplay gradually complicate that impression through notes, recordings and other contextual clues before any full reveal. Set up a story-event timeline soon: identify the early, middle and late-game emotional tone; place the first hints, corroborating evidence and full disclosures; define the prerequisites and optionality of each reveal; and track what the player and characters know at each stage. The timeline should coordinate the frame conversions, cognitive integrations, living exhibit, family mystery and other major revelations without requiring every player to pursue every optional investigation.

### Previous findings still open

- **L-07 — Partially addressed:** Faction and character sheets now provide substantially more writing direction; implementation and playtesting of a complete arc, and development of the remaining planned cast, are still outstanding.
- **L-08 — Open:** The parents' message and the observable threshold for the farm being “whole again” remain undecided.
- **L-09 — Partially addressed:** The faction sheets' trial-specific safeguards do not yet establish shared tone rules for humor and other dark material.
- **L-10 — Open:** The sleeper's personal wants, initial assumptions and player-expressed growth remain less defined than the surrounding characters' views of them.
- **L-11 — Open:** Working names remain in use and there is still no concise policy distinguishing changeable display names from stable, save-facing IDs.
- **L-17 — Partially addressed:** [StoryTimeline.md](StoryTimeline.md) establishes provisional phases, reveals and knowledge guardrails. Exact quest prerequisites and final character-specific outcomes still need the playable story slice.

**Review limits:** This is a cross-document narrative and world-rule audit, not a technical or scientific validation of Cinderium, a gameplay playtest, or a judgment on final dialogue. The branch and agency findings identify authoring risks; testing whether their eventual treatment lands will require a playable story slice.
