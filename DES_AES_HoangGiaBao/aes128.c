#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define SO_TU_KHOI_NB   4
#define SO_TU_KHOA_NK   4
#define SO_VONG_NR      10

// Bảng S-box chuẩn AES
static const uint8_t HOP_S[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// Bảng InvS-box dùng cho giải mã
static const uint8_t HOP_S_NGUOC[256] = {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

// Hằng số vòng Rcon[1..10]
static const uint8_t HANG_SO_RCON[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

void in_chuoi_16byte_hex(const uint8_t du_lieu[16]) {
    for (int i = 0; i < 16; i++) {
        printf("%02X", du_lieu[i]);
        if (i % 2 == 1 && i < 15) {
            printf(" ");
        }
    }
}

void in_trang_thai_hex(const char *nhan, uint8_t state[4][4]) {
    uint8_t tam[16];
    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            tam[cot * 4 + hang] = state[hang][cot];
        }
    }
    printf("    + %-22s: ", nhan);
    in_chuoi_16byte_hex(tam);
    printf("\n");
}

void in_ma_tran_4x4(uint8_t state[4][4]) {
    for (int hang = 0; hang < 4; hang++) {
        printf("      [ ");
        for (int cot = 0; cot < 4; cot++) {
            printf("%02X ", state[hang][cot]);
        }
        printf("]\n");
    }
}

// Nhân với 0x02 trong trường GF(2^8)
uint8_t nhan_xtime(uint8_t x) {
    uint8_t ket_qua = (uint8_t)(x << 1);
    if (x & 0x80) {
        ket_qua ^= 0x1B;
    }
    return ket_qua;
}

// Nhân hai byte bất kỳ trong GF(2^8)
uint8_t nhan_gf28(uint8_t a, uint8_t b) {
    uint8_t ket_qua = 0;
    uint8_t p = a;
    for (int bit = 0; bit < 8; bit++) {
        if (b & 1) {
            ket_qua ^= p;
        }
        p = nhan_xtime(p);
        b >>= 1;
    }
    return ket_qua;
}

// Mở rộng khóa 16 byte thành 11 khóa vòng (44 từ w[0..43])
void mo_rong_khoa(const uint8_t khoa_goc[16], uint8_t khoa_vong[11][16], int in_chi_tiet) {
    uint8_t w[44][4];

    for (int i = 0; i < SO_TU_KHOA_NK; i++) {
        w[i][0] = khoa_goc[4 * i];
        w[i][1] = khoa_goc[4 * i + 1];
        w[i][2] = khoa_goc[4 * i + 2];
        w[i][3] = khoa_goc[4 * i + 3];
    }

    for (int i = SO_TU_KHOA_NK; i < SO_TU_KHOI_NB * (SO_VONG_NR + 1); i++) {
        uint8_t tam[4] = { w[i - 1][0], w[i - 1][1], w[i - 1][2], w[i - 1][3] };

        if (i % SO_TU_KHOA_NK == 0) {
            // RotWord: dịch vòng trái 1 byte
            uint8_t byte_dau = tam[0];
            tam[0] = tam[1];
            tam[1] = tam[2];
            tam[2] = tam[3];
            tam[3] = byte_dau;

            // SubWord: thay thế từng byte qua S-box
            for (int k = 0; k < 4; k++) {
                tam[k] = HOP_S[tam[k]];
            }

            // XOR với hằng số vòng Rcon[i / 4]
            tam[0] ^= HANG_SO_RCON[i / SO_TU_KHOA_NK];
        }

        for (int k = 0; k < 4; k++) {
            w[i][k] = w[i - SO_TU_KHOA_NK][k] ^ tam[k];
        }
    }

    for (int vong = 0; vong <= SO_VONG_NR; vong++) {
        for (int tu = 0; tu < 4; tu++) {
            for (int b = 0; b < 4; b++) {
                khoa_vong[vong][tu * 4 + b] = w[vong * 4 + tu][b];
            }
        }
    }

    if (in_chi_tiet) {
        printf("\n--- [GIAI DOAN 1] MO RONG KHOA (KEY EXPANSION: 16 -> 176 BYTE) ---\n");
        for (int vong = 0; vong <= SO_VONG_NR; vong++) {
            printf("  Khoa vong k_%-2d : ", vong);
            in_chuoi_16byte_hex(khoa_vong[vong]);
            printf("\n");
        }
    }
}

void add_round_key(uint8_t state[4][4], const uint8_t k_vong[16]) {
    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            state[hang][cot] ^= k_vong[cot * 4 + hang];
        }
    }
}

void sub_bytes(uint8_t state[4][4]) {
    for (int hang = 0; hang < 4; hang++) {
        for (int cot = 0; cot < 4; cot++) {
            state[hang][cot] = HOP_S[state[hang][cot]];
        }
    }
}

// Dịch vòng trái hàng r đi r vị trí
void shift_rows(uint8_t state[4][4]) {
    uint8_t tam;

    tam = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = tam;

    tam = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = tam;
    tam = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = tam;

    tam = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = tam;
}

void mix_columns(uint8_t state[4][4]) {
    for (int cot = 0; cot < 4; cot++) {
        uint8_t a0 = state[0][cot];
        uint8_t a1 = state[1][cot];
        uint8_t a2 = state[2][cot];
        uint8_t a3 = state[3][cot];

        state[0][cot] = nhan_gf28(0x02, a0) ^ nhan_gf28(0x03, a1) ^ a2 ^ a3;
        state[1][cot] = a0 ^ nhan_gf28(0x02, a1) ^ nhan_gf28(0x03, a2) ^ a3;
        state[2][cot] = a0 ^ a1 ^ nhan_gf28(0x02, a2) ^ nhan_gf28(0x03, a3);
        state[3][cot] = nhan_gf28(0x03, a0) ^ a1 ^ a2 ^ nhan_gf28(0x02, a3);
    }
}

void inv_sub_bytes(uint8_t state[4][4]) {
    for (int hang = 0; hang < 4; hang++) {
        for (int cot = 0; cot < 4; cot++) {
            state[hang][cot] = HOP_S_NGUOC[state[hang][cot]];
        }
    }
}

void inv_shift_rows(uint8_t state[4][4]) {
    uint8_t tam;

    tam = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = tam;

    tam = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = tam;
    tam = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = tam;

    tam = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = tam;
}

void inv_mix_columns(uint8_t state[4][4]) {
    for (int cot = 0; cot < 4; cot++) {
        uint8_t a0 = state[0][cot];
        uint8_t a1 = state[1][cot];
        uint8_t a2 = state[2][cot];
        uint8_t a3 = state[3][cot];

        state[0][cot] = nhan_gf28(0x0E, a0) ^ nhan_gf28(0x0B, a1) ^ nhan_gf28(0x0D, a2) ^ nhan_gf28(0x09, a3);
        state[1][cot] = nhan_gf28(0x09, a0) ^ nhan_gf28(0x0E, a1) ^ nhan_gf28(0x0B, a2) ^ nhan_gf28(0x0D, a3);
        state[2][cot] = nhan_gf28(0x0D, a0) ^ nhan_gf28(0x09, a1) ^ nhan_gf28(0x0E, a2) ^ nhan_gf28(0x0B, a3);
        state[3][cot] = nhan_gf28(0x0B, a0) ^ nhan_gf28(0x0D, a1) ^ nhan_gf28(0x09, a2) ^ nhan_gf28(0x0E, a3);
    }
}

void ma_hoa_aes128(const uint8_t ban_ro[16], const uint8_t khoa[16],
                   uint8_t ban_ma[16], int in_chi_tiet) {
    uint8_t khoa_vong[11][16];
    mo_rong_khoa(khoa, khoa_vong, in_chi_tiet);

    // Nạp 16 byte vào ma trận trạng thái theo cột
    uint8_t state[4][4];
    for (int i = 0; i < 16; i++) {
        state[i % 4][i / 4] = ban_ro[i];
    }

    if (in_chi_tiet) {
        printf("\n--- [GIAI DOAN 2] KHOI TAO & 10 VONG BIEN DOI AES-128 ---\n");
        in_trang_thai_hex("Ban ro dau vao", state);
        printf("  Ma tran trang thai ban dau (nap theo cot):\n");
        in_ma_tran_4x4(state);
    }

    add_round_key(state, khoa_vong[0]);
    if (in_chi_tiet) {
        printf("\n  [Khoi tao] Cong khoa dau k_0:\n");
        in_trang_thai_hex("Ban ro ^ k_0", state);
    }

    // Vòng 1 đến Vòng 9
    for (int vong = 1; vong <= SO_VONG_NR - 1; vong++) {
        if (in_chi_tiet) {
            printf("\n  [Vong %2d]\n", vong);
        }

        sub_bytes(state);
        if (in_chi_tiet) in_trang_thai_hex("Sau SubBytes", state);

        shift_rows(state);
        if (in_chi_tiet) in_trang_thai_hex("Sau ShiftRows", state);

        mix_columns(state);
        if (in_chi_tiet) in_trang_thai_hex("Sau MixColumns", state);

        if (in_chi_tiet) {
            printf("    + %-22s: ", "Khoa vong k_i");
            in_chuoi_16byte_hex(khoa_vong[vong]);
            printf("\n");
        }

        add_round_key(state, khoa_vong[vong]);
        if (in_chi_tiet) {
            char nhan_ket_thuc[32];
            snprintf(nhan_ket_thuc, sizeof(nhan_ket_thuc), "Het vong %d", vong);
            in_trang_thai_hex(nhan_ket_thuc, state);
        }
    }

    // Vòng 10 (bỏ qua MixColumns)
    if (in_chi_tiet) {
        printf("\n  [Vong 10 - Vong cuoi (bo qua MixColumns)]\n");
    }

    sub_bytes(state);
    if (in_chi_tiet) in_trang_thai_hex("Sau SubBytes", state);

    shift_rows(state);
    if (in_chi_tiet) in_trang_thai_hex("Sau ShiftRows", state);

    if (in_chi_tiet) {
        printf("    + %-22s: ", "Khoa vong k_10");
        in_chuoi_16byte_hex(khoa_vong[10]);
        printf("\n");
    }

    add_round_key(state, khoa_vong[10]);
    if (in_chi_tiet) {
        in_trang_thai_hex("Het vong 10 (Ban ma)", state);
        printf("  Ma tran trang thai cuoi cung:\n");
        in_ma_tran_4x4(state);
    }

    // Đọc ma trận trạng thái theo cột ra bản mã
    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            ban_ma[cot * 4 + hang] = state[hang][cot];
        }
    }
}

void giai_ma_aes128(const uint8_t ban_ma[16], const uint8_t khoa[16], uint8_t ban_ro[16]) {
    uint8_t khoa_vong[11][16];
    mo_rong_khoa(khoa, khoa_vong, 0);

    uint8_t state[4][4];
    for (int i = 0; i < 16; i++) {
        state[i % 4][i / 4] = ban_ma[i];
    }

    add_round_key(state, khoa_vong[10]);

    for (int vong = SO_VONG_NR - 1; vong >= 1; vong--) {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        add_round_key(state, khoa_vong[vong]);
        inv_mix_columns(state);
    }

    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, khoa_vong[0]);

    for (int cot = 0; cot < 4; cot++) {
        for (int hang = 0; hang < 4; hang++) {
            ban_ro[cot * 4 + hang] = state[hang][cot];
        }
    }
}

int main(void) {
    // Bản rõ p: 15 byte 0x00 và byte cuối 0x11
    uint8_t ban_ro[16] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x11
    };

    // Khóa K: 16 byte 0x00
    uint8_t khoa[16] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };

    uint8_t ban_ma[16];
    uint8_t ban_giai_ma[16];

    printf("========================================================================\n");
    printf("KIEM DINH THUAT TOAN AES-128\n");
    printf("========================================================================\n");
    printf("Ban ro (128-bit) : ");
    in_chuoi_16byte_hex(ban_ro);
    printf("\nKhoa   (128-bit) : ");
    in_chuoi_16byte_hex(khoa);
    printf("\n");

    ma_hoa_aes128(ban_ro, khoa, ban_ma, 1);
    giai_ma_aes128(ban_ma, khoa, ban_giai_ma);

    printf("\n>>> TONG KET KIEM DINH AES-128 <<<\n");
    printf("Ban ro dau vao   : ");
    in_chuoi_16byte_hex(ban_ro);
    printf("\nKhoa 128-bit     : ");
    in_chuoi_16byte_hex(khoa);
    printf("\nBan ma tinh duoc : ");
    in_chuoi_16byte_hex(ban_ma);
    printf("\nBan giai ma      : ");
    in_chuoi_16byte_hex(ban_giai_ma);
    printf("\nKiem tra giai ma : %s\n\n",
           (memcmp(ban_giai_ma, ban_ro, 16) == 0) ? "PASS - KHOP BAN RO GOC" : "FAIL");

    return 0;
}