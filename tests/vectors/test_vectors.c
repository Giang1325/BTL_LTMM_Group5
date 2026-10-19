#include "test_vectors.h"

const TriviumTestVector trivium_vectors[NUM_VECTORS] = {
    {
        .source = "eSTREAM (ecrypt.eu.org)",
        .key_hex = "00000000000000000000",
        .iv_hex = "00000000000000000000",
        .keystream_hex = "FBE0BF265859051B517A2E4E239FC97F563203161907CF2DE7A8790FA1B2E9CD"
    },
    {
        .source = "eSTREAM (ecrypt.eu.org)",
        .key_hex = "80000000000000000000",
        .iv_hex = "00000000000000000000",
        .keystream_hex = "3373AEDE99BD9E0459C45A11488A3FF9F50F65AA7E137772FD2B8414615B6710"
    },
    {
        .source = "eSTREAM (ecrypt.eu.org)",
        .key_hex = "00000000000000000000",
        .iv_hex = "80000000000000000000",
        .keystream_hex = "9A5C56169E7FA406709E293EBE5B3E73D72313A88EBFA9C8ADD82437B45FB7B4"
    },
    {
        .source = "avr-crypto-lib (GitHub)",
        .key_hex = "0F62B5085BAE0154A7FA",
        .iv_hex = "288FF65DC42B92F960C7",
        .keystream_hex = "D39A693FFFEC5CA9FC8396179907F462AA9D4340A256E9B46F9E574680102A9F"
    }
};