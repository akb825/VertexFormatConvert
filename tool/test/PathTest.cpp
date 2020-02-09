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
#include <gtest/gtest.h>

TEST(PathTest, IsAbsolute)
{
	EXPECT_FALSE(path::isAbsolute(""));
	EXPECT_FALSE(path::isAbsolute("foo"));
	EXPECT_TRUE(path::isAbsolute("/foo"));
#if VFC_WINDOWS
	EXPECT_TRUE(path::isAbsolute("C:\\foo"));
#endif
}

TEST(PathTest, GetFileName)
{
	EXPECT_EQ("", path::getFileName(""));
	EXPECT_EQ("foo", path::getFileName("foo"));
	EXPECT_EQ("foo", path::getFileName("/foo"));
	EXPECT_EQ("bar", path::getFileName("foo/bar"));
#if VFC_WINDOWS
	EXPECT_EQ("bar", path::getFileName("C:\\foo\\bar"));
#endif
	EXPECT_EQ("", path::getFileName("foo/"));
}

TEST(PathTest, getParentDirectory)
{
	EXPECT_EQ("", path::getParentDirectory(""));
	EXPECT_EQ("", path::getParentDirectory("foo"));
	EXPECT_EQ("foo", path::getParentDirectory("foo/"));
	EXPECT_EQ("foo", path::getParentDirectory("foo/bar"));
	EXPECT_EQ("foo/bar", path::getParentDirectory("foo/bar/baz"));
	EXPECT_EQ("/", path::getParentDirectory("/"));
	EXPECT_EQ("/", path::getParentDirectory("/foo"));
#if VFC_WINDOWS
	EXPECT_EQ("C:\\", path::getParentDirectory("C:\\"));
	EXPECT_EQ("C:\\", path::getParentDirectory("C:\\foo"));
#endif
}

TEST(PathTest, Join)
{
	EXPECT_EQ("", path::join("", ""));
	EXPECT_EQ("foo", path::join("foo", ""));
	EXPECT_EQ("bar", path::join("", "bar"));

#if VFC_WINDOWS
	EXPECT_EQ("foo\\bar", path::join("foo", "bar"));
	EXPECT_EQ("foo/bar", path::join("foo/", "bar"));
	EXPECT_EQ("foo\\bar", path::join("foo\\", "bar"));
	EXPECT_EQ("C:\\bar", path::join("foo", "C:\\bar"));
#else
	EXPECT_EQ("foo/bar", path::join("foo", "bar"));
	EXPECT_EQ("foo/bar", path::join("foo/", "bar"));
	EXPECT_EQ("/bar", path::join("foo", "/bar"));
#endif
}
