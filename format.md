# zrec file format

## Overview

| header             | dictionary       | chunks            |
|:------------------:|:----------------:|:-----------------:|
| basic info + index | compressed frame | compressed frames |

The dictionary is downloaded only once (until it gets changed).
The header is downloaded on each update.  Apart from basic information,
the header also works as a special *index* for the chunks: it carries
just enough hash material to compare and match chunks, so that only missing
chunks are downloaded on update.

## Header

| `struct header` | chunk hashes    | chunk sizes |
|:---------------:|:---------------:|:-----------:|
| 64 bytes        | 32-bit integers | FSE blob    |

The header section starts with the following fixed-size structure:
```c
struct header {
    char magic[4];          // magic string: '\0' 'Z' 'R' '\xEC'
    uint32_t comboSum;      // combined checksum (see below)
    uint32_t headerSum;     // header checksum
    uint32_t nbChunks;      // number of chunks
    uint32_t nbSuperChunks; // number of superchunks (see below)
    uint32_t fseBlobSize;   // size of FSE blob
    uint32_t dictFrameSum;  // dictionary frame checksum
    uint32_t dictFrameSize; // size of dictionary frame
};
```

Throughout the code, `uint32_t` integers are understood to be in native
byte order.  However, when written to disk, they are converted (if needed)
to little-endian byte order.

The fixed-size structure is followed by an array of hash values, further
subdivided into two different kinds:
```c
    uint32_t chunkHashes[nbChunks];
    unit32_t superChunkHashes[nbSuperChunks];
```

Thus each chunk gets its own 32-bit hash.  Since this is not enough for
reliable matching, chunks are loosely coupled into *superchunks*, and each
superchunk gets an additional (combined) hash.

Likewise, `struct header` provides three 32-bit checksums: one for the header,
another for the dictionary frame, and the combined checksum.  These three
checksums are derived from two 64-bit hash values as follows:
```c
    uint64_t dictID = hash64(dictFrame, dictFrameSize, SEED0);
    header.dictFrameSum = dictID;
    // Every field below headerSum must now be filled in;
    // struct header, hashes, and FSE blob got to be back-to-back in memory.
    uint64_t headerHash = hash64(&header.headerSum + 1, /* to the end of FSE blob */, SEED0);
    header.headerSum = headerHash >> 32;
    header.comboSum = headerHash ^ (dictID >> 32);
```

The two leading checksums, `comboSum` and `headerSum`, can be used for weak identification.
(That is, using them as a 64-bit hash should be quite enough to detect a change;
but such a hash would be too weak to identify many different versions.)
