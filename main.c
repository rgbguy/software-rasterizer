#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float r, g, b;
} Color;

typedef struct {
    Vec2 position;
    Color color;
} Vertex;

typedef struct {
    int width;
    int height;
    unsigned char* colorBuffer;
} Framebuffer;

void initFramebuffer(Framebuffer* fb, int width, int height) {
    fb->width = width;
    fb->height = height;
    fb->colorBuffer = malloc(width * height * 3);
}

void clearFramebuffer(Framebuffer* fb, unsigned char r, unsigned char g, unsigned char b) {
    for (int i = 0; i < fb->width * fb->height; i++) {
        fb->colorBuffer[i*3 + 0] = r;
        fb->colorBuffer[i*3 + 1] = g;
        fb->colorBuffer[i*3 + 2] = b;
    }
}

void savePNG(Framebuffer* fb, const char* filename) {
    stbi_write_png(
        filename,
        fb->width,
        fb->height,
        3,
        fb->colorBuffer,
        fb->width * 3
    );
}

float edgeFunction(Vec2 a, Vec2 b, Vec2 c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void drawTriangle(Framebuffer* fb, Vertex v0, Vertex v1, Vertex v2) {

    // Bounding box
    float minX = fminf(fminf(v0.position.x, v1.position.x), v2.position.x);
    float minY = fminf(fminf(v0.position.y, v1.position.y), v2.position.y);
    float maxX = fmaxf(fmaxf(v0.position.x, v1.position.x), v2.position.x);
    float maxY = fmaxf(fmaxf(v0.position.y, v1.position.y), v2.position.y);

    int x0 = (int)fmaxf(floorf(minX), 0);
    int y0 = (int)fmaxf(floorf(minY), 0);
    int x1 = (int)fminf(ceilf(maxX), fb->width - 1);
    int y1 = (int)fminf(ceilf(maxY), fb->height - 1);

    float area = edgeFunction(v0.position, v1.position, v2.position);

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {

            Vec2 p = { x + 0.5f, y + 0.5f };

            float w0 = edgeFunction(v1.position, v2.position, p);
            float w1 = edgeFunction(v2.position, v0.position, p);
            float w2 = edgeFunction(v0.position, v1.position, p);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                (w0 <= 0 && w1 <= 0 && w2 <= 0)){
                w0 /= area;
                w1 /= area;
                w2 /= area;

                float r = w0*v0.color.r + w1*v1.color.r + w2*v2.color.r;
                float g = w0*v0.color.g + w1*v1.color.g + w2*v2.color.g;
                float b = w0*v0.color.b + w1*v1.color.b + w2*v2.color.b;

                int index = (y * fb->width + x) * 3;

                fb->colorBuffer[index + 0] = (unsigned char)(r * 255);
                fb->colorBuffer[index + 1] = (unsigned char)(g * 255);
                fb->colorBuffer[index + 2] = (unsigned char)(b * 255);
            }
        }
    }
}

int main() {

    Framebuffer fb;
    initFramebuffer(&fb, 800, 600);
    clearFramebuffer(&fb, 30, 30, 30);

    Vertex v0 = { {100, 100}, {1, 0, 0} };
    Vertex v1 = { {700, 150}, {0, 1, 0} };
    Vertex v2 = { {400, 500}, {0, 0, 1} };

    drawTriangle(&fb, v0, v1, v2);

    savePNG(&fb, "output.png");
    
    free(fb.colorBuffer);

    return 0;
}