
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// function_traits.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++11
// 
// ABOUT
//     Adds function traits
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once

#include <type_traits>
#include <tuple>
#include <functional>
#include "cpp_version.hpp"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

#if defined(__clang__)
#define LJH_CALLING_CONVENTION_cdecl              __attribute__((cdecl             ))
#define LJH_CALLING_CONVENTION_stdcall            __attribute__((stdcall           ))
#define LJH_CALLING_CONVENTION_fastcall           __attribute__((fastcall          ))
#define LJH_CALLING_CONVENTION_vectorcall         __attribute__((vectorcall        ))
#define LJH_CALLING_CONVENTION_ms_abi             __attribute__((ms_abi            ))
#define LJH_CALLING_CONVENTION_preserve_all       __attribute__((preserve_all      ))
#define LJH_CALLING_CONVENTION_preserve_most      __attribute__((preserve_most     ))
#define LJH_CALLING_CONVENTION_regcall            __attribute__((regcall           ))
#define LJH_CALLING_CONVENTION_thiscall           __attribute__((thiscall          ))
#define LJH_CALLING_CONVENTION_aarch64_vector_pcs __attribute__((aarch64_vector_pcs))
#elif defined(__GNUC__)
#define LJH_CALLING_CONVENTION_cdecl              __attribute__((cdecl             ))
#define LJH_CALLING_CONVENTION_stdcall            __attribute__((stdcall           ))
#define LJH_CALLING_CONVENTION_fastcall           __attribute__((fastcall          ))
#define LJH_CALLING_CONVENTION_vectorcall         __attribute__((vectorcall        ))
#define LJH_CALLING_CONVENTION_preserve_all       __attribute__((preserve_all      ))
#define LJH_CALLING_CONVENTION_preserve_most      __attribute__((preserve_most     ))
#define LJH_CALLING_CONVENTION_regcall            __attribute__((regcall           ))
#define LJH_CALLING_CONVENTION_thiscall           __attribute__((thiscall          ))
#define LJH_CALLING_CONVENTION_aarch64_vector_pcs __attribute__((aarch64_vector_pcs))
#if !defined(_WIN32)
#undef  LJH_CALLING_CONVENTION_ms_abi             
#define LJH_CALLING_CONVENTION_ms_abi             __attribute__((cdecl             ))
#endif
#elif defined(_MSC_VER)
#define LJH_CALLING_CONVENTION_cdecl              __cdecl     
#define LJH_CALLING_CONVENTION_stdcall            __stdcall   
#define LJH_CALLING_CONVENTION_fastcall           __fastcall  
#define LJH_CALLING_CONVENTION_vectorcall         __vectorcall
#define LJH_CALLING_CONVENTION_ms_abi             __cdecl     
#define LJH_CALLING_CONVENTION_preserve_all       __cdecl     
#define LJH_CALLING_CONVENTION_preserve_most      __cdecl     
#define LJH_CALLING_CONVENTION_regcall            __cdecl     
#define LJH_CALLING_CONVENTION_thiscall           __thiscall  
#define LJH_CALLING_CONVENTION_aarch64_vector_pcs __cdecl     
#elif defined(__INTEL_COMPILER)
#define LJH_CALLING_CONVENTION_cdecl              __cdecl             
#define LJH_CALLING_CONVENTION_stdcall            __stdcall           
#define LJH_CALLING_CONVENTION_fastcall           __fastcall          
#define LJH_CALLING_CONVENTION_vectorcall         __vectorcall        
#define LJH_CALLING_CONVENTION_ms_abi             __ms_abi            
#define LJH_CALLING_CONVENTION_preserve_all       __preserve_all      
#define LJH_CALLING_CONVENTION_preserve_most      __preserve_most     
#define LJH_CALLING_CONVENTION_regcall            __regcall           
#define LJH_CALLING_CONVENTION_thiscall           __thiscall          
#define LJH_CALLING_CONVENTION_aarch64_vector_pcs __aarch64_vector_pcs
#else
#define LJH_CALLING_CONVENTION_cdecl              
#define LJH_CALLING_CONVENTION_stdcall            
#define LJH_CALLING_CONVENTION_fastcall           
#define LJH_CALLING_CONVENTION_vectorcall         
#define LJH_CALLING_CONVENTION_ms_abi             
#define LJH_CALLING_CONVENTION_preserve_all       
#define LJH_CALLING_CONVENTION_preserve_most      
#define LJH_CALLING_CONVENTION_regcall            
#define LJH_CALLING_CONVENTION_thiscall           
#define LJH_CALLING_CONVENTION_aarch64_vector_pcs 
#endif

#if defined(cdecl)
#undef cdecl
#endif

namespace ljh
{
	namespace calling_conventions
	{
		namespace cdecl_              { using type = void(LJH_CALLING_CONVENTION_cdecl             *)(); const int id = 0; }
		namespace stdcall_            { using type = void(LJH_CALLING_CONVENTION_stdcall           *)(); const int id = 1; }
		namespace fastcall_           { using type = void(LJH_CALLING_CONVENTION_fastcall          *)(); const int id = 2; }
		namespace vectorcall_         { using type = void(LJH_CALLING_CONVENTION_vectorcall        *)(); const int id = 3; }
		namespace ms_abi_             { using type = void(LJH_CALLING_CONVENTION_ms_abi            *)(); const int id = 4; }
		namespace preserve_all_       { using type = void(LJH_CALLING_CONVENTION_preserve_all      *)(); const int id = 5; }
		namespace preserve_most_      { using type = void(LJH_CALLING_CONVENTION_preserve_most     *)(); const int id = 6; }
		namespace regcall_            { using type = void(LJH_CALLING_CONVENTION_regcall           *)(); const int id = 7; }
		namespace thiscall_           { using type = void(LJH_CALLING_CONVENTION_thiscall          *)(); const int id = 8; }
		namespace aarch64_vector_pcs_ { using type = void(LJH_CALLING_CONVENTION_aarch64_vector_pcs*)(); const int id = 9; }
	};

	template<typename R, typename... Args>
	struct _function_traits_impl
	{
		using return_type                           = R;
		using argument_types                        = std::tuple<Args...>;
		static constexpr std::size_t argument_count = sizeof...(Args);
		template<std::size_t N> using argument_type = typename std::tuple_element<N, argument_types>::type;
	}; 

	template<typename F, int = 0> struct function_traits;

#define TRAIT_INTERALS(FUNCTION, CC, Noexcept, ArgList)\
	template<typename R, typename... Args>\
	struct function_traits<\
		FUNCTION,\
		std::is_same<calling_conventions::cdecl_::type, calling_conventions::CC##_::type>::value\
			? calling_conventions::CC##_::id \
			: 0 \
	> : _function_traits_impl<R, Args...>\
	{\
		struct as\
		{\
			using function         = R LJH_CALLING_CONVENTION_##CC ArgList LJH_NOEXCEPT_FUNCTION_TYPE(Noexcept);\
			using function_pointer = R (LJH_CALLING_CONVENTION_##CC*) ArgList LJH_NOEXCEPT_FUNCTION_TYPE(Noexcept);\
			using std_function     = std::function<function>;\
		};\
		struct is\
		{\
			constexpr static bool no_exceptions    = Noexcept;\
			constexpr static bool variadic         = !std::is_same<void(*) ArgList, void (*)(Args...)>::value;\
			constexpr static bool function         = std::is_same<FUNCTION, typename as::function        >::value;\
			constexpr static bool function_pointer = std::is_same<FUNCTION, typename as::function_pointer>::value;\
			constexpr static bool std_function     = std::is_same<FUNCTION, typename as::std_function    >::value;\
			constexpr static bool member_function  = false;\
			constexpr static bool constant         = false;\
		};\
		constexpr static int  calling_convention  = calling_conventions::CC##_::id;\
	}

	
#define TRAIT_INTERALS_MEMBER(FUNCTION, CC, Noexcept, ArgList, Const)\
	template<typename R, typename M, typename... Args>\
	struct function_traits<\
		FUNCTION,\
		std::is_same<calling_conventions::cdecl_::type, calling_conventions::CC##_::type>::value\
			? calling_conventions::CC##_::id \
			: 0 \
	> : _function_traits_impl<R, Args...>\
	{\
		struct as\
		{\
			using function_pointer = R (LJH_CALLING_CONVENTION_##CC M::*) ArgList LJH_NOEXCEPT_FUNCTION_TYPE(Noexcept);\
			using std_function     = std::function<function_pointer>;\
		};\
		struct is\
		{\
			constexpr static bool no_exceptions    = Noexcept;\
			constexpr static bool variadic         = !std::is_same<void(*) ArgList, void (*)(Args...)>::value;\
			constexpr static bool function         = false;\
			constexpr static bool function_pointer = std::is_same<FUNCTION, typename as::function_pointer>::value;\
			constexpr static bool std_function     = std::is_same<FUNCTION, typename as::std_function    >::value;\
			constexpr static bool member_function  = true;\
			constexpr static bool constant         = Const;\
		};\
		constexpr static int calling_convention  = calling_conventions::CC##_::id;\
	}
	
#define MAKE_ALL_TRAITS(CC, Noexcept, ArgList)\
		TRAIT_INTERALS       (              R (LJH_CALLING_CONVENTION_##CC    *) ArgList       noexcept(Noexcept) , CC, Noexcept, ArgList       );\
		TRAIT_INTERALS       (              R  LJH_CALLING_CONVENTION_##CC       ArgList       noexcept(Noexcept) , CC, Noexcept, ArgList       );\
		TRAIT_INTERALS       (std::function<R  LJH_CALLING_CONVENTION_##CC       ArgList       noexcept(Noexcept)>, CC, Noexcept, ArgList       );\
		TRAIT_INTERALS_MEMBER(              R (LJH_CALLING_CONVENTION_##CC M::*) ArgList       noexcept(Noexcept) , CC, Noexcept, ArgList, false);\
		TRAIT_INTERALS_MEMBER(              R (LJH_CALLING_CONVENTION_##CC M::*) ArgList const noexcept(Noexcept) , CC, Noexcept, ArgList, true );\
		TRAIT_INTERALS_MEMBER(std::function<R (LJH_CALLING_CONVENTION_##CC M::*) ArgList       noexcept(Noexcept)>, CC, Noexcept, ArgList, false);\
		TRAIT_INTERALS_MEMBER(std::function<R (LJH_CALLING_CONVENTION_##CC M::*) ArgList const noexcept(Noexcept)>, CC, Noexcept, ArgList, true )

#define MAKE_ALL_TRAITS_M(CC, Noexcept, ArgList)\
		TRAIT_INTERALS_MEMBER(              R (LJH_CALLING_CONVENTION_##CC M::*) ArgList       noexcept(Noexcept) , CC, Noexcept, ArgList, false);\
		TRAIT_INTERALS_MEMBER(              R (LJH_CALLING_CONVENTION_##CC M::*) ArgList const noexcept(Noexcept) , CC, Noexcept, ArgList, true );\
		TRAIT_INTERALS_MEMBER(std::function<R (LJH_CALLING_CONVENTION_##CC M::*) ArgList       noexcept(Noexcept)>, CC, Noexcept, ArgList, false);\
		TRAIT_INTERALS_MEMBER(std::function<R (LJH_CALLING_CONVENTION_##CC M::*) ArgList const noexcept(Noexcept)>, CC, Noexcept, ArgList, true )

#if __cpp_noexcept_function_type >= 201510L
#define MAKE_TRAITS(CC, ArgList)\
		MAKE_ALL_TRAITS(CC, false, ArgList);\
		MAKE_ALL_TRAITS(CC, true , ArgList)
#else
#define MAKE_TRAITS(CC, ArgList)\
		MAKE_ALL_TRAITS(CC, false, ArgList)
#endif
#if __cpp_noexcept_function_type >= 201510L
#define MAKE_TRAITS_M(CC, ArgList)\
		MAKE_ALL_TRAITS_M(CC, false, ArgList);\
		MAKE_ALL_TRAITS_M(CC, true , ArgList)
#else
#define MAKE_TRAITS_M(CC, ArgList)\
		MAKE_ALL_TRAITS_M(CC, false, ArgList)
#endif

	MAKE_TRAITS  (cdecl             , (Args...)     );
	MAKE_TRAITS  (cdecl             , (Args..., ...));
	MAKE_TRAITS  (stdcall           , (Args...)     );
//	MAKE_TRAITS  (stdcall           , (Args..., ...));
	MAKE_TRAITS  (fastcall          , (Args...)     );
//	MAKE_TRAITS  (fastcall          , (Args..., ...));
	MAKE_TRAITS  (vectorcall        , (Args...)     );
//	MAKE_TRAITS  (vectorcall        , (Args..., ...));
	MAKE_TRAITS  (regcall           , (Args...)     );
//	MAKE_TRAITS  (regcall           , (Args..., ...));
	MAKE_TRAITS  (ms_abi            , (Args...)     );
	MAKE_TRAITS  (ms_abi            , (Args..., ...));
	MAKE_TRAITS  (preserve_all      , (Args...)     );
	MAKE_TRAITS  (preserve_all      , (Args..., ...));
	MAKE_TRAITS  (preserve_most     , (Args...)     );
	MAKE_TRAITS  (preserve_most     , (Args..., ...));
	MAKE_TRAITS_M(thiscall          , (Args...)     );
//	MAKE_TRAITS_M(thiscall          , (Args..., ...));
	MAKE_TRAITS  (aarch64_vector_pcs, (Args...)     );
	MAKE_TRAITS  (aarch64_vector_pcs, (Args..., ...));

#undef MAKE_TRAITS
#undef MAKE_ALL_TRAITS
#undef TRAIT_INTERALS
#undef TRAIT_INTERALS_MEMBER
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
