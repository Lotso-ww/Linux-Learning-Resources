#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_DATA 30
#define HEIGHT 20
#define WIDTH 60

int main() {
    int data[MAX_DATA] = {0};
    int index = 0;
    
    // 初始化ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);  // 非阻塞输入
    keypad(stdscr, TRUE);
    
    // 初始化颜色（如果终端支持）
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
    }
    
    // 生成初始随机数据
    srand(time(NULL));
    for (int i = 0; i < MAX_DATA; i++) {
        data[i] = rand() % (HEIGHT - 2) + 1;
    }
    
    // 主循环
    int ch;
    while ((ch = getch()) != 'q') {
        clear();
        
        // 绘制边框和标题
        if (has_colors()) attron(COLOR_PAIR(4));
        box(stdscr, 0, 0);
        mvprintw(0, 2, " CPU Usage Monitor (Press 'q' to quit) ");
        if (has_colors()) attroff(COLOR_PAIR(4));
        
        // 绘制坐标轴
        mvaddch(HEIGHT, 1, ACS_LTEE);
        for (int i = 0; i < WIDTH - 2; i++) {
            mvaddch(HEIGHT, i + 2, ACS_HLINE);
        }
        mvaddch(HEIGHT, WIDTH - 1, ACS_RTEE);
        
        // 绘制Y轴刻度
        mvprintw(1, 1, "100%%");
        mvprintw(HEIGHT/2, 1, " 50%%");
        mvprintw(HEIGHT-1, 1, "  0%%");
        
        // 更新数据（模拟实时变化）
        data[index] = rand() % (HEIGHT - 2) + 1;
        index = (index + 1) % MAX_DATA;
        
        // 绘制数据点并连线
        for (int i = 0; i < MAX_DATA; i++) {
            int x = (i * (WIDTH - 4)) / MAX_DATA + 5;
            int y = HEIGHT - data[(index + i) % MAX_DATA];
            
            // 根据数值选择颜色
            if (has_colors()) {
                if (data[(index + i) % MAX_DATA] > (HEIGHT * 2) / 3)
                    attron(COLOR_PAIR(3));
                else if (data[(index + i) % MAX_DATA] > HEIGHT / 3)
                    attron(COLOR_PAIR(2));
                else
                    attron(COLOR_PAIR(1));
            }
            
            // 绘制点
            mvaddch(y, x, '*');
            
            // 绘制连线（如果下一个点存在）
            if (i < MAX_DATA - 1) {
                int next_x = ((i + 1) * (WIDTH - 4)) / MAX_DATA + 5;
                int next_y = HEIGHT - data[(index + i + 1) % MAX_DATA];
                
                // 简单的连线算法
                int dx = next_x - x;
                int dy = next_y - y;
                int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
                
                for (int s = 1; s < steps; s++) {
                    int inter_x = x + (dx * s) / steps;
                    int inter_y = y + (dy * s) / steps;
                    if (inter_y >= 1 && inter_y < HEIGHT) {
                        mvaddch(inter_y, inter_x, '.');
                    }
                }
            }
            
            if (has_colors()) attroff(COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3));
        }
        
        // 显示当前数值
        mvprintw(HEIGHT + 1, 2, "Current: %3d%%   Average: %3d%%", 
                 (data[index] * 100) / HEIGHT,
                 ((HEIGHT - (HEIGHT - data[index]) / 2) * 100) / HEIGHT);
        
        refresh();
        usleep(300000);  // 每0.3秒更新一次
    }
    
    endwin();
    return 0;
}

