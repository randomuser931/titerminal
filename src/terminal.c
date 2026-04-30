static void term_print(const char *s) {
    gfx_PrintString(s);
}

static void term_println(const char *s) {
    gfx_PrintString(s);
    gfx_PrintString("\n");
}

static void term_newline(void) {
    gfx_PrintString("\n");
}

typedef void (*cmd_func_t)(int argc, char **argv);

static void cmd_help(int argc, char **argv);
static void cmd_clear(int argc, char **argv);
static void cmd_exit(int argc, char **argv);
static void cmd_about(int argc, char **argv);
static void cmd_version(int argc, char **argv);
static void cmd_uptime(int argc, char **argv);
static void cmd_date(int argc, char **argv);
static void cmd_time(int argc, char **argv);
static void cmd_echo(int argc, char **argv);
static void cmd_cls(int argc, char **argv);

static void cmd_ls(int argc, char **argv);
static void cmd_run(int argc, char **argv);
static void cmd_del(int argc, char **argv);
static void cmd_rename(int argc, char **argv);
static void cmd_type(int argc, char **argv);
static void cmd_mkdir(int argc, char **argv);
static void cmd_rmdir(int argc, char **argv);
static void cmd_touch(int argc, char **argv);
static void cmd_cp(int argc, char **argv);
static void cmd_mv(int argc, char **argv);

static void cmd_mem(int argc, char **argv);
static void cmd_ram(int argc, char **argv);
static void cmd_archive(int argc, char **argv);
static void cmd_vars(int argc, char **argv);
static void cmd_size(int argc, char **argv);

static void cmd_cd(int argc, char **argv);
static void cmd_pwd(int argc, char **argv);
static void cmd_tree(int argc, char **argv);
static void cmd_mount(int argc, char **argv);
static void cmd_unmount(int argc, char **argv);

static void cmd_print(int argc, char **argv);
static void cmd_hex(int argc, char **argv);
static void cmd_dump(int argc, char **argv);
static void cmd_head(int argc, char **argv);
static void cmd_tail(int argc, char **argv);

static void cmd_brightness(int argc, char **argv);
static void cmd_battery(int argc, char **argv);
static void cmd_keytest(int argc, char **argv);
static void cmd_reset(int argc, char **argv);
static void cmd_mode(int argc, char **argv);

static void cmd_peek(int argc, char **argv);
static void cmd_poke(int argc, char **argv);
static void cmd_regs(int argc, char **argv);
static void cmd_disasm(int argc, char **argv);
static void cmd_bench(int argc, char **argv);

static void cmd_rand(int argc, char **argv);
static void cmd_calc(int argc, char **argv);
static void cmd_banner(int argc, char **argv);
static void cmd_color(int argc, char **argv);
static void cmd_beep(int argc, char **argv);

typedef struct {
    const char *name;
    cmd_func_t func;
    const char *desc;
} command_t;

static const command_t commands[] = {
    {"help", cmd_help, "Show this help"},
    {"clear", cmd_clear, "Clear screen"},
    {"exit", cmd_exit, "Exit terminal"},
    {"about", cmd_about, "About this terminal"},
    {"version", cmd_version, "Show version"},
    {"uptime", cmd_uptime, "Fake uptime"},
    {"date", cmd_date, "Show date (fake)"},
    {"time", cmd_time, "Show time (fake)"},
    {"echo", cmd_echo, "Echo text"},
    {"cls", cmd_cls, "Alias for clear"},

    {"ls", cmd_ls, "List programs"},
    {"run", cmd_run, "Run program"},
    {"del", cmd_del, "Delete file"},
    {"rename", cmd_rename, "Rename file"},
    {"type", cmd_type, "Show AppVar contents"},
    {"mkdir", cmd_mkdir, "Fake mkdir"},
    {"rmdir", cmd_rmdir, "Fake rmdir"},
    {"touch", cmd_touch, "Create empty AppVar"},
    {"cp", cmd_cp, "Copy file"},
    {"mv", cmd_mv, "Move file"},

    {"mem", cmd_mem, "Memory info"},
    {"ram", cmd_ram, "RAM info"},
    {"archive", cmd_archive, "Archive info"},
    {"vars", cmd_vars, "List vars"},
    {"size", cmd_size, "File size"},

    {"cd", cmd_cd, "Change dir"},
    {"pwd", cmd_pwd, "Current dir"},
    {"tree", cmd_tree, "Tree view"},
    {"mount", cmd_mount, "Mount"},
    {"unmount", cmd_unmount, "Unmount"},

    {"print", cmd_print, "Print text"},
    {"hex", cmd_hex, "Hex dump"},
    {"dump", cmd_dump, "Dump data"},
    {"head", cmd_head, "Head"},
    {"tail", cmd_tail, "Tail"},

    {"brightness", cmd_brightness, "Set brightness 0-9"},
    {"battery", cmd_battery, "Battery info"},
    {"keytest", cmd_keytest, "Key codes"},
    {"reset", cmd_reset, "Reset"},
    {"mode", cmd_mode, "Mode info"},

    {"peek", cmd_peek, "Read memory"},
    {"poke", cmd_poke, "Write memory"},
    {"regs", cmd_regs, "Registers"},
    {"disasm", cmd_disasm, "Disassemble"},
    {"bench", cmd_bench, "Benchmark"},

    {"rand", cmd_rand, "Random number"},
    {"calc", cmd_calc, "Simple calc"},
    {"banner", cmd_banner, "Banner"},
    {"color", cmd_color, "Colors"},
    {"beep", cmd_beep, "Beep"},
};

static int tokenize(char *line, char **argv, int max_args) {
    int argc = 0;
    char *p = line;

    while (*p && argc < max_args) {
        while (*p == ' ') p++;
        if (!*p) break;

        argv[argc++] = p;

        while (*p && *p != ' ') p++;
        if (*p) {
            *p = 0;
            p++;
        }
    }
    return argc;
}

static int read_line(char *buf, int max_len) {
    int pos = 0;
    buf[0] = 0;

    while (1) {
        kb_Scan();

        if (kb_Data[6] & kb_Clear) {
            return -1;
        }

        uint8_t key = os_GetCSC();
        if (!key) continue;

        if (key == skEnter) {
            term_newline();
            buf[pos] = 0;
            return pos;
        } else if (key == skDel || key == skBackspace) {
            if (pos > 0) {
                pos--;
                buf[pos] = 0;
                gfx_PrintString("\b \b");
            }
        } else if (key >= 32 && key <= 126 && pos < max_len - 1) {
            char c = (char)key;
            buf[pos++] = c;

            char s[2] = {c, 0};
            term_print(s);
        }
    }
}

static void cmd_help(int argc, char **argv) {
    term_println("Commands:");
    for (int i = 0; i < NUM_COMMANDS; i++) {
        term_print("  ");
        term_print(commands[i].name);
        term_print(" - ");
        term_println(commands[i].desc);
    }
}

static void cmd_clear(int argc, char **argv) {
    gfx_FillScreen(0);
    gfx_SetTextXY(0, 0);
}

static void cmd_exit(int argc, char **argv) {
    (void)argc;
    (void)argv;
}

static void cmd_about(int argc, char **argv) {
    term_println("Titerminal - CE Terminal");
    term_println("Single-file C terminal with 50 commands.");
}

static void cmd_version(int argc, char **argv) {
    term_println("Titerminal v1.0");
}

static void cmd_uptime(int argc, char **argv) {
    term_println("Uptime not tracked.");
}

static void cmd_date(int argc, char **argv) {
    term_println("Date not available.");
}

static void cmd_time(int argc, char **argv) {
    term_println("Time not available.");
}

static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        term_print(argv[i]);
        if (i + 1 < argc) term_print(" ");
    }
    term_newline();
}

static void cmd_cls(int argc, char **argv) {
    cmd_clear(argc, argv);
}

static void cmd_ls(int argc, char **argv) {
    char name[9];
    uint8_t type;
    ti_var_t search = ti_DetectAny(&type, name);

    while (search) {
        term_print(name);
        term_newline();
        search = ti_DetectAny(&type, name);
    }
}

static void cmd_run(int argc, char **argv) {
    if (argc < 2) {
        term_println("Usage: run NAME");
        return;
    }
    ti_CloseAll();
    os_RunPrgm(argv[1], NULL, 0);
}

static void cmd_del(int argc, char **argv) {
    if (argc < 2) {
        term_println("Usage: del NAME");
        return;
    }

    if (ti_Delete(argv[1])) {
        term_println("Deleted.");
    } else {
        term_println("Delete failed.");
    }
}

static void cmd_rename(int argc, char **argv) {
    if (argc < 3) {
        term_println("Usage: rename OLD NEW");
        return;
    }

    if (ti_Rename(argv[1], argv[2])) {
        term_println("Renamed.");
    } else {
        term_println("Rename failed.");
    }
}

static void cmd_type(int argc, char **argv) {
    if (argc < 2) {
        term_println("Usage: type APPVAR");
        return;
    }

    ti_var_t v = ti_Open(argv[1], "r");
    if (!v) {
        term_println("Not found.");
        return;
    }

    uint16_t size = ti_GetSize(v);
    uint8_t buf[33];
    uint16_t read = 0;

    while (read < size) {
        uint16_t chunk = (size - read > 32) ? 32 : (size - read);
        ti_Read(buf, chunk, 1, v);
        buf[chunk] = 0;
        term_print((char*)buf);
        read += chunk;
    }

    ti_Close(v);
    term_newline();
}

static void cmd_touch(int argc, char **argv) {
    if (argc < 2) {
        term_println("Usage: touch NAME");
        return;
    }

    ti_var_t v = ti_Open(argv[1], "w");
    if (!v) {
        term_println("Failed.");
        return;
    }

    ti_Close(v);
    term_println("Created.");
}

static int dispatch_command(int argc, char **argv, int *should_exit) {
    if (argc == 0) return 0;

    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (!strcmp(argv[0], commands[i].name)) {
            if (commands[i].func == cmd_exit) {
                *should_exit = 1;
                return 0;
            }
            commands[i].func(argc, argv);
            return 0;
        }
    }

    term_println("Unknown command. Type 'help'.");
    return 0;
}

int main(void) {
    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_FillScreen(0);

    gfx_SetTextFGColor(0x07);
    gfx_SetTextBGColor(0);
    gfx_SetTextTransparentColor(0);
    gfx_SetTextXY(0, 0);

    term_println("Titerminal v1.0 - type 'help'");
    term_println("Press CLEAR to exit.");

    char line[MAX_INPUT];
    char *argv[MAX_ARGS];
    int should_exit = 0;

    while (!should_exit) {
        term_print("> ");
        int size = read_line(line, MAX_INPUT);

        if (size < 0) {
            cmd_clear(0, NULL);
            break;
        }

        int argc = tokenize(line, argv, MAX_ARGS);
        dispatch_command(argc, argv, &should_exit);
    }

    gfx_End();
    return 0;
}
