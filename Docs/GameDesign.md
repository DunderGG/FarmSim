# FarmSim game design

What the game is, how it plays and why. This document steers development: when a feature idea comes up, check it against the pillars and the scope here first.

- [Architecture.md](Architecture.md) covers how the code is put together. [CodingGuidelines.md](CodingGuidelines.md) covers how the code looks.
- The story, the world's history, the currency and each character's backstory go in the separate lore document, [Lore.md](Spoilers/Lore.md). This document only says where the game needs lore, and marks those places with **Lore hook**.

Keep this document up to date. When a design choice is made or changed, update the relevant section and add a line to the [decision log](#14-decision-log).

Contents

1. [The game in one paragraph](#1-the-game-in-one-paragraph)
2. [Pillars](#2-pillars)
3. [Premise and setting](#3-premise-and-setting)
4. [Game loops](#4-game-loops)
5. [Time, days and seasons](#5-time-days-and-seasons)
6. [Farming](#6-farming)
7. [Survival needs](#7-survival-needs)
8. [Robots and people](#8-robots-and-people)
9. [Progression](#9-progression)
10. [World and exploration](#10-world-and-exploration)
11. [Combat](#11-combat)
12. [Presentation](#12-presentation)
13. [Scope and milestones](#13-scope-and-milestones)
14. [Decision log](#14-decision-log)
15. [Open questions](#15-open-questions)

---

## 1. The game in one paragraph

A 3D life and farming sim set in a retro-futuristic, post-apocalyptic world. You wake from decades of cryosleep under an overgrown pre-war homestead. Humans have almost vanished. Robots carry on with the jobs people gave them, often for nobody. You bring the farm back to life, from hand tools to automated machines, help rebuild the struggling robot town next door, form friendships and romances with robots and with the rare humans you find, and slowly push out into the wasteland. The tone is mostly serious, with comic relief throughout.

## 2. Pillars

Every feature should serve at least one pillar. Features that serve none are cut or postponed.

1. **Bring the land back.** Farming is the heart of the game. Poisoned soil, scarce water and storms make growth feel earned, and a thriving farm in a dead world is the main reward. Healing the land also clears the sky above it and powers the farm.
2. **Old hands, new machines.** Progress runs from rusty hand tools to pre-war tech to high-tech automation. The game mixes old-fashioned farm life with futuristic technology, and automation is a goal in itself.
3. **A world of robots, and rare people.** Robots fill every role people used to: neighbours, friends, love interests, helpers and enemies. Each one has its own history. Humans are rare, and finding one should feel special.
4. **Serious world, light touch.** The setting is grim and the stakes are real, but the game is warm and funny in its details: robots stuck in pre-war routines, absurd old advertising, dry humour.
5. **Your pace.** No forced sleep, no failing out of the game. Days have limits and late nights have costs, but the player decides how to spend their time. Survival needs can be switched off.

**Design goal: moddable.** Crops, items, tools, robots, recipes and tech tree entries are data that mods can add. See [Architecture.md section 12](Architecture.md#12-designing-for-mods). Every system in this document should be designed so a mod can add more of its content.

**Design goal: multiplayer later.** The game is solo first. Co-op is wanted some day, so systems should not make it impossible (see [open questions](#15-open-questions)).

## 3. Premise and setting

### Premise

- The player wakes in a failing cryo pod in a hidden room under an old farmhouse. They don't know exactly how long they've been asleep.
- The farm's **caretaker robot** has kept the place barely alive for decades, waiting for "the family" to wake up. It is the player's first relationship and the tutorial guide.
- Robots now run most of society, and they don't see themselves as lesser than humans. More and more of them consider themselves clearly superior. Humans are at best a curiosity, and some robots are openly hostile to them.
- Next to the farm lies a small **robot town**: a settlement of robots still running their pre-war jobs, falling apart because nobody fixes the water pump, the power line or the clinic. Many robots there won't admit that they need a human's help.
- **Main goal:** restore the farm and rebuild the town. The town's broken facilities are restored with crops, materials and machines the player produces, and each restored facility changes the town and unlocks something new.
- **Lore hook:** how robots came to run society and why so many feel superior to humans; how and why the player ended up in the pod, what the war was, what happened to the family, why the caretaker robot stayed.

### Setting

- A post-apocalyptic countryside decades after a war. Ruins, overgrowth, radiation, scrap, pre-war tech that sometimes still works.
- The world is our own. Don't borrow names, iconography or signature ideas from other games or franchises. The cryo pod, the robot-run world and the rarity of humans are our identity.
- **Lore hook:** the world's history, factions, the currency, the companies that built the robots.

## 4. Game loops

### Core loop (minutes)

Prepare the soil, plant, water, protect, harvest, then use or sell the harvest to improve the farm. Each step is a tool action on a tile.

### Daily loop (one in-game day)

1. Wake at 06:00 with full stamina (less if exhausted, see [section 5](#5-time-days-and-seasons)).
2. Tend the farm: water, harvest, clear radiation, refill water stores.
3. Spend the rest of the day on a choice: visit robots in town, work on machines and the tech tree, explore a wasteland zone, gather materials.
4. Go home and sleep. The day ends, crops grow, the game saves.

The day should always offer more to do than there's time for, so choosing is the game.

### Seasonal loop (28 days)

- Each season has its own crops, weather and town events.
- Crops that don't fit the new season die when it changes, so planning matters.
- A season ends with a festival or event in town (**Lore hook:** pre-war holidays the robots still celebrate, without remembering why).

### Long loop (years)

- Restore the town facility by facility.
- Climb the tech tree from hand tools to a fully automated farm.
- Deepen relationships, find the rare humans, and possibly marry.
- Open up wasteland zones further from home.

## 5. Time, days and seasons

- **Clock:** 24-hour display (06:40, 21:30). A day runs from 06:00 onwards.
- **Seasons:** four seasons (spring, summer, autumn, winter) of 28 days each. The wasteland twist comes from weather events, not from strange seasons.
- **Weather:** clear, rain (waters crops), dust storms, and radiation storms (raise tile radiation, dangerous outdoors). Weather is announced a day ahead (**Lore hook:** a robot weather broadcaster).
- **Stamina:** tool actions cost stamina. At zero, the player can still move but works slowly.
- **No forced sleep.** The player can stay up as long as they like, but late nights cost:
  - After midnight, **hostile robots** roam the farm and its surroundings. They attack the player, never crops or buildings.
  - Stamina drains faster.
  - Sleeping late makes the player **exhausted**: the next day starts with less stamina.
  - With survival needs on, cold and night radiation also build up (see [section 7](#7-survival-needs)).
- This replaces the prototype's "pass out at 02:00" rule.

## 6. Farming

### Tiles

A farm tile has a state (grass, tilled, planted), and on top of that:

- **Water level:** watered tiles grow. Water is a resource (see below), not free.
- **Radiation level:** contaminated soil grows crops slowly or not at all, and can cause mutations. Clean tiles with tools and machines (soil filters, special crops that absorb radiation, fertilizer).

### Crops

- Each crop is a data asset with a namespaced ID (`Base.Carrot`). It defines its seasons, growth stages, water needs, radiation tolerance, yield and value.
- **Mutated crops** are a special kind: they grow in irradiated soil and can be useful (more yield, special effects, rare ingredients) or risky (harmful to eat, unpredictable). A normal crop in irradiated soil may also mutate.
- Crops that span several seasons and regrowing crops come later.

### Water

- Clean water is stored and has to be secured: a well, rain collectors, a water purifier. The watering can fills from these sources, not from thin air.
- Dirty water is easy to get but raises tile radiation.
- Automation (sprinklers, robot farmhands) uses the same water stores.

### Buildings and machines

- The player places buildings and machines on the farm grid: greenhouse, water purifier, rain collector, robot charging station, storage, crafting stations.
- The house can be decorated.
- **Later:** animals, as mutated livestock (**Lore hook:** what they mutated from).

### Energy

- Solar is the main energy source. **Solar panels** charge **power cells**, and power cells run the farm's machines and automation: water purifiers, robot farmhands, the robot charging station, later machines. Power cells are a resource, not a currency.
- A panel's daily output depends on the time of day (nothing at night), the weather (dust storms block most of it) and **the haze** over the farm (see below).
- Machines draw a set amount of charge to run. When the stored charge runs out, they stop until it's recharged. Early tools and simple sprinklers don't need power, so a new player isn't blocked by energy.
- Energy sources (panels, later other generators) and consumers are data assets, so mods can add both.
- **Lore hook:** Sunstone dust and the haze, see [Lore.md section 4](Spoilers/Lore.md#energy-after-the-war).

### Land health and the haze

The player's actions slowly improve or damage the environment, and a healthier environment gives more solar energy. A restored farm is visibly a brighter place.

- Each map has a **land health** value, computed from its tiles: clean, planted and healthy tiles raise it, and irradiated, polluted or bare tiles lower it. It changes slowly, over days rather than minutes.
- **Improves land health:** growing healthy crops, cleaning radiation, using clean water, leaving ground planted or covered, and growing certain mutated crops that absorb Sunstone dust especially well.
- **Damages land health:** dirty water, burning scrap, stripping soil without replanting, and "dirty" machines that are fast or cheap but pollute.
- **Effects:** land health sets how thick the haze is over the map. A thinner haze means more solar output, a brighter and more colourful sky and landscape, and over time fewer radiation storm effects on that map.
- **The trade-off:** dirty shortcuts (dirty water, polluting machines) make the farm faster now and weaker later. The game never forbids them; the player chooses.
- Land health should be easy to read: a simple meter or sky indicator, and haze you can see.

### Implementation order

1. Radiation on tiles and mutated crops.
2. Clean water as a resource.
3. Weather events.
4. Solar energy and power cells, then land health and the haze.
5. Scavenging (later, together with exploration).

## 7. Survival needs

- **Needs:** hunger, thirst and radiation sickness. Eating crops and drinking clean water keep them in check, which feeds back into farming.
- **Optional:** a setting turns survival needs off for casual players. With them off, the game is a pure farming and life sim; nothing else should depend on needs being on.
- Needs never kill the player outright. Neglecting them lowers stamina and work speed, and at worst the player wakes at home with a penalty (the same as being knocked out in combat).

## 8. Robots and people

### Robots

Robots fill every role a human would. They don't see themselves as servants: most start out wary of or dismissive towards the player, and some are hostile. That attitude is what makes winning a robot's friendship, and later its love, feel special.

- **Townsfolk and friends:** each robot has a name, a personality, a daily schedule, things it likes and dislikes, and its own lore and backstory.
- **Love interests:** romance is an important part of the game, with both robots and humans.
- **Farm help:** automation robots that water, harvest and haul, unlocked through the tech tree. Robots befriended in town can also be recruited to the farm.
- **Companion:** the caretaker robot, and later others, can follow the player and help in the field.
- **Enemies:** feral or hostile robots in the wasteland and on the farm at night.

### Humans

- Very rare. Each human the player finds is a story moment and a special relationship.
- Found through exploration and story progress, not standing around in town.
- **Lore hook:** who they are, how they survived, why they are alone.

### Relationships

- Robots start at a low or even negative opinion of the player, because the player is human. Humans start neutral or friendly.
- Friendship grows by talking, giving gifts, helping with requests and taking part in events.
- Higher friendship unlocks: the robot's backstory and personal events, gifts and recipes, upgrades for that robot, recruiting it to the farm, and romance with eligible characters.
- Romance leads to dating and marriage. A partner moves to the farm.
- Characters are data assets so mods can add new robots and humans.

## 9. Progression

The player grows along several tracks at once:

- **Skills:** farming, engineering, scavenging and combat. They level up by doing, and each level gives a small bonus or unlock.
- **Tool tiers:** rusty, then pre-war, then high-tech. Better tools cost less stamina, work on more tiles at once, or do new things (for example a tool that cleans radiation).
- **Tech tree:** the backbone of engineering progress. Branches include automation (sprinklers, then robot farmhands, then full automation), tools, water and soil machines, energy (solar panels and power cells), and robot upgrades. Nodes are unlocked with materials, currency and engineering levels. Tech tree entries are data assets so mods can add branches.
- **Town restoration:** the main story progress. Each restored facility (water pump, power, clinic, market...) opens new shops, robots, areas or mechanics.
- **Relationships:** see [section 8](#8-robots-and-people).

### Economy

- There is one currency from the start (**Lore hook:** what it is).
- Selling crops and goods gives currency, which buys seeds, tools, tech tree unlocks and buildings.
- The economy is a supporting system, not a focus early on. Balancing and depth (shops, prices, trading) come later.

## 10. World and exploration

- The world is a set of **small separate maps** with loading between them. This is far easier to build and run than one open world.
- **The farm:** the player's home and the biggest map.
- **The robot town:** next to the farm. Shops, robots, events, facilities to restore.
- **Wasteland zones:** further out and unlocked over time, for example a ruined city, an old research facility, a dam. Zones are where enemies, scavenging, rare materials and the rare humans are.
- Zones are data driven, so mods can add new ones.

## 11. Combat

- Simple real-time combat: melee (a wrench to start) and ranged (improvised guns later).
- Never the focus. The farm and town are mostly safe by day. Combat gets harder further out in the wasteland and on the farm at night.
- Losing a fight never ends the game: the player wakes at home with a penalty (some lost items or currency, exhaustion).

## 12. Presentation

- **Camera:** third-person, as now. Other camera angles (for example a higher, angled view) may be added later, so camera code should not be hard-wired into gameplay.
- **Art:** aiming for a more realistic look. The current low-poly assets are placeholders. Realistic art is a big cost for a solo developer, so plan to lean on Unreal's tools (Lumen lighting, Nanite, Quixel Megascans and marketplace assets) and save custom Blender work for what makes the game unique: robots, machines, crops.
- **Mood:** properly dark nights, overgrown ruins, warm light around the farm and the town.
- **UI:** 24-hour clock. Clear and light, with pre-war flavour (**Lore hook:** the style of pre-war tech interfaces).

## 13. Scope and milestones

### Done: prototype

Third-person farmer, a grid of tiles, tilling, planting, watering, harvesting, a day and night cycle with a clock, a HUD, and saving and loading.

### Next: vertical slice

One full season that shows every pillar in a small form:

- 3 to 4 crops, including one mutated crop.
- Radiation and clean water on tiles.
- Stamina with the late-night penalties (faster drain, exhaustion; hostile robots may wait until combat exists).
- The caretaker robot with a little dialogue and one relationship track.
- One automation unlock: a sprinkler.
- Selling crops for currency.
- Crops, tools and items as data assets (needed for moddability before content grows).

### Later

Roughly in order, to be planned when we get there:

- Weather events, more crops and all four seasons.
- Solar energy and power cells, then land health and the haze.
- The robot town, more robots, gifts, events and romance.
- Skills, tool tiers and the tech tree.
- Buildings and machines placed on the farm grid, house decoration.
- Survival needs and the setting to switch them off.
- Combat, hostile robots at night, the first wasteland zone.
- Scavenging, rare humans, more zones.
- Animals.
- Other camera angles.
- Mod loading and a mod kit.
- Multiplayer.

### Out of scope for now

- One seamless open world.
- C++ mods.
- Deep economy simulation.

## 14. Decision log

Newest first. Add an entry whenever a design choice is made or reversed. Never delete an entry; add a new one that says so.

| Date | Decision | Why |
|---|---|---|
| 2026-10-26 | Solar energy charges power cells that run machines; land health sets the haze, which sets solar output | Makes healing the land pay off in automation, and gives dirty shortcuts a long-term cost. Idea from David, lore in Lore.md |
| 2026-10-26 | Next milestone is a one-season vertical slice ([section 13](#13-scope-and-milestones)) | Shows every pillar in a small form before building wide |
| 2026-10-26 | Third-person camera; other angles possibly later | Fits the current prototype; other views are a nice-to-have |
| 2026-10-26 | Aim for a realistic art style | David's preference; low-poly assets are placeholders |
| 2026-10-26 | Small separate maps with loading between them | Much simpler for a solo developer than an open world |
| 2026-10-26 | Simple real-time combat, never the focus | Robots as enemies need some combat; farming and relationships come first |
| 2026-10-26 | One currency from the start; economy depth later | Selling is part of the farming loop, but the economy isn't a focus |
| 2026-10-26 | Romance with robots and humans | Relationships are a core pillar |
| 2026-10-26 | Skills, tool tiers, tech tree and town restoration as progression | Several parallel tracks, with the tech tree carrying automation |
| 2026-10-26 | Robots run society and many feel superior to humans; robots start with a low opinion of the player | Explains hostile robots and makes robot friendship and romance feel earned |
| 2026-10-26 | Robots fill every human role; humans are rare | Our own identity, and it makes each human special |
| 2026-10-26 | No forced sleep; late-night penalties instead (hostile robots attack the player, faster stamina drain, exhaustion) | Player decides their pace; hostile robots never damage property |
| 2026-10-26 | Survival needs (hunger, thirst, radiation) that can be switched off | Adds depth for those who want it without hurting casual play |
| 2026-10-26 | Four seasons of 28 days; weather events instead of odd seasons | Familiar structure, with the twist in the weather |
| 2026-10-26 | Farming twists in order: radiation and mutated crops, clean water, weather, then scavenging | Most impact on the core loop first |
| 2026-10-26 | Player wakes from cryosleep under the farmhouse; rebuild a robot town | A distinct opening that explains why humans are rare |
| 2026-10-26 | Retro-futuristic post-apocalyptic world; mostly serious with comic relief | David's vision |
| 2026-10-26 | Solo first, multiplayer wanted later | Keep the scope small now without ruling co-op out |

## 15. Open questions

To be decided later. Move each one into the relevant section and the decision log when it's settled.

- **Lore:** the details still open in [Lore.md](Spoilers/Lore.md), such as names marked as working names.
- **Multiplayer:** co-op is wanted, but [Architecture.md](Architecture.md#16-decision-log) currently says single-player only. When we start, decide what shared state looks like (shared farm or separate farms, shared clock and sleep) and update the architecture.
- **Land health:** whether it is one value per map or varies across a map, how fast it changes, and whether damage spreads to neighbouring tiles.
- **Energy balance:** how much charge panels give and machines use, and whether the vertical slice needs any of it (probably not).
- **Mutated crops:** exactly which effects are good and which are risky, and how often normal crops mutate.
- **Death and knock-out penalty:** what exactly is lost.
- **Game title** and the world's name.
