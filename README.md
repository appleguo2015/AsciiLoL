# ASCII Art Generator (with GIF support)

A small C++ command-line tool that converts images into ASCII art and can play GIF animations in the terminal.

## Features

- Convert static images to ASCII art.
- Play GIF files as ASCII animation.
- GIF playback uses the real GIF frame count (not hardcoded).
- GIF playback can use embedded per-frame delays, or a user override delay.
- Option flags can be combined in any order (g++-style parsing).

## Requirements

- `g++` with C++11 support
- A terminal that supports basic ANSI clear screen behavior
- `stb_image.h` (already included in this repository)

## Build

```bash
make
```

This builds the executable:

```bash
./asciilol
```

Manual build (equivalent):

```bash
g++ -std=c++11 -o asciilol main.cpp -lm
```

## Usage

```bash
./asciilol [options] <input file>
```

### Options

- `-f`  
  Force static image mode.

- `-gif`, `-g`  
  Force GIF mode.

- `-s <symbols>`  
  Set ASCII character set (dark to bright).

- `-r <rate>`  
  Set sample rate, range `0.1` to `1.0` (default `0.3`).

- `-d <delay>`  
  GIF frame delay in milliseconds.  
  - `<= 0`: use GIF embedded frame delays
  - `> 0`: override frame delay (clamped to `10..1000`)

- `-l <loops>`  
  GIF loop count.  
  - `0`: infinite loop
  - `> 0`: play that many loops

- `-APPLEGUO`  
  Use preset symbols: `" app:lEGUO"`

- `-h`, `--help`  
  Show help.

## Examples

Convert a static image:

```bash
./asciilol -f test.png
```

Auto mode (detect by extension, `.gif` => GIF mode):

```bash
./asciilol frog.gif
```

GIF with custom symbols, delay override, and loop count:

```bash
./asciilol -gif -s " .,:;08@" -d 50 -l 3 frog.gif
```

Force static mode even for a GIF file:

```bash
./asciilol -f -s " .:-=+*#%@" frog.gif
```

## Notes

- Only one input file is supported per run.
- If your terminal shows clear-screen issues, try:

```bash
TERM=xterm ./asciilol frog.gif
```
