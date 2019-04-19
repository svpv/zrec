// Copyright (c) 2018, 2019 Alexey Tourbin
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

#include <stdint.h>

#pragma GCC visibility push(hidden)

// A chunker tells you how to group records into chunks.
struct chunker *chunker_new(void);
void chunker_free(struct chunker *C);

// Feed a record to the chunker.  For the purpose of chunking, a record
// is represented with its nameHash (see a detailed comment in enc.h).
// Returns 0 when the current chunk is still being cooked.  Otherwise
// returns 2..8, indicating the number of records in the cooked-up chunk.
unsigned chunker_add(struct chunker *C, uint64_t nameHash);

// When all records are added, call flush repeatedly until it returns 0.
unsigned chunker_flush(struct chunker *C);

#pragma GCC visibility pop
