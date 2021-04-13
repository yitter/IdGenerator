/*
  +----------------------------------------------------------------------+
  | snowdrift                                                            |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: 63851587@qq.com                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "src/snowflake/shm.h"
#include "php_snowdrift.h"
#include "src/snowflake/snowflake.h"

/* True global resources - no need for thread safety here */
static struct shm shmctx;
static snowflake *sf;
zend_class_entry snowdrift_ce;

static uint8_t wid_num = 0;

/* {{{ PHP_INI  */

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("snowdrift.Method", "1", PHP_INI_SYSTEM, OnUpdateLongGEZero, Method, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.BaseTime", "1582136402000", PHP_INI_SYSTEM, OnUpdateLongGEZero, BaseTime, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.WorkerId", "1", PHP_INI_SYSTEM, OnUpdateLongGEZero, WorkerId, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.WorkerIdBitLength", "6", PHP_INI_SYSTEM, OnUpdateLongGEZero, WorkerIdBitLength, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.SeqBitLength", "6", PHP_INI_SYSTEM, OnUpdateLongGEZero, SeqBitLength, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.MaxSeqNumber", "0", PHP_INI_SYSTEM, OnUpdateLongGEZero, MaxSeqNumber, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.MinSeqNumber", "5", PHP_INI_SYSTEM, OnUpdateLongGEZero, MinSeqNumber, zend_snowdrift_globals, snowdrift_globals)
STD_PHP_INI_ENTRY("snowdrift.TopOverCostCount", "2000", PHP_INI_SYSTEM, OnUpdateLongGEZero, TopOverCostCount, zend_snowdrift_globals, snowdrift_globals)
PHP_INI_END()

/* }}} */

static int snowdrift_init()
{
  wid_num = (-1L << SD_G(WorkerIdBitLength)) ^ -1L;
  shmctx.size = wid_num * sizeof(snowflake);
  if (shm_alloc(&shmctx) == -1)
  {
    return FAILURE;
  }
  if (SD_G(MaxSeqNumber) != 0 && SD_G(MaxSeqNumber) < SD_G(MinSeqNumber))
  {
    return FAILURE;
  }
  bzero(shmctx.addr, wid_num * sizeof(snowflake));
  sf = (snowflake *)shmctx.addr;
  for (int i = 0; i < wid_num; i++)
  {
    snowflake *tmp = (sf + i);
    tmp->Method = SD_G(Method);
    tmp->BaseTime = SD_G(BaseTime);
    tmp->WorkerId = i + 1;
    tmp->WorkerIdBitLength = SD_G(WorkerIdBitLength);
    tmp->SeqBitLength = SD_G(SeqBitLength);
    tmp->MaxSeqNumber = SD_G(MaxSeqNumber);
    tmp->MinSeqNumber = SD_G(MinSeqNumber);
    tmp->TopOverCostCount = SD_G(TopOverCostCount);
    Config(tmp);
  }
  return SUCCESS;
}

PHP_METHOD(snowdrift, NextId)
{
  zend_long wid = SD_G(WorkerId);
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &wid) == FAILURE)
  {
    RETURN_FALSE;
  }
  wid--;
  if (wid < 0 || wid > wid_num)
  {
    zend_throw_exception_ex(NULL, 0, "wid error! wid between 1 and %d", wid_num);
    RETURN_NULL();
  }
  snowflake *flake = (sf + wid);
  RETURN_LONG(NextId(flake));
}

PHP_METHOD(snowdrift, NextNumId)
{
  zend_long num = 1;
  zend_long wid = SD_G(WorkerId);
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &num, &wid) == FAILURE)
  {
    RETURN_FALSE;
  }
  wid--;
  if (wid < 0 || wid > wid_num)
  {
    zend_throw_exception_ex(NULL, 0, "wid error! wid between 1 and %d", wid_num);
    RETURN_NULL();
  }
  snowflake *flake = (sf + wid);
  array_init(return_value);
  for (int i = 0; i < num; i++)
  {
    add_next_index_long(return_value, NextId(flake));
  }
}

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(snowdrift)
{
  UNREGISTER_INI_ENTRIES();
  shm_free(&shmctx);
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(snowdrift)
{
#if defined(COMPILE_DL_SNOWFLAKE) && defined(ZTS)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(snowdrift)
{
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(snowdrift)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "snowfrift support", "enabled");
  php_info_print_table_row(2, "Version", PHP_SNOWDRIFT_VERSION);
  php_info_print_table_end();

  /* Remove comments if you have entries in php.ini */
  DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_snowdrift_void, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_snowdrift_nextid, 0, 0, 1)
ZEND_ARG_INFO(0, wid)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_snowdrift_nextnumid, 0, 0, 1)
ZEND_ARG_INFO(0, num)
ZEND_ARG_INFO(0, wid)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ snowdrift_functions[]
 *
 * Every user visible function must have an entry in snowdrift_functions[].
 */
const zend_function_entry snowdrift_functions[] = {
    PHP_FE_END /* Must be the last line in snowdrift_functions[] */
};
/* }}} */

/* {{{ snowdrift_method[]
 *
 * Every user visible function must have an entry in snowdrift_functions[].
 */
static const zend_function_entry snowdrift_methods[] = {
    PHP_ME(snowdrift, NextId, arginfo_snowdrift_nextid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_ME(snowdrift, NextNumId, arginfo_snowdrift_nextnumid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC) PHP_FE_END};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(snowdrift)
{
  INIT_CLASS_ENTRY(snowdrift_ce, "snowdrift", snowdrift_methods);
  REGISTER_INI_ENTRIES();
  zend_register_internal_class(&snowdrift_ce);

  if (snowdrift_init() == FAILURE)
  {
    return FAILURE;
  }
  return SUCCESS;
}
/* }}} */

/* {{{ snowdrift_module_entry
 */
zend_module_entry snowdrift_module_entry = {
    STANDARD_MODULE_HEADER,
    "snowdrift",
    snowdrift_functions,
    PHP_MINIT(snowdrift),
    PHP_MSHUTDOWN(snowdrift),
    PHP_RINIT(snowdrift),     /* Replace with NULL if there's nothing to do at
                                 request start */
    PHP_RSHUTDOWN(snowdrift), /* Replace with NULL if there's nothing to do at
                                 request end */
    PHP_MINFO(snowdrift),
    PHP_SNOWDRIFT_VERSION,
    STANDARD_MODULE_PROPERTIES};
/* }}} */

#ifdef COMPILE_DL_SNOWDRIFT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(snowdrift)
#endif
