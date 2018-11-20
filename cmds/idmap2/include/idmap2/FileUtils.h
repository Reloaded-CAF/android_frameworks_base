/*
 * Copyright (C) 2018 The Android Open Source Project
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

#ifndef IDMAP2_INCLUDE_IDMAP2_FILEUTILS_H_
#define IDMAP2_INCLUDE_IDMAP2_FILEUTILS_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace android {
namespace idmap2 {
namespace utils {
typedef std::function<bool(unsigned char type /* DT_* from dirent.h */, const std::string& path)>
    FindFilesPredicate;
std::unique_ptr<std::vector<std::string>> FindFiles(const std::string& root, bool recurse,
                                                    const FindFilesPredicate& predicate);

std::unique_ptr<std::string> ReadFile(int fd);

std::unique_ptr<std::string> ReadFile(const std::string& path);

}  // namespace utils
}  // namespace idmap2
}  // namespace android

#endif  // IDMAP2_INCLUDE_IDMAP2_FILEUTILS_H_
