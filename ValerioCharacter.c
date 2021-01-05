// Copyright (c) 2020 Marcus A. Mosley All rights reserved.
//
// Created by Marcus A. Mosley
// Created on December 2020
// This program is the character

#include <gb/gb.h>

// generical character structure: id, position, graphics

struct ValerioCharacter {
    UBYTE spritids[4];  // all characters use 4 sprites
    UINT8 x;
    UINT8 y;
    UINT8 width;
    UINT8 height;
};
