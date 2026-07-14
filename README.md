# ansui

Terminal GUI library for better handling of interface on terminals

Stands for ANSII TUI

## ABOUT IT
Are you sick of having to handle your terminal all the time?  
Have you thought about using any sample media library but you are stuck on TTY's?  
Are you done handling yourself all the graphical user interfaces that has none to do with the agility of your terminal?  

Then **ANSUI** is the best library available for you, let me explain:  

With a simple `ansuiCreateWindow()` you can create a huge area of expandable graphical interface for your application!  
- "But I want my window to have custom configuration..."  
No worries! With as simple as `cfg = ansuiLoadConfig()` and `cfg->#` you can customize all you want!  
- "But what if-"
No worries, brother or sister! We have everything you need for your terminal applications to **finally** look like it belongs to any graphical system operation! Even if you're stuck on old IBM 68000 pcs!  

## TECHNICAL README
Zero-dependency terminal user interface header for uses with any application that's mainly focused windows.  
Huge inspiration on SDL's media workaround, planning to add more support for CLI applications, like ratatui or ncurses, but keeping a it not heavy.  

## EXAMPLES
You can find a lot of examples on `demos/`, just run `./build.sh { example.c }` and you'll run the example code you selected  
> [!Note]
> When the shell script is done, it also removes the compiled version of the code. For saving it - if you ever want to `gdb` it - run with `-s` or `--saved` parameter.  

--- WINDOW CREATION
Anyway, the most basic script _for window creation_ is:  
```c

#include "ansui.h"

int main() {
    ansuiInit(ANSUI_FLAG_NONE);

    ANSUI_WIN_CONFIG* cfg = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
    ANSUI_WIN* win = ansuiCreateWindow(cfg, ANSUI_FLAG_NONE);

    ansuiRender(win);

    ansuiQuit();
}

```

In this case: `ansuiInit()` and `ansuiQuit()` are **important** => will be explained soon.  
For windows, each has:
```c
    typedef struct ANSUI_WIN {
        ANSUI_WIN_CONFIG* cfg; // Their configs = width, height, x, y
        struct ANSUI_PIXEL* pxa; // Pixel Array = used on ansuiRender()
    } ANSUI_WIN;
```

Where:
- **cfg** is the brain of the window: stores the **width**, **height**, **x**, **y**, **char**, **char color** and **background color**;  
- **pxa** is the pixel array => important mainly on `ansuiRender()`

### Moving to Codeberg

I'm planning on moving my projects to Codeberg ( since I don't want any AI judging my code haha )  
So, on GitHub, It'll be just a showcase. The main repo ( with Issues, PR's ) is on [Codeberg](https://codeberg.org/jwe/ansui)  
