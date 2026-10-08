# Dropminal

A small command-line shell for Windows, written in C by waterdroplett.

Dropminal reads commands, runs programs, and keeps track of your working folder. It started as a learning project, so the code is short and easy to read.

## Features

- Runs any program on your `PATH` (or in the current folder), with arguments
- Built-in `path` command to show or change the current folder
- Quoted arguments, so folders with spaces work: `path "C:/My Projects"`
- Colored prompt and banner
- Works with [Assemdows](#using-with-assemdows): run `.asdw` programs straight from the shell

## Commands

| Command | What it does |
| --- | --- |
| `path` | Shows the current folder |
| `path <folder>` | Changes to that folder (supports `..`, relative paths, and `/` or `\`) |
| `exit` | Closes Dropminal |
| anything else | Runs it as a program, e.g. `python script.py` |

Programs that are not `.exe` files, such as `dir`, `echo`, and `.bat` scripts, are `cmd.exe` features and are not supported yet.

## Example

```
Dropminal C:\Users\you > path "C:/Projects"
Dropminal C:\Projects > python hello.py
Hello, world!
Dropminal C:\Projects > exit
```

## Download

Grab the latest `dropminal-windows.zip` from the [Releases](../../releases) page, unzip it, and run `dropminal.exe`.

Windows SmartScreen may warn about an unknown publisher, since the program is not code-signed. If you would rather not run a downloaded `.exe`, build it from source below.

## Build from source

You need a C compiler for Windows, such as GCC from [MSYS2](https://www.msys2.org/).

```
gcc dropminal.c -o dropminal.exe
```

To build a copy that runs on other PCs without extra DLLs:

```
gcc dropminal.c -static -o dropminal.exe
```

If the linker says `Permission denied`, a copy of `dropminal.exe` is still running. Type `exit` in every open Dropminal window and build again.

## Using with Assemdows

If `assemdows.exe` is in a folder on your `PATH`, you can run Assemdows programs directly:

```
Dropminal C:\Projects > assemdows test.asdw
```

## Project status

Early and experimental. Planned ideas include command history, tab completion, and more built-in commands.

## License

Licensed under the GNU General Public License v3.0. See the `LICENSE` file for details.
