
#include "max7219_matrix.h"

static uint8_t matrix[8];

inline void max7219_matrix_set_pixel(uint8_t row, uint8_t col, uint8_t state) {
    if (state) {
        max7219_matrix_activate_pixel(row, col);
    } else {
        max7219_matrix_clear_pixel(row, col);
    }
    // update is called inside each of the two functions above, no need for redundancy here
}

inline void max7219_matrix_activate_pixel(uint8_t row, uint8_t col) {
    matrix[row] |= (1u << (uint8_t) (7 - col));
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_clear_pixel(uint8_t row, uint8_t col) {
    matrix[row] &= ~(1u << (uint8_t) (7-col));
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_set_row(uint8_t row, uint8_t data) {
    matrix[row] = data;
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

//MSB of data will be in row 0, LSB of data will be in row 7
inline void max7219_matrix_set_col(uint8_t col, uint8_t data) {
    uint8_t mask = ~(1u << (uint8_t) (7 - col));
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] &= mask; //set the column col in row i to 0
        //maybe change this to a conditional branch for the sake of readability
        matrix[i] |= ((uint8_t) ((uint8_t) data >> (uint8_t) (7 - i)) & 1u) << (uint8_t) (7 - col); //set the data
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_set_matrix(const uint8_t data[8]) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = data[i];
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_set_hex_string(uint64_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = 0xFFu & (data >> (uint8_t) (8 * (7 - i)));
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_shift_left(void) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] <<= 1u;
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_shift_right(void) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] >>= 1u;
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_shift_left_circular(void) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = (uint8_t) (matrix[i] << 1u) | (uint8_t) (matrix[i] >> 7u);
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_shift_right_circular(void) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = (uint8_t) (matrix[i] << 7u) | (uint8_t) (matrix[i] >> 1u);
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_invert(void) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = ~matrix[i];
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_rotate_left(void) {
    uint8_t temp[8] = {0};
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            temp[j] |= (uint8_t) ((uint8_t) ((uint8_t) (matrix[i] & (uint8_t) (1u << j)) >> j) << (uint8_t) (7 - i));
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = temp[i];
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_rotate_right(void) {
    uint8_t temp[8] = {0};
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            temp[7-j] |= (uint8_t) ((uint8_t) ((uint8_t) (matrix[i] & (uint8_t) (1u << j)) >> j) << i);
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = temp[i];
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_flip_horizontal(void) {
    uint8_t temp;
    for (uint8_t i = 0; i < 4; i++) {
        temp = matrix[7-i];
        matrix[7-i] = matrix[i];
        matrix[i] = temp;
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_flip_vertical(void) {
    uint8_t b;
    for (uint8_t i = 0; i < 8; i++) {
        b = matrix[i];
        //reverse a byte
        b = (b & 0xF0u) >> 4u | (b & 0x0Fu) << 4u;
        b = (b & 0xCCu) >> 2u | (b & 0x33u) << 2u;
        b = (b & 0xAAu) >> 1u | (b & 0x55u) << 1u;
        matrix[i] = b;
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_clear(void) {
    for (uint8_t i = 0; i < 8; i++) {
        matrix[i] = 0;
    }
#ifndef MAX7219_MATRIX_UPDATE_MANUALLY
    max7219_matrix_update();
#endif
}

inline void max7219_matrix_update(void) {
    for (uint8_t i = 0; i < 8; i++) {
        max7219_send_command(i + 1, matrix[i]);
    }
}