#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>

#define PROGRESS_BAR_WIDTH 30
#define BORDER_PADDING 2
#define WINDOW_WIDTH (PROGRESS_BAR_WIDTH + 2 * BORDER_PADDING + 2)
#define WINDOW_HEIGHT 5
#define PROGRESS_INCREMENT 3
#define DELAY 300000  // 300毫秒

int main() {
    initscr();          // 初始化ncurses
    start_color();       // 启用颜色
    // 定义颜色对（前景色：绿色，背景色：黑色）
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    cbreak();           // 禁用行缓冲
    noecho();           // 不回显输入
    curs_set(FALSE);    // 隐藏光标

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    // 窗口居中显示
    int start_y = (max_y - WINDOW_HEIGHT) / 2;
    int start_x = (max_x - WINDOW_WIDTH) / 2;
    WINDOW *win = newwin(WINDOW_HEIGHT, WINDOW_WIDTH, start_y, start_x);
    box(win, 0, 0);  // 绘制边框
    wrefresh(win);

    int progress = 0;
    int max_progress = PROGRESS_BAR_WIDTH;
    while (progress <= max_progress) {
        werase(win);  // 清空窗口
        int completed = progress;
        int remaining = max_progress - progress;

        // 绘制已完成进度（绿色）
        attron(COLOR_PAIR(1));
        for (int i = 0; i < completed; i++) {
            mvwprintw(win, 1, BORDER_PADDING + 1 + i, "#");
        }
        attroff(COLOR_PAIR(1));

        // 绘制剩余进度（红色）
        attron(A_BOLD | COLOR_PAIR(2));
        for (int i = completed; i < max_progress; i++) {
            mvwprintw(win, 1, BORDER_PADDING + 1 + i, " ");
        }
        attroff(A_BOLD | COLOR_PAIR(2));

        // 显示百分比
        char percent_str[10];
        snprintf(percent_str, sizeof(percent_str), "%d%%", (progress * 100) / max_progress);
        int percent_x = (WINDOW_WIDTH - strlen(percent_str)) / 2;
        mvwprintw(win, WINDOW_HEIGHT - 1, percent_x, percent_str);

        wrefresh(win);
        progress += PROGRESS_INCREMENT;
        usleep(DELAY);
    }

    // 清理资源
    delwin(win);
    endwin();
    return 0;
}
