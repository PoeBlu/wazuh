/*
 * Copyright (C) 2015-2020, Wazuh Inc.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "../../headers/shared.h"
#include "../../analysisd/logtest.h"

int w_logtest_init_parameters();
void *w_logtest_init();
void w_logtest_remove_session(char *token);
void *w_logtest_check_inactive_sessions(__attribute__((unused)) void * arg);

int logtest_enabled = 1;

/* setup/teardown */



/* wraps */

int __wrap_OS_BindUnixDomain(const char *path, int type, int max_msg_size) {
    return mock();
}

int __wrap_accept(int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len) {
    return mock();
}

void __wrap__merror(const char * file, int line, const char * func, const char *msg, ...) {
    char formatted_msg[OS_MAXSTR];
    va_list args;

    va_start(args, msg);
    vsnprintf(formatted_msg, OS_MAXSTR, msg, args);
    va_end(args);

    check_expected(formatted_msg);
}

int __wrap_pthread_mutex_init() {
    return mock();
}

int __wrap_pthread_mutex_lock() {
    return mock();
}

int __wrap_pthread_mutex_unlock() {
    return mock();
}

int __wrap_pthread_mutex_destroy() {
    return mock();
}

int __wrap_ReadConfig(int modules, const char *cfgfile, void *d1, void *d2) {
    if (!logtest_enabled) {
        w_logtest_conf.enabled = false;
    }
    return mock();
}

OSHash *__wrap_OSHash_Create() {
    return mock_type(OSHash *);
}

int __wrap_OSHash_setSize(OSHash *self, unsigned int new_size) {
    if (new_size) check_expected(new_size);
    return mock();
}

OSList *__wrap_OSList_Create() {
    return mock_type(OSList *);
}

int __wrap_OSList_SetMaxSize() {
    return mock();
}

void __wrap__minfo(const char * file, int line, const char * func, const char *msg, ...) {
    char formatted_msg[OS_MAXSTR];
    va_list args;

    va_start(args, msg);
    vsnprintf(formatted_msg, OS_MAXSTR, msg, args);
    va_end(args);

    check_expected(formatted_msg);
}

void __wrap_w_mutex_init() {
    return;
}

void __wrap_w_mutex_destroy() {
    return;
}

void __wrap_w_create_thread() {
    return;
}

int __wrap_close (int __fd) {
    return mock();
}

int __wrap_getDefine_Int() {
    return mock();
}

void * __wrap_OSHash_Delete_ex(OSHash *self, const char *key) {
    if (key) check_expected(key);
    return mock_type(void *);
}

void __wrap_os_remove_rules_list(RuleNode *node) {
    return;
}

void * __wrap_OSHash_Free(OSHash *self) {
    return mock_type(void *);
}

void __wrap_os_remove_decoders_list(OSDecoderNode *decoderlist_pn, OSDecoderNode *decoderlist_npn) {
    return;
}

void __wrap_os_remove_cdblist(ListNode **l_node) {
    return;
}

void __wrap_os_remove_cdbrules(ListRule **l_rule) {
    os_free(*l_rule);
    return;
}

void __wrap_os_remove_eventlist(EventList *list) {
    return;
}

unsigned int __wrap_sleep (unsigned int __seconds) {
    return mock_type(unsigned int);
}

OSHashNode *__wrap_OSHash_Begin(const OSHash *self, unsigned int *i) {
    return mock_type(OSHashNode *);
}

time_t __wrap_time(time_t *t) {
    return mock_type(time_t);
}

double __wrap_difftime (time_t __time1, time_t __time0) {
    return mock();
}

OSHashNode *__wrap_OSHash_Next(const OSHash *self, unsigned int *i, OSHashNode *current) {
    return mock_type(OSHashNode *);
}

OSStore *__wrap_OSStore_Free(OSStore *list) {
    return mock_type(OSStore *);
}

void __wrap_OS_CreateEventList(int maxsize, EventList *list) {
    return;
}

int __wrap_ReadDecodeXML(const char *file, OSDecoderNode **decoderlist_pn,
                        OSDecoderNode **decoderlist_nopn, OSStore **decoder_list,
                        OSList* log_msg) {
    return mock_type(int);
}

int __wrap_SetDecodeXML(OSList* log_msg, OSStore **decoder_list,
                        OSDecoderNode **decoderlist_npn, OSDecoderNode **decoderlist_pn) {
    return mock_type(int);
}

int __wrap_Lists_OP_LoadList(char * files, ListNode ** cdblistnode) {
    return mock_type(int);
}

void __wrap_Lists_OP_MakeAll(int force, int show_message, ListNode **lnode) {
    return;
}

int __wrap_Rules_OP_ReadRules(char * file, RuleNode ** rule_list, ListNode ** cbd , EventList ** evet , OSList * msg) {
    return mock_type(int);
}

void __wrap_OS_ListLoadRules(ListNode **l_node, ListRule **lrule) {
    return;
}

int __wrap__setlevels(RuleNode *node, int nnode) {
    return mock_type(int);
}

int __wrap_AddHash_Rule(RuleNode *node) {
    return mock_type(int);
}

int __wrap_Accumulate_Init(OSHash **acm_store, int *acm_lookups, time_t *acm_purge_ts) {
    return mock_type(int);
}

/* tests */

/* w_logtest_init_parameters */
void test_w_logtest_init_parameters_invalid(void **state)
{
    will_return(__wrap_ReadConfig, OS_INVALID);

    int ret = w_logtest_init_parameters();
    assert_int_equal(ret, OS_INVALID);

}

void test_w_logtest_init_parameters_done(void **state)
{
    will_return(__wrap_ReadConfig, 0);

    int ret = w_logtest_init_parameters();
    assert_int_equal(ret, OS_SUCCESS);

}

/* w_logtest_init */
void test_w_logtest_init_error_parameters(void **state)
{
    will_return(__wrap_ReadConfig, OS_INVALID);

    expect_string(__wrap__merror, formatted_msg, "(7304): Invalid wazuh-logtest configuration");

    w_logtest_init();

}


void test_w_logtest_init_logtest_disabled(void **state)
{
    will_return(__wrap_ReadConfig, 0);

    logtest_enabled = 0;

    expect_string(__wrap__minfo, formatted_msg, "(7201): Logtest disabled");

    w_logtest_init();

    logtest_enabled = 1;

}

void test_w_logtest_init_conection_fail(void **state)
{
    will_return(__wrap_ReadConfig, 0);

    will_return(__wrap_OS_BindUnixDomain, OS_SOCKTERR);

    expect_string(__wrap__merror, formatted_msg, "(7300): Unable to bind to socket '/queue/ossec/logtest'. Errno: (0) Success");

    w_logtest_init();

}

void test_w_logtest_init_OSHash_create_fail(void **state)
{
    will_return(__wrap_ReadConfig, 0);

    will_return(__wrap_OS_BindUnixDomain, OS_SUCCESS);

    will_return(__wrap_OSHash_Create, NULL);

    expect_string(__wrap__merror, formatted_msg, "(7303): Failure to initialize all_sesssions hash");

    w_logtest_init();

}

void test_w_logtest_init_OSHash_setSize_fail(void **state)
{
    will_return(__wrap_ReadConfig, 0);

    will_return(__wrap_OS_BindUnixDomain, OS_SUCCESS);

    will_return(__wrap_OSHash_Create, 1);

    expect_value(__wrap_OSHash_setSize, new_size, 2048);
    will_return(__wrap_OSHash_setSize, NULL);

    expect_string(__wrap__merror, formatted_msg, "(7305): Failure to resize all_sesssions hash");

    w_logtest_init();

}

void test_w_logtest_init_done(void **state)
{
    will_return(__wrap_ReadConfig, 0);

    will_return(__wrap_OS_BindUnixDomain, OS_SUCCESS);

    will_return(__wrap_OSHash_Create, 1);

    expect_value(__wrap_OSHash_setSize, new_size, 2048);
    will_return(__wrap_OSHash_setSize, 1);

    expect_string(__wrap__minfo, formatted_msg, "(7200): Logtest started");

    // Needs to implement w_logtest_main

    w_logtest_init();

}

/* w_logtest_fts_init */
void test_w_logtest_fts_init_create_list_failure(void **state)
{
    OSList *fts_list;
    OSHash *fts_store;

    will_return(__wrap_getDefine_Int, 5);

    will_return(__wrap_OSList_Create, NULL);

    expect_string(__wrap__merror, formatted_msg, "(1290): Unable to create a new list (calloc).");

    int ret = w_logtest_fts_init(&fts_list, &fts_store);
    assert_int_equal(ret, 0);

}

void test_w_logtest_fts_init_SetMaxSize_failure(void **state)
{
    OSList *fts_list;
    OSHash *fts_store;
    OSList *list = (OSList *) 1;

    will_return(__wrap_getDefine_Int, 5);

    will_return(__wrap_OSList_Create, list);

    will_return(__wrap_OSList_SetMaxSize, 0);

    expect_string(__wrap__merror, formatted_msg, "(1292): Error setting error size.");

    int ret = w_logtest_fts_init(&fts_list, &fts_store);
    assert_int_equal(ret, 0);

}

void test_w_logtest_fts_init_create_hash_failure(void **state)
{
    OSList *fts_list;
    OSHash *fts_store;
    OSList *list = (OSList *) 1;

    will_return(__wrap_getDefine_Int, 5);

    will_return(__wrap_OSList_Create, list);

    will_return(__wrap_OSList_SetMaxSize, 1);

    will_return(__wrap_OSHash_Create, NULL);

    expect_string(__wrap__merror, formatted_msg, "(1295): Unable to create a new hash (calloc).");

    int ret = w_logtest_fts_init(&fts_list, &fts_store);
    assert_int_equal(ret, 0);

}

void test_w_logtest_fts_init_setSize_failure(void **state)
{
    OSList *fts_list;
    OSHash *fts_store;
    OSList *list = (OSList *) 1;
    OSHash *hash = (OSHash *) 1;

    will_return(__wrap_getDefine_Int, 5);

    will_return(__wrap_OSList_Create, list);

    will_return(__wrap_OSList_SetMaxSize, 1);

    will_return(__wrap_OSHash_Create, hash);

    expect_value(__wrap_OSHash_setSize, new_size, 2048);
    will_return(__wrap_OSHash_setSize, 0);

    expect_string(__wrap__merror, formatted_msg, "(1292): Error setting error size.");

    int ret = w_logtest_fts_init(&fts_list, &fts_store);
    assert_int_equal(ret, 0);

}

void test_w_logtest_fts_init_success(void **state)
{
    OSList *fts_list;
    OSHash *fts_store;
    OSList *list = (OSList *) 1;
    OSHash *hash = (OSHash *) 1;

    will_return(__wrap_getDefine_Int, 5);

    will_return(__wrap_OSList_Create, list);

    will_return(__wrap_OSList_SetMaxSize, 1);

    will_return(__wrap_OSHash_Create, hash);

    expect_value(__wrap_OSHash_setSize, new_size, 2048);
    will_return(__wrap_OSHash_setSize, 1);

    int ret = w_logtest_fts_init(&fts_list, &fts_store);
    assert_int_equal(ret, 1);

}

/* w_logtest_remove_session */
void test_w_logtest_remove_session_fail(void **state)
{
    char * key = "test";

    expect_value(__wrap_OSHash_Delete_ex, key, "test");
    will_return(__wrap_OSHash_Delete_ex, NULL);

    w_logtest_remove_session(key);

}

void test_w_logtest_remove_session_OK(void **state)
{
    char * key = "test";
    w_logtest_session_t *session;
    os_calloc(1, sizeof(w_logtest_session_t), session);

    expect_value(__wrap_OSHash_Delete_ex, key, "test");
    will_return(__wrap_OSHash_Delete_ex, session);

    will_return(__wrap_OSStore_Free, session->decoder_store);

    will_return(__wrap_OSHash_Free, session);

    will_return(__wrap_OSHash_Free, session);

    will_return(__wrap_OSHash_Free, session);

    w_logtest_remove_session(key);

}

/* w_logtest_check_inactive_sessions */
void test_w_logtest_check_inactive_sessions_no_remove(void **state)
{
    w_logtest_session_t *session;
    os_calloc(1, sizeof(w_logtest_session_t), session);
    session->last_connection = 1;

    OSHashNode *hash_node;
    os_calloc(1, sizeof(OSHashNode), hash_node);
    hash_node->key = "test";
    hash_node->data = session;

    will_return(__wrap_FOREVER, 1);

    will_return(__wrap_sleep, 0);

    will_return(__wrap_OSHash_Begin, hash_node);

    will_return(__wrap_time, NULL);

    will_return(__wrap_difftime, 1);

    will_return(__wrap_OSHash_Next, NULL);

    will_return(__wrap_FOREVER, 0);

    w_logtest_check_inactive_sessions(NULL);

    os_free(session);
    os_free(hash_node);

}

void test_w_logtest_check_inactive_sessions_remove(void **state)
{
    w_logtest_session_t *session;
    os_calloc(1, sizeof(w_logtest_session_t), session);
    session->last_connection = 1;

    OSHashNode *hash_node;
    os_calloc(1, sizeof(OSHashNode), hash_node);
    hash_node->key = "test";
    hash_node->data = session;

    will_return(__wrap_FOREVER, 1);

    will_return(__wrap_sleep, 0);

    will_return(__wrap_OSHash_Begin, hash_node);

    will_return(__wrap_time, NULL);

    will_return(__wrap_difftime, 1000000);

    // test_w_logtest_remove_session_ok
    char * key = "test";

    expect_value(__wrap_OSHash_Delete_ex, key, "test");
    will_return(__wrap_OSHash_Delete_ex, session);

    will_return(__wrap_OSStore_Free, NULL);

    will_return(__wrap_OSHash_Free, session);

    will_return(__wrap_OSHash_Free, session);

    will_return(__wrap_OSHash_Free, session);


    will_return(__wrap_OSHash_Next, NULL);

    will_return(__wrap_FOREVER, 0);

    w_logtest_check_inactive_sessions(NULL);

    os_free(hash_node);

}

/* w_logtest_initialize_session */
void test_w_logtest_initialize_session_error_decoders(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;

    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_pthread_mutex_init, 0);
    will_return(__wrap_ReadDecodeXML, 0);

    session = w_logtest_initialize_session(token, msg);

    assert_null(session);
    
    os_free(Config.decoders);

}

void test_w_logtest_initialize_session_error_cbd_list(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;
    
    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    char * cbd_file = "test.xml";
    Config.lists = calloc(2, sizeof(char *));
    Config.lists[0] = cbd_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_ReadDecodeXML, 1);
    will_return(__wrap_SetDecodeXML, 0);
    will_return(__wrap_Lists_OP_LoadList, -1);

    session = w_logtest_initialize_session(token, msg);

    assert_null(session);
    
    os_free(Config.decoders);
    os_free(Config.lists);

}

void test_w_logtest_initialize_session_error_rules(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;
    
    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    char * cbd_file = "test.xml";
    Config.lists = calloc(2, sizeof(char *));
    Config.lists[0] = cbd_file;

    char * include_file = "test.xml";
    Config.includes = calloc(2, sizeof(char *));
    Config.includes[0] = include_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_ReadDecodeXML, 1);
    will_return(__wrap_Lists_OP_LoadList, 0);
    will_return(__wrap_Rules_OP_ReadRules, -1);

    session = w_logtest_initialize_session(token, msg);

    assert_null(session);


    os_free(Config.includes);
    os_free(Config.decoders);
    os_free(Config.lists);

}

void test_w_logtest_initialize_session_error_hash_rules(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;

    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    char * cbd_file = "test.xml";
    Config.lists = calloc(2, sizeof(char *));
    Config.lists[0] = cbd_file;

    char * include_file = "test.xml";
    Config.includes = calloc(2, sizeof(char *));
    Config.includes[0] = include_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_ReadDecodeXML, 1);
    will_return(__wrap_Lists_OP_LoadList, 0);
    will_return(__wrap_Rules_OP_ReadRules, 0);
    will_return(__wrap__setlevels, 0);
    will_return(__wrap_OSHash_Create, 0);

    session = w_logtest_initialize_session(token, msg);

    assert_null(session);

    os_free(Config.includes);
    os_free(Config.decoders);
    os_free(Config.lists);
}

void test_w_logtest_initialize_session_error_fts_init(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;

    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    char * cbd_file = "test.xml";
    Config.lists = calloc(2, sizeof(char *));
    Config.lists[0] = cbd_file;

    char * include_file = "test.xml";
    Config.includes = calloc(2, sizeof(char *));
    Config.includes[0] = include_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_ReadDecodeXML, 1);
    will_return(__wrap_Lists_OP_LoadList, 0);
    will_return(__wrap_Rules_OP_ReadRules, 0);
    will_return(__wrap__setlevels, 0);
    will_return(__wrap_OSHash_Create, 1);
    will_return(__wrap_AddHash_Rule, 0);

    /* FTS init fail */
    OSList * fts_list;
    OSHash * fts_store;
    will_return(__wrap_getDefine_Int, 5);
    will_return(__wrap_OSList_Create, NULL);
    expect_string(__wrap__merror, formatted_msg, "(1290): Unable to create a new list (calloc).");

    session = w_logtest_initialize_session(token, msg);

    assert_null(session);

    os_free(Config.includes);
    os_free(Config.decoders);
    os_free(Config.lists);

}

void test_w_logtest_initialize_session_error_accumulate_init(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;

    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    char * cbd_file = "test.xml";
    Config.lists = calloc(2, sizeof(char *));
    Config.lists[0] = cbd_file;

    char * include_file = "test.xml";
    Config.includes = calloc(2, sizeof(char *));
    Config.includes[0] = include_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_ReadDecodeXML, 1);
    will_return(__wrap_Lists_OP_LoadList, 0);
    will_return(__wrap_Rules_OP_ReadRules, 0);
    will_return(__wrap__setlevels, 0);
    will_return(__wrap_OSHash_Create, 1);
    will_return(__wrap_AddHash_Rule, 0);

    /* FTS init success */
    OSList *fts_list;
    OSHash *fts_store;
    OSList *list = (OSList *) 1;
    OSHash *hash = (OSHash *) 1;
    will_return(__wrap_getDefine_Int, 5);
    will_return(__wrap_OSList_Create, list);
    will_return(__wrap_OSList_SetMaxSize, 1);
    will_return(__wrap_OSHash_Create, hash);
    expect_value(__wrap_OSHash_setSize, new_size, 2048);
    will_return(__wrap_OSHash_setSize, 1);

    will_return(__wrap_Accumulate_Init, 0);

    session = w_logtest_initialize_session(token, msg);

    assert_null(session);

    os_free(Config.includes);
    os_free(Config.decoders);
    os_free(Config.lists);

}

void test_w_logtest_initialize_session_success(void ** state) {

    char * token = "test";
    OSList * msg = (OSList *) 1;
    w_logtest_session_t * session;

    char * decoder_file = "test.xml";
    Config.decoders = calloc(2, sizeof(char *));
    Config.decoders[0] = decoder_file;

    char * cbd_file = "test.xml";
    Config.lists = calloc(2, sizeof(char *));
    Config.lists[0] = cbd_file;

    char * include_file = "test.xml";
    Config.includes = calloc(2, sizeof(char *));
    Config.includes[0] = include_file;

    will_return(__wrap_time, 0);
    will_return(__wrap_ReadDecodeXML, 1);
    will_return(__wrap_Lists_OP_LoadList, 0);
    will_return(__wrap_Rules_OP_ReadRules, 0);
    will_return(__wrap__setlevels, 0);
    will_return(__wrap_OSHash_Create, 1);
    will_return(__wrap_AddHash_Rule, 0);

    /* FTS init success */
    OSList *fts_list;
    OSHash *fts_store;
    OSList *list = (OSList *) 1;
    OSHash *hash = (OSHash *) 1;
    will_return(__wrap_getDefine_Int, 5);
    will_return(__wrap_OSList_Create, list);
    will_return(__wrap_OSList_SetMaxSize, 1);
    will_return(__wrap_OSHash_Create, hash);
    expect_value(__wrap_OSHash_setSize, new_size, 2048);
    will_return(__wrap_OSHash_setSize, 1);

    will_return(__wrap_Accumulate_Init, 1);
    
    session = w_logtest_initialize_session(token, msg);

    assert_non_null(session);

    os_free(Config.includes);
    os_free(Config.decoders);
    os_free(Config.lists);

}

int main(void)
{
    const struct CMUnitTest tests[] = {
        // Tests w_logtest_init_parameters
        cmocka_unit_test(test_w_logtest_init_parameters_invalid),
        cmocka_unit_test(test_w_logtest_init_parameters_done),
        // Tests w_logtest_init
        cmocka_unit_test(test_w_logtest_init_error_parameters),
        cmocka_unit_test(test_w_logtest_init_logtest_disabled),
        cmocka_unit_test(test_w_logtest_init_conection_fail),
        cmocka_unit_test(test_w_logtest_init_OSHash_create_fail),
        // Tests w_logtest_fts_init
        cmocka_unit_test(test_w_logtest_fts_init_create_list_failure),
        cmocka_unit_test(test_w_logtest_fts_init_SetMaxSize_failure),
        cmocka_unit_test(test_w_logtest_fts_init_create_hash_failure),
        cmocka_unit_test(test_w_logtest_fts_init_setSize_failure),
        cmocka_unit_test(test_w_logtest_fts_init_success),
        // Tests w_logtest_remove_session
        cmocka_unit_test(test_w_logtest_remove_session_fail),
        cmocka_unit_test(test_w_logtest_remove_session_OK),
        // Tests w_logtest_check_inactive_sessions
        cmocka_unit_test(test_w_logtest_check_inactive_sessions_no_remove),
        cmocka_unit_test(test_w_logtest_check_inactive_sessions_remove),
        // w_logtest_initialize_session
        cmocka_unit_test(test_w_logtest_initialize_session_error_decoders),
        cmocka_unit_test(test_w_logtest_initialize_session_error_cbd_list),
        cmocka_unit_test(test_w_logtest_initialize_session_error_rules),
        cmocka_unit_test(test_w_logtest_initialize_session_error_hash_rules),
        cmocka_unit_test(test_w_logtest_initialize_session_error_fts_init),
        cmocka_unit_test(test_w_logtest_initialize_session_error_accumulate_init),
        cmocka_unit_test(test_w_logtest_initialize_session_success)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
