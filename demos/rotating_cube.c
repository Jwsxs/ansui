#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "../include/ansui.h"

// === 3D MATH (no deps, just what we need) ===

typedef struct { float x, y, z; } Vec3;

static Vec3 rotateX(Vec3 p, float a) {
	float s = sinf(a), c = cosf(a);
	return (Vec3){ p.x, p.y * c - p.z * s, p.y * s + p.z * c };
}

static Vec3 rotateY(Vec3 p, float a) {
	float s = sinf(a), c = cosf(a);
	return (Vec3){ p.x * c + p.z * s, p.y, -p.x * s + p.z * c };
}

// Unit cube: 8 vertices centered at origin
static Vec3 cube[8] = {
	{-1,-1,-1}, { 1,-1,-1}, { 1, 1,-1}, {-1, 1,-1},
	{-1,-1, 1}, { 1,-1, 1}, { 1, 1, 1}, {-1, 1, 1},
};

// 12 edges as vertex index pairs
static int edges[12][2] = {
	{0,1},{1,2},{2,3},{3,0}, // back face
	{4,5},{5,6},{6,7},{7,4}, // front face
	{0,4},{1,5},{2,6},{3,7}, // connecting edges
};

// === BRESENHAM LINE, DRAWN DIRECTLY INTO win->pxa ===

static void draw_line(ANSUI_WIN* win, int x0, int y0, int x1, int y1, char c) {
	int w = win->cfg->w, h = win->cfg->h;
	int dx = abs(x1 - x0), dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	while (1) {
		if (x0 >= 0 && x0 < w && y0 >= 0 && y0 < h) {
			win->pxa[y0 * w + x0].c = c;
			win->pxa[y0 * w + x0].char_color = CH_BWHITE;
			win->pxa[y0 * w + x0].bg_color = BG_RBLACK;
		}
		if (x0 == x1 && y0 == y1) break;
		int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

static void clear_pxa(ANSUI_WIN* win) {
	int size = win->cfg->w * win->cfg->h;
	for (int i = 0; i < size; i++) {
		win->pxa[i].c = ' ';
		win->pxa[i].char_color = CH_BWHITE;
		win->pxa[i].bg_color = BG_RBLACK;
	}
}

int main() {
	struct winsize* ws = (struct winsize*)ansuiInit(ANSUI_FLAG_INPUT);
	int term_w = ws->ws_col;
	int term_h = ws->ws_row;

	ANSUI_WIN_CONFIG* cfg = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
	cfg->x = 0;
	cfg->y = 0;
	cfg->w = term_w - 1;
	cfg->h = term_h - 1;
	cfg->c = ' ';

	ANSUI_WIN* win = ansuiCreateWindow(cfg, ANSUI_WIN_FLAG_NONE);

	float angle_x = 0, angle_y = 0;
	int cx = cfg->w / 2;
	int cy = cfg->h / 2;
	float scale = (cfg->h < cfg->w ? cfg->h : cfg->w) * 0.35f;

	int running = 1;
	while (running) {
		ansuiClear(BG_BBLACK);
		
		if (ansuiGetKey() == ANSUI_KEY_Q) running = 0;
		clear_pxa(win);

		Vec3 proj[8];
		for (int i = 0; i < 8; i++) {
			Vec3 p = rotateX(cube[i], angle_x);
			p = rotateY(p, angle_y);

			// simple perspective: push cube away from camera, divide by z
			float z = p.z + 4.0f;
			float px = p.x / z;
			float py = p.y / z;

			// terminal chars are ~2x taller than wide, so stretch x to compensate
			proj[i].x = cx + px * scale * 2.0f;
			proj[i].y = cy + py * scale;
		}

		for (int e = 0; e < 12; e++) {
			Vec3 a = proj[edges[e][0]];
			Vec3 b = proj[edges[e][1]];
			draw_line(win, (int)a.x, (int)a.y, (int)b.x, (int)b.y, '#');
		}

		ansuiRender(win);

		angle_x += 0.04f;
		angle_y += 0.07f;

		usleep(60000);
	}

	ansuiWinDestroy(win);
	ansuiQuit();

	return ANSUI_SUCCESS;
}
