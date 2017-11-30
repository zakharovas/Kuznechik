//
// Created by alexander on 21.11.2017.
//

#ifndef KUZNECHIK_KUZNECHIK_H
#define KUZNECHIK_KUZNECHIK_H

#include <emmintrin.h>
#include <immintrin.h>


class Kuznechik {
public:
    Kuznechik();

    void GenerateKey();

    void SetKey(__m256i key);

    void EncryptBlock(__m128i *block) const;

    void Encrypt2Blocks(__m256i *block) const;

//    void DecryptBlock(__m128i *block) const;

    __m128i keys_[10];
    __m256i keys256_[10];
    static uint16_t nonlinear_matrix_[0x10000];
    static __m128i linear_matrix_[8][0x10000];
    static __m128i byte_linear_matrix_[16][0x100];
    static uint8_t byte_nonlinear_matrix_[0x100];
    static __m128i byte_sl_matrix_[16][0x100];
    static __m128i C_[32];
    static __m128i sl_matrix_[8][0x10000];

    static __m128i FairL(int block_position, uint8_t number);

    static void ComputeNonlinearMatrix();

    static void ComputeByteLinearMatrix();

    static void ComputeC();

    static void ComputeLinearMatrix();

    static void ComputeSLMatrix();

    static void Initialize();

    inline void X(__m128i *block, size_t key_number) const;

    inline void X2(__m256i *block, size_t key_number) const;

    void S(__m128i *block) const;

    static void L(__m128i *block);

    inline void SL(__m128i *block) const;

    inline void SL2(__m256i *block) const;

    void SL16(__m128i *block) const;

    void F(__m128i C, __m128i &first, __m128i &second);

    __m128i GetKey(int i) const;
};

#endif //KUZNECHIK_KUZNECHIK_H
