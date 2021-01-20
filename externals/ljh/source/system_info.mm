
//          Copyright Jared Irwin 2020-2021
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#if __APPLE__

#import <Foundation/Foundation.h>
#import <CoreServices/CoreServices.h>
#include "ljh/version.hpp"
#include <string>

#include <TargetConditionals.h>

ljh::version __os_info_get_version_obj_c()
{
	if ([[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersion)])
	{
		NSOperatingSystemVersion os = [[NSProcessInfo processInfo] operatingSystemVersion];
		return ljh::version{(ljh::version::value_type)os.majorVersion, (ljh::version::value_type)os.minorVersion, (ljh::version::value_type)os.patchVersion};
	}
	else
	{
		SInt32 major, minor, patch;
		Gestalt(gestaltSystemVersionMajor , &major);
		Gestalt(gestaltSystemVersionMinor , &minor);
		Gestalt(gestaltSystemVersionBugFix, &patch);
		return ljh::version{(ljh::version::value_type)major, (ljh::version::value_type)minor, (ljh::version::value_type)patch};
	}
}

std::string __os_info_get_string_obj_c()
{
	std::string string;
	if ([[NSProcessInfo processInfo] respondsToSelector:@selector(operatingSystemVersionString)])
	{
		NSString* text = [[NSProcessInfo processInfo] operatingSystemVersionString];
		string = {[text cStringUsingEncoding:NSUTF8StringEncoding], [text lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
	}
	return string;
}

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>

std::string __os_info_get_model_obj_c()
{
	std::string string;
	if ([[UIDevice currentDevice] respondsToSelector:@selector(model)])
	{
		NSString* text = [[UIDevice currentDevice] model];
		string = {[text cStringUsingEncoding:NSUTF8StringEncoding], [text lengthOfBytesUsingEncoding:NSUTF8StringEncoding]};
	}
	return string;
}
#endif
#endif