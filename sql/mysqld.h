/* Copyright (c) 2010, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#ifndef MYSQLD_INCLUDED
#define MYSQLD_INCLUDED

#include "my_global.h"
#include "mysql_com.h"                     // SERVER_VERSION_LENGTH
#include "my_atomic.h"                     // my_atomic_add64
#include "my_thread.h"                     // my_thread_attr_t
#include "my_thread_local.h"               // my_get_thread_local
#include "sql_cmd.h"                       // SQLCOM_END
#include "sql_const.h"                     // UUID_LENGTH
#include "atomic_class.h"                  /* Atomic_int32 */

class my_decimal;
class THD;
class Time_zone;
struct handlerton;
template <uint default_width> class Bitmap;

typedef struct st_mysql_lex_string LEX_STRING;
typedef struct st_mysql_const_lex_string LEX_CSTRING;
typedef struct st_mysql_show_var SHOW_VAR;
typedef struct st_bitmap MY_BITMAP;
typedef struct charset_info_st CHARSET_INFO;

#if MAX_INDEXES <= 64
typedef Bitmap<64>  Key_map;          /* Used for finding keys */
#elif MAX_INDEXES > 255
#error "MAX_INDEXES values greater than 255 is not supported."
#else
typedef Bitmap<((MAX_INDEXES+7)/8*8)> Key_map; /* Used for finding keys */
#endif

	/* Bits from testflag */
#define TEST_PRINT_CACHED_TABLES 1
#define TEST_NO_KEY_GROUP	 2
#define TEST_MIT_THREAD		4
/*
  TEST_BLOCKING is made obsolete and is not used any
  where in the code base and is retained here so that
  the other bit flag values are not changed.
*/
#define OBSOLETE_TEST_BLOCKING	8
#define TEST_KEEP_TMP_TABLES	16
#define TEST_READCHECK		64	/**< Force use of readcheck */
#define TEST_NO_EXTRA		128
#define TEST_CORE_ON_SIGNAL	256	/**< Give core if signal */
#define TEST_NO_STACKTRACE	512
#define TEST_SIGINT	        1024    /**< Allow sigint on threads */
#define TEST_SYNCHRONIZATION    2048    /**< get server to do sleep in
                                           some places */
#define TEST_DO_QUICK_LEAK_CHECK 4096   /**< Do Valgrind leak check for
                                           each command. */

#define SPECIAL_NO_NEW_FUNC	2		/* Skip new functions */
#define SPECIAL_SKIP_SHOW_DB    4               /* Don't allow 'show db' */
#define SPECIAL_NO_RESOLVE     64		/* Don't use gethostname */
#define SPECIAL_NO_HOST_CACHE	512		/* Don't cache hosts */
#define SPECIAL_SHORT_LOG_FORMAT 1024

/* Function prototypes */
#ifndef EMBEDDED_LIBRARY
void kill_mysql(void);
#endif
void refresh_status(THD *thd);
bool is_secure_file_path(char *path);
ulong sql_rnd_with_mutex();

// These are needed for unit testing.
void set_remaining_args(int argc, char **argv);
int init_common_variables();
void my_init_signals();
bool gtid_server_init();
void gtid_server_cleanup();

extern "C" MYSQL_PLUGIN_IMPORT CHARSET_INFO *system_charset_info;
extern MYSQL_PLUGIN_IMPORT CHARSET_INFO *files_charset_info ;
extern MYSQL_PLUGIN_IMPORT CHARSET_INFO *national_charset_info;
extern MYSQL_PLUGIN_IMPORT CHARSET_INFO *table_alias_charset;
extern CHARSET_INFO *character_set_filesystem;

enum enum_server_operational_state
{
  SERVER_BOOTING,      /* Server is not operational. It is starting */
  SERVER_OPERATING,    /* Server is fully initialized and operating */
  SERVER_SHUTTING_DOWN /* erver is shutting down */
};
enum_server_operational_state get_server_state();

extern MY_BITMAP temp_pool;
extern bool opt_large_files, server_id_supplied;
extern bool opt_bin_log;
extern my_bool opt_log_slave_updates;
extern bool opt_general_log, opt_slow_log, opt_general_log_raw;
extern ulonglong log_output_options;
extern my_bool opt_log_queries_not_using_indexes;
extern ulong opt_log_throttle_queries_not_using_indexes;
extern bool opt_disable_networking, opt_skip_show_db;
extern bool opt_skip_name_resolve;
extern my_bool opt_help;
extern my_bool opt_verbose;
extern bool opt_ignore_builtin_innodb;
extern my_bool opt_character_set_client_handshake;
extern MYSQL_PLUGIN_IMPORT int32 volatile connection_events_loop_aborted_flag;
extern bool opt_install_server;
extern my_bool opt_bootstrap, opt_initialize;
extern my_bool opt_safe_user_create;
extern my_bool opt_local_infile, opt_myisam_use_mmap;
extern my_bool opt_slave_compressed_protocol, use_temp_pool;
extern ulong slave_exec_mode_options;

enum enum_slave_type_conversions { SLAVE_TYPE_CONVERSIONS_ALL_LOSSY,
                                   SLAVE_TYPE_CONVERSIONS_ALL_NON_LOSSY,
                                   SLAVE_TYPE_CONVERSIONS_ALL_UNSIGNED,
                                   SLAVE_TYPE_CONVERSIONS_ALL_SIGNED};
extern ulonglong slave_type_conversions_options;

extern my_bool read_only, opt_readonly;
extern my_bool super_read_only, opt_super_readonly;
extern my_bool lower_case_file_system;

enum enum_slave_rows_search_algorithms { SLAVE_ROWS_TABLE_SCAN = (1U << 0),
                                         SLAVE_ROWS_INDEX_SCAN = (1U << 1),
                                         SLAVE_ROWS_HASH_SCAN  = (1U << 2)};
extern ulonglong slave_rows_search_algorithms_options;
extern my_bool opt_require_secure_transport;

#ifdef HAVE_REPLICATION
extern my_bool opt_slave_preserve_commit_order;
#endif

#ifndef DBUG_OFF
extern uint slave_rows_last_search_algorithm_used;
#endif
extern ulong mts_parallel_option;
#ifdef _WIN32
extern my_bool opt_enable_named_pipe;
extern my_bool opt_enable_shared_memory;
#endif
extern my_bool opt_allow_suspicious_udfs;
extern my_bool opt_secure_auth;
extern char* opt_secure_file_priv;
extern my_bool opt_log_slow_admin_statements, opt_log_slow_slave_statements;
extern my_bool sp_automatic_privileges, opt_noacl;
extern my_bool opt_old_style_user_limits, trust_function_creators;
extern my_bool check_proxy_users, mysql_native_password_proxy_users, sha256_password_proxy_users;
extern char *shared_memory_base_name, *mysqld_unix_port;
extern char *default_tz_name;
extern Time_zone *default_tz;
extern char *default_storage_engine;
extern char *default_tmp_storage_engine;
extern ulong internal_tmp_disk_storage_engine;
extern bool  using_udf_functions;
extern my_bool locked_in_memory;
extern bool opt_using_transactions;
extern ulong current_pid;
extern ulong expire_logs_days;
extern uint sync_binlog_period, sync_relaylog_period,
            sync_relayloginfo_period, sync_masterinfo_period,
            opt_mts_checkpoint_period, opt_mts_checkpoint_group;
extern ulong opt_tc_log_size, tc_log_max_pages_used, tc_log_page_size;
extern ulong tc_log_page_waits;
extern my_bool relay_log_purge;
extern my_bool relay_log_recovery;
extern my_bool offline_mode;
extern my_bool opt_log_builtin_as_identified_by_password;
extern uint test_flags,select_errors,ha_open_options;
extern uint protocol_version, mysqld_port;

enum enum_delay_key_write { DELAY_KEY_WRITE_NONE, DELAY_KEY_WRITE_ON,
			    DELAY_KEY_WRITE_ALL };
extern ulong delay_key_write_options;

extern ulong opt_log_timestamps;
extern const char *timestamp_type_names[];
extern char *opt_general_logname, *opt_slow_logname, *opt_bin_logname,
            *opt_relay_logname;
extern char *mysql_home_ptr, *pidfile_name_ptr;
extern char *default_auth_plugin;
extern uint default_password_lifetime;
extern char *my_bind_addr_str;
extern char glob_hostname[FN_REFLEN];
extern char system_time_zone[30], *opt_init_file;
extern char *opt_tc_log_file;
extern char server_uuid[UUID_LENGTH+1];
extern const char *server_uuid_ptr;
extern const double log_10[309];
extern ulong binlog_cache_use, binlog_cache_disk_use;
extern ulong binlog_stmt_cache_use, binlog_stmt_cache_disk_use;
extern ulong aborted_threads;
extern ulong delayed_insert_timeout;
extern ulong delayed_insert_limit, delayed_queue_size;
extern Atomic_int32 slave_open_temp_tables;
extern ulong query_cache_size, query_cache_min_res_unit;
extern ulong slow_launch_time;
extern ulong table_cache_size;
extern ulong schema_def_size;
extern ulong stored_program_def_size;
extern ulong table_def_size;
extern ulong tablespace_def_size;
extern MYSQL_PLUGIN_IMPORT ulong max_connections;
extern ulong max_digest_length;
extern ulong max_connect_errors, connect_timeout;
extern my_bool opt_slave_allow_batching;
extern ulong slave_trans_retries;
extern uint  slave_net_timeout;
extern ulong opt_mts_slave_parallel_workers;
extern ulonglong opt_mts_pending_jobs_size_max;
extern ulong rpl_stop_slave_timeout;
extern my_bool log_bin_use_v1_row_events;
extern ulong what_to_log,flush_time;
extern ulong max_prepared_stmt_count, prepared_stmt_count;
extern ulong open_files_limit;
extern ulong binlog_cache_size, binlog_stmt_cache_size;
extern ulonglong max_binlog_cache_size, max_binlog_stmt_cache_size;
extern int32 opt_binlog_max_flush_queue_time;
extern ulong opt_binlog_group_commit_sync_delay;
extern ulong opt_binlog_group_commit_sync_no_delay_count;
extern ulong max_binlog_size, max_relay_log_size;
extern ulong slave_max_allowed_packet;
extern ulong opt_binlog_rows_event_max_size;
extern ulong binlog_checksum_options;
extern const char *binlog_checksum_type_names[];
extern my_bool opt_master_verify_checksum;
extern my_bool opt_slave_sql_verify_checksum;
extern uint32 gtid_executed_compression_period;
extern my_bool binlog_gtid_simple_recovery;
extern ulong binlog_error_action;
extern ulong locked_account_connection_count;
enum enum_binlog_error_action
{
  /// Ignore the error and let server continue without binlogging
  IGNORE_ERROR= 0,
  /// Abort the server
  ABORT_SERVER= 1
};
extern const char *binlog_error_action_list[];

extern ulong stored_program_cache_size;
extern ulong back_log;
extern "C" MYSQL_PLUGIN_IMPORT ulong server_id;
extern time_t server_start_time;
extern char *opt_mysql_tmpdir;
extern size_t mysql_unpacked_real_data_home_len;
extern MYSQL_PLUGIN_IMPORT MY_TMPDIR mysql_tmpdir_list;
extern const char *show_comp_option_name[];
extern const char *first_keyword, *binary_keyword;
extern MYSQL_PLUGIN_IMPORT const char  *my_localhost;
extern const char *in_left_expr_name, *in_additional_cond, *in_having_cond;
extern SHOW_VAR status_vars[];
extern struct System_variables max_system_variables;
extern struct System_status_var global_status_var;
extern struct rand_struct sql_rand;
extern handlerton *myisam_hton;
extern handlerton *heap_hton;
extern handlerton *innodb_hton;
extern uint opt_server_id_bits;
extern ulong opt_server_id_mask;
#ifdef WITH_NDBCLUSTER_STORAGE_ENGINE
/* engine specific hook, to be made generic */
extern int(*ndb_wait_setup_func)(ulong);
extern ulong opt_ndb_wait_setup;
#endif
extern const char *load_default_groups[];
extern struct my_option my_long_early_options[];
extern bool mysqld_server_started;
extern "C" MYSQL_PLUGIN_IMPORT int orig_argc;
extern "C" MYSQL_PLUGIN_IMPORT char **orig_argv;
extern my_thread_attr_t connection_attrib;
extern my_bool old_mode;
extern my_bool avoid_temporal_upgrade;
extern LEX_STRING opt_init_connect, opt_init_slave;
#ifndef EMBEDDED_LIBRARY
extern ulong connection_errors_internal;
extern ulong connection_errors_peer_addr;
#endif
extern ulong log_warnings;
extern bool  opt_log_syslog_enable;
extern char *opt_log_syslog_tag;
#ifndef _WIN32
extern bool  opt_log_syslog_include_pid;
extern char *opt_log_syslog_facility;
#endif
/** The size of the host_cache. */
extern uint host_cache_size;
extern ulong log_error_verbosity;

/** System variable show_compatibility_56. */
extern my_bool show_compatibility_56;

extern LEX_CSTRING sql_statement_names[(uint) SQLCOM_END + 1];

/*
  THR_MALLOC is a key which will be used to set/get MEM_ROOT** for a thread,
  using my_set_thread_local()/my_get_thread_local().
*/
extern thread_local_key_t THR_MALLOC;
extern bool THR_MALLOC_initialized;

static inline MEM_ROOT ** my_thread_get_THR_MALLOC()
{
  DBUG_ASSERT(THR_MALLOC_initialized);
  return (MEM_ROOT**) my_get_thread_local(THR_MALLOC);
}

static inline int my_thread_set_THR_MALLOC(MEM_ROOT ** hdl)
{
  DBUG_ASSERT(THR_MALLOC_initialized);
  return my_set_thread_local(THR_MALLOC, hdl);
}


#ifdef HAVE_PSI_INTERFACE

extern PSI_mutex_key key_LOCK_tc;
extern PSI_mutex_key key_hash_filo_lock;
extern PSI_mutex_key key_LOCK_error_log;
extern PSI_mutex_key key_LOCK_gdl;
extern PSI_mutex_key key_LOCK_thd_data;
extern PSI_mutex_key key_LOCK_thd_sysvar;
extern PSI_mutex_key key_LOG_LOCK_log;
extern PSI_mutex_key key_master_info_data_lock;
extern PSI_mutex_key key_master_info_run_lock;
extern PSI_mutex_key key_master_info_sleep_lock;
extern PSI_mutex_key key_master_info_thd_lock;
extern PSI_mutex_key key_mutex_slave_reporting_capability_err_lock;
extern PSI_mutex_key key_relay_log_info_data_lock;
extern PSI_mutex_key key_relay_log_info_sleep_lock;
extern PSI_mutex_key key_relay_log_info_thd_lock;
extern PSI_mutex_key key_relay_log_info_log_space_lock;
extern PSI_mutex_key key_relay_log_info_run_lock;
extern PSI_mutex_key key_mutex_slave_parallel_pend_jobs;
extern PSI_mutex_key key_mutex_slave_parallel_worker;
extern PSI_mutex_key key_mutex_slave_parallel_worker_count;
extern PSI_mutex_key key_structure_guard_mutex;
extern PSI_mutex_key key_TABLE_SHARE_LOCK_ha_data;
extern PSI_mutex_key key_LOCK_query_plan;
extern PSI_mutex_key key_LOCK_thd_query;
extern PSI_mutex_key key_LOCK_cost_const;
extern PSI_mutex_key key_LOCK_current_cond;
extern PSI_mutex_key key_RELAYLOG_LOCK_commit;
extern PSI_mutex_key key_RELAYLOG_LOCK_commit_queue;
extern PSI_mutex_key key_RELAYLOG_LOCK_done;
extern PSI_mutex_key key_RELAYLOG_LOCK_flush_queue;
extern PSI_mutex_key key_RELAYLOG_LOCK_index;
extern PSI_mutex_key key_RELAYLOG_LOCK_log;
extern PSI_mutex_key key_RELAYLOG_LOCK_sync;
extern PSI_mutex_key key_RELAYLOG_LOCK_sync_queue;
extern PSI_mutex_key key_RELAYLOG_LOCK_xids;
extern PSI_mutex_key key_gtid_ensure_index_mutex;
extern PSI_mutex_key key_mts_temp_table_LOCK;
extern PSI_mutex_key key_mts_gaq_LOCK;
extern PSI_mutex_key key_thd_timer_mutex;
#ifdef HAVE_REPLICATION
extern PSI_mutex_key key_commit_order_manager_mutex;
extern PSI_mutex_key key_mutex_slave_worker_hash;
#endif

extern PSI_rwlock_key key_rwlock_LOCK_grant;
extern PSI_rwlock_key key_rwlock_LOCK_logger;
extern PSI_rwlock_key key_rwlock_query_cache_query_lock;
extern PSI_rwlock_key key_rwlock_channel_map_lock;
extern PSI_rwlock_key key_rwlock_channel_lock;

extern PSI_cond_key key_PAGE_cond;
extern PSI_cond_key key_COND_active;
extern PSI_cond_key key_COND_pool;
extern PSI_cond_key key_COND_cache_status_changed;
extern PSI_cond_key key_item_func_sleep_cond;
extern PSI_cond_key key_master_info_data_cond;
extern PSI_cond_key key_master_info_start_cond;
extern PSI_cond_key key_master_info_stop_cond;
extern PSI_cond_key key_master_info_sleep_cond;
extern PSI_cond_key key_relay_log_info_data_cond;
extern PSI_cond_key key_relay_log_info_log_space_cond;
extern PSI_cond_key key_relay_log_info_start_cond;
extern PSI_cond_key key_relay_log_info_stop_cond;
extern PSI_cond_key key_relay_log_info_sleep_cond;
extern PSI_cond_key key_cond_slave_parallel_pend_jobs;
extern PSI_cond_key key_cond_slave_parallel_worker;
extern PSI_cond_key key_cond_mts_gaq;
extern PSI_cond_key key_RELAYLOG_COND_done;
extern PSI_cond_key key_RELAYLOG_update_cond;
extern PSI_cond_key key_RELAYLOG_prep_xids_cond;
extern PSI_cond_key key_gtid_ensure_index_cond;
extern PSI_cond_key key_COND_thr_lock;
#ifdef HAVE_REPLICATION
extern PSI_cond_key key_cond_slave_worker_hash;
extern PSI_cond_key key_commit_order_manager_cond;
#endif
extern PSI_thread_key key_thread_daemon_plugin;
extern PSI_thread_key key_thread_bootstrap;
extern PSI_thread_key key_thread_handle_manager;
extern PSI_thread_key key_thread_one_connection;
extern PSI_thread_key key_thread_compress_gtid_table;
extern PSI_thread_key key_thread_parser_service;
extern PSI_thread_key key_thread_background;

extern PSI_file_key key_file_binlog;
extern PSI_file_key key_file_binlog_cache;
extern PSI_file_key key_file_binlog_index;
extern PSI_file_key key_file_binlog_index_cache;
extern PSI_file_key key_file_dbopt;
extern PSI_file_key key_file_des_key_file;
extern PSI_file_key key_file_ERRMSG;
extern PSI_file_key key_select_to_file;
extern PSI_file_key key_file_fileparser;
extern PSI_file_key key_file_frm;
extern PSI_file_key key_file_global_ddl_log;
extern PSI_file_key key_file_load;
extern PSI_file_key key_file_loadfile;
extern PSI_file_key key_file_log_event_data;
extern PSI_file_key key_file_log_event_info;
extern PSI_file_key key_file_misc;
extern PSI_file_key key_file_partition_ddl_log;
extern PSI_file_key key_file_tclog;
extern PSI_file_key key_file_trg;
extern PSI_file_key key_file_trn;
extern PSI_file_key key_file_init;
extern PSI_file_key key_file_general_log;
extern PSI_file_key key_file_slow_log;
extern PSI_file_key key_file_relaylog;
extern PSI_file_key key_file_relaylog_cache;
extern PSI_file_key key_file_relaylog_index;
extern PSI_file_key key_file_relaylog_index_cache;

extern PSI_socket_key key_socket_tcpip;
extern PSI_socket_key key_socket_unix;
extern PSI_socket_key key_socket_client_connection;

#endif /* HAVE_PSI_INTERFACE */


/*
  MAINTAINER: Please keep this list in order, to limit merge collisions.
  Hint: grep PSI_stage_info | sort -u
*/
extern PSI_stage_info stage_after_create;
extern PSI_stage_info stage_allocating_local_table;
extern PSI_stage_info stage_alter_inplace_prepare;
extern PSI_stage_info stage_alter_inplace;
extern PSI_stage_info stage_alter_inplace_commit;
extern PSI_stage_info stage_changing_master;
extern PSI_stage_info stage_checking_master_version;
extern PSI_stage_info stage_checking_permissions;
extern PSI_stage_info stage_checking_privileges_on_cached_query;
extern PSI_stage_info stage_checking_query_cache_for_query;
extern PSI_stage_info stage_cleaning_up;
extern PSI_stage_info stage_closing_tables;
extern PSI_stage_info stage_compressing_gtid_table;
extern PSI_stage_info stage_connecting_to_master;
extern PSI_stage_info stage_converting_heap_to_ondisk;
extern PSI_stage_info stage_copying_to_group_table;
extern PSI_stage_info stage_copying_to_tmp_table;
extern PSI_stage_info stage_copy_to_tmp_table;
extern PSI_stage_info stage_creating_sort_index;
extern PSI_stage_info stage_creating_table;
extern PSI_stage_info stage_creating_tmp_table;
extern PSI_stage_info stage_deleting_from_main_table;
extern PSI_stage_info stage_deleting_from_reference_tables;
extern PSI_stage_info stage_discard_or_import_tablespace;
extern PSI_stage_info stage_end;
extern PSI_stage_info stage_executing;
extern PSI_stage_info stage_execution_of_init_command;
extern PSI_stage_info stage_explaining;
extern PSI_stage_info stage_finished_reading_one_binlog_switching_to_next_binlog;
extern PSI_stage_info stage_flushing_relay_log_and_master_info_repository;
extern PSI_stage_info stage_flushing_relay_log_info_file;
extern PSI_stage_info stage_freeing_items;
extern PSI_stage_info stage_fulltext_initialization;
extern PSI_stage_info stage_got_handler_lock;
extern PSI_stage_info stage_got_old_table;
extern PSI_stage_info stage_init;
extern PSI_stage_info stage_insert;
extern PSI_stage_info stage_invalidating_query_cache_entries_table;
extern PSI_stage_info stage_invalidating_query_cache_entries_table_list;
extern PSI_stage_info stage_killing_slave;
extern PSI_stage_info stage_logging_slow_query;
extern PSI_stage_info stage_making_temp_file_append_before_load_data;
extern PSI_stage_info stage_making_temp_file_create_before_load_data;
extern PSI_stage_info stage_manage_keys;
extern PSI_stage_info stage_master_has_sent_all_binlog_to_slave;
extern PSI_stage_info stage_opening_tables;
extern PSI_stage_info stage_optimizing;
extern PSI_stage_info stage_preparing;
extern PSI_stage_info stage_purging_old_relay_logs;
extern PSI_stage_info stage_query_end;
extern PSI_stage_info stage_queueing_master_event_to_the_relay_log;
extern PSI_stage_info stage_reading_event_from_the_relay_log;
extern PSI_stage_info stage_registering_slave_on_master;
extern PSI_stage_info stage_removing_duplicates;
extern PSI_stage_info stage_removing_tmp_table;
extern PSI_stage_info stage_rename;
extern PSI_stage_info stage_rename_result_table;
extern PSI_stage_info stage_requesting_binlog_dump;
extern PSI_stage_info stage_reschedule;
extern PSI_stage_info stage_searching_rows_for_update;
extern PSI_stage_info stage_sending_binlog_event_to_slave;
extern PSI_stage_info stage_sending_cached_result_to_client;
extern PSI_stage_info stage_sending_data;
extern PSI_stage_info stage_setup;
extern PSI_stage_info stage_slave_has_read_all_relay_log;
extern PSI_stage_info stage_slave_waiting_event_from_coordinator;
extern PSI_stage_info stage_slave_waiting_for_workers_to_process_queue;
extern PSI_stage_info stage_slave_waiting_worker_queue;
extern PSI_stage_info stage_slave_waiting_worker_to_free_events;
extern PSI_stage_info stage_slave_waiting_worker_to_release_partition;
extern PSI_stage_info stage_slave_waiting_workers_to_exit;
extern PSI_stage_info stage_sorting_for_group;
extern PSI_stage_info stage_sorting_for_order;
extern PSI_stage_info stage_sorting_result;
extern PSI_stage_info stage_sql_thd_waiting_until_delay;
extern PSI_stage_info stage_statistics;
extern PSI_stage_info stage_storing_result_in_query_cache;
extern PSI_stage_info stage_storing_row_into_queue;
extern PSI_stage_info stage_system_lock;
extern PSI_stage_info stage_update;
extern PSI_stage_info stage_updating;
extern PSI_stage_info stage_updating_main_table;
extern PSI_stage_info stage_updating_reference_tables;
extern PSI_stage_info stage_upgrading_lock;
extern PSI_stage_info stage_user_sleep;
extern PSI_stage_info stage_verifying_table;
extern PSI_stage_info stage_waiting_for_gtid_to_be_committed;
extern PSI_stage_info stage_waiting_for_handler_insert;
extern PSI_stage_info stage_waiting_for_handler_lock;
extern PSI_stage_info stage_waiting_for_handler_open;
extern PSI_stage_info stage_waiting_for_insert;
extern PSI_stage_info stage_waiting_for_master_to_send_event;
extern PSI_stage_info stage_waiting_for_master_update;
extern PSI_stage_info stage_waiting_for_relay_log_space;
extern PSI_stage_info stage_waiting_for_slave_mutex_on_exit;
extern PSI_stage_info stage_waiting_for_slave_thread_to_start;
extern PSI_stage_info stage_waiting_for_query_cache_lock;
extern PSI_stage_info stage_waiting_for_table_flush;
extern PSI_stage_info stage_waiting_for_the_next_event_in_relay_log;
extern PSI_stage_info stage_waiting_for_the_slave_thread_to_advance_position;
extern PSI_stage_info stage_waiting_to_finalize_termination;
extern PSI_stage_info stage_worker_waiting_for_its_turn_to_commit;
extern PSI_stage_info stage_worker_waiting_for_commit_parent;
extern PSI_stage_info stage_suspending;
extern PSI_stage_info stage_starting;
#ifdef HAVE_PSI_STATEMENT_INTERFACE
/**
  Statement instrumentation keys (sql).
  The last entry, at [SQLCOM_END], is for parsing errors.
*/
extern PSI_statement_info sql_statement_info[(uint) SQLCOM_END + 1];

/**
  Statement instrumentation keys (com).
  The last entry, at [COM_END], is for packet errors.
*/
extern PSI_statement_info com_statement_info[(uint) COM_END + 1];

/**
  Statement instrumentation key for replication.
*/
extern PSI_statement_info stmt_info_rpl;
#endif /* HAVE_PSI_STATEMENT_INTERFACE */

#ifdef HAVE_OPENSSL
extern struct st_VioSSLFd * ssl_acceptor_fd;
#endif /* HAVE_OPENSSL */

extern my_bool opt_large_pages;
extern uint opt_large_page_size;
extern char lc_messages_dir[FN_REFLEN];
extern char *lc_messages_dir_ptr;
extern const char *log_error_dest;
extern MYSQL_PLUGIN_IMPORT char reg_ext[FN_EXTLEN];
extern MYSQL_PLUGIN_IMPORT uint reg_ext_length;
extern MYSQL_PLUGIN_IMPORT uint lower_case_table_names;
extern MYSQL_PLUGIN_IMPORT bool mysqld_embedded;

extern long tc_heuristic_recover;

extern ulong specialflag;
extern size_t mysql_data_home_len;
extern const char *mysql_real_data_home_ptr;
extern MYSQL_PLUGIN_IMPORT char  *mysql_data_home;
extern "C" MYSQL_PLUGIN_IMPORT char server_version[SERVER_VERSION_LENGTH];
extern MYSQL_PLUGIN_IMPORT char mysql_real_data_home[];
extern char mysql_unpacked_real_data_home[];
extern MYSQL_PLUGIN_IMPORT struct System_variables global_system_variables;
extern char default_logfile_name[FN_REFLEN];

#define mysql_tmpdir (my_tmpdir(&mysql_tmpdir_list))

extern MYSQL_PLUGIN_IMPORT const Key_map key_map_empty;
extern MYSQL_PLUGIN_IMPORT Key_map key_map_full; // Should be treated as const

/*
  Server mutex locks and condition variables.
 */
extern mysql_mutex_t LOCK_status;
extern mysql_mutex_t LOCK_uuid_generator;
extern mysql_mutex_t LOCK_crypt;
extern mysql_mutex_t LOCK_manager;
extern mysql_mutex_t LOCK_global_system_variables;
extern mysql_mutex_t LOCK_user_conn;
extern mysql_mutex_t LOCK_log_throttle_qni;
extern mysql_mutex_t LOCK_prepared_stmt_count;
extern mysql_mutex_t LOCK_error_messages;
extern mysql_mutex_t LOCK_sql_slave_skip_counter;
extern mysql_mutex_t LOCK_slave_net_timeout;
extern mysql_mutex_t LOCK_offline_mode;
extern mysql_mutex_t LOCK_default_password_lifetime;
#ifdef HAVE_OPENSSL
extern char* des_key_file;
extern mysql_mutex_t LOCK_des_key_file;
#endif
extern mysql_mutex_t LOCK_server_started;
extern mysql_mutex_t LOCK_reset_gtid_table;
extern mysql_mutex_t LOCK_compress_gtid_table;

extern mysql_cond_t COND_server_started;
extern mysql_cond_t COND_compress_gtid_table;
extern mysql_cond_t COND_manager;

extern mysql_rwlock_t LOCK_sys_init_connect;
extern mysql_rwlock_t LOCK_sys_init_slave;
extern mysql_rwlock_t LOCK_system_variables_hash;

extern char *opt_ssl_ca, *opt_ssl_capath, *opt_ssl_cert, *opt_ssl_cipher,
            *opt_ssl_key, *opt_ssl_crl, *opt_ssl_crlpath;

extern char *opt_disabled_storage_engines;

/* query_id */
typedef int64 query_id_t;
extern query_id_t global_query_id;

/* increment query_id and return it.  */
inline __attribute__((warn_unused_result)) query_id_t next_query_id()
{
  query_id_t id= my_atomic_add64(&global_query_id, 1);
  return (id+1);
}

#define ER(X) please_use_ER_THD_or_ER_DEFAULT_instead(X)

/* Accessor function for _connection_events_loop_aborted flag */
inline __attribute__((warn_unused_result))
bool connection_events_loop_aborted()
{
  return my_atomic_load32(&connection_events_loop_aborted_flag);
}

/* only here because of unireg_init(). */
static inline void set_connection_events_loop_aborted(bool value)
{
  my_atomic_store32(&connection_events_loop_aborted_flag, value);
}

#endif /* MYSQLD_INCLUDED */
