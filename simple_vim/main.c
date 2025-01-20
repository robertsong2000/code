#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define EDITOR_VERSION "0.0.1"
#define BUFFER_INIT {NULL, 0}
#define EDITOR_TAB_STOP 8
#define ABUF_INIT {NULL, 0}

// 添加按键枚举
enum editorKey {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN
};

// 在 struct editorConfig 之前添加行结构体
typedef struct erow {
    int size;
    char *chars;
} erow;

// 修改 editorConfig 结构体，添加文本缓冲区相关字段
struct editorConfig {
    int cx, cy;          // 光标位置
    int rx;              // 渲染位置（用于制表符）
    int rowoff;          // 行偏移（用于滚动）
    int coloff;          // 列偏移
    int screenrows;      // 屏幕行数
    int screencols;      // 屏幕列数
    int numrows;         // 文件行数
    erow *row;          // 文本行数组
    char *filename;      // 文件名
    char statusmsg[80];  // 状态消息
    int mode;           // 编辑器模式 (0: 命令模式, 1: 插入模式)
    struct termios orig_termios;  // 原始终端属性
    char *cmdline;       // 命令行缓冲区
    int cmdline_size;    // 命令行大小
    int first_g;         // 是否已经按下第一个 g
    int dirty;          // 文件是否被修改
    int show_line_numbers;  // 是否显示行号
};

// 添加缓冲区结构体（用于屏幕刷新）
struct abuf {
    char *b;
    int len;
};

struct editorConfig E;

// 函数声明
void die(const char *s);
void disableRawMode(void);
void enableRawMode(void);
int editorReadKey(void);
void editorProcessKeypress(void);
void initEditor(void);
void editorRefreshScreen(void);
int getWindowSize(int *rows, int *cols);
void editorOpen(char *filename);
void editorInsertChar(int c);
void editorDelChar(void);
void editorInsertNewline(void);
void editorSave(void);
void editorRowInsertChar(erow *row, int at, int c);
void editorRowDelChar(erow *row, int at);
void editorFreeRow(erow *row);
void editorDelRow(int at);
void editorRowAppendString(erow *row, char *s, size_t len);
void editorInsertRow(int at, char *s, size_t len);
char *editorRowsToString(int *buflen);
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);
void editorMoveCursor(int key);
void editorScroll(void);
void editorProcessCommand(void);
void editorLoadConfig(void);

// 错误处理
void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

// 禁用原始模式
void disableRawMode(void) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

// 启用原始模式
void enableRawMode(void) {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    
    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

// 读取按键
int editorReadKey(void) {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                }
            }
        }
        return '\x1b';
    } else {
        return c;
    }
}

// 添加一个新的函数来处理光标移动
void editorMoveCursor(int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    
    switch (key) {
        case ARROW_LEFT:
        case 'h':
            if (E.cx != 0) {
                E.cx--;
            } else if (E.cy > 0) {
                // 移动到上一行末尾
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_RIGHT:
        case 'l':
            if (row && E.cx < row->size) {
                E.cx++;
            } else if (row && E.cx == row->size && E.cy < E.numrows - 1) {
                // 移动到下一行开头
                E.cy++;
                E.cx = 0;
            }
            break;
        case ARROW_UP:
        case 'k':
            if (E.cy != 0) {
                E.cy--;
            }
            break;
        case ARROW_DOWN:
        case 'j':
            if (E.cy < E.numrows - 1) {
                E.cy++;
            }
            break;
    }
    
    // 如果光标移动到新行，确保 cx 不超过行长度
    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) {
        E.cx = rowlen;
    }
}

// 修改 editorProcessKeypress 函数中的 PAGE_UP/DOWN 处理部分
void editorProcessKeypress(void) {
    static int in_command = 0;  // 是否在命令输入状态
    
    int c = editorReadKey();
    
    if (in_command) {
        if (c == '\r') {  // Enter 键
            editorProcessCommand();
            free(E.cmdline);
            E.cmdline = NULL;
            E.cmdline_size = 0;
            in_command = 0;
        } else if (c == CTRL_KEY('[') || c == CTRL_KEY('c')) {  // ESC 或 Ctrl-C
            free(E.cmdline);
            E.cmdline = NULL;
            E.cmdline_size = 0;
            in_command = 0;
        } else if (c == 127) {  // Backspace
            if (E.cmdline_size > 1) {  // 保留 ':' 字符
                E.cmdline_size--;
                E.cmdline[E.cmdline_size] = '\0';
            }
        } else if (!iscntrl(c)) {
            E.cmdline = realloc(E.cmdline, E.cmdline_size + 2);
            E.cmdline[E.cmdline_size++] = c;
            E.cmdline[E.cmdline_size] = '\0';
        }
        return;
    }
    
    if (E.mode == 0) {  // 命令模式
        switch (c) {
            case 'i':
                E.mode = 1;  // 切换到插入模式
                break;
            case 'h':
            case ARROW_LEFT:
            case 'l':
            case ARROW_RIGHT:
            case 'k':
            case ARROW_UP:
            case 'j':
            case ARROW_DOWN:
                editorMoveCursor(c);
                break;
            case PAGE_UP:
            case PAGE_DOWN:
                {
                    int times = E.screenrows;
                    while (times--)
                        editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
                }
                break;
            case CTRL_KEY('f'):
                {
                    int times = E.screenrows;
                    while (times--)
                        editorMoveCursor(ARROW_DOWN);
                }
                break;
            case CTRL_KEY('b'):
                {
                    int times = E.screenrows;
                    while (times--)
                        editorMoveCursor(ARROW_UP);
                }
                break;
            case ':':
                in_command = 1;
                E.cmdline = malloc(2);
                E.cmdline[0] = ':';
                E.cmdline[1] = '\0';
                E.cmdline_size = 1;
                break;
            case 'q':
                write(STDOUT_FILENO, "\x1b[2J", 4);
                write(STDOUT_FILENO, "\x1b[H", 3);
                exit(0);
                break;
            case 'g':
                if (E.first_g) {
                    // 第二个 g，跳转到文件开头
                    E.cy = 0;
                    E.cx = 0;
                    E.first_g = 0;
                } else {
                    // 第一个 g，设置标志
                    E.first_g = 1;
                }
                break;
            default:
                E.first_g = 0;  // 任何其他键都重置 g 状态
                break;
        }
    } else {  // 插入模式
        switch (c) {
            case CTRL_KEY('['):  // ESC键
                E.mode = 0;  // 切换回命令模式
                if (E.cx > 0) E.cx--;
                break;
            case '\r':  // Enter键
                editorInsertNewline();
                break;
            case CTRL_KEY('h'):  // Backspace
            case 127:            // Delete
                editorDelChar();
                break;
            case ARROW_LEFT:
            case ARROW_RIGHT:
            case ARROW_UP:
            case ARROW_DOWN:
                editorMoveCursor(c);
                break;
            case PAGE_UP:
            case PAGE_DOWN:
                {
                    int times = E.screenrows;
                    while (times--)
                        editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
                }
                break;
            default:
                if (!iscntrl(c)) {
                    editorInsertChar(c);
                }
                break;
        }
    }

    // 确保光标在有效位置
    if (E.cy < E.numrows) {
        if (E.cx > E.row[E.cy].size) {
            E.cx = E.row[E.cy].size;
        }
    }
}

// 初始化编辑器
void initEditor(void) {
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.numrows = 0;
    E.row = NULL;
    E.mode = 0;  // 默认为命令模式
    E.filename = NULL;
    E.cmdline = NULL;
    E.cmdline_size = 0;
    E.first_g = 0;
    E.dirty = 0;
    E.show_line_numbers = 0;  // 默认不显示行号
    
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

// 获取窗口大小
int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

// 刷新屏幕
void editorRefreshScreen(void) {
    editorScroll();
    
    struct abuf ab = ABUF_INIT;

    // 隐藏光标
    abAppend(&ab, "\x1b[?25l", 6);
    // 将光标定位到左上角
    abAppend(&ab, "\x1b[H", 3);

    // 计算行号占用的宽度
    int line_number_width = 0;
    if (E.show_line_numbers) {
        // 计算最大行号的位数
        int max_lines = E.numrows > 0 ? E.numrows : 1;
        while (max_lines > 0) {
            line_number_width++;
            max_lines /= 10;
        }
        line_number_width += 1;  // 为行号后的空格预留位置
    }
    
    // 绘制文件内容
    for (int y = 0; y < E.screenrows - 2; y++) {  // 减2，为状态行和命令行留出空间
        int filerow = y + E.rowoff;
        
        if (filerow >= E.numrows) {
            if (E.show_line_numbers) {
                // 显示空行的行号区域
                for (int i = 0; i < line_number_width; i++) {
                    abAppend(&ab, " ", 1);
                }
            }
            // 如果是空行，显示波浪号
            abAppend(&ab, "~", 1);
        } else {
            if (E.show_line_numbers) {
                char line_number[32];
                int number_len = snprintf(line_number, sizeof(line_number), 
                                        "%*d ", line_number_width - 1, filerow + 1);
                abAppend(&ab, line_number, number_len);
            }
            
            // 显示实际的文件内容
            int len = E.row[filerow].size - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols - line_number_width) 
                len = E.screencols - line_number_width;
            abAppend(&ab, &E.row[filerow].chars[E.coloff], len);
        }

        // 清除行末尾
        abAppend(&ab, "\x1b[K", 3);
        abAppend(&ab, "\r\n", 2);
    }

    // 绘制状态行
    abAppend(&ab, "\x1b[7m", 4);  // 反转颜色
    char status[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
        E.filename ? E.filename : "[No Name]", E.numrows,
        E.mode ? "-- INSERT --" : "-- NORMAL --");
    int rlen = 0;
    size_t max_rlen = sizeof(status) - len;  // 使用 size_t 存储大小差值
    
    while (len + rlen < E.screencols && (size_t)rlen < max_rlen) {
        status[len + rlen] = ' ';
        rlen++;
    }
    if (len > E.screencols) len = E.screencols;
    abAppend(&ab, status, len + rlen);
    abAppend(&ab, "\x1b[m", 3);   // 恢复正常颜色
    abAppend(&ab, "\r\n", 2);

    // 在状态行下方显示命令行或文件信息
    if (E.cmdline) {
        abAppend(&ab, E.cmdline, E.cmdline_size);
    } else {
        char info[32];
        int info_len = snprintf(info, sizeof(info), "%d, %d", E.cy + 1, E.cx + 1);
        abAppend(&ab, info, info_len);
    }

    // 将光标定位到当前位置
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", 
             (E.cy - E.rowoff) + 1, (E.cx - E.coloff) + 1);
    abAppend(&ab, buf, strlen(buf));

    // 显示光标
    abAppend(&ab, "\x1b[?25h", 6);

    // 写入所有内容到终端
    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

// 实现文件打开功能
void editorOpen(char *filename) {
    free(E.filename);
    E.filename = strdup(filename);

    FILE *fp = fopen(filename, "r");
    if (!fp) die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(E.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
}

// 实现文件保存功能
void editorSave(void) {
    if (E.filename == NULL) return;

    int len;
    char *buf = editorRowsToString(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1) {
            if (write(fd, buf, len) == len) {
                close(fd);
                free(buf);
                E.dirty = 0;  // 清除修改标记
                snprintf(E.statusmsg, sizeof(E.statusmsg), "%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }
    free(buf);
    snprintf(E.statusmsg, sizeof(E.statusmsg), "Can't save! I/O error: %s", strerror(errno));
}

// 实现文本编辑功能
void editorInsertChar(int c) {
    if (E.cy == E.numrows) {
        editorInsertRow(E.numrows, "", 0);
    }
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    E.cx++;
    E.dirty = 1;
}

void editorInsertNewline(void) {
    if (E.cx == 0) {
        editorInsertRow(E.cy, "", 0);
    } else {
        erow *row = &E.row[E.cy];
        editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
    }
    E.cy++;
    E.cx = 0;
}

void editorDelChar(void) {
    if (E.cy == E.numrows) return;
    if (E.cx == 0 && E.cy == 0) return;

    erow *row = &E.row[E.cy];
    if (E.cx > 0) {
        editorRowDelChar(row, E.cx - 1);
        E.cx--;
        E.dirty = 1;
    } else {
        E.cx = E.row[E.cy - 1].size;
        editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
        editorDelRow(E.cy);
        E.cy--;
        E.dirty = 1;
    }
}

// 行操作相关函数
void editorFreeRow(erow *row) {
    free(row->chars);
}

void editorDelRow(int at) {
    if (at < 0 || at >= E.numrows) return;
    
    editorFreeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
    E.numrows--;
}

void editorRowInsertChar(erow *row, int at, int c) {
    if (at < 0 || at > row->size) at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
}

void editorRowDelChar(erow *row, int at) {
    if (at < 0 || at >= row->size) return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
}

void editorInsertRow(int at, char *s, size_t len) {
    if (at < 0 || at > E.numrows) return;

    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
    memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));

    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';
    E.numrows++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
}

char *editorRowsToString(int *buflen) {
    int totlen = 0;
    int j;
    for (j = 0; j < E.numrows; j++)
        totlen += E.row[j].size + 1;
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < E.numrows; j++) {
        memcpy(p, E.row[j].chars, E.row[j].size);
        p += E.row[j].size;
        *p = '\n';
        p++;
    }

    return buf;
}

// 在 editorRowsToString 函数后添加缓冲区操作函数的实现
void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);
    
    if (new == NULL) return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) {
    free(ab->b);
}

void editorScroll(void) {
    // 更新垂直滚动
    if (E.cy < E.rowoff) {
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenrows) {
        E.rowoff = E.cy - E.screenrows + 1;
    }
    
    // 更新水平滚动
    if (E.cx < E.coloff) {
        E.coloff = E.cx;
    }
    if (E.cx >= E.coloff + E.screencols) {
        E.coloff = E.cx - E.screencols + 1;
    }
}

// 添加命令行处理函数
void editorProcessCommand(void) {
    if (E.cmdline_size <= 0) return;
    
    // 去除命令行末尾的空格
    while (E.cmdline_size > 0 && E.cmdline[E.cmdline_size - 1] == ' ')
        E.cmdline_size--;
    E.cmdline[E.cmdline_size] = '\0';
    
    // 跳过开头的 ':' 字符
    char *cmd = E.cmdline + 1;
    
    if (strcmp(cmd, "w") == 0) {
        editorSave();
    } else if (strcmp(cmd, "set nu") == 0 || strcmp(cmd, "set number") == 0) {
        E.show_line_numbers = 1;
    } else if (strcmp(cmd, "set nonu") == 0 || strcmp(cmd, "set nonumber") == 0) {
        E.show_line_numbers = 0;
    } else if (strcmp(cmd, "q") == 0) {
        if (E.dirty) {
            snprintf(E.statusmsg, sizeof(E.statusmsg), 
                    "Warning! File has unsaved changes. Use :q! to force quit");
            return;
        }
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
    } else if (strcmp(cmd, "q!") == 0) {
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
    } else if (strcmp(cmd, "wq") == 0) {
        editorSave();
        if (!E.dirty) {  // 只有在保存成功后才退出
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
        }
    } else {
        snprintf(E.statusmsg, sizeof(E.statusmsg), 
                "Unknown command: %s", cmd);
    }
}

// 添加配置文件加载函数
void editorLoadConfig(void) {
    char *home = getenv("HOME");
    if (home == NULL) return;
    
    char config_path[1024];
    snprintf(config_path, sizeof(config_path), "%s/.vimrc", home);
    
    FILE *fp = fopen(config_path, "r");
    if (fp == NULL) return;
    
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        // 去除行尾的换行符
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        line[linelen] = '\0';
        
        // 处理 set nu/nonu 命令
        if (strcmp(line, "set nu") == 0 || strcmp(line, "set number") == 0) {
            E.show_line_numbers = 1;
        } else if (strcmp(line, "set nonu") == 0 || strcmp(line, "set nonumber") == 0) {
            E.show_line_numbers = 0;
        }
    }
    
    free(line);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    enableRawMode();
    initEditor();
    editorLoadConfig();  // 添加这行
    
    if (argc >= 2) {
        editorOpen(argv[1]);
    }
    
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    
    return 0;
}
