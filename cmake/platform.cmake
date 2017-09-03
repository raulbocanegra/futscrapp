# Set platform dependent properties
function(set_platform_properties)	
	if (UNIX)
		add_compile_options(
			-m64 # 64 bits build flag
		)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
		set(CMAKE_CXX_STANDARD 11) # C++11...
		set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
		set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
	elseif (WIN32 AND MSVC)
	    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_TOOLCHAIN_DIR}/vcpkg.cmake" CACHE PATH "vcpkg.cmake directory") 
		add_definitions(
			-DNOMINMAX # https://support.microsoft.com/en-gb/help/143208/prb-using-stl-in-windows-program-can-cause-min-max-conflicts
		)
		add_compile_options(
			/MP # https://msdn.microsoft.com/en-us/library/bb385193.aspx
		)  
	endif()	
endfunction()