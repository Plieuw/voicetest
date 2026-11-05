# FM1 target notes

## Starting point

The user wants the FM1 returned to original Felucca first. Keep that as the starting firmware and recovery reference. The new VoiceTest firmware remains a separate, clean app.

## Hardware and platform

The FM1 uses the JieLi AC79 / pi32v2 platform. The shared felucca-bsp repository supplies the chip and board layer. The fm1-base project builds on that BSP and adds the app platform, audio callback, storage, USB, optional radios and host simulator.

The FM1 audio output is ALNK0/I2S to the external codec. Felucca's audio.c fills the output DMA buffer in its audio interrupt. Its USB audio recording tap sends the FM1 master output to a computer; it is not an application microphone-input callback. The optional USB audio output feature receives computer playback through the FM1 USB device endpoint and mixes it after the app's own output. Neither feature is a substitute for the phone's Wi-Fi audio input.

## Phone input route: Wi-Fi UDP

fm1-base has a Wi-Fi platform option:
- wifi = true in fm1.toml
- network source files listed in net = [...]
- plat_net_task() for socket work in a separate task
- lwIP sockets, including the standard UDP socket API
- Wi-Fi station setup/state through plat_wifi_on(), plat_wifi_state(), plat_wifi_ip(), and join/scan helpers

The network task must validate incoming VTP1 datagrams and copy PCM into a bounded ring. The audio callback must never call sockets or wait for packets. It should consume whatever samples are ready, track underruns, and fade safely to silence when the stream stops.

## Resource limits and radio trade-offs

The current fm1-base README documents a 581 KB app slot, 128 KB of data RAM and a 304 KB buffer pool in a Wi-Fi build. Its Wi-Fi example uses about 420 KB flash, 119 KB data RAM and 299 KB pool. Wi-Fi and Bluetooth are mutually exclusive in the documented platform version; the Wi-Fi example disables USB audio input and output. This leaves little room for additional app memory, so DSP blocks must be profiled individually and in combination.

## Licence and source boundary

Felucca is GPL-3.0-only. The fm1-base porting guide describes a clean-app route for Wi-Fi builds and states that its JieLi binary Wi-Fi libraries require an MPL-2.0 firmware. Do not copy Felucca's GPL application code into VoiceTest as part of that route. Use the independently licensed BSP/platform components and keep VoiceTest's DSP/app code original, with provenance and licence decisions documented before distribution.

## First hardware milestone

- Confirm the exact FM1 board and flash revision from the actual unit.
- Pin the exact fm1-base, felucca-bsp and AC79 SDK revisions.
- Build the clean VoiceTest app without flashing.
- Verify a low-CPU output/bypass path.
- Add Wi-Fi UDP receive and measure packet loss, queue depth, latency, CPU and RAM.
- Add heavier voice processing only after the combined Wi-Fi/audio budget is measured.

Do not flash an experimental image until the package, identity check and a tested recovery path are available.
