// Copyright (c) 2020 Marcus A. Mosley All rights reserved.
//
// Created by Marcus A. Mosley
// Created on January 2020
// This program includes the battle scene

#include <gb/gb.h>
#include <stdio.h>
#include "logo_data.c"
#include "logo_map.c"
#include "titlescreen_data.c"
#include "titlescreen_map.c"
#include "battle_data.c"
#include "battle_map.c"
#include "cave_data.c"
#include "cave_map.c"
#include "controls_data.c"
#include "controls_map.c"
#include "letter_data.c"
#include "letter_map.c"
#include "pause_data.c"
#include "pause_map.c"
#include "ValerioCharacter.c"
#include "ValerioSprites.c"
UINT8 i;

struct ValerioCharacter valerio;
UBYTE spritesize = 8;
const char blankmap[1] = {0x00};
UBYTE gamerunning, debug, stage;

void performantdelay(UINT8 numloops) {
    UINT8 ii;
    for (ii = 0; ii < numloops; ii++) {
        wait_vbl_done();
    }
}

void fadeout() {
    for (i=0; i < 4; i++) {
        switch (i) {
            case 0:
                BGP_REG = 0xE4;
                break;
            case 1:
                BGP_REG = 0xF9;
                break;
            case 2:
                BGP_REG = 0xFE;
                break;
            case 3:
                BGP_REG = 0xFF;
                break;
        }
        performantdelay(5);
    }
}

void fadein() {
    for (i=0; i < 3; i++) {
        switch (i) {
            case 0:
                BGP_REG = 0xFE;
                break;
            case 1:
                BGP_REG = 0xF9;
                break;
            case 2:
                BGP_REG = 0xE4;
                break;
        }
        performantdelay(5);
    }
}

void sign() {
    fadeout();
    HIDE_SPRITES;
    set_bkg_data(0, 103, letter_data);
    set_bkg_tiles(0, 0, 20, 18, letter_map);
    fadein();

    waitpad(J_A);

    DISPLAY_OFF;
    set_bkg_data(0, 99, controls_data);
    set_bkg_tiles(0, 0, 20, 18, controls_map);
    DISPLAY_ON;

    waitpad(J_B);

    fadeout();
    SHOW_SPRITES;
    set_bkg_data(0, 97, cave_data);
    set_bkg_tiles(0, 0, 20, 18, cave_map);
    fadein();
}

void movegamecharacter(struct ValerioCharacter* character, UINT8 x, UINT8 y) {
    move_sprite(character->spritids[0], x, y);
    move_sprite(character->spritids[1], x + spritesize, y);
    move_sprite(character->spritids[2], x, y + spritesize);
    move_sprite(character->spritids[3], x + spritesize, y + spritesize);
}

UBYTE canplayermove(UINT8 newplayerx, UINT8 newplayery) {
    UINT16 indexTLx, indexTLy, tileindexTL;
    UBYTE result;

    indexTLx = (newplayerx) / 8;
    indexTLy = (newplayery) / 8;
    tileindexTL = 20 * indexTLy + indexTLx;

    if (stage == 0) {
        result = cave_map[tileindexTL] == blankmap[0];
        if (tileindexTL == 87) {
            // Open Sign Menu
            sign();
        } else if (tileindexTL == 29 || tileindexTL == 30) {
            // Change to Battle Scene
            stage = 1;
            fadeout();
            HIDE_SPRITES;
            newplayerx = 80;
            newplayery = 120;
            movegamecharacter(&valerio, newplayerx, newplayery);
            set_bkg_data(0, 51, battle_data);
            set_bkg_tiles(0, 0, 20, 18, battle_map);
            SHOW_SPRITES;
            fadein();
        }
    } else {
        result = battle_map[tileindexTL] == blankmap[0];
        stage = 2;
        if (result == 1) {
            if (newplayerx <= 0 || newplayerx >= 160) {
                result = 0;
            } else if (newplayery <= 0) {
                result = 0;
            }
        }
    }

    if (debug) {
        printf("%u %u\n", (UINT16)(newplayerx), (UINT16)(newplayery));
        printf("%u %u %u\n", (UINT16)indexTLx,
            (UINT16)indexTLy, (UINT16)tileindexTL);
    }

    return result;
}

void setupvalerio() {
    valerio.x = 80;
    valerio.y = 80;
    valerio.width = 16;
    valerio.height = 16;

    // load sprites for valerio
    set_sprite_tile(0, 0);
    valerio.spritids[0] = 0;
    set_sprite_tile(1, 1);
    valerio.spritids[1] = 1;
    set_sprite_tile(2, 2);
    valerio.spritids[2] = 2;
    set_sprite_tile(3, 3);
    valerio.spritids[3] = 3;

    movegamecharacter(&valerio, valerio.x, valerio.y);
}

int valeriofront(int downpress) {
    // change sprites for valerio
    if (downpress > 0) {
        set_sprite_tile(0, 1);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 0);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 3);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 2);
        set_sprite_prop(3, S_FLIPX);
        if (downpress == 2) {
            downpress = 0;
        } else {
            downpress += 1;
        }
    } else {
        set_sprite_tile(0, 0);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 1);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 2);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 3);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (downpress == -1) {
            downpress = 1;
        } else {
            downpress -= 1;
        }
    }
    return downpress;
}

int valerioback(int uppress) {
    // change sprites for valerio
    if (uppress > 0) {
        set_sprite_tile(0, 5);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 4);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 7);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 6);
        set_sprite_prop(3, S_FLIPX);
        if (uppress == 2) {
            uppress = 0;
        } else {
            uppress += 1;
        }
    } else {
        set_sprite_tile(0, 4);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 5);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 6);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 7);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (uppress == -1) {
            uppress = 1;
        } else {
            uppress -= 1;
        }
    }
    return uppress;
}

int valerioleft(int leftpress) {
    // change sprites for valerio
    if (leftpress > 0) {
        set_sprite_tile(0, 9);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 8);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 11);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 10);
        set_sprite_prop(3, S_FLIPX);
        if (leftpress == 2) {
            leftpress = 0;
        } else {
            leftpress += 1;
        }
    } else {
        set_sprite_tile(0, 9);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 8);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 13);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 12);
        set_sprite_prop(3, S_FLIPX);
        if (leftpress == -1) {
            leftpress = 1;
        } else {
            leftpress -= 1;
        }
    }
    return leftpress;
}

int valerioright(int rightpress) {
    // change sprites for valerio
    if (rightpress > 0) {
        set_sprite_tile(0, 8);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 9);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 10);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 11);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (rightpress == 2) {
            rightpress = 0;
        } else {
            rightpress += 1;
        }
    } else {
        set_sprite_tile(0, 8);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 9);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 12);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 13);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (rightpress == -1) {
            rightpress = 1;
        } else {
            rightpress -= 1;
        }
    }
    return rightpress;
}

void start() {
    set_bkg_data(0, 110, logo_data);
    set_bkg_tiles(0, 0, 20, 18, logo_map);

    SHOW_BKG;
    DISPLAY_ON;

    performantdelay(30);

    fadeout();

    set_bkg_data(0, 45, titlescreen_data);
    set_bkg_tiles(0, 0, 20, 18, titlescreen_map);

    fadein();

    waitpad(J_START);
}

void pause() {
    DISPLAY_OFF;
    HIDE_SPRITES;
    set_bkg_data(0, 114, pause_data);
    set_bkg_tiles(0, 0, 20, 18, pause_map);
    DISPLAY_ON;

    waitpad(J_START);

    DISPLAY_OFF;
    SHOW_SPRITES;
    set_bkg_data(0, 97, cave_data);
    set_bkg_tiles(0, 0, 20, 18, cave_map);
    DISPLAY_ON;
}

void main() {
    int leftpress = 1;
    int rightpress = 1;
    int uppress = 1;
    int downpress = 1;

    start();

    set_bkg_data(0, 97, cave_data);
    set_bkg_tiles(0, 0, 20, 18, cave_map);

    set_sprite_data(0, 14, ValerioSprites);
    setupvalerio();

    gamerunning = 1;

    SHOW_SPRITES;
    DISPLAY_ON;

    while (gamerunning) {
        if (stage == 1) {
            valerio.x = 80;
            valerio.y = 120;
        }

        if (joypad() & J_A) {
            debug = 1;
        } else if (joypad() & J_SELECT) {
            pause();
        } else if (joypad() & J_LEFT) {
            if (canplayermove(valerio.x - 2, valerio.y)) {
                valerio.x -= 2;
                leftpress = valerioleft(leftpress);
                movegamecharacter(&valerio, valerio.x, valerio.y);
           }
        } else if (joypad() & J_RIGHT) {
            if (canplayermove(valerio.x + 2, valerio.y)) {
                valerio.x += 2;
                rightpress = valerioright(rightpress);
                movegamecharacter(&valerio, valerio.x, valerio.y);
            }
        } else if (joypad() & J_UP) {
            if (canplayermove(valerio.x, valerio.y - 2)) {
                valerio.y -= 2;
                uppress = valerioback(uppress);
                movegamecharacter(&valerio, valerio.x, valerio.y);
            }
        } else if (joypad() & J_DOWN) {
            if (canplayermove(valerio.x, valerio.y + 2)) {
                valerio.y += 2;
                downpress = valeriofront(downpress);
                movegamecharacter(&valerio, valerio.x, valerio.y);
            }
        }
        performantdelay(5);
    }
}
