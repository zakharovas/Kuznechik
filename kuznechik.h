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

    static void Initialize();

    static void L(__m128i *block);

    void S(__m128i *block) const;


    __m128i GetKey(int i) const;

private:
    __m128i keys_[10];
    __m256i keys256_[10];
    static __m128i byte_linear_matrix_[16][0x100];
    static uint8_t byte_nonlinear_matrix_[0x100];
    static __m128i byte_sl_matrix_[16][0x100];
    static __m128i C_[32];
    static bool initialized;

    void F(__m128i C, __m128i &first, __m128i &second);

    inline void SL(__m128i *block) const;

    inline void SL2(__m256i *block) const;

    inline void X(__m128i *block, size_t key_number) const;

    inline void X2(__m256i *block, size_t key_number) const;

    static __m128i FairL(int block_position, uint8_t number);

    static void ComputeNonlinearMatrix();

    static void ComputeByteLinearMatrix();

    static void ComputeC();

    static void ComputeSLMatrix();


};

#endif //KUZNECHIK_KUZNECHIK_H
