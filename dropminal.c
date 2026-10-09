#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>

#define D_VERSION "v0.2"

#define MAX_ARGS 10
#define ARG_LEN 256

#define MAX_WORDS 11
#define WORD_LEN  256

#define C_CYAN   "\x1b[96m"
#define C_BLUE   "\x1b[94m"
#define C_YELLOW "\x1b[93m"
#define C_GRAY   "\x1b[90m"
#define C_GREEN "\x1b[92m"
#define C_MAGENTA "\x1b[95m"
#define C_RESET  "\x1b[0m"

typedef struct {
    const char *name;
    const char *color;
} Builtin;

// Keep this in sync with the if/else chain in main()
static const Builtin builtins[] = {
    { "exit",      C_GREEN },
    { "help",      C_CYAN },
    { "dropcalc",   C_MAGENTA},
};
#define BUILTIN_COUNT (sizeof builtins / sizeof builtins[0])

static const char *color_for_command(const char *word, const char *fallback) {
    for (size_t i = 0; i < BUILTIN_COUNT; i++)
        if (!strcmp(word, builtins[i].name)) return builtins[i].color;
    return fallback;
}

void setup_console(void) {
    SetConsoleOutputCP(CP_UTF8);                  // lets box characters display
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);   // enables colors
}

void color_line(const char *buf) {
    char args[MAX_WORDS][WORD_LEN];
    int starts[MAX_WORDS], ends[MAX_WORDS];
    int argc = 0;
    int i = 0;

    // pass 1: split into words (the args array is ready for other uses later)
    while (buf[i] != '\0' && argc < MAX_WORDS) {
        while (isspace((unsigned char)buf[i])) i++;
        if (buf[i] == '\0') break;

        int start = i;
        while (buf[i] != '\0' && !isspace((unsigned char)buf[i])) i++;

        int n = i - start;
        if (n > WORD_LEN - 1) n = WORD_LEN - 1;
        memcpy(args[argc], buf + start, n);
        args[argc][n] = '\0';            // terminate it
        starts[argc] = start;
        ends[argc]   = i;
        argc++;
    }

    // pass 2: print with colors, copying the original spacing from buf
    int pos = 0;
    for (int k = 0; k < argc; k++) {
        printf("%.*s", starts[k] - pos, buf + pos);      // spaces before this word

        const char *color = C_RESET;
        if (k == 0) color = color_for_command(args[0], C_YELLOW);
        else if (args[k][0] == '-' && args[k][1] == '-') color = C_GRAY;

        printf("%s%.*s%s", color, ends[k] - starts[k], buf + starts[k], C_RESET);
        pos = ends[k];
    }
    fputs(buf + pos, stdout);                            // trailing spaces, or words past the limit
}

#define HISTORY_MAX 50
#define HISTORY_LEN 512

static char history[HISTORY_MAX][HISTORY_LEN];
static int  history_count = 0;

// Remembers a finished line. Skips empty lines and repeats of the last one.
static void history_add(const char *line) {
    if (line[0] == '\0') return;
    if (history_count > 0 && !strcmp(history[history_count - 1], line)) return;

    if (history_count == HISTORY_MAX) {               // full: drop the oldest
        memmove(history[0], history[1], (HISTORY_MAX - 1) * HISTORY_LEN);
        history_count--;
    }
    snprintf(history[history_count++], HISTORY_LEN, "%s", line);
}

// Reads one line a key at a time and redraws it with colors after every keypress.
// Up/Down arrows scroll through earlier lines.
// Returns 1 when a line was entered, 0 on Ctrl+Z (end of input).
int read_line(const char *prompt, char *buf, size_t size) {
    size_t len = 0;
    int hist_pos = history_count;             // == history_count means "the line being typed"
    char draft[HISTORY_LEN] = "";             // what you had typed before pressing Up
    buf[0] = '\0';
    printf("%s", prompt);

    while (1) {
        int c = _getch();                     // waits for one key, no echo

        if (c == '\r') {                      // Enter
            putchar('\n');
            history_add(buf);
            return 1;
        }
        if (c == 26) {                        // Ctrl+Z
            putchar('\n');
            return 0;
        }
        if (c == 3) {                         // Ctrl+C: cancel this line
            printf("^C\n");
            buf[0] = '\0';
            return 1;
        }
        if (c == 0 || c == 224) {             // arrow/function keys send two codes
            int k = _getch();                 // 72 = Up, 80 = Down
            if (k == 72 && hist_pos > 0) {
                if (hist_pos == history_count)            // leaving the live line: keep it
                    snprintf(draft, sizeof draft, "%s", buf);
                hist_pos--;
                snprintf(buf, size, "%s", history[hist_pos]);
            } else if (k == 80 && hist_pos < history_count) {
                hist_pos++;
                snprintf(buf, size, "%s", hist_pos == history_count ? draft : history[hist_pos]);
            } else {
                continue;                     // other keys, or nothing further to scroll to
            }
            len = strlen(buf);
        } else if (c == '\b') {               // Backspace
            if (len == 0) continue;
            buf[--len] = '\0';
        } else if (c >= 32 && c < 127 && len < size - 1) {   // printable characters
            buf[len++] = (char)c;
            buf[len] = '\0';
        } else {
            continue;                         // ignore everything else
        }

        printf("\r%s", prompt);               // jump to line start, reprint the prompt
        color_line(buf);                      // then the colored text
        printf("\x1b[K");                     // erase leftovers (needed after backspace)
    }
}

void print_banner(void) {
    printf(C_CYAN
        "╔════════════════════════════════╗\n"
        "║" C_BLUE "   D R O P M I N A L   "D_VERSION"     " C_CYAN "║\n"
        "║" C_GRAY "   by waterdroplett             " C_CYAN "║\n"
        "╚════════════════════════════════╝\n"
        C_RESET "\n");
}

void print_help(void) {
    printf(
        "\n"C_CYAN"Dropminal " C_YELLOW D_VERSION "\n"
        C_GRAY"by waterdroplett\n\n"C_RESET

        "GitHub repository: "C_YELLOW"https://github.com/waterdroplett/Dropminal"C_BLUE" (use ctrl + click to follow)"C_RESET"\n"
        "\n"
    );
}

int change_dir(const char *target, char *out, DWORD out_size) {
    if (!SetCurrentDirectoryA(target)) return -1;
    DWORD n = GetCurrentDirectoryA(out_size, out);
    if (n == 0 || n >= out_size) return -1;
    return 0;
}

void list_dir(const char *folder) {
    char pattern[MAX_PATH];
    snprintf(pattern, sizeof pattern, "%s\\*", folder);   // * means "everything in this folder"

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        printf("list: can't open folder: %s\n", folder);
        return;
    }

    do {
        if (!strcmp(fd.cFileName, ".") || !strcmp(fd.cFileName, "..")) continue;

        int is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        unsigned long long size = ((unsigned long long)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;

        SYSTEMTIME utc, local;
        FileTimeToSystemTime(&fd.ftLastWriteTime, &utc);
        SystemTimeToTzSpecificLocalTime(NULL, &utc, &local);

        printf("%04d-%02d-%02d %02d:%02d  %-6s %12llu  %s\n",
               local.wYear, local.wMonth, local.wDay, local.wHour, local.wMinute,
               is_dir ? "<DIR>" : "",
               is_dir ? 0ULL : size,
               fd.cFileName);
    } while (FindNextFileA(h, &fd));

    FindClose(h);
}

// Runs a program and waits for it to finish.
// Returns 1 if it launched (exit code goes in *exit_code), 0 if launching failed.
int run_program(const char *line, DWORD *exit_code, DWORD *error) {
    char cmdline[1024];                       // CreateProcess may modify this, so it must be a copy
    snprintf(cmdline, sizeof cmdline, "%s", line);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;
    ZeroMemory(&pi, sizeof pi);

    if (!CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        *error = GetLastError();
        return 0;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 1;
}

int extract_command(const char *_in_string, char *_out_string, char args[MAX_ARGS][ARG_LEN]) {
    int i = 0;
    int argc = 0;
    while (_in_string[i] == ' ' || _in_string[i] == '\t')
    {
        i++;
    }
    int start = i;
    while (_in_string[i] != '\0' && _in_string[i] != ' ' && _in_string[i] != '\t' &&
           _in_string[i] != '\n' && _in_string[i] != '\r')
    {
        i++;
    }
    int n = i - start;
    if (n > 24) {n = 24;}
    memcpy(_out_string,_in_string + start,n);
    _out_string[n] = '\0';

    while (argc < MAX_ARGS)
    {
        while (_in_string[i] == ' ' || _in_string[i] == '\t') i++;
        if (_in_string[i] == '\0' || _in_string[i] == '\n' || _in_string[i] == '\r') break;

        int quoted = (_in_string[i] == '"');
        if (quoted) i++;
        start = i;
        while (_in_string[i] != '\0' && _in_string[i] != '\n' && _in_string[i] != '\r') {
            if (quoted ? (_in_string[i] == '"') : (_in_string[i] == ' ' || _in_string[i] == '\t')) break;
            i++;
        }
        n = i - start;
        if (n > ARG_LEN - 1) n = ARG_LEN - 1;
        memcpy(args[argc], _in_string + start, n);
        args[argc][n] = '\0';
        argc++;

        if (quoted && _in_string[i] == '"') i++;
    }
    return argc;
}

int main(void) {
    setup_console();

    char buffer[512];
    char command[25];
    char args[MAX_ARGS][ARG_LEN];
    int argc = 0;

    char path[ARG_LEN];
    GetCurrentDirectoryA(sizeof(path), path);

    char prompt[512];

    print_banner();
    char exe_dir[MAX_PATH];
    GetModuleFileNameA(NULL, exe_dir, sizeof exe_dir);
    *strrchr(exe_dir, '\\') = '\0';
    while (1) {
        snprintf(prompt, sizeof prompt, C_CYAN"Dropminal " C_YELLOW"%s" C_CYAN"> "C_RESET, path);
        if (!read_line(prompt, buffer, sizeof(buffer))) break;

        argc = extract_command(buffer,command,args);

        if (command[0] == '\0') continue;
        if (!strcmp(command,"exit")) { break; }
        else if (!strcmp(command,"clear")) { printf("\x1b[2J\x1b[H"); }
        else if (!strcmp(command,"help")) { print_help(); }
        else if (!strcmp(command,"path")) {
            if (argc < 1) {
                printf(C_YELLOW"path" C_RESET" requires atleast 1 argument, you have provided %d arguments\n", argc);
                continue;
            }
            if (change_dir(args[0], path, sizeof(path)) != 0) {
                printf(C_YELLOW"path" C_RESET": can't open folder: %s\n", args[0]);
            }
        }
        else if (!strcmp(command, "listfiles")) {
            list_dir(argc > 0 ? args[0] : ".");
        }
        else if (!strcmp(command, "print")) {
            if (argc < 1) {
                printf(C_YELLOW"print" C_RESET" requires atleast 1 argument, you have provided %d arguments\n", argc);
                continue;
            }
            printf("%s\n", args[0]);
        }
        else {
            const char *line = buffer;
            while (*line == ' ' || *line == '\t') line++;

            const char *rest = line;                       // arguments start after the first word
            while (*rest && *rest != ' ' && *rest != '\t') rest++;

            // 1. Look for Resources\<command>.exe next to dropminal.exe
            int word_len = (int)(rest - line);
            int has_exe = word_len > 4 && !_strnicmp(line + word_len - 4, ".exe", 4);

            char exe_path[MAX_PATH];
            snprintf(exe_path, sizeof exe_path, "%s\\Resources\\%.*s%s",
                     exe_dir, word_len, line, has_exe ? "" : ".exe");

            DWORD attr = GetFileAttributesA(exe_path);
            int in_resources = attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);

            char full[1024];
            if (in_resources)
                snprintf(full, sizeof full, "\"%s\"%s", exe_path, rest);   // found: run that copy
            else
                snprintf(full, sizeof full, "%s", line);                   // not found: normal search

            DWORD code = 0, err = 0;
            if (!run_program(full, &code, &err)) {
                if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
                    printf(C_YELLOW"%s" C_RESET" is not recognized as a command.\n", command);
                else
                    printf("%s: failed to start (error %lu)\n", command, err);
            }
        }
    }
    return 0;
}