#include <iostream>
#include <unistd.h>
#include <X11/extensions/XTest.h>
#include <X11/Xlib.h>
#include <cstring>
#include <cmath>
#include <signal.h>

bool running = true;

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);

    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Cannot open display" << std::endl;
        return 1;
    }

    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);

    // Settings
    int scrollSensitivity = 100;
    int scrollDelay = 20;
    int paddingTop = 70;
    int paddingBot = 60;
    int screenHeight = DisplayHeight(display, screen);

    // Button codes
    int middleMouseButton = 2;
    int mouseScrollUp = 4;
    int mouseScrollDown = 5;
    int mouseScrollLeft = 6;
    int mouseScrollRight = 7;

    // Init mouse pos
    int startx, starty, winx, winy;
    unsigned int mask;
    Window child_win, root_win;
    XQueryPointer(display, root, &root_win, &child_win, &startx, &starty, &winx, &winy, &mask);

    while (running) {
        XQueryPointer(display, root, &root_win, &child_win, &winx, &winy, &winx, &winy, &mask);
        if (mask & Button2Mask) {
            if (winy > paddingTop && winy < screenHeight - paddingBot) {
                int dy = winy - starty;
                int dx = winx - startx;
                int numVerticalScrolls = abs(dy) / scrollSensitivity;
                int numHorizontalScrolls = abs(dx) / scrollSensitivity;

                int vButton = dy > 0 ? mouseScrollDown : mouseScrollUp;
                int hButton = dx > 0 ? mouseScrollRight : mouseScrollLeft;

                for (int i = 0; i < numVerticalScrolls; ++i) {
                    XTestFakeButtonEvent(display, vButton, True, CurrentTime);
                    XTestFakeButtonEvent(display, vButton, False, CurrentTime);
                }
                for (int i = 0; i < numHorizontalScrolls; ++i) {
                    XTestFakeButtonEvent(display, hButton, True, CurrentTime);
                    XTestFakeButtonEvent(display, hButton, False, CurrentTime);
                }

                usleep(scrollDelay * 1000);
            }
        }
    }

    XCloseDisplay(display);
    return 0;
}
