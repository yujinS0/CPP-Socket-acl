#ifndef __DGATE_UTIL_INCLUDE_H__
#define __DGATE_UTIL_INCLUDE_H__

#include "lib_acl.h"
#ifdef __cplusplus
extern "C" {
#endif

ACL_ARGV *res_a_create(const ACL_RFC1035_RR *answer, int n);

#ifdef __cplusplus
}
#endif

#endif
