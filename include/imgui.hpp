#pragma once
/*
* Covariant Script ImGUI Extension Header
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
*
* Copyright (C) 2017-2024 Michael Lee(李登淳)
*
* Email:   mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: https://covscript.org.cn
*/

// Covariant Script Header
#include <covscript/dll.hpp>
#include <covscript/cni.hpp>

namespace imgui_cs {
	struct font {
		const char *name;
		const char *data;

		font(const char *n, const char *d) : name(n), data(d) {}
	};

	const char *get_default_font_data();
}