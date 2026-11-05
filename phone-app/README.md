# VoiceTest Phone Link

A minimal Android companion app for sending the phone microphone to the FM1 over local Wi-Fi.

## Status

The Kotlin prototype is intended to capture mono 48 kHz PCM16 and send 20 ms UDP packets to a user-entered IPv4 address on port 39000. It streams only while the user explicitly presses Start in the foreground activity.

The FM1 UDP receiver is not implemented yet. This app alone does not make the FM1 receive audio.

## Build

Open phone-app/android in Android Studio and let Gradle sync. Build and install the app. Grant microphone permission when prompted.

Connect the phone and FM1 to the same Wi-Fi network, enter the FM1's reachable IP address, and press Start. Stop ends capture and closes the UDP socket.

This is a developer prototype. It does not discover the FM1 automatically or display packet-loss/latency statistics.
