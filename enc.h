// Copyright (c) 2018 Alexey Tourbin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
extern "C" {
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif

struct zrecEnc *zrecEnc_new(void);
void zrecEnc_free(struct zrecEnc *enc);

// After the encoder is created, the first thing to do is to load either
// a dictionary or an existing .zrec file (which contains a dictionary).
// In the latter case, the file also serves as a reference: chunks that
// turn out to be the same are copied without recompression.
bool zrecEnc_loadDict(struct zrecEnc *enc, const char *path, const char *err[2]);
bool zrecEnc_loadRef(struct zrecEnc *enc, const char *path, const char *err[2]);

// The recommended hash function to be used in conjunction with zrecEnc_addRec.
// Internally, it tail-calls t1ha2, with the dictionary ID as a seed.
uint64_t zrecEnc_hash64(struct zrecEnc *enc, const void *data, size_t size);

// Feed a record to the encoder.  The records should come in a well-defined
// order which exhibits local correlations (put simply, the records should be
// sorted by name).  The encoder will concatenate a few records and compress
// them as a single chunk.  Therefore, the records should be self-delimiting,
// or their individual sizes should be easily deducible (as is the case with
// rpm headers).  The hash value is used for the sole purpose of stable chunking
// (as opposed to fingerprinting / content identification, which is handled by
// a different hashing scheme not exposed in this API).  While it is okay
// to hash the whole record, better scenarios exist:  1) If the records have
// names, but are otherwise unrelated, e.g. source packages, the name should
// be hashed, without a version (provided that all or most names are unique).
// 2) Kindred records, such as subpackages built from the same source package,
// should come in a row and have the same hash value which indicates the source
// name, still without a version (the encoder will couple kindred records more
// aggressively).
bool zrecEnc_addRec(struct zrecEnc *enc, const void *data, size_t size,
		    uint64_t nameHash, const char *err[2]);

// After all the records are added, finalize the structure and write it to
// a file descriptor.  (The structure cannot be streamed / written gradually,
// because the size of its leading section, which contains metadata such as
// chunk sizes, is not known in advance.  The encoder may use a temporary file
// to accumulate compressed chunks which are then replicated on the descriptor.)
bool zrecEnc_writeAll(struct zrecEnc *enc, int fd, const char *err[2]);

#ifdef __cplusplus
}
#endif
