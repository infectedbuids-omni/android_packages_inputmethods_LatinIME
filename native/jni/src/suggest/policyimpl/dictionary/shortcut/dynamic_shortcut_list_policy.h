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

#ifndef LATINIME_DYNAMIC_SHORTCUT_LIST_POLICY_H
#define LATINIME_DYNAMIC_SHORTCUT_LIST_POLICY_H

#include <stdint.h>

#include "defines.h"
#include "suggest/core/policy/dictionary_shortcuts_structure_policy.h"
#include "suggest/policyimpl/dictionary/shortcut/shortcut_list_reading_utils.h"
#include "suggest/policyimpl/dictionary/utils/extendable_buffer.h"

namespace latinime {

/*
 * This is a dynamic version of ShortcutListPolicy and supports an additional buffer.
 */
class DynamicShortcutListPolicy : public DictionaryShortcutsStructurePolicy {
 public:
    DynamicShortcutListPolicy(const uint8_t *const shortcutBuf, const int bufSize,
            const ExtendableBuffer *const additionalBuffer)
            : mShortcutsBuf(shortcutBuf), mBufSize(bufSize), mAdditionalBuffer(additionalBuffer) {}

    ~DynamicShortcutListPolicy() {}

    int getStartPos(const int pos) const {
        if (pos == NOT_A_DICT_POS) {
            return NOT_A_DICT_POS;
        }
        return pos + ShortcutListReadingUtils::getShortcutListSizeFieldSize();
    }

    void getNextShortcut(const int maxCodePointCount, int *const outCodePoint,
            int *const outCodePointCount, bool *const outIsWhitelist, bool *const outHasNext,
            int *const pos) const {
        const bool usesAdditionalBuffer = *pos >= mBufSize;
        const uint8_t *const buffer = usesAdditionalBuffer
                ? mAdditionalBuffer->getBuffer() : mShortcutsBuf;
        if (usesAdditionalBuffer) {
            *pos -= mBufSize;
        }
        const ShortcutListReadingUtils::ShortcutFlags flags =
                ShortcutListReadingUtils::getFlagsAndForwardPointer(buffer, pos);
        if (outHasNext) {
            *outHasNext = ShortcutListReadingUtils::hasNext(flags);
        }
        if (outIsWhitelist) {
            *outIsWhitelist = ShortcutListReadingUtils::isWhitelist(flags);
        }
        if (outCodePoint) {
            *outCodePointCount = ShortcutListReadingUtils::readShortcutTarget(
                    buffer, maxCodePointCount, outCodePoint, pos);
        }
        if (usesAdditionalBuffer) {
            *pos += mBufSize;
        }
    }

    void skipAllShortcuts(int *const pos) const {
        if (*pos >= mBufSize) {
            *pos -= mBufSize;
            const int shortcutListSize = ShortcutListReadingUtils
                    ::getShortcutListSizeAndForwardPointer(mAdditionalBuffer->getBuffer(), pos);
            *pos += mBufSize + shortcutListSize;
        } else {
            const int shortcutListSize = ShortcutListReadingUtils
                    ::getShortcutListSizeAndForwardPointer(mShortcutsBuf, pos);
            *pos += shortcutListSize;
        }
    }

 private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(DynamicShortcutListPolicy);

    const uint8_t *const mShortcutsBuf;
    const int mBufSize;
    const ExtendableBuffer *const mAdditionalBuffer;
};
} // namespace latinime
#endif // LATINIME_DYNAMIC_SHORTCUT_LIST_POLICY_H