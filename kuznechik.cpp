//
// Created by alexander on 21.11.2017.
//
#include <iostream>
#include "kuznechik.h"

static void print_m128(__m128i a) {
    uint8_t *b = (uint8_t *) &a;
    for (int i = 0; i < 16; ++i) {
        std::cout << std::hex << (uint16_t) b[i] << " ";
    }
    std::cout << std::endl;
}

uint8_t Kuznechik::byte_nonlinear_matrix_[0x100];

uint16_t Kuznechik::nonlinear_matrix_[0x10000];

__m128i Kuznechik::byte_linear_matrix_[16][0x100];

__m128i Kuznechik::byte_sl_matrix_[16][0x100];

__m128i Kuznechik::sl_matrix_[8][0x10000];

__m128i Kuznechik::linear_matrix_[8][0x10000];

__m128i Kuznechik::C_[32];


void Kuznechik::EncryptBlock(__m128i *block) const {
    for (size_t i = 0; i < 9; ++i) {
        X(block, i);
        SL(block);
        //L(block);
    }
    X(block, 9);
}

void Kuznechik::GenerateKey() {
    __m256i basic_key;
    uint16_t key_as_vector[16];
    for (size_t i = 0; i < 16; ++i) {
        key_as_vector[i] = uint16_t(rand());
    }
    basic_key = _mm256_load_si256((__m256i *) key_as_vector);
    SetKey(basic_key);

}

Kuznechik::Kuznechik() {
    GenerateKey();
    ComputeSLMatrix();
}

void Kuznechik::X(__m128i *block, size_t key_number) const {
    *block = _mm_xor_si128(*block, keys_[key_number]);
}

void Kuznechik::S(__m128i *block) const {
    uint8_t *as_array = (uint8_t *) block;
    for (size_t i = 0; i < 16; ++i) {
        as_array[i] = Kuznechik::byte_nonlinear_matrix_[as_array[i]];
    }
}

void Kuznechik::ComputeNonlinearMatrix() {
    uint8_t byte_matrix[256] = {
            252, 238, 221, 17, 207, 110, 49, 22, 251, 196, 250, 218, 35, 197, 4, 77,
            233, 119, 240, 219, 147, 46, 153, 186, 23, 54, 241, 187, 20, 205, 95, 193,
            249, 24, 101, 90, 226, 92, 239, 33, 129, 28, 60, 66, 139, 1, 142, 79,
            5, 132, 2, 174, 227, 106, 143, 160, 6, 11, 237, 152, 127, 212, 211, 31,
            235, 52, 44, 81, 234, 200, 72, 171, 242, 42, 104, 162, 253, 58, 206, 204,
            181, 112, 14, 86, 8, 12, 118, 18, 191, 114, 19, 71, 156, 183, 93, 135,
            21, 161, 150, 41, 16, 123, 154, 199, 243, 145, 120, 111, 157, 158, 178, 177,
            50, 117, 25, 61, 255, 53, 138, 126, 109, 84, 198, 128, 195, 189, 13, 87,
            223, 245, 36, 169, 62, 168, 67, 201, 215, 121, 214, 246, 124, 34, 185, 3,
            224, 15, 236, 222, 122, 148, 176, 188, 220, 232, 40, 80, 78, 51, 10, 74,
            167, 151, 96, 115, 30, 0, 98, 68, 26, 184, 56, 130, 100, 159, 38, 65,
            173, 69, 70, 146, 39, 94, 85, 47, 140, 163, 165, 125, 105, 213, 149, 59,
            7, 88, 179, 64, 134, 172, 29, 247, 48, 55, 107, 228, 136, 217, 231, 137,
            225, 27, 131, 73, 76, 63, 248, 254, 141, 83, 170, 144, 202, 216, 133, 97,
            32, 113, 103, 164, 45, 43, 9, 91, 203, 155, 37, 208, 190, 229, 108, 82,
            89, 166, 116, 210, 230, 244, 180, 192, 209, 102, 175, 194, 57, 75, 99, 182
    };
    for (size_t i = 0; i < 0x100; ++i) {
        byte_nonlinear_matrix_[i] = byte_matrix[i];
    }
    for (size_t i = 0; i < 0x10000; ++i) {
        uint16_t first = i >> 8;
        uint16_t second = i & 0xFF;
        nonlinear_matrix_[i] = ((byte_matrix[first] << 8) | (byte_matrix[second]));
    }

}

void Kuznechik::Initialize() {
    ComputeNonlinearMatrix();
    ComputeByteLinearMatrix();
    ComputeLinearMatrix();
    ComputeSLMatrix();
    ComputeC();
}

void Kuznechik::L(__m128i *block) {
    __m128i answer = _mm_setzero_si128();
    uint8_t *as_array = (uint8_t *) block;
    for (size_t i = 0; i < 16; ++i) {
        answer = _mm_xor_si128(answer, Kuznechik::byte_linear_matrix_[i][as_array[i]]);
    }
    *block = answer;

}


void Kuznechik::SL(__m128i *block) const {
    __m128i answer = _mm_setzero_si128();
    uint8_t *as_array = (uint8_t *) block;
    for (size_t i = 0; i < 16; ++i) {
        answer = _mm_xor_si128(answer, Kuznechik::byte_sl_matrix_[i][as_array[i]]);
    }
    *block = answer;
}

void Kuznechik::SL16(__m128i *block) const {
    __m128i answer = _mm_setzero_si128();
    uint16_t *as_array = (uint16_t *) block;
    for (size_t i = 0; i < 8; ++i) {
        answer = _mm_xor_si128(answer, Kuznechik::sl_matrix_[i][as_array[i]]);
    }
    *block = answer;
}

void Kuznechik::ComputeLinearMatrix() {

    for (int position = 0; position < 8; ++position) {
        for (uint32_t number = 0; number < 0x10000; ++number) {
            uint16_t first = (number >> 8);
            uint16_t second = (number & 0xFF);
            linear_matrix_[position][number] = _mm_xor_si128(
                    byte_linear_matrix_[2 * position + 1][first],
                    byte_linear_matrix_[2 * position][second]);
        }
    }

}


//https://en.wikipedia.org/wiki/Finite_field_arithmetic
uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0; /* the product of the multiplication */
    while (a && b) {
        if (b & 1) /* if b is odd, then add the corresponding a to p (final product = sum of all a's corresponding to odd b's) */
            p ^= a; /* since we're in GF(2^m), addition is an XOR */

        if (a & 0x80) /* GF modulo: if a >= 128, then it will overflow when shifted left, so reduce */
            a = (a << 1) ^ 0xC3;
        else
            a <<= 1; /* equivalent to a*2 */
        b >>= 1; /* equivalent to b // 2 */
    }
    return p;
}


__m128i Kuznechik::FairL(int block_position, uint8_t number) {
    uint8_t coeffs[16] = {
            148, 32, 133, 16, 194, 192, 1, 251,
            1, 192, 194, 16, 133, 32, 148, 1
    };
    uint8_t full_block[16];
    for (int i = 0; i < 16; ++i) {
        if (i == block_position) {
            full_block[i] = number;
        } else {
            full_block[i] = 0;
        }
    }
    uint8_t answer;
    for (int i = 0; i < 16; ++i) {
        answer = 0;
        for (int j = 15; j >= 0; --j) {
            if (j < 15) {
                full_block[j + 1] = full_block[j];
            }
            answer ^= gmul(full_block[j], coeffs[j]);
        }
        full_block[0] = answer;
    }
    __m128i block_as_128int = *((__m128i *) full_block);
    return block_as_128int;
}

void Kuznechik::ComputeByteLinearMatrix() {
    for (int position = 0; position < 16; ++position) {
        for (uint16_t number = 0; number < 0x100; ++number) {
            byte_linear_matrix_[position][number] = FairL(position, number);
        }
    }
}

void Kuznechik::ComputeSLMatrix() {
    for (int position = 0; position < 16; ++position) {
        for (uint16_t number = 0; number < 0x100; ++number) {
            byte_sl_matrix_[position][number] = byte_linear_matrix_[position][byte_nonlinear_matrix_[number]];
        }
    }
    for (int position = 0; position < 8; ++position) {
        for (uint32_t number = 0; number < 0x10000; ++number) {
            sl_matrix_[position][number] = linear_matrix_[position][nonlinear_matrix_[number]];
        }
    }

}

void Kuznechik::SetKey(__m256i key) {
    keys_[0] = _mm256_extractf128_si256(key, 0);
    keys_[1] = _mm256_extractf128_si256(key, 1);
    for (int i = 0; i < 4; ++i) {
        _mm_store_si128(&keys_[2 * i + 2], keys_[2 * i]);
        _mm_store_si128(&keys_[2 * i + 3], keys_[2 * i + 1]);
        for (int j = 1; j <= 8; ++j) {
            F(C_[8 * i + j - 1], keys_[2 * i + 2], keys_[2 * i + 3]);
        }
    }


}

void Kuznechik::F(__m128i C, __m128i &first, __m128i &second) {
    __m128i x = _mm_xor_si128(C, first);
    SL(&x);
    x = _mm_xor_si128(x, second);
    _mm_store_si128(&second, first);
    _mm_store_si128(&first, x);
}

void Kuznechik::ComputeC() {
    for (int i = 0; i < 32; ++i) {
        uint8_t current[16];
        for (int j = 0; j < 15; ++j) {
            current[j] = 0;
        }
        current[15] = i + 1;
        __m128i x = *((__m128i *) current);
        L(&x);
        _mm_store_si128(&C_[i], x);
    }

}

__m128i Kuznechik::GetKey(int i) const {
    return keys_[i];
}



