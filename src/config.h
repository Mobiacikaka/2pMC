/**
 * \file
 * \author
 * \copyright
 * \brief
 */ 

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstddef>
#include <limits>
#include <cstdint>

#define concat(a, b) a ## b

// Pruning Steps Parameter
const size_t kS = 10;

// Range of the Universe
const int16_t kMIN_INT16 = std::numeric_limits<int16_t>::min();
const int16_t kMAX_INT16 = std::numeric_limits<int16_t>::max();

const uint32_t kMIN_UINT16 = std::numeric_limits<uint32_t>::min();
const uint32_t kMAX_UINT16 = std::numeric_limits<uint32_t>::max();

const int32_t kMIN_INT32 = std::numeric_limits<int32_t>::min();
const int32_t kMAX_INT32 = std::numeric_limits<int32_t>::max();

const uint32_t kMIN_UINT32 = std::numeric_limits<uint32_t>::min();
const uint32_t kMAX_UINT32 = std::numeric_limits<uint32_t>::max();

const int64_t kMIN_INT64 = std::numeric_limits<int64_t>::min();
const int64_t kMAX_INT64 = std::numeric_limits<int64_t>::max();

const uint64_t kMIN_UINT64 = std::numeric_limits<uint64_t>::min();
const uint64_t kMAX_UINT64 = std::numeric_limits<uint64_t>::max();

const auto kA = kMIN_INT32;
const auto kB = kMAX_INT32;

#define random_range(min, max) \
    min + (rand() % static_cast<int>(max - min + 1))

const size_t kRANDOM_LIST_MAX_LENGTH = 10001;
const size_t kRANDOM_LIST_MIN_LENGTH = kRANDOM_LIST_MAX_LENGTH * 0.8;

#endif
