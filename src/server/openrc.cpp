// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include <ljh/system_info.hpp>

// This file is for functions to operate openrc with out needing to have
// openrc installed.

#if defined(LJH_TARGET_Linux)
#include "openrc.hpp"
#include <dlfcn.h>

static void* load_function(const char* name)
{
    static void* lib_handle = dlopen("rc.so", RTLD_NOW|RTLD_GLOBAL);
    if (lib_handle == nullptr) return nullptr;
    return dlsym(lib_handle, name);
}

ljh::function_pointer<char*         (                                                                               )> rc_runlevel_get                 = load_function("rc_runlevel_get"                );
ljh::function_pointer<bool          (const char*                                                                    )> rc_runlevel_exists              = load_function("rc_runlevel_exists"             );
ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_runlevel_stack               = load_function("rc_runlevel_stack"              );
ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_runlevel_unstack             = load_function("rc_runlevel_unstack"            );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_runlevel_stacks              = load_function("rc_runlevel_stacks"             );
ljh::function_pointer<RC_STRINGLIST*(                                                                               )> rc_runlevel_list                = load_function("rc_runlevel_list"               );
ljh::function_pointer<bool          (const char*                                                                    )> rc_runlevel_set                 = load_function("rc_runlevel_set"                );
ljh::function_pointer<bool          (                                                                               )> rc_runlevel_starting            = load_function("rc_runlevel_starting"           );
ljh::function_pointer<bool          (                                                                               )> rc_runlevel_stopping            = load_function("rc_runlevel_stopping"           );
ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_add                  = load_function("rc_service_add"                 );
ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_delete               = load_function("rc_service_delete"              );
ljh::function_pointer<bool          (const char*, const char*, const char* const*, const char*, bool                )> rc_service_daemon_set           = load_function("rc_service_daemon_set"          );
ljh::function_pointer<char*         (const char*, const char*                                                       )> rc_service_description          = load_function("rc_service_description"         );
ljh::function_pointer<bool          (const char*                                                                    )> rc_service_exists               = load_function("rc_service_exists"              );
ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_in_runlevel          = load_function("rc_service_in_runlevel"         );
ljh::function_pointer<bool          (const char*, RC_SERVICE                                                        )> rc_service_mark                 = load_function("rc_service_mark"                );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_service_extra_commands       = load_function("rc_service_extra_commands"      );
ljh::function_pointer<char*         (const char*                                                                    )> rc_service_resolve              = load_function("rc_service_resolve"             );
ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_schedule_start       = load_function("rc_service_schedule_start"      );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_scheduled_by        = load_function("rc_services_scheduled_by"       );
ljh::function_pointer<bool          (const char*                                                                    )> rc_service_schedule_clear       = load_function("rc_service_schedule_clear"      );
ljh::function_pointer<RC_SERVICE    (const char*                                                                    )> rc_service_state                = load_function("rc_service_state"               );
ljh::function_pointer<bool          (const char*, const char*,const char* const*, int                               )> rc_service_started_daemon       = load_function("rc_service_started_daemon"      );
ljh::function_pointer<char*         (const char*, const char*                                                       )> rc_service_value_get            = load_function("rc_service_value_get"           );
ljh::function_pointer<bool          (const char*, const char*, const char*                                          )> rc_service_value_set            = load_function("rc_service_value_set"           );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_in_runlevel         = load_function("rc_services_in_runlevel"        );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_in_runlevel_stacked = load_function("rc_services_in_runlevel_stacked");
ljh::function_pointer<RC_STRINGLIST*(RC_SERVICE                                                                     )> rc_services_in_state            = load_function("rc_services_in_state"           );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_scheduled           = load_function("rc_services_scheduled"          );
ljh::function_pointer<bool          (const char*                                                                    )> rc_service_daemons_crashed      = load_function("rc_service_daemons_crashed"     );
ljh::function_pointer<const char*   (                                                                               )> rc_sys                          = load_function("rc_sys"                         );
ljh::function_pointer<bool          (const char*, const char*, time_t*, char*                                       )> rc_newer_than                   = load_function("rc_newer_than"                  );
ljh::function_pointer<bool          (const char*, const char*, time_t*, char*                                       )> rc_older_than                   = load_function("rc_older_than"                  );
ljh::function_pointer<char*         (const char*                                                                    )> rc_proc_getent                  = load_function("rc_proc_getent"                 );
ljh::function_pointer<bool          (                                                                               )> rc_deptree_update               = load_function("rc_deptree_update"              );
ljh::function_pointer<bool          (time_t*, char*                                                                 )> rc_deptree_update_needed        = load_function("rc_deptree_update_needed"       );
ljh::function_pointer<RC_DEPTREE*   (                                                                               )> rc_deptree_load                 = load_function("rc_deptree_load"                );
ljh::function_pointer<RC_DEPTREE*   (const char*                                                                    )> rc_deptree_load_file            = load_function("rc_deptree_load_file"           );
ljh::function_pointer<RC_STRINGLIST*(const RC_DEPTREE*, const char*, const char*                                    )> rc_deptree_depend               = load_function("rc_deptree_depend"              );
ljh::function_pointer<RC_STRINGLIST*(const RC_DEPTREE*, const RC_STRINGLIST*, const RC_STRINGLIST*, const char*, int)> rc_deptree_depends              = load_function("rc_deptree_depends"             );
ljh::function_pointer<RC_STRINGLIST*(const RC_DEPTREE*, const char*, int                                            )> rc_deptree_order                = load_function("rc_deptree_order"               );
ljh::function_pointer<void          (RC_DEPTREE*                                                                    )> rc_deptree_free                 = load_function("rc_deptree_free"                );
ljh::function_pointer<int           (RC_HOOK, const char*                                                           )> rc_plugin_hook                  = load_function("rc_plugin_hook"                 );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_config_list                  = load_function("rc_config_list"                 );
ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_config_load                  = load_function("rc_config_load"                 );
ljh::function_pointer<char*         (RC_STRINGLIST*, const char*                                                    )> rc_config_value                 = load_function("rc_config_value"                );
ljh::function_pointer<char*         (const char*                                                                    )> rc_conf_value                   = load_function("rc_conf_value"                  );
ljh::function_pointer<bool          (const char*                                                                    )> rc_yesno                        = load_function("rc_yesno"                       );
ljh::function_pointer<RC_STRINGLIST*(                                                                               )> rc_stringlist_new               = load_function("rc_stringlist_new"              );
ljh::function_pointer<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_add               = load_function("rc_stringlist_add"              );
ljh::function_pointer<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_addu              = load_function("rc_stringlist_addu"             );
ljh::function_pointer<bool          (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_delete            = load_function("rc_stringlist_delete"           );
ljh::function_pointer<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_find              = load_function("rc_stringlist_find"             );
ljh::function_pointer<RC_STRINGLIST*(const char*, const char*                                                       )> rc_stringlist_split             = load_function("rc_stringlist_split"            );
ljh::function_pointer<void          (RC_STRINGLIST**                                                                )> rc_stringlist_sort              = load_function("rc_stringlist_sort"             );
ljh::function_pointer<void          (RC_STRINGLIST*                                                                 )> rc_stringlist_free              = load_function("rc_stringlist_free"             );
ljh::function_pointer<RC_PIDLIST*   (const char*, const char* const*, uid_t, pid_t                                  )> rc_find_pids                    = load_function("rc_find_pids"                   );
ljh::function_pointer<bool          (const char*, char**, size_t*                                                   )> rc_getfile                      = load_function("rc_getfile"                     );
ljh::function_pointer<ssize_t       (char**, size_t*, FILE*                                                         )> rc_getline                      = load_function("rc_getline"                     );

#endif