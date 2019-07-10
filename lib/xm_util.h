/*
 *
 *      Filename: xm_util.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:36:34
 * Last Modified: 2017-01-18 10:36:34
 */

#ifndef XM_UTIL_H
#define XM_UTIL_H

#define UNICODE_ERROR_CHARACTERS_MISSING    -1
#define UNICODE_ERROR_INVALID_ENCODING      -2
#define UNICODE_ERROR_OVERLONG_CHARACTER    -3
#define UNICODE_ERROR_RESTRICTED_CHARACTER  -4
#define UNICODE_ERROR_DECODING_ERROR        -5

#include "xm_mpool.h"
#include "xm_tables.h"

#define CREATEMODE_UNISTD ( S_IRUSR | S_IWUSR | S_IRGRP )
#define CREATEMODE ( XM_UREAD | XM_UWRITE | XM_GREAD )



extern char  *xm_utf8_unicode_inplace_ex(xm_pool_t *mp, unsigned char *input, long int input_len, int *changed);

extern int  xm_normalize_path_inplace(unsigned char *input, int len, int win, int *changed);

extern int  xm_parse_boolean(const char *input);

extern char  *xm_remove_quotes(xm_pool_t *mptmp, const char *input, int input_len);


extern char  *xm_remove_escape(xm_pool_t *mptmp, const char *input, int input_len);

extern int  xm_parse_name_eq_value(xm_pool_t *mp, const char *input, char **name, char **value);

extern char  *xm_url_encode(xm_pool_t *mp, char *input, unsigned int input_len, int *changed);

extern char  *xm_file_dirname(xm_pool_t *p, const char *filename);

extern char  *xm_file_basename(xm_pool_t *p, const char *filename);

extern int  xm_sql_hex2bytes_inplace(unsigned char *data, int len);

extern int  xm_hex2bytes_inplace(unsigned char *data, int len);

extern char  *xm_bytes2hex(xm_pool_t *pool, unsigned char *data, int len);

extern char  *xm_bytes2hex_malloc(unsigned char *data, int len);

extern int  xm_is_token_char(unsigned char c);

extern int  xm_remove_lf_crlf_inplace(char *text);

extern char  *xm_guess_tmp_dir(xm_pool_t *p);

extern char  *xm_current_logtime(xm_pool_t *mp);

extern char  *xm_current_logtime_with_buf(char *buf,size_t buf_size);

extern char  *xm_current_filetime(xm_pool_t *mp);

extern unsigned long xm_get_current_timems(void);

extern int  xm_mkstemp_ex(char *templat, int mode);

extern int  xm_mkstemp(char *templat);

extern char  *xm_strtolower_inplace(unsigned char *str);

extern char  *xm_log_escape_re(xm_pool_t *p, const char *text);

extern char  *xm_log_escape(xm_pool_t *p, const char *text);

extern char  *xm_log_escape_nq(xm_pool_t *p, const char *text);

extern char  *xm_log_escape_ex(xm_pool_t *p, const char *text, unsigned long int text_length);

extern char  *xm_log_escape_nq_ex(xm_pool_t *p, const char *text, unsigned long int text_length);

extern char  *xm_log_escape_hex(xm_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern char  *xm_log_escape_raw(xm_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern char  *xm_log_escape_nul(xm_pool_t *mp, const unsigned char *text, unsigned long int text_length);

extern int  xm_decode_base64_ext(char *plain_text, const unsigned char *input, int input_len);

extern unsigned char* xm_encode_base64(xm_pool_t *mp,const unsigned char *input,int input_len);

extern unsigned char* xm_encode_base64_malloc(const unsigned char *input,int input_len);

extern int  xm_convert_to_int(const char c);

extern int  xm_js_decode_nonstrict_inplace(unsigned char *input, long int input_len);


extern int  xm_urldecode_nonstrict_inplace_ex(unsigned char *input, long int input_length, int *invalid_count, int *changed);

extern int  xm_html_entities_decode_inplace(xm_pool_t *mp, unsigned char *input, int len);

extern int  xm_ansi_c_sequences_decode_inplace(unsigned char *input, int len);


extern char  *xm_resolve_relative_path(xm_pool_t *pool, const char *parent_filename, const char *filename);

extern int  xm_css_decode_inplace(unsigned char *input, long int input_len);

extern unsigned char xm_is_netmask_v4(char *ip_strv4);

extern unsigned char xm_is_netmask_v6(char *ip_strv6);

extern int  xm_headers_to_buffer(const xm_array_header_t *arr, char *buffer, int max_length);

extern int xm_read_line(char *buff, int size, FILE *fp);

extern int xm_dir_make_r(const char *orig_path);

static inline int xm_port_equal(uint16_t src_port,uint16_t dst_port,uint16_t v){

    return src_port == v || dst_port == v;
}

static inline int xm_ports_equal(uint16_t *ports,int n,uint16_t src_port,uint16_t dst_port){

    int i ;

    uint16_t v;

    for(i= 0;i<n;i++){
    
        v = ports[i];
		
		if(v == 0)
			break;

        if(xm_port_equal(src_port,dst_port,v))
            return 1;
        
    }

    return 0;
}
#endif /* XM_UTIL_H */
