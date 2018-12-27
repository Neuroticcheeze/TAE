#include "ComputeManager.hpp"
#include <Framework/Console/ConsoleManager.hpp>
#include <Framework/File/FileManager.hpp>
#include <Framework/Engine.hpp>
#include "ComputePrecompiled.hpp"

//=====================================================================================
void ComputeManager::Init()
{
}

//=====================================================================================
void ComputeManager::InitPost()
{
	PROFILE;

	std::vector< cl::Platform > platforms;
	cl::Platform::get( &platforms );

	cl_uint bestSpecs = 0;
	cl::Device bestDevice = cl::Device::getDefault();
	cl::Platform bestPlatform = cl::Platform::getDefault();

	ConsoleManager::Instance().Log( CString::Format( "OpenCL 2.1 \\[%u Platforms]", platforms.size() ) );
	for ( const cl::Platform & platform : platforms )
	{
		std::string pl_name;
		std::vector< cl::Device > devices;
		platform.getDevices( CL_DEVICE_TYPE_ALL, &devices );
		platform.getInfo( CL_PLATFORM_NAME, &pl_name );
		
		ConsoleManager::Instance().Whisper( CString::Format( "* Platform: %s \\[%u Devices]", pl_name.c_str(), devices.size() ) );
		
		for ( const cl::Device & device : devices )
		{
			cl_uint numComputeUnits;
			cl_uint numClockFreq;
			device.getInfo( CL_DEVICE_MAX_COMPUTE_UNITS, &numComputeUnits );
			device.getInfo( CL_DEVICE_MAX_CLOCK_FREQUENCY, &numClockFreq );

			cl_device_type dtype;
			std::string name, vendor;
			device.getInfo( CL_DEVICE_NAME, &name );
			device.getInfo( CL_DEVICE_VENDOR, &vendor );
			device.getInfo( CL_DEVICE_TYPE, &dtype );

			cl_bool available, hasCompiler;
			device.getInfo( CL_DEVICE_AVAILABLE, &available );
			device.getInfo( CL_DEVICE_COMPILER_AVAILABLE, &hasCompiler );

			ConsoleManager::Instance().Praise( CString::Format( "    Device: %s ( %s ) " FONTCOLOUR_TAG( 44BBAAFF ) "{ cu=%u, cl=%u }", name.c_str(), vendor.c_str(), numComputeUnits, numClockFreq ) );

			if ( dtype == CL_DEVICE_TYPE_GPU && available && hasCompiler )
			{
				cl_uint specs = numComputeUnits * numClockFreq;
				if ( specs >= bestSpecs )//>= instead of > because best devices are generally last.
				{
					bestSpecs = specs;
					bestDevice = device;
					bestPlatform = platform;
				}
			}
		}
	}

	std::string bestDeviceName;
	bestDevice.getInfo( CL_DEVICE_NAME, &bestDeviceName );
	ConsoleManager::Instance().Log( CString::Format( FONTCOLOUR_TAG( 33FF33FF ) "Chosen Compute Device: %s", bestDeviceName.c_str() ) );

	Engine & engine = Engine::Instance();


#if defined (__APPLE__)
	CGLContextObj kCGLContext = CGLGetCurrentContext();
	CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
	cl_context_properties props[] =
	{
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
		0
	};
	cxGPUContext = clCreateContext(props, 0, 0, NULL, NULL, &ciErrNum);
#else
#ifdef UNIX
	cl_context_properties props[] =
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform,
		0
	};
	cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
#else // Win32

	cl_context_properties contextProps[] =
	{
		CL_GL_CONTEXT_KHR,   (cl_context_properties)engine.GetGLContext(),
		CL_WGL_HDC_KHR,     (cl_context_properties)engine.GetHDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)bestPlatform(),
		0
	};
	m_Context = new cl::Context( bestDevice, contextProps );
#endif
#endif

	m_Device = new cl::Device( bestDevice );
	bestDevice.getInfo( CL_DEVICE_MAX_WORK_GROUP_SIZE, &m_DeviceInfo.MaxWorkGroupSize );

	Array< CString > incDirs;
	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE, "res/kernels/includes/" );
	incDirs.Append( FileManager::Instance().GetWorkingDirectory() );
	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );
	SetGlobalIncludeDirectories( incDirs );

	cl::STRING_CLASS extensions;
	bestPlatform.getInfo( CL_PLATFORM_EXTENSIONS, &extensions );


	m_Infos.SupportsGLSharing = extensions.find( 
#if PLATFORM_OS == APPLE_OS
		"cl_apple_gl_sharing"
#else
		"cl_khr_gl_sharing"
#endif
		, 0, 1 ) != cl::STRING_CLASS::npos;
}

//=====================================================================================
void ComputeManager::Finalise()
{
	// Destroy Command Queues
	while ( m_CommandQueues.Count() > 0 )
	{
		SetCommandQueue( CommandQueue( *m_CommandQueues.Last() ) );
		CqWait();
		DestroyCommandQueue( CommandQueue( *m_CommandQueues.Last() ) );
	}

	// Destroy Buffers
#ifdef DESTROY_BUFFERS
	while ( m_Buffers.Count() > 0 )
	{
		DestroyBuffer( Buffer( *m_Buffers.Last() ) );
	}
#endif//DESTROY_BUFFERS

	// Unload Kernels
	while ( m_ComputeKernels.GetEntries().Count() > 0 )
	{
		UnloadComputeKernel( ComputeKernel( m_ComputeKernels.GetEntries().Last()->Value ) );
	}

	// Unload Programs
	while ( m_ComputePrograms.GetEntries().Count() > 0 )
	{
		UnloadComputeProgram( ComputeProgram( m_ComputePrograms.GetEntries().Last()->Value ) );
	}
	
	delete ___context_p;
	m_Context = nullptr;

	delete ___device_p;
	m_Device = nullptr;
}

//=====================================================================================
void ComputeManager::SetGlobalIncludeDirectories( const Array< CString > & a_IncludeDirectories )
{
	auto it = a_IncludeDirectories.First();
	auto end = a_IncludeDirectories.End();

	m_InclDirsOption = "";
	while ( it != end )
	{
		m_InclDirsOption += " -I \"" + *it + "\"";
		++it;
	}
}

//=====================================================================================
ComputeManager::ComputeProgram ComputeManager::LoadComputeProgram( uint32_t a_HashName, const InitialiserList< CString > & a_SourcePaths, const InitialiserList< CString > & a_Macros, CompilerOptions a_CompilerOptions, const char * a_AdditionalOptions )
{
	if ( !m_ComputePrograms.Contains( a_HashName ) )
	{
		CString compilerOptions = a_AdditionalOptions;
		auto it = NBegin( a_Macros );
		const auto end = NEnd( a_Macros );
		while ( it != end )
		{
			compilerOptions += " -D \"" + *it + "\"";
			++it;
		}
		
		if ( a_CompilerOptions & CCO_SINGLE_PRECISION_CONST )	compilerOptions += " -cl-single-precision-constant";
		if ( a_CompilerOptions & CCO_DENORMS_ARE_ZERO )			compilerOptions += " -cl-denorms-are-zero";			
		if ( a_CompilerOptions & CCO_OPT_DISABLE )				compilerOptions += " -cl-opt-disable";				
		if ( a_CompilerOptions & CCO_STRICT_ALIASING )			compilerOptions += " -cl-strict-aliasing";			
		if ( a_CompilerOptions & CCO_MAD_ENABLE )				compilerOptions += " -cl-mad-enable";				
		if ( a_CompilerOptions & CCO_NO_SIGNED_ZEROS )			compilerOptions += " -cl-no-signed-zeros";			
		if ( a_CompilerOptions & CCO_UNSAFE_MATH_OPTS )			compilerOptions += " -cl-unsafe-math-optimizations";	
		if ( a_CompilerOptions & CCO_FINITE_MATH_ONLY )			compilerOptions += " -cl-finite-math-only";			
		if ( a_CompilerOptions & CCO_FAST_RELAXED_MATH )		compilerOptions += " -cl-fast-relaxed-math";			
		if ( a_CompilerOptions & CCO_INHIBIT_WARNINGS )			compilerOptions += " -w";							
		if ( a_CompilerOptions & CCO_WARNINGS_ARE_ERRORS )		compilerOptions += " -Werror";						

		compilerOptions += " " + m_InclDirsOption;

		// Interface with opencl api to create the cl program
		cl::Program::Sources sources;
		std::vector< cl::STRING_CLASS > tmp;

		FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE, "res/kernels/" );

		CString paths;

		auto sourcePathsIt = NBegin( a_SourcePaths );
		auto sourcePathsEnd = NEnd( a_SourcePaths );
		while ( sourcePathsIt != sourcePathsEnd )
		{
			paths += ";" + *sourcePathsIt;

			CString src = FileManager::Instance().ReadAll( sourcePathsIt->Get() );
			tmp.push_back( cl::STRING_CLASS( src.Get() ) );

			sources.push_back( { tmp.back().c_str(), tmp.back().length() } );
			
			//CString srcHomeDir = FileManager::Instance().GetWorkingDirectory( *sourcePathsIt );
			//int32_t lastSlash = srcHomeDir.FindLast( "/");
			//if ( lastSlash != -1 ) srcHomeDir = srcHomeDir.SubString( 0, lastSlash + 1 );
			//compilerOptions += " -I \"" + srcHomeDir + "\"";

			++sourcePathsIt;
		}

		cl::Program * prog = new cl::Program( ___context, sources );
		tmp.clear();

		try
		{
			prog->build( compilerOptions.Get() );
		}
		catch ( const cl::Error & e )
		{
			cl::STRING_CLASS err = prog->getBuildInfo< CL_PROGRAM_BUILD_LOG >( ___device );
			ASSERT_WARN( false, "%s", err.c_str() );

			delete prog;
			prog = nullptr;
		}

		if ( !ASSERT_WARN( prog, "Failed to load and build compute program [id=%u]", a_HashName, paths.Get() ) )
		{
			FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );
			return ComputeProgram();
		}

		// We succeeded

		m_ComputePrograms.Put( a_HashName, PComputeProgram( prog ) );

		ConsoleManager::Instance().Praise( CString::Format( "Loaded Compute Program: \"%s\"", paths.Get() ) );
	}
	
	FileManager::Instance().SetWorkingDirectory( FileManager::UserLocation::EXECUTABLE );
	return ComputeProgram( *m_ComputePrograms[ a_HashName ] );
}

//=====================================================================================
bool ComputeManager::UnloadComputeProgram( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_ComputePrograms.Contains( a_HashName ), "(%s) The compute program by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PComputeProgram & computeProg = *m_ComputePrograms[ a_HashName ];
		
		auto program = ___program_p( *computeProg.Ptr() );
		delete program;

		return m_ComputePrograms.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
bool ComputeManager::UnloadComputeProgram( const ComputeProgram & a_ComputeProgram )
{
	auto pair = m_ComputePrograms.GetEntries().Find< const ComputeProgram & >( []( const ComputeProgram & a_It, const Dictionary< uint32_t, PComputeProgram >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_ComputeProgram );
	
	if ( pair )
	{
		return UnloadComputeProgram( pair->Key );
	}

	return false;
}

//=====================================================================================
ComputeManager::ComputeProgram ComputeManager::GetComputeProgram( uint32_t a_HashName ) const
{
	PComputeProgram * ptr = m_ComputePrograms[ a_HashName ];
	return ptr ? ComputeProgram( *ptr ) : ComputeProgram();
}

//=====================================================================================
ComputeManager::ComputeKernel ComputeManager::LoadComputeKernel( const char * a_StrName, const ComputeProgram & a_Program )
{
	const uint32_t hashName = CRC32( a_StrName );
	if ( !m_ComputePrograms.Contains( hashName ) )
	{
		if ( !a_Program )
		{
			ASSERT_WARN( false, "Failed to load compute kernel [%s]", a_StrName );
			return ComputeKernel();
		}

		cl_uint kwgsize = 0;
		cl::Kernel * kernel = new cl::Kernel( ___program( *a_Program.Ptr() ), a_StrName );
		kernel->getWorkGroupInfo( ___device, CL_KERNEL_WORK_GROUP_SIZE, &kwgsize );

		m_ComputeKernels.Put( hashName, PComputeKernel( PComputeKernel::PtrType( kernel, static_cast< uint32_t >( kwgsize ) ) ) );

		ConsoleManager::Instance().Praise( CString::Format( "Loaded Compute Kernel: \"%s\"", a_StrName ) );
	}
	
	return ComputeKernel( *m_ComputeKernels[ hashName ] );
}

//=====================================================================================
bool ComputeManager::UnloadComputeKernel( uint32_t a_HashName )
{
	if ( ASSERT_WARN( m_ComputeKernels.Contains( a_HashName ), "(%s) The compute kernel by ID '%u' does not exist, ignoring.", __FUNCTION__, a_HashName ) )
	{
		PComputeKernel & computeKern = *m_ComputeKernels[ a_HashName ];
		
		auto kernel = ___kernel_p( computeKern.Ptr() );
		delete kernel;

		return m_ComputeKernels.Remove( a_HashName );
	}

	return false;
}

//=====================================================================================
bool ComputeManager::UnloadComputeKernel( const ComputeKernel & a_ComputeKernel )
{
	auto pair = m_ComputeKernels.GetEntries().Find< const ComputeKernel & >( []( const ComputeKernel & a_It, const Dictionary< uint32_t, PComputeKernel >::Pair & a_Pair ) {
		return a_Pair.Value == a_It;
	}, a_ComputeKernel );
	
	if ( pair )
	{
		return UnloadComputeKernel( pair->Key );
	}

	return false;
}

//=====================================================================================
ComputeManager::ComputeKernel ComputeManager::GetComputeKernel( uint32_t a_HashName ) const
{
	PComputeKernel * ptr = m_ComputeKernels[ a_HashName ];
	return ptr ? ComputeKernel( *ptr ) : ComputeKernel();
}

//=====================================================================================
void ComputeManager::SetComputeKernelArg( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, void * a_ArgPtr, size_t a_ArgSize )
{
	if ( a_ComputeKernel )
	{
		___kernel_p( a_ComputeKernel.Ptr() )->setArg( static_cast< cl_uint >( a_Index ), a_ArgSize, a_ArgPtr );
	}
}

//=====================================================================================
void ComputeManager::_Impl_SCKA_Buffer( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, const void * a_ArgBufferPtr )
{
	if ( a_ComputeKernel )
	{
		const Buffer& buffer = *static_cast< const Buffer* >( a_ArgBufferPtr );
		___kernel_p( a_ComputeKernel.Ptr() )->setArg( static_cast< cl_uint >( a_Index ), ___buffer( *buffer.Ptr() ) );
	}
}

//=====================================================================================
ComputeManager::Buffer ComputeManager::CreateBuffer( void * a_Data, uint32_t a_DataSize, MemoryFlags a_MemoryFlags )
{
	cl_mem_flags clFlags = 0;
	if ( a_MemoryFlags & MEM_READ_WRITE )		clFlags |= CL_MEM_READ_WRITE;
	if ( a_MemoryFlags & MEM_WRITE_ONLY )		clFlags |= CL_MEM_WRITE_ONLY;
	if ( a_MemoryFlags & MEM_READ_ONLY )		clFlags |= CL_MEM_READ_ONLY;
	if ( a_MemoryFlags & MEM_USE_HOST_PTR )		clFlags |= CL_MEM_USE_HOST_PTR;
	if ( a_MemoryFlags & MEM_ALLOC_HOST_PTR )	clFlags |= CL_MEM_ALLOC_HOST_PTR;
	if ( a_MemoryFlags & MEM_COPY_HOST_PTR )	clFlags |= CL_MEM_COPY_HOST_PTR;

	cl::Buffer * clbuffer = new cl::Buffer( ___context, clFlags, a_DataSize, a_Data );
	
	PBuffer ptr = PBuffer( clbuffer );
	m_Buffers.Append( ptr );

	return Buffer( ptr );
}

//=====================================================================================
void ComputeManager::DestroyBuffer( const Buffer & a_Buffer )
{
	if ( a_Buffer )
	{
		auto clbuffer = ___buffer_p( *a_Buffer.Ptr() );
		delete clbuffer;

		m_Buffers.Remove( a_Buffer );
	}
}

//=====================================================================================
ComputeManager::CommandQueue ComputeManager::CreateCommandQueue( CommandQueueProperties a_Properties )
{
	cl_command_queue_properties clFlags = 0;
	if ( a_Properties & CQ_PROFILING ) clFlags |= CL_QUEUE_PROFILING_ENABLE;
	if ( a_Properties & CQ_UNORDERED ) clFlags |= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;

	cl::CommandQueue * clCq = new cl::CommandQueue( ___context, clFlags );
	
	PCommandQueue ptr = PCommandQueue( clCq );
	m_CommandQueues.Append( ptr );

	return CommandQueue( ptr );
}

//=====================================================================================
void ComputeManager::DestroyCommandQueue( const CommandQueue & a_CommandQueue )
{
	if ( a_CommandQueue )
	{
		auto clcq = ___cqueue_p( *a_CommandQueue.Ptr() );
		delete clcq;

		m_CommandQueues.Remove( a_CommandQueue );
	}
}

//=====================================================================================
void ComputeManager::SetCommandQueue( const CommandQueue & a_CommandQueue )
{
	m_ActiveCommandQueue = a_CommandQueue;
}
//=====================================================================================
void ComputeManager::CqWriteBuffer( Buffer & a_Buffer, const void * a_BufferPtr, uint32_t a_BufferSize, uint32_t a_WriteOffset, bool a_Blocking )
{
	PROFILE;

	if ( !ASSERT( m_ActiveCommandQueue, "No active command queue, activate one with SetCommandQueue" ) )
	{
		return;
	}

	try 
	{
		___active_cqueue.enqueueWriteBuffer( ___buffer( *a_Buffer.Ptr() ), a_Blocking, a_WriteOffset, a_BufferSize, a_BufferPtr );
	}
	catch ( const cl::Error & error )
	{
		cl::STRING_CLASS errStr = error.what();
		ASSERT_WARN( false, "CL Error %d \"%s\"", error.err(), errStr.c_str() );
	}
}

//=====================================================================================
void ComputeManager::CqReadBuffer( Buffer & a_Buffer, void * o_BufferPtr, uint32_t a_BufferSize, uint32_t a_ReadOffset, bool a_Blocking )
{
	PROFILE;

	if ( !ASSERT( m_ActiveCommandQueue, "No active command queue, activate one with SetCommandQueue" ) )
	{
		return;
	}

	try 
	{
		___active_cqueue.enqueueReadBuffer( ___buffer( *a_Buffer.Ptr() ), a_Blocking, a_ReadOffset, a_BufferSize, o_BufferPtr );
	}
	catch ( const cl::Error & error )
	{
		cl::STRING_CLASS errStr = error.what();
		ASSERT_WARN( false, "CL Error %d \"%s\"", error.err(), errStr.c_str() );
	}
}

//=====================================================================================
void ComputeManager::CqDispatch( const ComputeKernel & a_ComputeKernel, const DispatchSize & a_TotalWorkSize, const DispatchSize & a_Offset )
{
	if ( !a_ComputeKernel )
	{
		return;
	}

	PROFILE;
	if ( !ASSERT( m_ActiveCommandQueue, "No active command queue, activate one with SetCommandQueue" ) )
	{
		return;
	}

	cl::NDRange offset = cl::NullRange;
	cl::NDRange local = cl::NullRange;
	cl::NDRange global = cl::NullRange;

	try 
	{
		switch ( a_TotalWorkSize.Dimensions )
		{
		case 1: 
			global = cl::NDRange( a_TotalWorkSize.Size[0] );
			offset = cl::NDRange( a_Offset.Size[0] );
			break;
		case 2: 
			global = cl::NDRange( a_TotalWorkSize.Size[0], a_TotalWorkSize.Size[1] );
			offset = cl::NDRange( a_Offset.Size[0], a_Offset.Size[1] );
			break;
		case 3: 
			global = cl::NDRange( a_TotalWorkSize.Size[0], a_TotalWorkSize.Size[1], a_TotalWorkSize.Size[2] );
			offset = cl::NDRange( a_Offset.Size[0], a_Offset.Size[1], a_Offset.Size[2] );
			break;
		default:
			ASSERT( false, "TotalWorkSize range has dimension of 0! This is illegal.." );
			break;
		}

		___active_cqueue.enqueueNDRangeKernel( ___kernel( a_ComputeKernel.Ptr() ), offset, global, cl::NullRange );
		
	}
	catch ( const cl::Error & error )
	{
		cl::STRING_CLASS errStr = error.what();
		ASSERT_WARN( false, "CL Error %d \"%s\"", error.err(), errStr.c_str() );
	}

	/*
	cl_uint kwgsize = a_ComputeKernel->Value1;

	try 
	{
		ASSERT( IsMultipleOf( a_TotalWorkSize.Size[ 0 ], kwgsize ) && 
			    IsMultipleOf( a_TotalWorkSize.Size[ 1 ], kwgsize ) && 
			    IsMultipleOf( a_TotalWorkSize.Size[ 2 ], kwgsize ),
				"" );

		switch ( a_TotalWorkSize.Dimensions )
		{
		case 1: 
			global = cl::NDRange( a_TotalWorkSize.Size[0] );
			local = cl::NDRange( kwgsize );
			break;
		case 2: 
			global = cl::NDRange( a_TotalWorkSize.Size[0], a_TotalWorkSize.Size[1] );
			local = cl::NDRange( kwgsize, kwgsize );
			break;
		case 3: 
			global = cl::NDRange( a_TotalWorkSize.Size[0], a_TotalWorkSize.Size[1], a_TotalWorkSize.Size[2] );
			local = cl::NDRange( kwgsize, kwgsize, kwgsize );
			break;
		default:
			ASSERT( false, "TotalWorkSize range has dimension of 0! This is illegal.." );
			break;
		}

		___active_cqueue.enqueueNDRangeKernel( ___kernel( a_ComputeKernel.Ptr() ), offset, global, local );
	}
	catch ( const cl::Error & error )
	{
		cl::STRING_CLASS errStr = error.what();
		ASSERT_WARN( false, "CL Error %d \"%s\"", error.err(), errStr.c_str() );
	}*/
}

//=====================================================================================
void ComputeManager::CqWait()
{
	PROFILE;
	if ( ASSERT( m_ActiveCommandQueue, "No active command queue, activate one with SetCommandQueue" ) )
	{
		___active_cqueue.finish();
	}
}