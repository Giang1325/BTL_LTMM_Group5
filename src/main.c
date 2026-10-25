#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "trivium.h"
#include "hex.h"

#define KEY_SIZE 10
#define IV_SIZE  10
#define BUF_SIZE 4096

static void print_usage(const char *prog) {
    fprintf(stderr, "Cach dung:\n");
    fprintf(stderr, "  %s ks  <khoa_hex_20_ky_tu> <iv_hex_20_ky_tu> <so_byte>\n", prog);
    fprintf(stderr, "  %s enc <khoa_hex_20_ky_tu> <file_vao> <file_ra>\n", prog);
    fprintf(stderr, "  %s dec <khoa_hex_20_ky_tu> <file_vao> <file_ra>\n", prog);
}

/* Lenh 1: Sinh va in keystream ra man hinh */
static int cmd_ks(const char *key_hex, const char *iv_hex, const char *len_str) {
    uint8_t key[KEY_SIZE];
    uint8_t iv[IV_SIZE];

    if (hex_to_bytes(key_hex, key, KEY_SIZE) != 0) {
        fprintf(stderr, "Loi: Khoa hex khong hop le (can dung 20 ky tu hex = 10 byte).\n");
        return 1;
    }
    if (hex_to_bytes(iv_hex, iv, IV_SIZE) != 0) {
        fprintf(stderr, "Loi: IV hex khong hop le (can dung 20 ky tu hex = 10 byte).\n");
        return 1;
    }

    char *endptr = NULL;
    long num_bytes = strtol(len_str, &endptr, 10);
    if (*len_str == '\0' || *endptr != '\0' || num_bytes <= 0) {
        fprintf(stderr, "Loi: So byte phai la so nguyen duong.\n");
        return 1;
    }

    uint8_t *ks_buf = (uint8_t *)malloc((size_t)num_bytes);
    if (ks_buf == NULL) {
        fprintf(stderr, "Loi: Khong du bo nho.\n");
        return 1;
    }

    trivium_ctx ctx;
    trivium_init(&ctx, key, iv);
    trivium_keystream(&ctx, ks_buf, (size_t)num_bytes);

    print_hex(ks_buf, (size_t)num_bytes);

    free(ks_buf);
    return 0;
}

/* Lenh 2: Ma hoa file (tu sinh IV ngau nhien, ghi 10 byte IV dau file ra) */
static int cmd_enc(const char *key_hex, const char *in_path, const char *out_path) {
    uint8_t key[KEY_SIZE];
    uint8_t iv[IV_SIZE];

    /* Kiem tra file dau vao va dau ra co trung ten khong */
    if (strcmp(in_path, out_path) == 0) {
        fprintf(stderr, "Loi: File dau ra khong duoc trung ten voi file dau vao.\n");
        return 1;
    }

    if (hex_to_bytes(key_hex, key, KEY_SIZE) != 0) {
        fprintf(stderr, "Loi: Khoa hex khong hop le (can dung 20 ky tu hex = 10 byte).\n");
        return 1;
    }

    FILE *fin = fopen(in_path, "rb");
    if (fin == NULL) {
        fprintf(stderr, "Loi: Khong the mo file dau vao '%s'.\n", in_path);
        return 1;
    }

    FILE *fout = fopen(out_path, "wb");
    if (fout == NULL) {
        fprintf(stderr, "Loi: Khong the tao file dau ra '%s'.\n", out_path);
        fclose(fin);
        return 1;
    }

    /*
     * Luu y: Sinh IV bang rand() + time(NULL) chi du dung cho bai tap minh hoa,
     * CHUA AN TOAN that trong thuc te mat ma (thuc te can dung CSPRNG cua HDH).
     */
    srand((unsigned int)time(NULL));
    for (int i = 0; i < IV_SIZE; i++) {
        iv[i] = (uint8_t)(rand() & 0xFF);
    }

    /* Ghi 10 byte IV vao dau file dau ra */
    if (fwrite(iv, 1, IV_SIZE, fout) != IV_SIZE) {
        fprintf(stderr, "Loi: Khong the ghi IV vao file dau ra.\n");
        fclose(fin);
        fclose(fout);
        return 1;
    }

    trivium_ctx ctx;
    trivium_init(&ctx, key, iv);

    uint8_t in_buf[BUF_SIZE];
    uint8_t out_buf[BUF_SIZE];
    size_t n_read;

    while ((n_read = fread(in_buf, 1, BUF_SIZE, fin)) > 0) {
        trivium_crypt(&ctx, in_buf, out_buf, n_read);
        if (fwrite(out_buf, 1, n_read, fout) != n_read) {
            fprintf(stderr, "Loi: Ghi du lieu ma hoa that bai.\n");
            fclose(fin);
            fclose(fout);
            return 1;
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Ma hoa thanh cong '%s' -> '%s'\n", in_path, out_path);
    printf("IV da sinh (10 byte): ");
    print_hex(iv, IV_SIZE);
    return 0;
}

/* Lenh 3: Giai ma file (doc 10 byte IV dau file vao roi giai ma phan con lai) */
static int cmd_dec(const char *key_hex, const char *in_path, const char *out_path) {
    uint8_t key[KEY_SIZE];
    uint8_t iv[IV_SIZE];

    /* Kiem tra file dau vao va dau ra co trung ten khong */
    if (strcmp(in_path, out_path) == 0) {
        fprintf(stderr, "Loi: File dau ra khong duoc trung ten voi file dau vao.\n");
        return 1;
    }

    if (hex_to_bytes(key_hex, key, KEY_SIZE) != 0) {
        fprintf(stderr, "Loi: Khoa hex khong hop le (can dung 20 ky tu hex = 10 byte).\n");
        return 1;
    }

    FILE *fin = fopen(in_path, "rb");
    if (fin == NULL) {
        fprintf(stderr, "Loi: Khong the mo file dau vao '%s'.\n", in_path);
        return 1;
    }

    /* Doc 10 byte IV o dau file ma hoa */
    if (fread(iv, 1, IV_SIZE, fin) != IV_SIZE) {
        fprintf(stderr, "Loi: File dau vao qua ngan, khong chua du 10 byte IV.\n");
        fclose(fin);
        return 1;
    }

    FILE *fout = fopen(out_path, "wb");
    if (fout == NULL) {
        fprintf(stderr, "Loi: Khong the tao file dau ra '%s'.\n", out_path);
        fclose(fin);
        return 1;
    }

    trivium_ctx ctx;
    trivium_init(&ctx, key, iv);

    uint8_t in_buf[BUF_SIZE];
    uint8_t out_buf[BUF_SIZE];
    size_t n_read;

    while ((n_read = fread(in_buf, 1, BUF_SIZE, fin)) > 0) {
        trivium_crypt(&ctx, in_buf, out_buf, n_read);
        if (fwrite(out_buf, 1, n_read, fout) != n_read) {
            fprintf(stderr, "Loi: Ghi du lieu giai ma that bai.\n");
            fclose(fin);
            fclose(fout);
            return 1;
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Giai ma thanh cong '%s' -> '%s'\n", in_path, out_path);
    printf("IV doc tu file (10 byte): ");
    print_hex(iv, IV_SIZE);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        print_usage(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "ks") == 0) {
        return cmd_ks(argv[2], argv[3], argv[4]);
    } else if (strcmp(cmd, "enc") == 0) {
        return cmd_enc(argv[2], argv[3], argv[4]);
    } else if (strcmp(cmd, "dec") == 0) {
        return cmd_dec(argv[2], argv[3], argv[4]);
    } else {
        fprintf(stderr, "Loi: Lenh '%s' khong hop le.\n", cmd);
        print_usage(argv[0]);
        return 1;
    }
}
