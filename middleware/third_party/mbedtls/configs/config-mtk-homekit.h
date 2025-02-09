/*
 *  Minimal configuration for TLS 1.1 (RFC 4346)
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Minimal configuration for TLS 1.1 (RFC 4346), implementing only the
 * required ciphersuite: MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA
 *
 * See README.txt for usage instructions.
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_CALLOC_MACRO pvPortCalloc //mbedtls_calloc //
#define MBEDTLS_PLATFORM_FREE_MACRO	vPortFree //mbedtls_free //



/* mbed TLS feature support */
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
//#define MBEDTLS_SSL_PROTO_SSL3
//#error "MBEDTLS_SSL_PROTO_SSL3 (SSL v3.0 support) was removed in Mbed TLS 3.0. See https://github.com/Mbed-TLS/mbedtls/issues/4031"
#define MBEDTLS_SSL_PROTO_TLS1
#define MBEDTLS_SSL_PROTO_TLS1_1
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_ALT
#define MBEDTLS_PLATFORM_C

/*
 * Use only CCM_8 ciphersuites, and
 * save ROM and a few bytes of RAM by specifying our own ciphersuite list
 */
//#define MBEDTLS_SSL_CIPHERSUITES                        \
        MBEDTLS_TLS_PSK_WITH_AES_256_CCM_8,             \
        MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_CCM_C
//#define MBEDTLS_ARC4_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_DES_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_MD_C
//#define MBEDTLS_MD5_C
#define MBEDTLS_NET_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_RSA_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_USE_C
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_ALPN
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT

/* TLS protocol feature support */
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2
/*
 * Save RAM at the expense of interoperability: do this only if you control
 * both ends of the connection!  (See comments in "mbedtls/ssl.h".)
 * The optimal size here depends on the typical size of records.
 */
#define MBEDTLS_SSL_IN_CONTENT_LEN              (4 * 1024)
#define MBEDTLS_SSL_OUT_CONTENT_LEN             1024


/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

/* Save some RAM by adjusting to your exact needs */
#define MBEDTLS_PSK_MAX_LEN    16 /* 128-bits keys are generally enough */

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "platform_entropy_poll" source, but you may want to add other ones
 * Minimum is 2 for the entropy test suite.
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES 2

// add for iCloud feature
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECDH_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_GCM_C

#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define EXTEND_BUF_WITH_TERMINATE 
#define MBEDTLS_ECP_NIST_OPTIM
 
/* For test certificates */
#define MBEDTLS_BASE64_C
#define MBEDTLS_CERTS_C
#define MBEDTLS_PEM_PARSE_C

//#define CONFIG_MBEDTLS_HW_CRYPTO

#ifdef CONFIG_MBEDTLS_HW_CRYPTO
#define MBEDTLS_AES_ALT
#define MBEDTLS_DES_ALT
#define MBEDTLS_MD5_ALT
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT
//#define MBEDTLS_SHA512_ALT
#endif

#ifdef MTK_HOMEKIT_ICLOUD_16_TOKEN
#define MBEDTLS_SSL_MAX_CONTENT_LEN         (16*1024)   /**< Size of the input / output buffer */
#else
#define MBEDTLS_SSL_MAX_CONTENT_LEN         (4*1024)   /**< Size of the input / output buffer */
#endif
//#define MBEDTLS_SSL_MAX_CONTENT_LEN         (6*1024)   /**< Size of the input / output buffer */
//#define MBEDTLS_SSL_IN_CONTENT_LEN              1024
//#define MBEDTLS_SSL_OUT_CONTENT_LEN             1024
//#define MBEDTLS_SSL_MAX_CONTENT_LEN         (4*1024)   /**< Size of the input / output buffer */


#define MBEDTLS_AES_ROM_TABLES

#ifndef MTK_DEBUG_LEVEL_NONE
#define MBEDTLS_DEBUG_C
#endif

#define MBEDTLS_SELF_TEST

/* MTK revisions */
#define MBEDTLS_MTK

#define MBEDTLS_ECP_RESTARTABLE
#define MBEDTLS_ECDH_LEGACY_CONTEXT //used by BT mesh

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_H */
