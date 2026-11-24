#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* =========================================================================
 * CÁC BẢNG HẰNG SỐ CHUẨN FIPS 46 CỦA THUẬT TOÁN DES
 * Quy ước đánh số bit: Bit 1 là bit trái cùng (MSB), Bit N là bit phải cùng (LSB)
 * ========================================================================= */

// 1. Bảng hoán vị khởi tạo IP (64 -> 64 bit)
static const uint8_t BANG_IP[64] = {
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6,
    64, 56, 48, 40, 32, 24, 16,  8,
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7
};

// 2. Bảng hoán vị kết thúc IP^-1 (64 -> 64 bit)
static const uint8_t BANG_IP_NGUOC[64] = {
    40,  8, 48, 16, 56, 24, 64, 32,
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25
};

// 3. Bảng hoán vị chọn 1 - PC1 (64 -> 56 bit, tự động loại 8 bit parity)
static const uint8_t BANG_PC1[56] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12,  4
};

// 4. Bảng hoán vị chọn 2 - PC2 (56 -> 48 bit)
static const uint8_t BANG_PC2[48] = {
    14, 17, 11, 24,  1,  5,  3, 28,
    15,  6, 21, 10, 23, 19, 12,  4,
    26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56,
    34, 53, 46, 42, 50, 36, 29, 32
};

// 5. Lịch số bit dịch vòng trái cho 16 vòng sinh khóa
static const uint8_t SO_BIT_DICH[16] = {
    1, 1, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 1
};

// 6. Bảng mở rộng EP trong hàm vòng f (32 -> 48 bit)
static const uint8_t BANG_EP[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

// 7. Bảng hoán vị P cuối hàm vòng f (32 -> 32 bit)
static const uint8_t BANG_P[32] = {
    16,  7, 20, 21,
    29, 12, 28, 17,
     1, 15, 23, 26,
     5, 18, 31, 10,
     2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
};

// 8. Tám hộp thế S-box (8 hộp x 4 hàng x 16 cột, nén 48 -> 32 bit)
static const uint8_t HOP_S[8][4][16] = {
    // S1
    {
        {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
        { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
        { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
        {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}
    },
    // S2
    {
        {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
        { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
        { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
        {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}
    },
    // S3
    {
        {10,  0,  9, 14,  6, 3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
        {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
        {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
        { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
    },
    // S4
    {
        { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
        {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
        {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
        { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
    },
    // S5
    {
        { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
        {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
        { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
        {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}
    },
    // S6
    {
        {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
        {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
        { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
        { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}
    },
    // S7
    {
        { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
        {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
        { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
        { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}
    },
    // S8
    {
        {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
        { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
        { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
        { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}
    }
};

/* =========================================================================
 * CÁC HÀM THAO TÁC BIT CƠ BẢN
 * ========================================================================= */

// Hàm hoán vị bit tổng quát dùng cho IP, IP^-1, PC1, PC2, EP, P
uint64_t hoan_vi(uint64_t dau_vao, const uint8_t *bang, int so_bit_vao, int so_bit_ra) {
    uint64_t ket_qua = 0;
    for (int i = 0; i < so_bit_ra; i++) {
        uint8_t vi_tri = bang[i]; // Vị trí bit tính từ 1 (trái sang phải)
        uint64_t bit = (dau_vao >> (so_bit_vao - vi_tri)) & 1ULL;// bit cần lấy 
        ket_qua = (ket_qua << 1) | bit; 
    }
    return ket_qua;
}

// Dịch vòng trái trên thanh ghi 28 bit (dùng độc lập cho C và D)
uint32_t dich_vong_trai_28(uint32_t thanh_ghi, uint8_t so_bit) {
    thanh_ghi &= 0x0FFFFFFF; // Vì thanh ghi là 32 bit nên dòng này giúp loại bỏ 4 bit đầu, giữ lại 28 bit phía sau
    return ((thanh_ghi << so_bit) | (thanh_ghi >> (28 - so_bit))) & 0x0FFFFFFF; // dùng để dịch vòng trái với so_bit tương ứng
}

/* =========================================================================
 * NHÁNH 1: LẬP LỊCH KHÓA (KEY SCHEDULE: 64 bit -> 16 khóa vòng 48 bit)
 * ========================================================================= */
void sinh_khoa_con(uint64_t khoa_64, uint64_t khoa_vong[16], int in_chi_tiet) {
    // Bước 1: Hoán vị chọn PC1 (64 -> 56 bit, bỏ 8 bit parity)
    uint64_t sau_pc1 = hoan_vi(khoa_64, BANG_PC1, 64, 56);
    uint32_t C = (uint32_t)(sau_pc1 >> 28); // 28 bit trái C0
    uint32_t D = (uint32_t)(sau_pc1 & 0x000000000FFFFFFF); // 28 bit phải D0

    if (in_chi_tiet) {
        printf("\n--- [GIAI DOAN 1] LAP LICH KHOA (KEY SCHEDULE) ---\n");
        printf("Khoa goc K (64-bit) : %016llX\n", (unsigned long long)khoa_64);
        printf("Sau PC1 (56-bit)    : %014llX  =>  C0 = %07X | D0 = %07X\n",
               (unsigned long long)sau_pc1, C, D);
        printf("------------------------------------------------------------------------\n");
        printf("Vong | Dich |    C_i    |    D_i    |   Khoa vong k_i (48-bit Hex)\n");
        printf("------------------------------------------------------------------------\n");
    }

    // Bước 2: Lặp 16 vòng dịch trái và nén qua PC2
    for (int vong = 0; vong < 16; vong++) {
        C = dich_vong_trai_28(C, SO_BIT_DICH[vong]);
        D = dich_vong_trai_28(D, SO_BIT_DICH[vong]);

        // Ghép C_i (28 bit cao) và D_i (28 bit thấp) thành khối 56 bit
        uint64_t cap_CD = ((uint64_t)C << 28) | D;

        // Nén qua PC2 (56 -> 48 bit)
        khoa_vong[vong] = hoan_vi(cap_CD, BANG_PC2, 56, 48);

        if (in_chi_tiet) {
            printf(" %2d  |  %d   |  %07X  |  %07X  |  %012llX\n",
                   vong + 1, SO_BIT_DICH[vong], C, D,
                   (unsigned long long)khoa_vong[vong]);
        }
    }
}

/* =========================================================================
 * HÀM VÒNG FEISTEL: f(k_i, R_{i-1}) = P(S(EP(R_{i-1}) XOR k_i))
 * ========================================================================= */
uint32_t ham_vong_f(uint32_t R_truoc, uint64_t k_vong, int in_chi_tiet) {
    // 1. Mở rộng EP: 32 -> 48 bit
    uint64_t sau_ep = hoan_vi(R_truoc, BANG_EP, 32, 48);

    // 2. Cộng khóa XOR với k_i (48 bit)
    uint64_t sau_xor = sau_ep ^ k_vong;

    // 3. Thay thế qua 8 hộp S-box: 48 -> 32 bit
    uint32_t sau_sbox = 0;
    for (int j = 0; j < 8; j++) {
        uint8_t cum_6bit = (uint8_t)((sau_xor >> (42 - j * 6)) & 0x3F);
        uint8_t hang = ((cum_6bit & 0x20) >> 4) | (cum_6bit & 0x01); // 2 bit ngoài b1b6
        uint8_t cot  = (cum_6bit >> 1) & 0x0F;                        // 4 bit giữa b2b3b4b5
        uint8_t gia_tri_4bit = HOP_S[j][hang][cot];
        sau_sbox = (sau_sbox << 4) | gia_tri_4bit;
    }

    // 4. Hoán vị P: 32 -> 32 bit
    uint32_t ket_qua_f = (uint32_t)hoan_vi(sau_sbox, BANG_P, 32, 32);

    if (in_chi_tiet) {
        printf("    + EP(R)       (48-bit): %012llX\n", (unsigned long long)sau_ep);
        printf("    + EP(R) ^ k_i (48-bit): %012llX\n", (unsigned long long)sau_xor);
        printf("    + Sau S-box   (32-bit): %08X\n", sau_sbox);
        printf("    + Sau P = f() (32-bit): %08X\n", ket_qua_f);
    }

    return ket_qua_f;
}

/* =========================================================================
 * NHÁNH 2: MÃ HÓA & GIẢI MÃ KHỐI 64-BIT DES
 * ========================================================================= */
uint64_t ma_hoa_des(uint64_t ban_ro, uint64_t khoa_64, int in_chi_tiet) {
    uint64_t khoa_vong[16];
    sinh_khoa_con(khoa_64, khoa_vong, in_chi_tiet);

    // Bước 1: Hoán vị khởi tạo IP (64 -> 64 bit)
    uint64_t sau_ip = hoan_vi(ban_ro, BANG_IP, 64, 64);
    uint32_t L = (uint32_t)(sau_ip >> 32);         // 32 bit trái L0
    uint32_t R = (uint32_t)(sau_ip & 0xFFFFFFFF);  // 32 bit phải R0

    if (in_chi_tiet) {
        printf("\n--- [GIAI DOAN 2] MA HOA KHOI DU LIEU (16 VONG FEISTEL) ---\n");
        printf("Ban ro M (64-bit)   : %016llX\n", (unsigned long long)ban_ro);
        printf("Sau IP   (64-bit)   : %016llX  =>  L0 = %08X | R0 = %08X\n",
               (unsigned long long)sau_ip, L, R);
    }

    // Bước 2: 16 vòng mạng Feistel
    for (int vong = 0; vong < 16; vong++) {
        uint32_t L_cu = L;
        uint32_t R_cu = R;

        if (in_chi_tiet) {
            printf("\n  [Vong %2d] Khoa k_%d = %012llX | Dau vao: L_%d = %08X, R_%d = %08X\n",
                   vong + 1, vong + 1, (unsigned long long)khoa_vong[vong],
                   vong, L_cu, vong, R_cu);
        }

        uint32_t f_ra = ham_vong_f(R_cu, khoa_vong[vong], in_chi_tiet);

        L = R_cu;           // L_i = R_{i-1}
        R = L_cu ^ f_ra;    // R_i = L_{i-1} XOR f(k_i, R_{i-1})

        if (in_chi_tiet) {
            printf("    => Dau ra Vong %2d: L_%d = %08X | R_%d = %08X\n",
                   vong + 1, vong + 1, L, vong + 1, R);
        }
    }

    // Bước 3: Đổi chỗ hai nửa 32 bit sau vòng 16 (L16R16 -> R16L16)
    uint64_t doi_cho_cuoi = ((uint64_t)R << 32) | L;

    // Bước 4: Hoán vị kết thúc IP^-1 (64 -> 64 bit)
    uint64_t ban_ma = hoan_vi(doi_cho_cuoi, BANG_IP_NGUOC, 64, 64);

    if (in_chi_tiet) {
        printf("\n--- [GIAI DOAN 3] KET THUC MA HOA ---\n");
        printf("Doi cho R16L16      : %016llX\n", (unsigned long long)doi_cho_cuoi);
        printf("Sau IP^-1 (Ban ma C): %016llX\n", (unsigned long long)ban_ma);
    }

    return ban_ma;
}

uint64_t giai_ma_des(uint64_t ban_ma, uint64_t khoa_64) {
    uint64_t khoa_vong[16];
    sinh_khoa_con(khoa_64, khoa_vong, 0);

    uint64_t sau_ip = hoan_vi(ban_ma, BANG_IP, 64, 64);
    uint32_t L = (uint32_t)(sau_ip >> 32);
    uint32_t R = (uint32_t)(sau_ip & 0xFFFFFFFF);

    // Giải mã dùng thứ tự khóa ngược từ k_16 về k_1
    for (int vong = 15; vong >= 0; vong--) {
        uint32_t L_cu = L;
        uint32_t R_cu = R;
        L = R_cu;
        R = L_cu ^ ham_vong_f(R_cu, khoa_vong[vong], 0);
    }

    uint64_t doi_cho_cuoi = ((uint64_t)R << 32) | L;
    uint64_t ban_ro =  hoan_vi(doi_cho_cuoi, BANG_IP_NGUOC, 64, 64);
    return ban_ro;
}

/* =========================================================================
 * CHUYỂN ĐỔI CHUỖI VĂN BẢN ASCII <-> KHỐI 64-BIT (ĐỆM CHUẨN PKCS#5)
 * ========================================================================= */

// Ghép 8 byte liên tiếp thành 1 khối uint64_t (Big-Endian)
uint64_t ghep_8byte_thanh_khoi(const uint8_t *buf) {
    uint64_t khoi = 0;
    for (int i = 0; i < 8; i++) {
        khoi = (khoi << 8) | buf[i];
    }
    return khoi;
}

// Tách 1 khối uint64_t thành 8 byte
void tach_khoi_thanh_8byte(uint64_t khoi, uint8_t *buf) {
    for (int i = 7; i >= 0; i--) {
        buf[i] = (uint8_t)(khoi & 0xFF);
        khoi >>= 8;
    }
}

void xu_ly_chuoi_van_ban(const char *chuoi, uint64_t khoa_64, int in_chi_tiet_khoi_dau) { // tính toán độ dài, padding, số khối và kiểm tra dung lượng để chuẩn bị dữ liệu đầu vào cho quá trình mã hóa.
    size_t do_dai_goc = strlen(chuoi);
    uint8_t gia_tri_dem = (uint8_t)(8 - (do_dai_goc % 8));
    size_t do_dai_sau_dem = do_dai_goc + gia_tri_dem;
    size_t so_khoi = do_dai_sau_dem / 8;

    uint8_t bo_dem[512];
    if (do_dai_sau_dem > sizeof(bo_dem)) {
        printf("Chuoi qua dai (toi da 500 ky tu)!\n");
        return;
    }

    memcpy(bo_dem, chuoi, do_dai_goc); // để sao chép do_dai_goc byte từ vùng nhớ chuoi sang vùng nhớ bo_dem
    memset(bo_dem + do_dai_goc, gia_tri_dem, gia_tri_dem);

    printf("\n=== MA HOA CHUOI VAN BAN ASCII (CHUAN DEM PKCS#5) ===\n");
    printf("Chuoi goc        : \"%s\" (%zu byte)\n", chuoi, do_dai_goc);
    printf("Byte dem PKCS#5  : %u byte gia tri 0x%02X => Tong: %zu byte (%zu khoi 64-bit)\n",
           gia_tri_dem, gia_tri_dem, do_dai_sau_dem, so_khoi);

    uint64_t cac_khoi_ma[64]; // chia dữ liệu đã padding thành từng khối 8 byte, lần lượt đưa từng khối vào hàm mã hóa DES và lưu kết quả bản mã vào mảng cac_khoi_ma
    for (size_t k = 0; k < so_khoi; k++) {
        uint64_t khoi_ro = ghep_8byte_thanh_khoi(&bo_dem[k * 8]);
        printf("\n[Khoi %zu/%zu] Ban ro Hex: %016llX\n", k + 1, so_khoi, (unsigned long long)khoi_ro);
        cac_khoi_ma[k] = ma_hoa_des(khoi_ro, khoa_64, (k == 0) ? in_chi_tiet_khoi_dau : 0);
        printf("[Khoi %zu/%zu] Ban ma Hex: %016llX\n", k + 1, so_khoi, (unsigned long long)cac_khoi_ma[k]);
    }

    // Giải mã và gỡ padding để kiểm chứng
    uint8_t bo_dem_giai_ma[512];
    for (size_t k = 0; k < so_khoi; k++) {
        uint64_t khoi_giai = giai_ma_des(cac_khoi_ma[k], khoa_64);
        tach_khoi_thanh_8byte(khoi_giai, &bo_dem_giai_ma[k * 8]);
    }
    uint8_t pad_cuoi = bo_dem_giai_ma[do_dai_sau_dem - 1];
    size_t do_dai_khoi_phuc = do_dai_sau_dem - pad_cuoi;
    bo_dem_giai_ma[do_dai_khoi_phuc] = '\0';

    printf("\nChuoi giai ma khoi phuc: \"%s\" (%s)\n",
           (char *)bo_dem_giai_ma,
           (strcmp(chuoi, (char *)bo_dem_giai_ma) == 0) ? "PASS" : "FAIL");
}

/* =========================================================================
 * CẤU TRÚC TEST VECTOR & CHƯƠNG TRÌNH CHÍNH
 * ========================================================================= */
/* =========================================================================
 * CHƯƠNG TRÌNH CHÍNH
 * ========================================================================= */

void chay_mot_mau_hex(uint64_t ban_ro, uint64_t khoa) {
    printf("========================================================================\n");
    printf("KIEM THUAT TOAN DES\n");
    printf("========================================================================\n");

    uint64_t ban_ma = ma_hoa_des(ban_ro, khoa, 1);

    uint64_t ban_giai_ma = giai_ma_des(ban_ma, khoa);

    printf("\n>>> TONG KET KIEM DINH <<<\n");

    printf("Ban ro dau vao      : %016llX\n",
           (unsigned long long)ban_ro);

    printf("Khoa 64-bit         : %016llX\n",
           (unsigned long long)khoa);

    printf("Ban ma tinh duoc    : %016llX\n",
           (unsigned long long)ban_ma);

    printf("Ban giai ma         : %016llX\n",
           (unsigned long long)ban_giai_ma);

    printf("Kiem tra giai ma    : %s\n\n",
           (ban_giai_ma == ban_ro)
               ? "PASS - KHOP BAN RO GOC"
               : "FAIL");
}

int main(void) {
    // Khai báo bản rõ và khóa 
    uint64_t ban_ro = 0x0000000000000003ULL;
    uint64_t khoa   = 0x0000000000000000ULL;

    // Mã hóa và giải mã
    chay_mot_mau_hex(ban_ro, khoa);

    return 0;
}