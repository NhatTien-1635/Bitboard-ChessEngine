//
// Created by Hi on 3/20/2026.
//

#ifndef BITMAPMANIPULATOR_BITMAP_H
#define BITMAPMANIPULATOR_BITMAP_H

#include <cstdint>

class Bitmap {
public:
    void InsertBit(int index);
    void InsertBit(int row, int col);

    void SetBitmap(uint64_t);

    bool RemoveBit(int index);
    bool RemoveBit(int row, int col);

    bool GetBit(int index) const;
    bool GetBit(int row, int col) const;

    int GetFirstLSBIndex() const;

    int GetBitCount() const;

    static Bitmap GetRowMask(int row);
    static Bitmap GetColumnMask(int col);

    bool MoveBit(int from, int to);

    operator uint64_t() const;

    Bitmap& operator=(const Bitmap& other) = default;
    Bitmap operator|(const Bitmap& other) const;
    Bitmap operator&(const Bitmap& other) const;
    Bitmap operator^(const Bitmap& other) const;
    Bitmap operator<<(int) const;
    Bitmap operator>>(int) const;
    Bitmap operator~() const;

    void operator|=(const Bitmap& other);
    void operator&=(const Bitmap& other);
    void operator^=(const Bitmap& other);
    void operator<<=(int);
    void operator>>=(int);

    Bitmap operator+(const Bitmap& other) const;
    Bitmap& operator+=(const Bitmap& other);

    Bitmap operator-(const Bitmap& other) const;
    Bitmap& operator-=(const Bitmap& other);

    Bitmap& operator++();
    Bitmap operator++(int);
    Bitmap& operator--();
    Bitmap operator--(int);

    Bitmap operator*(const Bitmap& other) const;
    Bitmap& operator*=(const Bitmap& other);

    Bitmap operator/(const Bitmap& other) const;
    Bitmap& operator/=(const Bitmap& other);

    Bitmap operator%(const Bitmap& other) const;
    Bitmap& operator%=(const Bitmap& other);


    Bitmap() = default;
    Bitmap(const Bitmap& other) = default;

    Bitmap(uint64_t raw_bitmap);
    ~Bitmap() = default;

private:
    uint64_t raw_bitmap = 0;

    static const uint64_t filled_first_row_bitmap = 0x00000000000000FFULL;
    static const uint64_t filled_first_col_bitmap = 0x0101010101010101ULL;
};


#endif //BITMAPMANIPULATOR_BITMAP_H
