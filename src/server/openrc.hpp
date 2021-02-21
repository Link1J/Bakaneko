// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

// This file is for functions to operate openrc with out needing to have
// openrc installed.

#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <ljh/function_pointer.hpp>
#include <ljh/bitmask_operators.hpp>

const std::string RC_PREFIX             = "";
const std::string RC_SYSCONFDIR         = "/etc";
const std::string RC_LIBDIR             = "/lib/rc";
const std::string RC_LIBEXECDIR         = "/lib/rc";
const std::string RC_SVCDIR             = "/run/openrc";
const std::string RC_RUNLEVELDIR        = RC_SYSCONFDIR + "/runlevels";
const std::string RC_INITDIR            = RC_SYSCONFDIR + "/init.d";
const std::string RC_CONFDIR            = RC_SYSCONFDIR + "/conf.d";
const std::string RC_PLUGINDIR          = RC_LIBDIR + "/plugins";
const std::string RC_INIT_FIFO          = RC_SVCDIR + "/init.ctl";
const std::string RC_PROFILE_ENV        = RC_SYSCONFDIR + "/profile.env";
const std::string RC_SYS_WHITELIST      = RC_LIBEXECDIR + "/conf.d/env_whitelist";
const std::string RC_USR_WHITELIST      = RC_SYSCONFDIR + "/conf.d/env_whitelist";
const std::string RC_CONF               = RC_SYSCONFDIR + "/rc.conf";
const std::string RC_CONF_D             = RC_SYSCONFDIR + "/rc.conf.d";
const std::string RC_CONF_OLD           = RC_SYSCONFDIR + "/conf.d/rc";
const std::string RC_PATH_PREFIX        = RC_LIBEXECDIR + "/bin:/bin:/sbin:/usr/bin:/usr/sbin";
const std::string RC_PKG_PREFIX         = "/usr";
const std::string RC_PKG_INITDIR        = RC_PKG_PREFIX + "/etc/init.d";
const std::string RC_PKG_CONFDIR        = RC_PKG_PREFIX + "/etc/conf.d";
const std::string RC_LOCAL_PREFIX       = "/usr/local";
const std::string RC_LOCAL_INITDIR      = RC_LOCAL_PREFIX + "/etc/init.d";
const std::string RC_LOCAL_CONFDIR      = RC_LOCAL_PREFIX + "/etc/conf.d";
const std::string RC_LEVEL_SYSINIT      = "sysinit";
const std::string RC_LEVEL_SINGLE       = "single";
const std::string RC_LEVEL_SHUTDOWN     = "shutdown";
const std::string RC_SYS_DOCKER         = "DOCKER";
const std::string RC_SYS_JAIL           = "JAIL";
const std::string RC_SYS_NONE           = "";
const std::string RC_SYS_OPENVZ         = "OPENVZ";
const std::string RC_SYS_LXC            = "LXC";
const std::string RC_SYS_PREFIX         = "PREFIX";
const std::string RC_SYS_RKT            = "RKT";
const std::string RC_SYS_SYSTEMD_NSPAWN = "SYSTEMD-NSPAWN";
const std::string RC_SYS_UML            = "UML";
const std::string RC_SYS_VSERVER        = "VSERVER";
const std::string RC_SYS_XEN0           = "XEN0";
const std::string RC_SYS_XENU           = "XENU";

template<typename TYPE>
struct _i_TAILQ_ENTRY
{
    TYPE*  tqe_next;
    TYPE** tqe_prev;
};
template<typename TYPE>
struct _i_TAILQ_HEAD
{
    TYPE*  tqh_first;
    TYPE** tqh_last;
};
template<typename TYPE>
struct _i_LIST_ENTRY
{
    TYPE*  le_next;
    TYPE** le_prev;
};
template<typename TYPE>
struct _i_LIST_HEAD {
    TYPE* lh_first;
};

struct rc_string {
    char* value;
    _i_TAILQ_ENTRY<rc_string> entries;
};
using RC_STRING = rc_string;
using rc_stringlist = _i_TAILQ_HEAD<rc_string>;
using RC_STRINGLIST = rc_stringlist;

struct rc_pid
{
    pid_t pid;
    _i_LIST_ENTRY<rc_pid> entries;
};
using RC_PID = rc_pid;
using rc_pidlist = _i_LIST_HEAD<rc_pid>;
using RC_PIDLIST = rc_pidlist;

enum class RC_SERVICE
{
    STOPPED     = 0x0001,
    STARTED     = 0x0002,
    STOPPING    = 0x0004,
    STARTING    = 0x0008,
    INACTIVE    = 0x0010,
    HOTPLUGGED  = 0x0100,
    FAILED      = 0x0200,
    SCHEDULED   = 0x0400,
    WASINACTIVE = 0x0800,
    CRASHED     = 0x1000,
};

template<> struct ljh::bitmask_operators::enable<RC_SERVICE> : std::true_type {};

constexpr int RC_DEP_TRACE  = (1<<0);
constexpr int RC_DEP_STRICT = (1<<1);
constexpr int RC_DEP_START  = (1<<2);
constexpr int RC_DEP_STOP   = (1<<3);

using RC_DEPTREE = void*;

enum class RC_HOOK
{
    RUNLEVEL_STOP_IN   = 1,
    RUNLEVEL_STOP_OUT  = 4,
    RUNLEVEL_START_IN  = 5,
    RUNLEVEL_START_OUT = 8,
    ABORT              = 99,
    SERVICE_STOP_IN    = 101,
    SERVICE_STOP_NOW   = 102,
    SERVICE_STOP_DONE  = 103,
    SERVICE_STOP_OUT   = 104,
    SERVICE_START_IN   = 105,
    SERVICE_START_NOW  = 106,
    SERVICE_START_DONE = 107,
    SERVICE_START_OUT  = 108
};

inline auto rc_runlevel_get                 = ljh::load_function<char*         (                                                                               )>("librc.so", "rc_runlevel_get"                );
inline auto rc_runlevel_exists              = ljh::load_function<bool          (const char*                                                                    )>("librc.so", "rc_runlevel_exists"             );
inline auto rc_runlevel_stack               = ljh::load_function<bool          (const char*, const char*                                                       )>("librc.so", "rc_runlevel_stack"              );
inline auto rc_runlevel_unstack             = ljh::load_function<bool          (const char*, const char*                                                       )>("librc.so", "rc_runlevel_unstack"            );
inline auto rc_runlevel_stacks              = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_runlevel_stacks"             );
inline auto rc_runlevel_list                = ljh::load_function<RC_STRINGLIST*(                                                                               )>("librc.so", "rc_runlevel_list"               );
inline auto rc_runlevel_set                 = ljh::load_function<bool          (const char*                                                                    )>("librc.so", "rc_runlevel_set"                );
inline auto rc_runlevel_starting            = ljh::load_function<bool          (                                                                               )>("librc.so", "rc_runlevel_starting"           );
inline auto rc_runlevel_stopping            = ljh::load_function<bool          (                                                                               )>("librc.so", "rc_runlevel_stopping"           );
inline auto rc_service_add                  = ljh::load_function<bool          (const char*, const char*                                                       )>("librc.so", "rc_service_add"                 );
inline auto rc_service_delete               = ljh::load_function<bool          (const char*, const char*                                                       )>("librc.so", "rc_service_delete"              );
inline auto rc_service_daemon_set           = ljh::load_function<bool          (const char*, const char*, const char* const*, const char*, bool                )>("librc.so", "rc_service_daemon_set"          );
inline auto rc_service_description          = ljh::load_function<char*         (const char*, const char*                                                       )>("librc.so", "rc_service_description"         );
inline auto rc_service_exists               = ljh::load_function<bool          (const char*                                                                    )>("librc.so", "rc_service_exists"              );
inline auto rc_service_in_runlevel          = ljh::load_function<bool          (const char*, const char*                                                       )>("librc.so", "rc_service_in_runlevel"         );
inline auto rc_service_mark                 = ljh::load_function<bool          (const char*, RC_SERVICE                                                        )>("librc.so", "rc_service_mark"                );
inline auto rc_service_extra_commands       = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_service_extra_commands"      );
inline auto rc_service_resolve              = ljh::load_function<char*         (const char*                                                                    )>("librc.so", "rc_service_resolve"             );
inline auto rc_service_schedule_start       = ljh::load_function<bool          (const char*, const char*                                                       )>("librc.so", "rc_service_schedule_start"      );
inline auto rc_services_scheduled_by        = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_services_scheduled_by"       );
inline auto rc_service_schedule_clear       = ljh::load_function<bool          (const char*                                                                    )>("librc.so", "rc_service_schedule_clear"      );
inline auto rc_service_state                = ljh::load_function<RC_SERVICE    (const char*                                                                    )>("librc.so", "rc_service_state"               );
inline auto rc_service_started_daemon       = ljh::load_function<bool          (const char*, const char*,const char* const*, int                               )>("librc.so", "rc_service_started_daemon"      );
inline auto rc_service_value_get            = ljh::load_function<char*         (const char*, const char*                                                       )>("librc.so", "rc_service_value_get"           );
inline auto rc_service_value_set            = ljh::load_function<bool          (const char*, const char*, const char*                                          )>("librc.so", "rc_service_value_set"           );
inline auto rc_services_in_runlevel         = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_services_in_runlevel"        );
inline auto rc_services_in_runlevel_stacked = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_services_in_runlevel_stacked");
inline auto rc_services_in_state            = ljh::load_function<RC_STRINGLIST*(RC_SERVICE                                                                     )>("librc.so", "rc_services_in_state"           );
inline auto rc_services_scheduled           = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_services_scheduled"          );
inline auto rc_service_daemons_crashed      = ljh::load_function<bool          (const char*                                                                    )>("librc.so", "rc_service_daemons_crashed"     );
inline auto rc_sys                          = ljh::load_function<const char*   (                                                                               )>("librc.so", "rc_sys"                         );
inline auto rc_newer_than                   = ljh::load_function<bool          (const char*, const char*, time_t*, char*                                       )>("librc.so", "rc_newer_than"                  );
inline auto rc_older_than                   = ljh::load_function<bool          (const char*, const char*, time_t*, char*                                       )>("librc.so", "rc_older_than"                  );
inline auto rc_proc_getent                  = ljh::load_function<char*         (const char*                                                                    )>("librc.so", "rc_proc_getent"                 );
inline auto rc_deptree_update               = ljh::load_function<bool          (                                                                               )>("librc.so", "rc_deptree_update"              );
inline auto rc_deptree_update_needed        = ljh::load_function<bool          (time_t*, char*                                                                 )>("librc.so", "rc_deptree_update_needed"       );
inline auto rc_deptree_load                 = ljh::load_function<RC_DEPTREE*   (                                                                               )>("librc.so", "rc_deptree_load"                );
inline auto rc_deptree_load_file            = ljh::load_function<RC_DEPTREE*   (const char*                                                                    )>("librc.so", "rc_deptree_load_file"           );
inline auto rc_deptree_depend               = ljh::load_function<RC_STRINGLIST*(const RC_DEPTREE*, const char*, const char*                                    )>("librc.so", "rc_deptree_depend"              );
inline auto rc_deptree_depends              = ljh::load_function<RC_STRINGLIST*(const RC_DEPTREE*, const RC_STRINGLIST*, const RC_STRINGLIST*, const char*, int)>("librc.so", "rc_deptree_depends"             );
inline auto rc_deptree_order                = ljh::load_function<RC_STRINGLIST*(const RC_DEPTREE*, const char*, int                                            )>("librc.so", "rc_deptree_order"               );
inline auto rc_deptree_free                 = ljh::load_function<void          (RC_DEPTREE*                                                                    )>("librc.so", "rc_deptree_free"                );
inline auto rc_plugin_hook                  = ljh::load_function<int           (RC_HOOK, const char*                                                           )>("librc.so", "rc_plugin_hook"                 );
inline auto rc_config_list                  = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_config_list"                 );
inline auto rc_config_load                  = ljh::load_function<RC_STRINGLIST*(const char*                                                                    )>("librc.so", "rc_config_load"                 );
inline auto rc_config_value                 = ljh::load_function<char*         (RC_STRINGLIST*, const char*                                                    )>("librc.so", "rc_config_value"                );
inline auto rc_conf_value                   = ljh::load_function<char*         (const char*                                                                    )>("librc.so", "rc_conf_value"                  );
inline auto rc_yesno                        = ljh::load_function<bool          (const char*                                                                    )>("librc.so", "rc_yesno"                       );
inline auto rc_stringlist_new               = ljh::load_function<RC_STRINGLIST*(                                                                               )>("librc.so", "rc_stringlist_new"              );
inline auto rc_stringlist_add               = ljh::load_function<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )>("librc.so", "rc_stringlist_add"              );
inline auto rc_stringlist_addu              = ljh::load_function<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )>("librc.so", "rc_stringlist_addu"             );
inline auto rc_stringlist_delete            = ljh::load_function<bool          (RC_STRINGLIST*, const char*                                                    )>("librc.so", "rc_stringlist_delete"           );
inline auto rc_stringlist_find              = ljh::load_function<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )>("librc.so", "rc_stringlist_find"             );
inline auto rc_stringlist_split             = ljh::load_function<RC_STRINGLIST*(const char*, const char*                                                       )>("librc.so", "rc_stringlist_split"            );
inline auto rc_stringlist_sort              = ljh::load_function<void          (RC_STRINGLIST**                                                                )>("librc.so", "rc_stringlist_sort"             );
inline auto rc_stringlist_free              = ljh::load_function<void          (RC_STRINGLIST*                                                                 )>("librc.so", "rc_stringlist_free"             );
inline auto rc_find_pids                    = ljh::load_function<RC_PIDLIST*   (const char*, const char* const*, uid_t, pid_t                                  )>("librc.so", "rc_find_pids"                   );
inline auto rc_getfile                      = ljh::load_function<bool          (const char*, char**, size_t*                                                   )>("librc.so", "rc_getfile"                     );
inline auto rc_getline                      = ljh::load_function<ssize_t       (char**, size_t*, FILE*                                                         )>("librc.so", "rc_getline"                     );
