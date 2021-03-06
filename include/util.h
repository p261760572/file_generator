#include "base.h"
#include "tpos.h"
#ifndef __UTIL_H__
#define __UTIL_H__
int update_pub_field(glob_msg_stru * pub_data_stru,short id,int data_len,const char *data,char from ,char *control_info);
message_define *match_priv_stru(const char *msg_type);
int tpos_unpack(const char *msg_type, message_define *priv_def,char *src_buf,int src_len,glob_msg_stru *pub_data_stru);
int get_msg_key(glob_msg_stru * pub_data_stru);
int conver_data(glob_msg_stru * pub_data_stru, int flag);
int db_insert(glob_msg_stru * pub_data_stru,int flag);
int add_pub_field(glob_msg_stru * pub_data_stru,short id,int data_len,const char *data,char from,char *control_info);
int tpos_pack(const char *msg_type, message_define *priv_def,glob_msg_stru *pub_data_stru,char *buf);
int folder_to_insti_code(char *insti_code);
int iso_unpack(const char *msg_type, message_define *priv_def,char *src_buf,int src_len,glob_msg_stru *pub_data_stru);
int iso_pack(const char *msg_type, message_define *priv_def,glob_msg_stru *pub_data_stru,char *buf);
int pack_msg(const char *msg_type, message_define *priv_def,glob_msg_stru *pub_data_stru,char *buf);
int unpack_msg(  message_define *priv_def,const char *msg_type,char *src_buf,int src_len,glob_msg_stru *pub_data_stru);
int get_insti_code(glob_msg_stru *pub_data_stru);
int get_msg_intout_flag(glob_msg_stru * pub_data_stru);
int get_fold_id( const char *insti_code);
int get_route_insti_code(glob_msg_stru * pub_data_stru);
int err_set_msg(glob_msg_stru *pub_data_stru);
int check_and_match(glob_msg_stru *pub_data_stru);
void void_proc(glob_msg_stru *pub_data_stru);
int direct_respose(glob_msg_stru *pub_data_stru);
int genrate_field_conver(glob_msg_stru *pub_data_stru);
int priv_field_conver(glob_msg_stru * pub_data_stru);
int confirm_app_type(glob_msg_stru *pub_data_stru);
int _get_route_insti_code(glob_msg_stru *pub_data_stru);
int check_app_limit(glob_msg_stru *pub_data_stru);
int get_next_insti(glob_msg_stru *pub_data_stru);
int load_db_trans_info(glob_msg_stru *pub_data_stru);
int get_msgtype_total(void);
int load_priv_def(void);
int get_insti_info(glob_msg_stru * pub_data_stru);
int folder_to_insti_code(char *insti_code);
int get_msg_key(glob_msg_stru * pub_data_stru);
int get_route_trans_info(glob_msg_stru * pub_data_stru);
int get_trans_info_from_insticode(glob_msg_stru * pub_data_stru);
int get_trans_info_from_msg(glob_msg_stru * pub_data_stru);
int db_field_conver(glob_msg_stru * pub_data_stru);
int get_easy_route(glob_msg_stru * pub_data_stru);
int get_terminal_route(glob_msg_stru * pub_data_stru);
int get_bin_route(glob_msg_stru * pub_data_stru);
int get_route_trans_set(glob_msg_stru * pub_data_stru);
field_define* get_field_def(short id,const message_define *priv_def);
int get_pub_field_id(const char *msg_type,const char *name);
int get_field_data(int field_id,glob_msg_stru *pub_data_stru,char *data);
void rtrim( char * str);
int tpos_check_mac(glob_msg_stru * pub_data_stru);
int tpos_check_terminal(glob_msg_stru * pub_data_stru);
int tpos_check_prog_ver(glob_msg_stru * pub_data_stru);
int tpos_check_menu_ver(glob_msg_stru * pub_data_stru);
int tpos_login(glob_msg_stru * pub_data_stru);
int tpos_download_menu(glob_msg_stru * pub_data_stru);
int tpos_download_para(glob_msg_stru * pub_data_stru);
int tpos_link_test(glob_msg_stru * pub_data_stru);
field_define* get_priv_field_def(const char *name,short *id,const message_define *priv_def);
field_define* get_priv_field_def_for_id(short id,const message_define *priv_def);
int xml_pack(const char *msg_type,  message_define *priv_def, glob_msg_stru *pub_data_stru, char *buf);
int xml_unpack(const char *msg_type,  message_define *priv_def, char *src_buf, int src_len, glob_msg_stru *pub_data_stru);
int pin_change(char *para, short fldid, glob_msg_stru *pub_data_stru);
int specific_business_handle(char *para, short fldid, glob_msg_stru *pub_data_stru);
int general_buisiness_handle(char *para, short fldid, glob_msg_stru *pub_data_stru);
int card_info_check(char *para, short flag, glob_msg_stru *pub_data_stru);
int iso_check_mac_all_des(char *para, short flag, glob_msg_stru *pub_data_stru);
int iso_check_mac_all_3des(char *para, short flag, glob_msg_stru *pub_data_stru);
int mac_calc_all_3des(char *para, char *buf, int bufLen, char *mac, glob_msg_stru *pub_data_stru, int flag);
int mac_calc_all_des(char *para, char *buf, int bufLen, char *mac, glob_msg_stru *pub_data_stru, int flag);
int get_seq(char *para, short fldid, glob_msg_stru *pub_data_stru);
int get_ref(char *para, short fldid, glob_msg_stru *pub_data_stru);
int get_systime(char *para, short fldid, glob_msg_stru *pub_data_stru);
char *my_strtok(char *s1, const char s2);
int db_tpos_insert(glob_msg_stru *pub_data_stru,int flag);
int get_db_data(char *para, short fldid, glob_msg_stru *pub_data_stru);
int update_db_pay_app(char *para, short flag, glob_msg_stru *pub_data_stru);
int update_db_app_pay(char *para, short flag, glob_msg_stru *pub_data_stru);
int update_db_pay_ret(char *para, short flag, glob_msg_stru *pub_data_stru);
int update_db_app_ret(char *para, short flag, glob_msg_stru *pub_data_stru);
int get_default(char *para, short fldid, glob_msg_stru *pub_data_stru);
int tpos_field_pre_conv(char *para, short flag, glob_msg_stru *pub_data_stru);
int tpos_gen_field_conver(char *para, short fldid, glob_msg_stru *pub_data_stru);
int _tpos_check_terminal(char *para, short flag, glob_msg_stru *pub_data_stru);
int get_tpos_data(char *para, short fldid, glob_msg_stru *pub_data_stru);
int db_tpos_query(glob_msg_stru *pub_data_stru);
int update_db_tpos(char *para, short flag, glob_msg_stru *pub_data_stru);
int _tpos_check_mac(char *para, short flag, glob_msg_stru *pub_data_stru);
int _tpos_get_work_key( glob_msg_stru *pub_data_stru);
int tpos_reversed(glob_msg_stru * pub_data_stru);
int app_reversed(glob_msg_stru * pub_data_stru);
int print_format(char *para, short fldid, glob_msg_stru *pub_data_stru);
int get_org_trans_info(char *para, short fldid, glob_msg_stru *pub_data_stru);
int get_msg_data(char *para, short fldid, glob_msg_stru *pub_data_stru);
int make_data(char *para, short fldid, glob_msg_stru *pub_data_stru);
int reversed_replay(glob_msg_stru * pub_data_stru);
int set_field_service_entry(char *para, short fldid, glob_msg_stru *pub_data_stru);
int ic_para_data(char *para, short fldid, glob_msg_stru *pub_data_stru);
int check_replay_cd(char *para, short flag, glob_msg_stru *pub_data_stru);
int find_replay(glob_msg_stru * pub_data_stru);
int mac_calc_cbc_iso_3des(char *para, char *src_buf, int src_len, char *mac, glob_msg_stru *pub_data_stru, int flag);
int iso_check_mac_cbc_3des(char *para, short flag, glob_msg_stru *pub_data_stru);
int iso_cbc_str(char *para, char *macBuf, int *mac_len, char * field_name, const message_define *p_def, const char *buf, glob_msg_stru *pub_data_stru, int len_type);
int sign_request(glob_msg_stru * pub_data_stru);
int sign_m_p_32(glob_msg_stru * pub_data_stru);
int save_timeout(char *para, short fldid, glob_msg_stru *pub_data_stru);
int read_timeout(char *para, short fldid, glob_msg_stru *pub_data_stru);
int trans_cancle(char *para, short flag, glob_msg_stru *pub_data_stru);
int iso_tl_head_proc ( char *buf ,int start, int len);
int show_format(char *para, short fldid, glob_msg_stru *pub_data_stru);
//int save_addidata(char *para, short fldid, glob_msg_stru *pub_data_stru);
int bill_pay(glob_msg_stru *pub_data_stru);
int bill_fee_find(glob_msg_stru *pub_data_stru);
int tpos_get_last_addidata(char *para, short flag, glob_msg_stru * pub_data_stru);
int set_ctrl_info(char *para, short fldid, glob_msg_stru *pub_data_stru);
int calculate_fee(char *para, short flag, glob_msg_stru *pub_data_stru);
int electricity_payment_48(char *para, short fldid, glob_msg_stru *pub_data_stru);
int trans_control(char *para, short flag, glob_msg_stru *pub_data_stru);
int end_trans(glob_msg_stru *pub_data_stru);
int get_end_trans(char *para, short flag, glob_msg_stru *pub_data_stru);
int tpos_trans_cancle(char *para, short flag, glob_msg_stru *pub_data_stru);
int tpos_trans_refund(char *para, short flag, glob_msg_stru *pub_data_stru);
int ic55_ret(char *para, short fldid, glob_msg_stru *pub_data_stru);

int fdc_notify(char *para, char *inst_code,glob_msg_stru *pub_data_stru);
int query_expenses(glob_msg_stru * pub_data_stru);
int check_expenses_result(char *para, short fldid, glob_msg_stru *pub_data_stru);
int insert_expenses (char *para, char *inst_code,glob_msg_stru *pub_data_stru);
int key_reset(glob_msg_stru * pub_data_stru);
int get_msg_data_fill(char *para, short fldid, glob_msg_stru *pub_data_stru);
int get_data_standard(char *para, short fldid, glob_msg_stru *pub_data_stru);
int set_service_code(char *para, short fldid, glob_msg_stru *pub_data_stru);
int result_query_tl(glob_msg_stru * pub_data_stru);
int format_echo_input(char *para, short fldid, glob_msg_stru *pub_data_stru);
int tpos_query_expenses(glob_msg_stru *pub_data_stru);
int tpos_check_expenses_result(char *para, short flag, glob_msg_stru *pub_data_stru);
int tpos_discount_result(char *para, short flag, glob_msg_stru *pub_data_stru);
int set_field_60(char *para, short flag, glob_msg_stru *pub_data_stru);
int change_db_data(char *para, short fldid, glob_msg_stru *pub_data_stru);
int save_tc_value(glob_msg_stru * pub_data_stru);
int _get_field_data_safe(short field_id,glob_msg_stru *pub_data_stru,char *data, char flag,int size);
int get_field_data_safe(int field_id,glob_msg_stru *pub_data_stru,char *data,int size);
int AddUpTrans(char *cardno, int *amount, int *count, char *app_type, int flag);
int get_route_insti_info(glob_msg_stru * pub_data_stru);
void dcs_log(void *ptrbytes, int nbytes,const char * message,...);
int check_route_insti(glob_msg_stru *pub_data_stru);
int CalcFee(char *caFeeType, char *caFee, int amount, char *caKee);
int  fold_locate_folder(const char *folder_name);
int fold_write(int dest_folderId,int org_folderId,void *user_data,int nbytes);
int cacl_discount_1(char *amount,char *para,glob_msg_stru *pub_data_stru);
int update_db_result_pay(tl_trans_log_def *log,char *ret_code);
int get_term_discount_info(DISCNT_INFO *discnt_info);
int update_db_voidflag(tl_trans_log_def *log, char flag, char *ret_code);
int zpos_gen_work_key(glob_msg_stru *pub_data_stru,const char *psam);
int tpos_get_work_key(const char *psam,char *pin_indx,char *mac_indx,char * data_indx,char* pin_key,char *mac_key,char * data_key);
int get_tpos_info(char *psam,struct TPOS_TERM_INFO *terminfo);
int get_aid_data(char *buf, int bSize, char *node_set, int *last_ic_para_step, int *cnt);
int db_save_set_ic( char *node_set,int step,char *psam, int flag);
int count_aid(int step);
int get_pubkey_data(char *buf, int bSize, char *node_set, int *last_ic_para_step, int *cnt);
int count_pubkey( int step);
int empty_tmp_para( char *psam);
int get_ic_data(char *buf, int bSize, char *node_set, int *cnt);
int tpos_reset_download(char *psam, char flag);
int del_pub_field(glob_msg_stru * pub_data_stru,short id,char from );
int zpos_gen_work_key(glob_msg_stru *pub_data_stru,const char *psam);
int tpos_update_work_key(char *psam, char *pin_key,char *mac_key,char *cd_key);
void bcd_to_asc (unsigned char* ascii_buf ,unsigned char* bcd_buf , int conv_len ,unsigned char type );
int check_card_limit(char *card_no,char *iss_insti, char *flag);
int insti_login(glob_msg_stru * pub_data_stru);
int insti_gen_work_key(glob_msg_stru *pub_data_stru);
int result_query_hnyl(glob_msg_stru * pub_data_stru);
int get_msg_data_tlvfill(char *para, short fldid, glob_msg_stru *pub_data_stru);
int verify_passwd(char *para, short fldid, glob_msg_stru *pub_data_stru);
int get_db_data_fill(char *para, short fldid, glob_msg_stru *pub_data_stru);
int con_del_fld48(char *para, short fldid, glob_msg_stru *pub_data_stru);
int check_amount(char *para, short fldid, glob_msg_stru *pub_data_stru);
int check_cardbin(char *para, short fldid, glob_msg_stru *pub_data_stru);
int check_cntuser_curdate(char *para, short fldid, glob_msg_stru *pub_data_stru);
int check_valid_date(char *para, short fldid, glob_msg_stru *pub_data_stru);
int check_cnt_mer(char *para, short fldid, glob_msg_stru *pub_data_stru);
int cancle_trans_specific(char *para, short fldid, glob_msg_stru *pub_data_stru);
int is_valid_cardbin(char *para, short fldid, glob_msg_stru *pub_data_stru);
int tpos_settle(glob_msg_stru * pub_data_stru);
#endif
