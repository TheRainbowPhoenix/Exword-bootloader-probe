# EX-word bootloader probes

Tiny, independent experimental CASIO EX-word Test Menu programs shipped
as `UPDADN2.BIN` (not the usual `.d01` add-ins).

## First probe: key tester

The screen shows a scrolling history of the last 16 **DOWN** and **UP**
transitions, interpreted key name, raw keyboard-matrix row and column,
event counter, held-key count, and the live twelve-row bitmask. Even
unmapped matrix bits are logged; simultaneous presses are supported.
There is no key-repeat generation and no storage or firmware writing.

The name map is transcribed from
[gint-exword's EX-word translation table](https://github.com/TheRainbowPhoenix/gint-exword/blob/feature/exword-runtime-bridge/src/keysc/keydev.c).
The scanner reads the six KEYSC words at `0xa44b0000` directly.
Startup keys are used as a baseline (held keys do not generate a fake
DOWN; their eventual UP appears). Very fast taps between two scans
may still be missed.

The 528×320 LCD MMIO driver, raw start and linker layout are based on
[EXWord-Explorer's experimental UPDADN2 updater](https://github.com/TheRainbowPhoenix/EXWord-Explorer/tree/main/updater),
without its filesystem/OS-service code.

## Build/download

See [GitHub Actions](https://github.com/TheRainbowPhoenix/Exword-bootloader-probe/actions).
The `EXWord-key-tester-UPDADN2` build artifact contains `UPDADN2.BIN`.
A separate artifact contains the ELF and map for debugging.

For local builds, install
[fxsdk-exword's `fix/exword-data-load-alignment` branch](https://github.com/TheRainbowPhoenix/fxsdk-exword/tree/fix/exword-data-load-alignment)
and use:

```sh
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/fxsdk-exword/lib/cmake/fxsdk/EXWORD.cmake \
  -DCMAKE_MODULE_PATH=/path/to/fxsdk-exword/lib/cmake/fxsdk
cmake --build build
# build/UPDADN2.BIN
```

Host-only key tests:

```sh
cc -std=c11 -O2 -Wall -Wextra -Werror -Iapps/key-tester \
  tests/test_keys.c apps/key-tester/keys.c -o test_keys
./test_keys
```

`apps/key-tester/` holds the probe; `common/` holds minimal reusable
startup/linker/LCD routines; `tests/` holds host matrix tests.
Future mini apps can be separate `apps/<name>/` directories and targets.

## Safety / compatibility

**This is NOT validated on real EX-word E/Test Menu hardware.** The
update/recovery loader may behave differently on different models.
The program itself has **no flash, erase, storage, or firmware update
API calls**. However the bootloader may write firmware *before* it
loads/executes `UPDADN2.BIN`. An incompatible or malformed image
could brick the device. Do not experiment without a complete backup
and a *tested* recovery method. CI validates format and host logic,
not the safety of the bootloader or the hardware assumptions.
