/*
 * Copyright 2020 Aaron Barany
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Path.h"

#if VFC_WINDOWS
static const char pathSeps[] = "\\/";
static char prefPathSep = '\\';
#else
static const char pathSeps[] = "/";
static char prefPathSep = '/';
#endif

static bool isPathSep(char c)
{
	for (char s : pathSeps)
	{
		if (c == s)
			return true;
	}

	return false;
}

namespace path
{

bool isAbsolute(const std::string& path)
{
	if (path.empty())
		return false;

#if VFC_WINDOWS
	// e.g. C:
	if (path.size() >= 2 && path[1] == ':')
		return true;
#endif

	return isPathSep(path[0]);
}

std::string getFileName(const std::string& path)
{
	std::size_t sepPos = path.find_last_of(pathSeps);
	if (sepPos == std::string::npos)
		return path;

	return path.substr(sepPos + 1);
}

std::string getParentDirectory(const std::string& path)
{
	std::size_t sepPos = path.find_last_of(pathSeps);
	if (sepPos == std::string::npos)
		return std::string();

	if (sepPos == 0 && isPathSep(path[0]))
		return path.substr(0, 1);

#if VFC_WINDOWS
	// e.g. C:/
	if (sepPos <= 2 && path[1] == ':')
		return path.substr(0, sepPos + 1);
#endif

	return path.substr(0, sepPos);
}

std::string join(const std::string& left, const std::string& right)
{
	if (left.empty() || isAbsolute(right))
		return right;
	else if (right.empty())
		return left;

	std::string finalPath = left;
	if (!isPathSep(finalPath.back()))
		finalPath += prefPathSep;
	finalPath += right;
	return finalPath;
}

} // namespace path
