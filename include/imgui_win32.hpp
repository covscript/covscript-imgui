#pragma once
/*
* Covariant Script ImGUI Extension Win32 Header
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
* Copyright (C) 2017-2021 Michael Lee(李登淳)
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/

#include <imgui.hpp>

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Win32
#include <windows.h>
#include <dwmapi.h>
#include <tchar.h>

namespace imgui_cs {
    RECT GetScreenRect(int screen)
	{
		DISPLAY_DEVICEW device;
		device.cb = sizeof(device);
		BOOL result = EnumDisplayDevicesW(NULL, screen, &device, 0);

		DEVMODEW device_mode;
		device_mode.dmSize = sizeof(device_mode);
		device_mode.dmDriverExtra = 0;
		result = EnumDisplaySettingsExW(device.DeviceName, ENUM_CURRENT_SETTINGS, &device_mode, 0);

		int x = device_mode.dmPosition.x;
		int y = device_mode.dmPosition.y;
		int width = device_mode.dmPelsWidth;
		int height = device_mode.dmPelsHeight;

		return { x, y, x + width, y + height };
	}

    int get_monitor_count()
    {
        return GetSystemMetrics(SM_CMONITORS);
    }

    int get_monitor_width(int monitor_id)
    {
        int count = GetSystemMetrics(SM_CMONITORS);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		RECT size = GetScreenRect(monitor_id);
		return size.right - size.left;
    }

    int get_monitor_height(int monitor_id)
    {
        int count = GetSystemMetrics(SM_CMONITORS);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		RECT size = GetScreenRect(monitor_id);
		return size.bottom - size.top;
    }
}