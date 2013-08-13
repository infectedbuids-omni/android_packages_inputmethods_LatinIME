/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LATINIME_SHORTCUT_LIST_READING_UTILS_H
#define LATINIME_SHORTCUT_LIST_READING_UTILS_H

#include <stdint.h>

#include "defines.h"
#include "suggest/core/dictionary/byte_array_utils.h"

namespace latinime {

class ShortcutListReadingUtils {
 public:
    typedef uint8_t ShortcutFlags;

    static AK_FORCE_INLINE ShortcutFlags getFlagsAndForwardPointer(
            const uint8_t *const dictRoot, int *const pos) {
        return ByteArrayUtils::readUint8AndAdvancePosition(dictRoot, pos);
    }

    static AK_FORCE_INLINE int getProbabilityFromFlags(const ShortcutFlags flags) {
        return flags & MASK_ATTRIBUTE_PROBABILITY;
    }

    static AK_FORCE_INLINE bool hasNext(const ShortcutFlags flags) {
        return (flags & FLAG_ATTRIBUTE_HAS_NEXT) != 0;
    }

    // This method returns the size of the shortcut list region excluding the shortcut list size
    // field at the beginning.
    static AK_FORCE_INLINE int getShortcutListSizeAndForwardPointer(
            const uint8_t *const dictRoot, int *const pos) {
        // readUint16andAdvancePosition() returns an offset *including* the uint16 field itself.
        return ByteArrayUtils::readUint16AndAdvancePosition(dictRoot, pos)
                - SHORTCUT_LIST_SIZE_FIELD_SIZE;
    }

    static AK_FORCE_INLINE void skipShortcuts(const uint8_t *const dictRoot, int *const pos) {
        const int shortcutListSize = getShortcutListSizeAndForwardPointer(dictRoot, pos);
        *pos += shortcutListSize;
    }

    static AK_FORCE_INLINE bool isWhitelist(const ShortcutFlags flags) {
        return getProbabilityFromFlags(flags) == WHITELIST_SHORTCUT_PROBABILITY;
    }

    static AK_FORCE_INLINE int readShortcutTarget(
            const uint8_t *const dictRoot, const int maxLength,  int *const outWord,
            int *const pos) {
        return ByteArrayUtils::readStringAndAdvancePosition(dictRoot, maxLength, outWord, pos);
    }

 private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(ShortcutListReadingUtils);

    static const ShortcutFlags FLAG_ATTRIBUTE_HAS_NEXT;
    static const ShortcutFlags MASK_ATTRIBUTE_PROBABILITY;
    static const int SHORTCUT_LIST_SIZE_FIELD_SIZE;
    static const int WHITELIST_SHORTCUT_PROBABILITY;
};
} // namespace latinime
#endif // LATINIME_SHORTCUT_LIST_READING_UTILS_H