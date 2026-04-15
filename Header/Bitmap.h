//
// Created by Hi on 3/20/2026.
//

#ifndef BITMAPMANIPULATOR_BITMAP_H
#define BITMAPMANIPULATOR_BITMAP_H

#include <cstdint>

class Bitmap {
public:
    inline void InsertBit(int index);

    inline void InsertBit(int row, int col);

    inline int GetFirstLSBIndex() const;

    inline int GetBitCount() const;

    inline static Bitmap GetRowMask(int row);

    inline static Bitmap GetColumnMask(int col);

    inline bool MoveBit(int from, int to);

    inline operator uint64_t() const;

    Bitmap &operator=(const Bitmap &other) = default;

    inline Bitmap operator|(const Bitmap &other) const;

    inline Bitmap operator&(const Bitmap &other) const;

    inline Bitmap operator^(const Bitmap &other) const;

    inline Bitmap operator<<(int) const;

    inline Bitmap operator>>(int) const;

    inline Bitmap operator~() const;

    inline void operator|=(const Bitmap &other);

    inline void operator&=(const Bitmap &other);

    inline void operator^=(const Bitmap &other);

    inline void operator<<=(int);

    inline void operator>>=(int);

    inline Bitmap operator+(const Bitmap &other) const;

    inline Bitmap &operator+=(const Bitmap &other);

    inline Bitmap operator-(const Bitmap &other) const;

    inline Bitmap &operator-=(const Bitmap &other);

    inline Bitmap &operator++();

    inline Bitmap operator++(int);

    inline Bitmap &operator--();

    inline Bitmap operator--(int);

    inline Bitmap operator*(const Bitmap &other) const;

    inline Bitmap &operator*=(const Bitmap &other);

    inline Bitmap operator/(const Bitmap &other) const;

    inline Bitmap &operator/=(const Bitmap &other);

    inline Bitmap operator%(const Bitmap &other) const;

    inline Bitmap &operator%=(const Bitmap &other);

    inline void SetBitmap(uint64_t);

    inline bool RemoveBit(int index);

    inline bool RemoveBit(int row, int col);

    inline bool GetBit(int index) const;

    inline bool GetBit(int row, int col) const;

    Bitmap() = default;

    Bitmap(const Bitmap &other) = default;

    Bitmap(uint64_t raw_bitmap);

    ~Bitmap() = default;

private:
    uint64_t raw_bitmap = 0;

    static const uint64_t filled_first_row_bitmap = 0x00000000000000FFULL;
    static const uint64_t filled_first_col_bitmap = 0x0101010101010101ULL;
};

inline Bitmap::Bitmap(uint64_t raw_bitmap)
    : raw_bitmap(raw_bitmap)
{

}

inline void Bitmap::InsertBit(int index) {
    raw_bitmap |= (1ULL << index);
}

void Bitmap::InsertBit(int row, int col) {
    InsertBit(row * 8 + col);
}

void Bitmap::SetBitmap(uint64_t bitmap) {
    raw_bitmap = bitmap;
}

bool Bitmap::RemoveBit(int index) {
    uint64_t mask = (1ULL << index);
    if ((mask & raw_bitmap) == 0) {
        return false;
    }
    raw_bitmap &= ~mask;
    return true;
}

bool Bitmap::RemoveBit(int row, int col) {
    return RemoveBit(row * 8 + col);
}

bool Bitmap::GetBit(int index) const {
    return raw_bitmap & (1ULL << index);
}

bool Bitmap::GetBit(int row, int col) const {
    return GetBit(row * 8 + col);
}

bool Bitmap::MoveBit(int from, int to) {
    bool insert = RemoveBit(from);
    if (!insert) {
        return false;
    }

    InsertBit(to);
    return true;
}

Bitmap::operator uint64_t() const {
    return raw_bitmap;
}

Bitmap Bitmap::GetRowMask(int row) {
    return filled_first_row_bitmap << (row * 8);
}

Bitmap Bitmap::GetColumnMask(int col) {
    return filled_first_col_bitmap << col;
}

int Bitmap::GetFirstLSBIndex() const {
    if (!raw_bitmap) {
        return -1;
    }
    return Bitmap((raw_bitmap & -raw_bitmap) - 1).GetBitCount();
}

int Bitmap::GetBitCount() const {
    int count = 0;
    uint64_t count_map = raw_bitmap;
    while (count_map) {
        count_map &= count_map - 1;
        ++count;
    }

    return count;
}

Bitmap Bitmap::operator|(const Bitmap &other) const {
    return Bitmap(this->raw_bitmap | other.raw_bitmap);
}

Bitmap Bitmap::operator&(const Bitmap &other) const {
    return Bitmap(this->raw_bitmap & other.raw_bitmap);
}

Bitmap Bitmap::operator^(const Bitmap &other) const {
    return Bitmap(this->raw_bitmap ^ other.raw_bitmap);
}

Bitmap Bitmap::operator<<(int number) const {
    return Bitmap(raw_bitmap << number);
}

Bitmap Bitmap::operator>>(int number) const {
    return Bitmap(raw_bitmap >> number);
}

Bitmap Bitmap::operator~() const {
    return Bitmap(~raw_bitmap);
}

void Bitmap::operator|=(const Bitmap &other) {
    this->raw_bitmap |= other.raw_bitmap;
}

void Bitmap::operator&=(const Bitmap &other) {
    this->raw_bitmap &= other.raw_bitmap;
}

void Bitmap::operator^=(const Bitmap &other) {
    this->raw_bitmap ^= other.raw_bitmap;
}

void Bitmap::operator<<=(int number) {
    this->raw_bitmap <<= number;
}

void Bitmap::operator>>=(int number) {
    this->raw_bitmap >>= number;
}

Bitmap &Bitmap::operator+=(const Bitmap &other) {
    this->raw_bitmap += other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator+(const Bitmap &other) const {
    Bitmap result = *this;
    result += other;
    return result;
}

Bitmap &Bitmap::operator-=(const Bitmap &other) {
    this->raw_bitmap -= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator-(const Bitmap &other) const {
    Bitmap result = *this;
    result -= other;
    return result;
}

Bitmap &Bitmap::operator++() {
    ++this->raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator++(int) {
    Bitmap temp = *this;
    ++(*this);
    return temp;
}

Bitmap &Bitmap::operator--() {
    --this->raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator--(int) {
    Bitmap temp = *this;
    --(*this);
    return temp;
}

Bitmap &Bitmap::operator*=(const Bitmap &other) {
    this->raw_bitmap *= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator*(const Bitmap &other) const {
    Bitmap result = *this;
    result *= other;
    return result;
}

Bitmap &Bitmap::operator/=(const Bitmap &other) {
    this->raw_bitmap /= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator/(const Bitmap &other) const {
    Bitmap result = *this;
    result /= other;
    return result;
}

Bitmap &Bitmap::operator%=(const Bitmap &other) {
    this->raw_bitmap %= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator%(const Bitmap &other) const {
    Bitmap result = *this;
    result %= other;
    return result;
}

#endif //BITMAPMANIPULATOR_BITMAP_H
