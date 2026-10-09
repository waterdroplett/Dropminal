# Dropminal

A small command-line shell for Windows, written in C by waterdroplett.

Dropminal reads commands, runs programs, and keeps track of your working folder. It started as a learning project, so the code is short and easy to read.

## Features

- Runs programs with arguments, from a `Resources` folder or from your normal Windows search path
- Live syntax coloring as you type: built-in commands, resources, and `--flags` each get their own color
- Command history: press **Up** and **Down** to scroll through your last 50 commands
- Built-in file commands: list, create, delete, and show files
- Quoted arguments, so folders with spaces work: `path "C:/My Projects"`
- **Resources**: small add-on programs you can drop in, or download with `dropget`

## Commands

| Command | What it does |
| --- | --- |
| `help` | Shows the full command list |
| `exit` | Closes Dropminal |
| `clear` | Clears the screen |
| `path <folder>` | Changes the current folder (supports `..`, relative paths, and `/` or `\`) |
| `print <text>` | Prints its first argument. Use quotes for more than one word: `print "hello world"` |
| `file list [folder]` | Lists files and folders, with dates and sizes |
| `file create <name>` | Creates an empty file (it won't overwrite one that exists) |
| `file delete <name>` | Deletes a file. This is permanent and does not use the Recycle Bin |
| `file content <name>` | Prints the contents of a file |
| `dropget <resource>` | Downloads a resource into the `Resources` folder |
| `show w` | Shows the license (the GPL "show warranty" notice) |
| anything else | Runs it as a program, e.g. `python script.py` |

Windows commands that are part of `cmd.exe` itself, such as `dir` and `echo`, are not supported. Dropminal has its own versions where it matters (`file list`, `print`).

## Resources

A resource is just an `.exe` in the `Resources` folder next to `dropminal.exe`. Type its name to run it, with or without `.exe`. Dropminal checks `Resources` first, and then falls back to the normal Windows search, so your own programs and the ones on your `PATH` still work.

| Resource | How you get it | What it does |
| --- | --- | --- |
| `dropcalc` | Included | Calculates what you give it, such as `dropcalc 1 + 4 ^ pi` |
| `assemdows` | `dropget assemdows` | Runs `.asdw` programs, such as `assemdows test.asdw` |

To make your own, build any console program, put the `.exe` in `Resources`, and run it by name.

Resources are normal programs and run with your permissions, so only add ones you trust.

## Example

```
Dropminal C:\Users\you> path "C:/Projects"
Dropminal C:\Projects> file create notes.txt
Created notes.txt
Dropminal C:\Projects> dropcalc (2 + 3) * 4
20
Dropminal C:\Projects> python hello.py
Hello, world!
Dropminal C:\Projects> exit
```

## Download

Grab the latest `dropminal-windows.zip` from the [Releases](../../releases) page, unzip it, and run `dropminal.exe`.

Keep the files together in one folder:

```
dropminal.exe
LICENSE
Resources\
    dropcalc.exe
```

`show w` opens the `LICENSE` file next to `dropminal.exe`, and resources are found in the `Resources` folder, so don't separate them.

Windows SmartScreen may warn about an unknown publisher, since the program is not code-signed. If you would rather not run a downloaded `.exe`, build it from source below.

## Build from source

You need a C compiler for Windows, such as GCC from [MSYS2](https://www.msys2.org/).

```
gcc dropminal.c -o dropminal.exe -lurlmon
```

`-lurlmon` is needed for `dropget`, which downloads files with the Windows URL library.

To build a copy that runs on other PCs without extra DLLs:

```
gcc -O2 -static dropminal.c -o dropminal.exe -lurlmon
```

If the linker says `Permission denied`, a copy of `dropminal.exe` is still running. Type `exit` in every open Dropminal window and build again.

## Using with Assemdows

[Assemdows](https://github.com/waterdroplett/Assemdows) is an assembly-style interpreter that runs `.asdw` files. To install it from inside Dropminal:

```
Dropminal C:\Projects> dropget assemdows
downloading assemdows ...
saved to assemdows.exe
Dropminal C:\Projects> assemdows test.asdw
```

`dropget` downloads the latest release from GitHub into your `Resources` folder. If you type `assemdows` before installing it, Dropminal tells you how to get it.

## Project status

Version 0.2. Early and experimental. Ideas for later include cursor movement inside a line, tab completion, saving history between sessions, and more built-in commands.

## License

Copyright (C) 2026 waterdroplett.

Licensed under the GNU General Public License v3.0. This program comes with ABSOLUTELY NO WARRANTY. It is free software, and you are welcome to redistribute it. See the `LICENSE` file for details.