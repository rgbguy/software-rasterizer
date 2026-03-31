#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Set to 1, 2, 4, 8, or 16
#define MSAA_SAMPLES 16
#include "msaa_samples.h"

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

// basically cross prouduct of BA and CA.
// Since A B C are in 2D, cross product will be orthogonal = Z.
// And area of parallelogram = base x height. base is AB. height is ACxsin(theta)  which is cross proudct.
// So cross proudct = Z component only = Area of parallelogram ABCX. Half of it is area of triangle ABC.
// if same equation is given a point inside the triangle, it will give area of that triangle. (i.e. Area of Triangle PAB)
// this area is useful in barycentric interpolation

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

            // Test coverage at MSAA_SAMPLES positions
            int covered = 0;
            for (int s = 0; s < MSAA_SAMPLES; s++) {
                Vec2 sp = { x + sampleOffsets[s].x, y + sampleOffsets[s].y };
                float s0 = edgeFunction(v1.position, v2.position, sp);
                float s1 = edgeFunction(v2.position, v0.position, sp);
                float s2 = edgeFunction(v0.position, v1.position, sp);
                if ((s0 >= 0 && s1 >= 0 && s2 >= 0) ||
                    (s0 <= 0 && s1 <= 0 && s2 <= 0)) {
                    covered++;
                }
            }

            if (covered == 0) continue;

            // Shade once at pixel center
            Vec2 center = { x + 0.5f, y + 0.5f };
            float w0 = edgeFunction(v1.position, v2.position, center) / area;
            float w1 = edgeFunction(v2.position, v0.position, center) / area;
            float w2 = edgeFunction(v0.position, v1.position, center) / area;

            float r = w0*v0.color.r + w1*v1.color.r + w2*v2.color.r;
            float g = w0*v0.color.g + w1*v1.color.g + w2*v2.color.g;
            float b = w0*v0.color.b + w1*v1.color.b + w2*v2.color.b;

            // Blend shaded color with existing framebuffer based on coverage ratio
            float alpha = covered / (float)MSAA_SAMPLES;
            int index = (y * fb->width + x) * 3;

            fb->colorBuffer[index + 0] = (unsigned char)(alpha * r * 255 + (1 - alpha) * fb->colorBuffer[index + 0]);
            fb->colorBuffer[index + 1] = (unsigned char)(alpha * g * 255 + (1 - alpha) * fb->colorBuffer[index + 1]);
            fb->colorBuffer[index + 2] = (unsigned char)(alpha * b * 255 + (1 - alpha) * fb->colorBuffer[index + 2]);
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

    char filename[32];
    snprintf(filename, sizeof(filename), "output_msaa%d.png", MSAA_SAMPLES);
    savePNG(&fb, filename);
    
    free(fb.colorBuffer);

    return 0;
}