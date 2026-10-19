#include "test_vectors.h"

// Chuẩn C99 khai báo mảng dữ liệu test
const TriviumTestVector trivium_vectors[NUM_VECTORS] = {
    {
        .id = "TV1",
        .source = "eSTREAM (ecrypt.eu.org)",
        .key_hex = "00000000000000000000",
        .iv_hex = "00000000000000000000",
        .keystream_hex = "FBE0BF265859051B517A2E4E239FC97F"
    },
    {
        .id = "TV2",
        .source = "eSTREAM (ecrypt.eu.org)",
        .key_hex = "80000000000000000000",
        .iv_hex = "00000000000000000000",
        .keystream_hex = "3373AEDE99BD9E0459C45A11488A3FF9"
    },
    {
        .id = "TV3",
        .source = "eSTREAM (ecrypt.eu.org)",
        .key_hex = "00000000000000000000",
        .iv_hex = "80000000000000000000",
        .keystream_hex = "9A5C56169E7FA406709E293EBE5B3E73"
    },
    {
        .id = "TV4",
        .source = "avr-crypto-lib (GitHub)",
        .key_hex = "0F62B5085BAE0154A7FA",
        .iv_hex = "288FF65DC42B92F960C7",
        .keystream_hex = "D39A693FFFEC5CA9FC8396179907F462"
    },
    {
        .id = "TV5",
        .source = "Sinh tự động (Python)",
        .key_hex = "E4CFCA75C1651FC7985F",
        .iv_hex = "CD5D9953439C8B82D0F8",
        .keystream_hex = "B11B68A51F3AED92D3F545BEA1ED0144"
    },
    {
        .id = "TV6",
        .source = "Sinh tự động (Python)",
        .key_hex = "2C4107673EBEF30D8D34",
        .iv_hex = "11B33853B314ADEC02B1",
        .keystream_hex = "2AB408C6AD39CE158CCF0B2EDD1F61FA"
    },
    {
        .id = "TV7",
        .source = "Sinh tự động (Python)",
        .key_hex = "39FFE5BA6FA0F19F11CC",
        .iv_hex = "AE39643272FDAE657DBE",
        .keystream_hex = "0B44B28ECE98726C5ACD7353F9EC7393"
    },
    {
        .id = "TV8",
        .source = "Sinh tự động (Python)",
        .key_hex = "3349023D03BC58225C05",
        .iv_hex = "3608698D991FD24647B1",
        .keystream_hex = "F98FF778FF5E390D3A5B44106FCB253D"
    }
};