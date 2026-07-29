#include <stdint.h>

#define REG_DISPCNT *(volatile uint32_t*)0x04000000
#define MODE_3 0x0003
#define BG2_EN 0x0400
#define VRAM 0x06000000
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160
#define RGB15(r, g, b) ((r) | ((g) << 5) | ((b) << 10))

// Classic 15-bit colors, my friend
#define COLOR_DARKEST   RGB15(1, 7, 1)       // Black / Dash lines
#define COLOR_DARK      RGB15(6, 12, 6)      // General Background (Dark green)
#define COLOR_LIGHT     RGB15(17, 21, 1)     // Box Background (Light green)
#define COLOR_LIGHTEST  RGB15(19, 23, 1)     // Light text / white

#define REG_KEYINPUT *(volatile uint16_t*)0x04000130
#define KEY_A       0x0001
#define KEY_B       0x0002
#define KEY_SELECT  0x0004
#define KEY_START   0x0008
#define KEY_RIGHT   0x0010
#define KEY_LEFT    0x0020
#define KEY_UP      0x0040
#define KEY_DOWN    0x0080
#define KEY_R       0x0100
#define KEY_L       0x0200

// Constants for SRAM memory and Security, my friend
#define SRAM_BASE ((volatile uint8_t*)0x0E000000)
#define SAVE_MAGIC 0x4742 // "Retro" signature to identify a valid save
#define XOR_KEY 0xA5      // Base key for obfuscation

// Compressed 8x8 font matrix (ASCII characters 32 to 127)
const uint8_t font8x8[96][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x00},
    {0x66, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x6c, 0x6c, 0xfe, 0x6c, 0x6c, 0xfe, 0x6c, 0x6c},
    {0x18, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x18, 0x00},
    {0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00},
    {0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00},
    {0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00},
    {0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00},
    {0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00},
    {0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},
    {0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},
    {0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00},
    {0x3c, 0x66, 0x66, 0x6e, 0x76, 0x66, 0x3c, 0x00},
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00},
    {0x3c, 0x66, 0x06, 0x1c, 0x30, 0x66, 0x7e, 0x00},
    {0x3c, 0x66, 0x06, 0x1c, 0x06, 0x66, 0x3c, 0x00},
    {0x1c, 0x3c, 0x6c, 0xce, 0xfe, 0x0c, 0x0c, 0x00},
    {0x7e, 0x60, 0x7c, 0x06, 0x06, 0x66, 0x3c, 0x00},
    {0x3c, 0x66, 0x60, 0x7c, 0x66, 0x66, 0x3c, 0x00},
    {0x7e, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00},
    {0x3c, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x3c, 0x00},
    {0x3c, 0x66, 0x66, 0x3e, 0x06, 0x66, 0x3c, 0x00},
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},
    {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30},
    {0x06, 0x18, 0x60, 0x18, 0x06, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00},
    {0x60, 0x18, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00},
    {0x3c, 0x66, 0x06, 0x0c, 0x18, 0x00, 0x18, 0x00},
    {0x3c, 0x42, 0x5a, 0x5a, 0x42, 0x3c, 0x00, 0x00},
    {0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00},
    {0x7c, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x7c, 0x00},
    {0x3c, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3c, 0x00},
    {0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00},
    {0x7e, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x7e, 0x00},
    {0x7e, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00},
    {0x3c, 0x66, 0x60, 0x6e, 0x66, 0x66, 0x3c, 0x00},
    {0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00},
    {0x3e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3e, 0x00},
    {0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x00},
    {0x66, 0x36, 0x1e, 0x0e, 0x1e, 0x36, 0x66, 0x00},
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00},
    {0x63, 0x77, 0x7f, 0x6b, 0x63, 0x63, 0x63, 0x00},
    {0x66, 0x76, 0x7e, 0x7e, 0x6e, 0x66, 0x66, 0x00},
    {0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00},
    {0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x00},
    {0x3c, 0x66, 0x66, 0x66, 0x6a, 0x64, 0x3a, 0x00},
    {0x7c, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0x00},
    {0x3c, 0x66, 0x60, 0x3c, 0x06, 0x66, 0x3c, 0x00},
    {0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00},
    {0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00},
    {0x63, 0x63, 0x63, 0x6b, 0x7f, 0x77, 0x63, 0x00},
    {0x66, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x66, 0x00},
    {0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x00},
    {0x7e, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x7e, 0x00},
    {0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00},
    {0x80, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00},
    {0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00},
    {0x18, 0x3c, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00},
    {0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00},
    {0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x00},
    {0x00, 0x00, 0x3c, 0x66, 0x60, 0x66, 0x3c, 0x00},
    {0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x3e, 0x00},
    {0x00, 0x00, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00},
    {0x1c, 0x30, 0x30, 0x7c, 0x30, 0x30, 0x30, 0x00},
    {0x00, 0x00, 0x3e, 0x66, 0x66, 0x3e, 0x06, 0x3c},
    {0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x00},
    {0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00},
    {0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x38},
    {0x60, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0x00},
    {0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00},
    {0x00, 0x00, 0x66, 0x7f, 0x7f, 0x6b, 0x63, 0x00},
    {0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x00},
    {0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x00},
    {0x00, 0x00, 0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60},
    {0x00, 0x00, 0x3e, 0x66, 0x66, 0x3e, 0x06, 0x06},
    {0x00, 0x00, 0x7c, 0x66, 0x60, 0x60, 0x60, 0x00},
    {0x00, 0x00, 0x3e, 0x60, 0x3c, 0x06, 0x3c, 0x00},
    {0x10, 0x30, 0x7c, 0x30, 0x30, 0x34, 0x18, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00},
    {0x00, 0x00, 0x63, 0x6b, 0x7f, 0x7f, 0x36, 0x00},
    {0x00, 0x00, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x00},
    {0x00, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x3c},
    {0x00, 0x00, 0x7e, 0x0c, 0x18, 0x30, 0x7e, 0x00},
    {0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0e, 0x00},
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},
    {0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00},
    {0x38, 0x66, 0x76, 0x7e, 0x7e, 0x6e, 0x66, 0x66}, // Modified ñ character
    {0x3b, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

volatile uint16_t* videoBuffer = (volatile uint16_t*)VRAM;

// --- SECURITY AND STORAGE FUNCTIONS ---

void save_passwords(char passwords[10][11]) {
    volatile uint8_t* sram = SRAM_BASE;
    
    // Write the magic signature to validate the save
    sram[0] = (SAVE_MAGIC >> 8) & 0xFF;
    sram[1] = SAVE_MAGIC & 0xFF;
    
    int offset = 2;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            sram[offset] = passwords[i][j] ^ (XOR_KEY + offset);
            offset++;
        }
    }
}

void load_passwords(char passwords[10][11]) {
    volatile uint8_t* sram = SRAM_BASE;
    
    if (sram[0] == ((SAVE_MAGIC >> 8) & 0xFF) && sram[1] == (SAVE_MAGIC & 0xFF)) {
        int offset = 2;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                passwords[i][j] = sram[offset] ^ (XOR_KEY + offset);
                offset++;
            }
            passwords[i][10] = '\0';
        }
    } else {
        save_passwords(passwords);
    }
}

// --- ORIGINAL GRAPHIC FUNCTIONS ---

void waitForVBlank() {
    volatile uint16_t* vcount = (volatile uint16_t*)0x04000006;
    while (*vcount >= 160);
    while (*vcount < 160);
}

void drawRect(int x, int y, int w, int h, uint16_t color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            if (x+i >= 0 && x+i < SCREEN_WIDTH && y+j >= 0 && y+j < SCREEN_HEIGHT) {
                videoBuffer[(y+j) * SCREEN_WIDTH + (x+i)] = color;
            }
        }
    }
}

void drawChar(int x, int y, char c, uint16_t color, uint16_t bg_color, int transparent, int scale) {
    if (c < 32 || c >= 128) return;
    int index = c - 32;
    for (int j = 0; j < 8; j++) {
        uint8_t row = font8x8[index][j];
        for (int i = 0; i < 8; i++) {
            if (row & (0x80 >> i)) {
                if (scale == 1) {
                    if (x+i < SCREEN_WIDTH && y+j < SCREEN_HEIGHT)
                        videoBuffer[(y+j) * SCREEN_WIDTH + (x+i)] = color;
                } else {
                    for(int sy=0; sy<scale; sy++) {
                        for(int sx=0; sx<scale; sx++) {
                            int px = x + i*scale + sx;
                            int py = y + j*scale + sy;
                            if (px < SCREEN_WIDTH && py < SCREEN_HEIGHT)
                                videoBuffer[py * SCREEN_WIDTH + px] = color;
                        }
                    }
                }
            } else if (!transparent) {
                if (scale == 1) {
                    if (x+i < SCREEN_WIDTH && y+j < SCREEN_HEIGHT)
                        videoBuffer[(y+j) * SCREEN_WIDTH + (x+i)] = bg_color;
                } else {
                    for(int sy=0; sy<scale; sy++) {
                        for(int sx=0; sx<scale; sx++) {
                            int px = x + i*scale + sx;
                            int py = y + j*scale + sy;
                            if (px < SCREEN_WIDTH && py < SCREEN_HEIGHT)
                                videoBuffer[py * SCREEN_WIDTH + px] = bg_color;
                        }
                    }
                }
            }
        }
    }
}

void drawString(int x, int y, const char* str, uint16_t color, uint16_t bg_color, int transparent, int scale) {
    while (*str) {
        drawChar(x, y, *str, color, bg_color, transparent, scale);
        x += 8 * scale;
        str++;
    }
}

int main() {
    REG_DISPCNT = MODE_3 | BG2_EN;

    char passwords[10][11] = {
        "          ", "          ", "          ", "          ", "          ",
        "          ", "          ", "          ", "          ", "          "
    };

    load_passwords(passwords);

    const char charset[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int num_chars = 37;
    int current_slot = 0;
    int cursor_pos = 0;
    int show_password = 0;
    int needs_redraw = 1;
    uint16_t prev_keys = ~REG_KEYINPUT & 0x03FF;
    
    int save_timer = 0;
    int action_msg = 0; // 0 = Nothing, 1 = Saved, 2 = Cleared
    int l_r_held = 0;   // Controls that clearing is done only once per press
    
    const char* slots[] = {
        "(Slot 1 of 10)", "(Slot 2 of 10)", "(Slot 3 of 10)", "(Slot 4 of 10)",
        "(Slot 5 of 10)", "(Slot 6 of 10)", "(Slot 7 of 10)", "(Slot 8 of 10)",
        "(Slot 9 of 10)", "(Slot 10 of 10)"
    };

    while (1) {
        uint16_t keys = ~REG_KEYINPUT & 0x03FF;
        uint16_t keys_down = keys & ~prev_keys;
        prev_keys = keys;

        // --- FULL CLEAR (L + R Simultaneous) ---
        if ((keys & KEY_L) && (keys & KEY_R)) {
            if (!l_r_held) {
                // Clear data in RAM memory
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 10; j++) {
                        passwords[i][j] = ' ';
                    }
                    passwords[i][10] = '\0';
                }
                
                // Save empty vault to SRAM
                save_passwords(passwords); 
                
                save_timer = 60;
                action_msg = 2; // Special clear message
                needs_redraw = 1;
                l_r_held = 1;
            }
            // Mask L and R to prevent slot changing while held
            keys_down &= ~(KEY_L | KEY_R);
        } else {
            l_r_held = 0; // Reset when at least one is released
        }

        // --- GENERAL NAVIGATION ---
        if (keys_down & KEY_L) {
            current_slot = (current_slot - 1 + 10) % 10;
            cursor_pos = 0;
            needs_redraw = 1;
        }
        if (keys_down & KEY_R) {
            current_slot = (current_slot + 1) % 10;
            cursor_pos = 0;
            needs_redraw = 1;
        }
        if (keys_down & KEY_SELECT) {
            show_password = !show_password;
            needs_redraw = 1;
        }
        
        if (keys_down & KEY_START) {
            save_passwords(passwords); 
            save_timer = 60;
            action_msg = 1; // Save message
            needs_redraw = 1;
        }

        // --- D-PAD WRITING SYSTEM (Only if password is visible) ---
        if (show_password) {
            if (keys_down & KEY_LEFT) {
                cursor_pos = (cursor_pos - 1 + 10) % 10;
                needs_redraw = 1;
            }
            if (keys_down & KEY_RIGHT) {
                cursor_pos = (cursor_pos + 1) % 10;
                needs_redraw = 1;
            }
            if ((keys_down & KEY_UP) || (keys_down & KEY_DOWN)) {
                char current_c = passwords[current_slot][cursor_pos];
                int idx = 0;
                for(int i=0; i<num_chars; i++) {
                    if(charset[i] == current_c) { idx = i; break; }
                }
                
                if (keys_down & KEY_UP) {
                    idx = (idx + 1) % num_chars;
                } else {
                    idx = (idx - 1 + num_chars) % num_chars;
                }
                passwords[current_slot][cursor_pos] = charset[idx];
                needs_redraw = 1;
            }
        }

        if (save_timer > 0) {
            save_timer--;
            if (save_timer == 0) needs_redraw = 1;
        }

        if (needs_redraw) {
            needs_redraw = 0;
            
            drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DARK);
            drawString(28, 5, "Gabriel Plus Vault Bits", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            drawString(29, 5, "Gabriel Plus Vault Bits", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            drawRect(5, 20, 230, 45, COLOR_LIGHT);
            
            // Round edges
            videoBuffer[20 * 240 + 5] = COLOR_DARK; videoBuffer[20 * 240 + 6] = COLOR_DARK; videoBuffer[21 * 240 + 5] = COLOR_DARK;
            videoBuffer[20 * 240 + 234] = COLOR_DARK; videoBuffer[20 * 240 + 233] = COLOR_DARK; videoBuffer[21 * 240 + 234] = COLOR_DARK;
            videoBuffer[64 * 240 + 5] = COLOR_DARK; videoBuffer[64 * 240 + 6] = COLOR_DARK; videoBuffer[63 * 240 + 5] = COLOR_DARK;
            videoBuffer[64 * 240 + 234] = COLOR_DARK; videoBuffer[64 * 240 + 233] = COLOR_DARK; videoBuffer[63 * 240 + 234] = COLOR_DARK;
            
            // Message logic in the main box
            if (save_timer > 0) {
                if (action_msg == 1) {
                    drawString(50, 25, "Password Saved!", COLOR_DARKEST, COLOR_LIGHT, 0, 1);
                } else if (action_msg == 2) {
                    drawString(50, 25, "Vault Cleared!", COLOR_DARKEST, COLOR_LIGHT, 0, 1);
                }
            } else {
                drawString(28, 25, "Enter your password", COLOR_DARKEST, COLOR_LIGHT, 0, 1);
            }

            // Draw the password and cursors
            char* pass = passwords[current_slot];
            for (int i=0; i<10; i++) {
                if (show_password) {
                    drawChar(44 + i*16, 45, pass[i], COLOR_DARKEST, COLOR_LIGHT, 0, 1);
                    
                    if (i == cursor_pos) {
                        drawRect(44 + i*16, 55, 8, 3, COLOR_DARKEST); 
                    } else {
                        drawRect(44 + i*16, 55, 8, 1, COLOR_DARKEST); 
                    }
                } else {
                    drawRect(44 + i*16, 50, 10, 3, COLOR_DARKEST);
                }
            }
            drawString(5, 75, slots[current_slot], COLOR_LIGHTEST, COLOR_DARK, 1, 2);
            
            // Control menu
            drawString(5,  98, "Controls:", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            drawString(5, 110, "Pad: Move/Change Letter", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            drawString(5, 122, "Select: Show/Hide", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            drawString(5, 134, "Start : Save password", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            drawString(5, 146, "L / R : Change Slot", COLOR_LIGHTEST, COLOR_DARK, 1, 1);
            
            
        }
        
        waitForVBlank();
    }
    return 0;
}

// I hope you enjoy this retro project that I made with a lot of fun.
// https://github.com/thegabrielplusx/