# Moonlight OBS

Moonlight OBS is an open source OBS Studio plugin which utilises the Moonlight Stream common library for real-time streaming of another machine's desktop and/or game.

This doesn't aim to replace any of the currently existing Moonlight clients, and isn't associated with Moonlight in any way.

## Features

- Hardware accelerated video decoding, as well as software video decoding
- H.264, HEVC, and AV1 codec support (AV1 requires Sunshine and a supported host GPU) for the game stream within OBS Studio (final output encoding still handled by OBS Studio)
- HDR streaming support
- 7.1 surround sound audio support

## Key Difference From Moonlight PC

This plugin doesn't aim to replace setups that currently use Moonlight PC captured with desktop window capture within OBS Studio, but rather is more of a convenience plugin when attempting to stream multiple machines using one OBS Studio instance.

It provides no way to control the remote machine, as this plugin intends to provide a source which displays the remote machine's desktop (or game) in real-time.
