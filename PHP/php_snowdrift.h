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
  | 版权归属: yitter(yitter@126.com)                                      |
  | Author: amuluowin(63851587@qq.com)                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SNOWDRIFT_H
#define PHP_SNOWDRIFT_H

extern zend_module_entry snowdrift_module_entry;
#define phpext_snowdrift_ptr &snowdrift_module_entry

#define PHP_SNOWDRIFT_VERSION \
  "1.0.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#define PHP_SNOWDRIFT_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_SNOWDRIFT_API __attribute__((visibility("default")))
#else
#define PHP_SNOWDRIFT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

// PHP8
#if PHP_VERSION_ID >= 80000
#define TSRMLS_D void
#define TSRMLS_DC
#define TSRMLS_C
#define TSRMLS_CC
#define TSRMLS_FETCH()

#define ZEND_ACC_DTOR 0
#endif

ZEND_BEGIN_MODULE_GLOBALS(snowdrift)
uint8_t Method;
uint64_t BaseTime;
uint16_t WorkerId;
uint8_t WorkerIdBitLength;
uint8_t SeqBitLength;
uint32_t MaxSeqNumber;
uint32_t MinSeqNumber;
uint16_t TopOverCostCount;
uint8_t Multi;

ZEND_END_MODULE_GLOBALS(snowdrift)

ZEND_DECLARE_MODULE_GLOBALS(snowdrift)

#define SD_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(snowdrift, v)

#if defined(ZTS) && defined(COMPILE_DL_SNOWDRIFT)
ZEND_TSRMLS_CACHE_EXTERN()
#endif
static int snowdrift_init();

#endif /* PHP_SNOWDRIFT_H */
