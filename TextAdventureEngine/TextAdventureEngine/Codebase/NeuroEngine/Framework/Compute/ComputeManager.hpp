#pragma once
#ifndef COMPUTEMANAGER_H
#define COMPUTEMANAGER_H

#include <Framework/Template/Manager.hpp>
#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Utils/InitialiserList.hpp>
#include <Framework/Containers/CString.hpp>

//=====================================================================================
class ComputeManager final : public Manager< ComputeManager >
{
public:

	struct DispatchSize final
	{
		uint32_t Dimensions = 0;
		uint32_t Size[ 3 ] { 0, 0, 0 };

		DispatchSize() {}
		DispatchSize( uint32_t a_Size0 ) : Dimensions( 1 ) { Size[ 0 ] = a_Size0; }
		DispatchSize( uint32_t a_Size0, uint32_t a_Size1 ) : Dimensions( 2 ) { Size[ 0 ] = a_Size0; Size[ 1 ] = a_Size1; }
		DispatchSize( uint32_t a_Size0, uint32_t a_Size1, uint32_t a_Size2 ) : Dimensions( 3 ) { Size[ 0 ] = a_Size0; Size[ 1 ] = a_Size1; Size[ 2 ] = a_Size2; }
	};

	ENUM( CompilerOptions, uint16_t )
		CCO_DEFAULT					= 0 << 0,
		CCO_SINGLE_PRECISION_CONST	= 1 << 0,	//-cl-single-precision-constant		// Treat double precision floating-point constant as single precision constant.
		CCO_DENORMS_ARE_ZERO		= 1 << 1,	//-cl-denorms-are-zero				// This option controls how single precision and double precision denormalized numbers are handled. If specified as a build option, the single precision denormalized numbers may be flushed to zero and if the optional extension for double precision is supported, double precision denormalized numbers may also be flushed to zero. This is intended to be a performance hint and the OpenCL compiler can choose not to flush denorms to zero if the device supports single precision (or double precision) denormalized numbers.
		CCO_OPT_DISABLE				= 1 << 2,	//-cl-opt-disable					// This option disables all optimizations. The default is optimizations are enabled.
		CCO_STRICT_ALIASING			= 1 << 3,	//-cl-strict-aliasing				// This option allows the compiler to assume the strictest aliasing rules.
		CCO_MAD_ENABLE				= 1 << 4,	//-cl-mad-enable					// Allow a * b + c to be replaced by a mad. The mad computes a * b + c with reduced accuracy. For example, some OpenCL devices implement mad as truncate the result of a * b before adding it to c.
		CCO_NO_SIGNED_ZEROS			= 1 << 5,	//-cl-no-signed-zeros				// Allow optimizations for floating-point arithmetic that ignore the signedness of zero. IEEE 754 arithmetic specifies the behavior of distinct +0.0 and -0.0 values, which then prohibits simplification of expressions such as x+0.0 or 0.0*x (even with -clfinite-math only). This option implies that the sign of a zero result isn't significant.
		CCO_UNSAFE_MATH_OPTS		= 1 << 6,	//-cl-unsafe-math-optimizations		// Allow optimizations for floating-point arithmetic that (a) assume that arguments and results are valid, (b) may violate IEEE 754 standard and (c) may violate the OpenCL numerical compliance requirements as defined in section 7.4 for single-precision floating-point, section 9.3.9 for double-precision floating-point, and edge case behavior in section 7.5. This option includes the -cl-no-signed-zeros and -cl-mad-enable options.
		CCO_FINITE_MATH_ONLY		= 1 << 7,	//-cl-finite-math-only				// Allow optimizations for floating-point arithmetic that assume that arguments and results are not NaNs or ±∞. This option may violate the OpenCL numerical compliance requirements defined in in section 7.4 for single-precision floating-point, section 9.3.9 for double-precision floating-point, and edge case behavior in section 7.5.
		CCO_FAST_RELAXED_MATH		= 1 << 8,	//-cl-fast-relaxed-math				// Sets the optimization options -cl-finite-math-only and -cl-unsafe-math-optimizations. This allows optimizations for floating-point arithmetic that may violate the IEEE 754 standard and the OpenCL numerical compliance requirements defined in the specification in section 7.4 for single-precision floating-point, section 9.3.9 for double-precision floating-point, and edge case behavior in section 7.5. This option causes the preprocessor macro __FAST_RELAXED_MATH__ to be defined in the OpenCL program.
		CCO_INHIBIT_WARNINGS		= 1 << 9,	//-w								// Inhibit all warning messages.
		CCO_WARNINGS_ARE_ERRORS		= 1 << 10,	//-Werror							// Make all warnings into errors.
	END_ENUM_F2( CompilerOptions, uint16_t, 11 );
	
	ENUM( MemoryFlags, uint8_t )
		MEM_DEFAULT			= 0 << 0,
		MEM_READ_WRITE		= 1 << 0,
		MEM_WRITE_ONLY		= 1 << 1,
		MEM_READ_ONLY		= 1 << 2,
		MEM_USE_HOST_PTR	= 1 << 3,
		MEM_ALLOC_HOST_PTR	= 1 << 4,
		MEM_COPY_HOST_PTR	= 1 << 5,
	END_ENUM_F2( MemoryFlags, uint8_t, 6 );

	ENUM( CommandQueueProperties, uint8_t )
		CQ_DEFAULT	 = 0 << 0,
		CQ_UNORDERED = 1 << 0,
		CQ_PROFILING = 1 << 1,
	END_ENUM_F2( CommandQueueProperties, uint8_t, 2 );

	typedef WeakPointer< void* > ComputeProgram;
	typedef WeakPointer< Tuple2< void*, uint32_t > > ComputeKernel;
	typedef WeakPointer< void* > CommandQueue;
	typedef WeakPointer< void* > Buffer;
	
	void Init() override;
	void InitPost() override;
	void Finalise() override;

	void SetGlobalIncludeDirectories( const Array< CString > & a_IncludeDirectories );

	ComputeProgram LoadComputeProgram( uint32_t a_HashName, const InitialiserList< CString > & a_SourcePaths, const InitialiserList< CString > & a_Macros = {}, CompilerOptions a_CompilerOptions = CCO_DEFAULT, const char * a_AdditionalOptions = "" );
	bool UnloadComputeProgram( uint32_t a_HashName );
	bool UnloadComputeProgram( const ComputeProgram & a_ComputeProgram );
	ComputeProgram GetComputeProgram( uint32_t a_HashName ) const;

	ComputeKernel LoadComputeKernel( const char * a_StrName, const ComputeProgram & a_Program );
	bool UnloadComputeKernel( uint32_t a_HashName );
	bool UnloadComputeKernel( const ComputeKernel & a_ComputeKernel );
	ComputeKernel GetComputeKernel( uint32_t a_HashName ) const;

	template< typename T >
	void SetComputeKernelArg( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, T && a_Arg ) { T k = a_Arg; SetComputeKernelArg( a_ComputeKernel, a_Index, &k, sizeof( T ) ); }
	template< typename T >
	void SetComputeKernelArg( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, T & a_Arg ) { SetComputeKernelArg( a_ComputeKernel, a_Index, &a_Arg, sizeof( T ) ); }
	void SetComputeKernelArg( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, void * a_ArgPtr, size_t a_ArgSize );
	template<> void SetComputeKernelArg< const Buffer >( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, const Buffer & a_ArgBuffer ) { _Impl_SCKA_Buffer( a_ComputeKernel, a_Index, static_cast< const void* >( &a_ArgBuffer ) ); }
	template<> void SetComputeKernelArg< Buffer >( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, Buffer & a_ArgBuffer ) { SetComputeKernelArg< const Buffer >( a_ComputeKernel, a_Index, a_ArgBuffer ); }

	Buffer CreateBuffer( void * a_Data, uint32_t a_DataSize, MemoryFlags a_MemoryFlags = MEM_DEFAULT );
	void DestroyBuffer( const Buffer & a_Buffer );

	CommandQueue CreateCommandQueue( CommandQueueProperties a_Properties = CQ_DEFAULT );
	void DestroyCommandQueue( const CommandQueue & a_CommandQueue );
	void SetCommandQueue( const CommandQueue & a_CommandQueue );

	// Commands
	void CqWriteBuffer( Buffer & a_Buffer, const void * a_BufferPtr, uint32_t a_BufferSize, uint32_t a_WriteOffset = 0, bool a_Blocking = true );
	void CqReadBuffer( Buffer & a_Buffer, void * o_BufferPtr, uint32_t a_BufferSize, uint32_t a_ReadOffset = 0, bool a_Blocking = true );
	void CqDispatch( const ComputeKernel & a_ComputeKernel, const DispatchSize & a_TotalWorkSize, const DispatchSize & a_Offset );
	void CqWait();
	
	uint32_t GetMaxWorkGroupSize() const { return m_DeviceInfo.MaxWorkGroupSize; }
	bool GetGraphicsBufferSupport() const { return m_Infos.SupportsGLSharing; }
	inline void* GetContext() const { return m_Context; }
	inline void* GetDevice() const { return m_Device; }
	inline void* GetActiveCQ() const { return *m_ActiveCommandQueue.Ptr(); }

private:

	void _Impl_SCKA_Buffer( const ComputeKernel & a_ComputeKernel, uint32_t a_Index, const void * a_ArgBufferPtr );

	CString m_InclDirsOption;

	void * m_Context = nullptr;
	void * m_Device = nullptr;
	struct {
		uint32_t MaxWorkGroupSize = 0;
	} m_DeviceInfo;
	struct {
		bool SupportsGLSharing = false;
	} m_Infos;

	typedef Pointer< void* > PComputeProgram;
	typedef Pointer< Tuple2< void*, uint32_t > > PComputeKernel;
	typedef Pointer< void* > PCommandQueue;
	typedef Pointer< void* > PBuffer;

	Dictionary< uint32_t, PComputeProgram > m_ComputePrograms;
	Dictionary< uint32_t, PComputeKernel > m_ComputeKernels;
	Array< PBuffer > m_Buffers;
	Array< PCommandQueue > m_CommandQueues;
	CommandQueue m_ActiveCommandQueue;
};

//=====================================================================================
#define _GetComputeProgram( HASHNAME ) ComputeManager::Instance().GetComputeProgram( CRC32( HASHNAME ) )
#define _GetComputeKernel( HASHNAME ) ComputeManager::Instance().GetComputeKernel( CRC32( HASHNAME ) )

#endif//COMPUTEMANAGER_H