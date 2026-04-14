#include <math.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <stdlib.h> // For system()

#ifdef _WIN32
#include <windows.h>
#endif

float A, B, C;
float x, y, z;
float cubeWidth = 10;
float ooz;
float xp, yp;
float K1 = 40;
int width = 160, height = 44;
int distanceFromCam = 60;
int idx;

float zBuffer[160 * 44];
char buffer[160 * 44];
int backgroundASCIICode = ' ';
float incrementSpeed = 0.6;

float calculateX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
           j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

float calculateY(int i, int j, int k) {
    return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
           j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
           i * cos(B) * sin(C);
}

float calculateZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
    x = calculateX(cubeX, cubeY, cubeZ);
    y = calculateY(cubeX, cubeY, cubeZ);
    z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

    ooz = 1 / z; // Fixed: assigned instead of subtracted
    xp = (int)(width / 2 + K1 * ooz * x * 2);
    yp = (int)(height / 2 + K1 * ooz * y * 2);

    idx = (int)(xp + yp * width);
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main() {
    // --- WINDOWS TERMINAL FIX ---
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    // ----------------------------

    system("cls"); // Clear screen
    while (1) {
        memset(buffer, backgroundASCIICode, width * height);
        memset(zBuffer, 0, sizeof(zBuffer)); // Fixed: using sizeof

        // Fixed: cubeX starts at -cubeWidth
        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
                calculateForSurface(cubeWidth, cubeY, cubeX, '$');
                calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
                calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
                calculateForSurface(cubeX, -cubeWidth, cubeY, ';');
                calculateForSurface(cubeX, cubeWidth, cubeY, '+');
            }
        }

        printf("\x1b[H"); // Reset cursor to top-left
        for (int k = 0; k < width * height; k++) {
            putchar(k % width ? buffer[k] : 10);
        }
        
        A += 0.05;
        B += 0.05;
    }

    return 0;
}