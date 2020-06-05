#pragma once

constexpr auto WINDOW_CONTEXT_MAIN = 1u;
constexpr auto WINDOW_CONTEXT_LOCKPANEL = 3u;

constexpr unsigned long long GUIMSG_HEALTH =				0x0000000000000001;
constexpr unsigned long long GUIMSG_CHANGEWEAPON =			0x0000000000000010;
constexpr unsigned long long GUIMSG_LOADING =				0x0000000000000100;
constexpr unsigned long long GUIMSG_TOGGLEMENU =			0x0000000000001000;
constexpr unsigned long long GUIMSG_TOGGLENIGHTVISIONHINT =	0x0000000000010000;
constexpr unsigned long long GUIMSG_HIDESHIPPANEL =			0x0000000000100000;
constexpr unsigned long long GUIMSG_ITEMACQUIRED =			0x0000000001000000;