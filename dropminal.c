#include <stdio.h>
#include <string.h>
#include <windows.h>

#define D_VERSION "v0.1"

#define MAX_ARGS 10
#define ARG_LEN 256

#define C_CYAN   "\x1b[96m"
#define C_BLUE   "\x1b[94m"
#define C_YELLOW "\x1b[93m"
#define C_GRAY   "\x1b[90m"
#define C_RESET  "\x1b[0m"

void setup_console(void) {
    SetConsoleOutputCP(CP_UTF8);                  // lets box characters display
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);   // enables colors
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

    char buffer[128];
    char command[25];
    char args[MAX_ARGS][ARG_LEN];
    int argc = 0;

    char path[ARG_LEN];
    GetCurrentDirectoryA(sizeof(path), path);

    print_banner();
    while (1) {
        printf(C_CYAN"Dropminal " C_YELLOW"%s" C_CYAN"> "C_RESET, path);
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strcspn(buffer,"\n")] = '\0';

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
        else {
            const char *line = buffer;
            while (*line == ' ' || *line == '\t') line++;

            DWORD code = 0, err = 0;
            if (!run_program(line, &code, &err)) {
                if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
                    printf(C_YELLOW"%s" C_RESET" is not recognized as a command.\n", command);
                else
                    printf("%s: failed to start (error %lu)\n", command, err);
            }
        }
    }
}