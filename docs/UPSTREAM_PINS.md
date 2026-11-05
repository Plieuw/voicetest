# Upstream FM1 source pins

These are the exact upstream inputs selected for the first reproducible Felucca baseline build. They are deliberately pinned; do not silently track `main`.

| Component | Repository | Pinned revision |
|---|---|---|
| Felucca firmware | https://github.com/hugelton/Felucca | `a4f007c9211ea5b37298c01ff6d73430fe2cb96a` |
| Felucca hardware BSP | https://github.com/hugelton/felucca-bsp | `5942e68b89ab9ccd23f2c541d712ca38fe635f11` |
| Felucca engines | https://github.com/hugelton/Felucca-Engines | `d0260074ce100a47675814b7f38d61f11176c926` |
| JieLi AC79 SDK | `fw-AC79_AIoT_SDK` | `AC79NN_SDK_V1.2.1_2023-12-13` (per Felucca build guide) |

The upstream Felucca commit is the source baseline, not a claim that VoiceTest is already integrated. Its build guide requires the two submodules, the JieLi pi32v2 toolchain, three AC79 SDK files, Python/Pillow and (outside Linux x86-64) Docker.

## Build the untouched baseline

From the repository root:

```sh
sh tools/build_felucca_baseline.sh
```

The script checks out the pinned upstream commit and its exact submodule gitlinks in `build/upstream/Felucca`, then invokes Felucca's own build script. It does not flash the FM1 and does not modify the upstream repository.

## Why this baseline matters

The original Felucca renderer is driven by the ALNK0 audio interrupt. Its `audio_block()` mixes the synth and taps the USB recording stream before output scaling. The USB Audio Class direction documented by Felucca is FM1-to-host recording; it is not proof of phone-to-FM1 audio input.

The current Felucca application source does not configure a Wi-Fi socket task. The phone's UDP receiver therefore remains a separate integration task. The four-block queue now in VoiceTest is the portable hand-off primitive; it is not a network driver.

Do not install any image produced by this baseline script as VoiceTest. It is the untouched Felucca build used to prove the toolchain and recovery baseline first.
