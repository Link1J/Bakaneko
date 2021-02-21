// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

// This file is for functions to operate openrc with out needing to have
// openrc installed.

#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <ljh/function_pointer.hpp>

constexpr std::string RC_PREFIX             = "";
constexpr std::string RC_SYSCONFDIR         = "/etc";
constexpr std::string RC_LIBDIR             = "/lib/rc";
constexpr std::string RC_LIBEXECDIR         = "/lib/rc";
constexpr std::string RC_SVCDIR             = "/run/openrc";
constexpr std::string RC_RUNLEVELDIR        = RC_SYSCONFDIR + "/runlevels";
constexpr std::string RC_INITDIR            = RC_SYSCONFDIR + "/init.d";
constexpr std::string RC_CONFDIR            = RC_SYSCONFDIR + "/conf.d";
constexpr std::string RC_PLUGINDIR          = RC_LIBDIR + "/plugins";
constexpr std::string RC_INIT_FIFO          = RC_SVCDIR + "/init.ctl";
constexpr std::string RC_PROFILE_ENV        = RC_SYSCONFDIR + "/profile.env";
constexpr std::string RC_SYS_WHITELIST      = RC_LIBEXECDIR + "/conf.d/env_whitelist";
constexpr std::string RC_USR_WHITELIST      = RC_SYSCONFDIR + "/conf.d/env_whitelist";
constexpr std::string RC_CONF               = RC_SYSCONFDIR + "/rc.conf";
constexpr std::string RC_CONF_D             = RC_SYSCONFDIR + "/rc.conf.d";
constexpr std::string RC_CONF_OLD           = RC_SYSCONFDIR + "/conf.d/rc";
constexpr std::string RC_PATH_PREFIX        = RC_LIBEXECDIR + "/bin:/bin:/sbin:/usr/bin:/usr/sbin";
constexpr std::string RC_PKG_PREFIX         = "/usr";
constexpr std::string RC_PKG_INITDIR        = RC_PKG_PREFIX + "/etc/init.d";
constexpr std::string RC_PKG_CONFDIR        = RC_PKG_PREFIX + "/etc/conf.d";
constexpr std::string RC_LOCAL_PREFIX       = "/usr/local";
constexpr std::string RC_LOCAL_INITDIR      = RC_LOCAL_PREFIX + "/etc/init.d";
constexpr std::string RC_LOCAL_CONFDIR      = RC_LOCAL_PREFIX + "/etc/conf.d";
constexpr std::string RC_LEVEL_SYSINIT      = "sysinit";
constexpr std::string RC_LEVEL_SINGLE       = "single";
constexpr std::string RC_LEVEL_SHUTDOWN     = "shutdown";
constexpr std::string RC_SYS_DOCKER         = "DOCKER";
constexpr std::string RC_SYS_JAIL           = "JAIL";
constexpr std::string RC_SYS_NONE           = "";
constexpr std::string RC_SYS_OPENVZ         = "OPENVZ";
constexpr std::string RC_SYS_LXC            = "LXC";
constexpr std::string RC_SYS_PREFIX         = "PREFIX";
constexpr std::string RC_SYS_RKT            = "RKT";
constexpr std::string RC_SYS_SYSTEMD_NSPAWN = "SYSTEMD-NSPAWN";
constexpr std::string RC_SYS_UML            = "UML";
constexpr std::string RC_SYS_VSERVER        = "VSERVER";
constexpr std::string RC_SYS_XEN0           = "XEN0";
constexpr std::string RC_SYS_XENU           = "XENU";

template<typename TYPE>
struct TAILQ_ENTRY
{
    TYPE*  tqe_next;
    TYPE** tqe_prev;
};
template<typename TYPE>
struct TAILQ_HEAD
{
    TYPE*  tqe_next;
    TYPE** tqh_last;
};
template<typename TYPE>
struct LIST_ENTRY
{
    TYPE*  le_next;
    TYPE** le_prev;
};
template<typename TYPE>
struct LIST_HEAD {
    TYPE* lh_first;
}

struct rc_string {
    char* value;
    TAILQ_ENTRY<rc_string> entries;
};
using RC_STRING = rc_string;
using rc_stringlist = TAILQ_HEAD<rc_string>;
using RC_STRINGLIST = rc_stringlist;

struct rc_pid
{
    pid_t pid;
    LIST_ENTRY<rc_pid> entries;
};
using RC_PID = rc_pid;
using rc_pidlist = LIST_HEAD<rc_pid>;
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

extern ljh::function_pointer<char*         (                                                                               )> rc_runlevel_get                ;
extern ljh::function_pointer<bool          (const char*                                                                    )> rc_runlevel_exists             ;
extern ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_runlevel_stack              ;
extern ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_runlevel_unstack            ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_runlevel_stacks             ;
extern ljh::function_pointer<RC_STRINGLIST*(                                                                               )> rc_runlevel_list               ;
extern ljh::function_pointer<bool          (const char*                                                                    )> rc_runlevel_set                ;
extern ljh::function_pointer<bool          (                                                                               )> rc_runlevel_starting           ;
extern ljh::function_pointer<bool          (                                                                               )> rc_runlevel_stopping           ;
extern ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_add                 ;
extern ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_delete              ;
extern ljh::function_pointer<bool          (const char*, const char*, const char* const*, const char*, bool                )> rc_service_daemon_set          ;
extern ljh::function_pointer<char*         (const char*, const char*                                                       )> rc_service_description         ;
extern ljh::function_pointer<bool          (const char*                                                                    )> rc_service_exists              ;
extern ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_in_runlevel         ;
extern ljh::function_pointer<bool          (const char*, RC_SERVICE                                                        )> rc_service_mark                ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_service_extra_commands      ;
extern ljh::function_pointer<char*         (const char*                                                                    )> rc_service_resolve             ;
extern ljh::function_pointer<bool          (const char*, const char*                                                       )> rc_service_schedule_start      ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_scheduled_by       ;
extern ljh::function_pointer<bool          (const char*                                                                    )> rc_service_schedule_clear      ;
extern ljh::function_pointer<RC_SERVICE    (const char*                                                                    )> rc_service_state               ;
extern ljh::function_pointer<bool          (const char*, const char*,const char* const*, int                               )> rc_service_started_daemon      ;
extern ljh::function_pointer<char*         (const char*, const char*                                                       )> rc_service_value_get           ;
extern ljh::function_pointer<bool          (const char*, const char*, const char*                                          )> rc_service_value_set           ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_in_runlevel        ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_in_runlevel_stacked;
extern ljh::function_pointer<RC_STRINGLIST*(RC_SERVICE                                                                     )> rc_services_in_state           ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_services_scheduled          ;
extern ljh::function_pointer<bool          (const char*                                                                    )> rc_service_daemons_crashed     ;
extern ljh::function_pointer<const char*   (                                                                               )> rc_sys                         ;
extern ljh::function_pointer<bool          (const char*, const char*, time_t*, char*                                       )> rc_newer_than                  ;
extern ljh::function_pointer<bool          (const char*, const char*, time_t*, char*                                       )> rc_older_than                  ;
extern ljh::function_pointer<char*         (const char*                                                                    )> rc_proc_getent                 ;
extern ljh::function_pointer<bool          (                                                                               )> rc_deptree_update              ;
extern ljh::function_pointer<bool          (time_t*, char*                                                                 )> rc_deptree_update_needed       ;
extern ljh::function_pointer<RC_DEPTREE*   (                                                                               )> rc_deptree_load                ;
extern ljh::function_pointer<RC_DEPTREE*   (const char*                                                                    )> rc_deptree_load_file           ;
extern ljh::function_pointer<RC_STRINGLIST*(const RC_DEPTREE*, const char*, const char*                                    )> rc_deptree_depend              ;
extern ljh::function_pointer<RC_STRINGLIST*(const RC_DEPTREE*, const RC_STRINGLIST*, const RC_STRINGLIST*, const char*, int)> rc_deptree_depends             ;
extern ljh::function_pointer<RC_STRINGLIST*(const RC_DEPTREE*, const char*, int                                            )> rc_deptree_order               ;
extern ljh::function_pointer<void          (RC_DEPTREE*                                                                    )> rc_deptree_free                ;
extern ljh::function_pointer<int           (RC_HOOK, const char*                                                           )> rc_plugin_hook                 ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_config_list                 ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*                                                                    )> rc_config_load                 ;
extern ljh::function_pointer<char*         (RC_STRINGLIST*, const char*                                                    )> rc_config_value                ;
extern ljh::function_pointer<char*         (const char*                                                                    )> rc_conf_value                  ;
extern ljh::function_pointer<bool          (const char*                                                                    )> rc_yesno                       ;
extern ljh::function_pointer<RC_STRINGLIST*(                                                                               )> rc_stringlist_new              ;
extern ljh::function_pointer<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_add              ;
extern ljh::function_pointer<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_addu             ;
extern ljh::function_pointer<bool          (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_delete           ;
extern ljh::function_pointer<RC_STRING*    (RC_STRINGLIST*, const char*                                                    )> rc_stringlist_find             ;
extern ljh::function_pointer<RC_STRINGLIST*(const char*, const char*                                                       )> rc_stringlist_split            ;
extern ljh::function_pointer<void          (RC_STRINGLIST**                                                                )> rc_stringlist_sort             ;
extern ljh::function_pointer<void          (RC_STRINGLIST*                                                                 )> rc_stringlist_free             ;
extern ljh::function_pointer<RC_PIDLIST*   (const char*, const char* const*, uid_t, pid_t                                  )> rc_find_pids                   ;
extern ljh::function_pointer<bool          (const char*, char**, size_t*                                                   )> rc_getfile                     ;
extern ljh::function_pointer<ssize_t       (char**, size_t*, FILE*                                                         )> rc_getline                     ;
