#include <math.h>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

#ifdef _WIN32
#include <windows.h>
#endif

float A, B;
float cubeWidth = 10;
float K1 = 40;
int width = 160, height = 44;
int distanceFromCam = 60;

float zBuffer[160 * 44];
char buffer[160 * 44];
char outputBuffer[160 * 44 + 44]; // single flush buffer
float incrementSpeed = 0.6;

float calculateX(int i, int j, int k) {
    return j * sin(A) * sin(B) * cos(0) - k * cos(A) * sin(B) * cos(0) +
           j * cos(A) * sin(0) + k * sin(A) * sin(0) + i * cos(B) * cos(0);
}

float calculateY(int i, int j, int k) {
    return j * cos(A) * cos(0) + k * sin(A) * cos(0) -
           j * sin(A) * sin(B) * sin(0) + k * cos(A) * sin(B) * sin(0) -
           i * cos(B) * sin(0);
}

float calculateZ(int i, int j, int k) {
    return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
    float x = calculateX(cubeX, cubeY, cubeZ);
    float y = calculateY(cubeX, cubeY, cubeZ);
    float z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

    float ooz = 1 / z;
    int xp = (int)(width / 2 + K1 * ooz * x * 2);
    int yp = (int)(height / 2 + K1 * ooz * y * 2);

    int idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    printf("\x1b[?25l"); // hide cursor
    printf("\x1b[2J");   // clear screen once at start

    while (1) {
        memset(buffer, ' ', width * height);
        memset(zBuffer, 0, sizeof(zBuffer));

        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
                calculateForSurface(cubeWidth, cubeY, cubeX, '$');
                calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
                calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
                calculateForSurface(cubeX, -cubeWidth, cubeY, ';');
                calculateForSurface(cubeX, cubeWidth, cubeY, '.');
            }
        }

        // build entire frame into one buffer then flush once
        int outIdx = 0;
        printf("\x1b[H"); // reset cursor
        for (int k = 0; k < width * height; k++) {
            outputBuffer[outIdx++] = (k % width == 0 && k != 0) ? '\n' : buffer[k];
        }
        fwrite(outputBuffer, 1, outIdx, stdout);
        fflush(stdout);

        A += 0.007; // smoother rotation
        B += 0.007;

        usleep(6000); // ~60fps cap
    }

    // restore cursor if ever exits
    printf("\x1b[?25h");
    return 0;
}