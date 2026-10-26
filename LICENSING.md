# Licensing

FarmSim is open: anyone can read the code, build the game, make mods and contribute. The maintainer, David Bennehag, sells ready-built copies of the game on online stores for players who don't want to build it themselves.

Different parts of the repository are under different licenses:

| What | License | File |
|---|---|---|
| **Code**: everything in `Source/`, the `.uproject` file, `Config/`, build and tool scripts (for example `Art/Blender/*.py`) | GNU General Public License v3.0 or later, with the Unreal Engine permission below | [LICENSE](LICENSE) |
| **Assets and everything else**: art, models, textures, materials, Blueprints, levels, audio, music, game data and documentation | Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) | [LICENSE-ASSETS.txt](LICENSE-ASSETS.txt) |
| **Epic Games material**: see [Not covered](#not-covered-epic-games-material) | Unreal Engine EULA | https://www.unrealengine.com/eula |

## What this means in practice

- **You can** read, change and share the code. If you distribute a game or program built from it, you must release your version of the code under the same GPL license. Nobody can take the code and ship a closed-source game with it.
- **You can** use the assets for mods, fan projects, videos and anything else non-commercial, as long as you credit FarmSim and share your changes to the assets under the same license.
- **You can't** sell the assets or a game that contains them. Only the maintainer sells official builds of FarmSim.
- **Contributors are credited** in [CREDITS.md](CREDITS.md) and in the game's credits.

## Additional permission for Unreal Engine (GPLv3 section 7)

FarmSim is built on Unreal Engine, which is not free software. Without this permission, the GPL would not allow anyone, including the maintainer, to distribute a build of the game, because it links GPL code with the engine.

**This permission is part of the license terms for the code.** The code is licensed under the GPL in [LICENSE](LICENSE) together with the following additional permission, granted by the copyright holders:

> **Additional permission under GNU GPL version 3 section 7**
>
> If you modify this Program, or any covered work, by linking or combining it with Unreal Engine (or a modified version of it), or with the libraries and SDKs of a game distribution platform or console that are needed to publish the game there (together, the "Non-GPL Components"), containing parts covered by the terms of their own licenses, the licensors of this Program grant you additional permission to convey the resulting work. Corresponding Source for a non-source form of such a combination shall not include the source code for the parts of the Non-GPL Components used, but shall include the source code for the parts of this Program that are used.

This permission covers only the engine and platform components. Our own code stays under the GPL and must be shared.

## Not covered: Epic Games material

These parts of the repository come from Epic Games' Third Person template. They are used under the [Unreal Engine EULA](https://www.unrealengine.com/eula), not under our licenses:

- Source files that carry the line `Copyright Epic Games, Inc. All Rights Reserved.`: currently `Source/FarmSim.Target.cs`, `Source/FarmSimEditor.Target.cs`, `Source/FarmSim/FarmSim.Build.cs`, `FarmSim.h/.cpp`, `FarmSimCharacter.h/.cpp`, `FarmSimGameMode.h/.cpp`, `FarmSimPlayerController.h/.cpp`, and everything in `Source/FarmSim/Variant_*`.
- Template content: `Content/Characters`, `Content/ThirdPerson`, `Content/LevelPrototyping`, `Content/Input` and `Content/Variant_*`.

Changes we make to these files are ours, but the original Epic material stays under Epic's terms. The plan is to replace template code and content with our own over time; see [Docs/Architecture.md](Docs/Architecture.md).

## Contributions

By submitting a contribution (code, assets, documentation or anything else) you agree to the contributor terms in [CONTRIBUTING.md](CONTRIBUTING.md#11-licensing-and-credit): your contribution is licensed under the license for its part of the repository, and you also give the maintainer permission to use it in the official, sold builds of FarmSim.

## Questions

The binding terms are [LICENSE](LICENSE) together with the additional permission above for the code, and [LICENSE-ASSETS.txt](LICENSE-ASSETS.txt) for everything else. The rest of this page is a plain-language summary, not legal advice. If something is unclear, open an issue.
