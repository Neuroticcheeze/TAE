#pragma once
#ifndef PLATFORM_H
#define PLATFORM_H

//=====================================================================================
#pragma region Platform OS Switch

#define WINDOWS		0x01 //Microsoft Windows
#define APPLE_OS	0x02 //Apple OS
#define LINUX		0x04 //Linux
#define OTHER		0x00 //Unknown OS

#if defined( _WIN32 )
#define PLATFORM_OS WINDOWS
#elif defined( __MACH__ ) || defined( __OSX__ ) || defined( __APPLE__ )
#define PLATFORM_OS APPLE_OS
#elif defined( Linux ) || defined( __linux__ ) || defined( linux ) || defined( __linux )
#define PLATFORM_OS LINUX
#else
#define PLATFORM_OS OTHER
#endif

#pragma endregion

//=====================================================================================
#pragma region Graphics API Switch

#define DX_9 1 // DirectX 9
#define DX10 2 // DirectX 10
#define DX11 3 // DirectX 11
#define DX12 4 // DirectX 12
#define GL45 5 // OpenGL 4.5
#define VK10 6 // Vulkan 1.0
#define MNTL 7 // AMD Mantle

// Change this to compile with different graphics API
#define GRAPHICS_API DX12

#pragma endregion

//=====================================================================================
#pragma region Compute API Switch

#define CUDA 1 // nVIDIA CUDA
#define CL12 2 // OpenCL
#define VC10 3 // Vulkan Compute

// Change this to compile with different compute API
#define COMPUTE_API CL12

#pragma endregion

//=====================================================================================
#pragma region Compile Error Handling

// DirectX/Windows error check
#if PLATFORM_OS != WINDOWS && ( GRAPHICS_API == DX_9 || GRAPHICS_API == DX10 || GRAPHICS_API == DX11 || GRAPHICS_API == DX12 )
#error Only Windows platforms support DirectX!
#endif

// Only Windows is currently supported
#if PLATFORM_OS != WINDOWS
#error Only Windows platforms are supported at this time!
#endif

// Only DirectX 12 is currently supported
#if GRAPHICS_API != DX12
#error Only DirectX 12 is supported at this time!
#endif

#pragma endregion

#endif//PLATFORM_H