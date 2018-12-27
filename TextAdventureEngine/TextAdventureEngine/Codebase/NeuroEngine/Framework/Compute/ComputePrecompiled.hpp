#pragma once
#ifndef COMPUTEPRECOMPILED_H
#define COMPUTEPRECOMPILED_H

#define __CL_ENABLE_EXCEPTIONS
#if PLATFORM_OS == APPLE_OS
#include <OpenCL/cl.hpp> /* read cpp_wrapper_fix.txt */
#else
#include <CL/cl.hpp>
#endif

//=====================================================================================
#define ___device_p reinterpret_cast< cl::Device* >( GetDevice() )
#define ___ext_device_p reinterpret_cast< cl::Device* >( ComputeManager::Instance().GetDevice() )
#define ___device ( *___device_p )
#define ___context_p reinterpret_cast< cl::Context* >( GetContext() )
#define ___ext_context_p reinterpret_cast< cl::Context* >( ComputeManager::Instance().GetContext() )
#define ___context ( *___context_p )
#define ___kernel_p( ptr ) reinterpret_cast< cl::Kernel* >( ptr->Value0 )
#define ___kernel( ptr ) ( *___kernel_p( ptr ) )
#define ___program_p( ptr ) reinterpret_cast< cl::Program* >( ptr )
#define ___program( ptr ) ( *___program_p( ptr ) )
#define ___cqueue_p( ptr ) reinterpret_cast< cl::CommandQueue* >( ptr )
#define ___cqueue( ptr ) ( *___cqueue_p( ptr ) )
#define ___active_cqueue ___cqueue( GetActiveCQ() )
#define ___active_cqueue_p ___cqueue_p( GetActiveCQ() )
#define ___ext_active_cqueue ___cqueue( ComputeManager::Instance().GetActiveCQ() )
#define ___ext_active_cqueue_p ___cqueue_p( ComputeManager::Instance().GetActiveCQ() )
#define ___buffer_p( ptr ) reinterpret_cast< cl::Buffer* >( ptr )
#define ___buffer( ptr ) ( *___buffer_p( ptr ) )
#define ___ref_buffer_p( ptr ) reinterpret_cast< cl::Buffer* >( *ptr.Ptr() )
#define ___ref_buffer( ptr ) ( *___ref_buffer_p( ptr ) )

#endif//COMPUTEPRECOMPILED_H