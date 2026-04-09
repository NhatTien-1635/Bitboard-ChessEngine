//
// Created by Hi on 3/20/2026.
//

#include "../Header/Bitmap.h"

Bitmap::Bitmap(uint64_t raw_bitmap)
    : raw_bitmap(raw_bitmap) {
}

void Bitmap::InsertBit(int index) {
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

Bitmap& Bitmap::operator+=(const Bitmap &other) {
    this->raw_bitmap += other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator+(const Bitmap &other) const {
    Bitmap result = *this;
    result += other;
    return result;
}

Bitmap& Bitmap::operator-=(const Bitmap &other) {
    this->raw_bitmap -= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator-(const Bitmap &other) const {
    Bitmap result = *this;
    result -= other;
    return result;
}

Bitmap& Bitmap::operator++() {
    ++this->raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator++(int) {
    Bitmap temp = *this;
    ++(*this);
    return temp;
}

Bitmap& Bitmap::operator--() {
    --this->raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator--(int) {
    Bitmap temp = *this;
    --(*this);
    return temp;
}

Bitmap& Bitmap::operator*=(const Bitmap &other) {
    this->raw_bitmap *= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator*(const Bitmap &other) const {
    Bitmap result = *this;
    result *= other;
    return result;
}

Bitmap& Bitmap::operator/=(const Bitmap &other) {
    this->raw_bitmap /= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator/(const Bitmap &other) const {
    Bitmap result = *this;
    result /= other;
    return result;
}

Bitmap& Bitmap::operator%=(const Bitmap &other) {
    this->raw_bitmap %= other.raw_bitmap;
    return *this;
}

Bitmap Bitmap::operator%(const Bitmap &other) const {
    Bitmap result = *this;
    result %= other;
    return result;
}
