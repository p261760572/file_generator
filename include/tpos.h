#ifndef __TPOS_H__
#define __TPOS_H__


struct TPOS_TERM_INFO {
	char psam[16+1];
	char status[1+1];    //�ն�״̬ 0,�رգ�1��
	char menu_ver[8+1];  //�˵��汾��
	char tel_no[16+1];   //�����IP��ַ
	char comm_type[1+1];        //�ն�ͨ����������
	char is_bind[1+1];         //�Ƿ���󶨹�ϵ
	char menu_update_flag[1+1]; //1��Ҫ���� ,0����Ҫ����
	char download_flag[1+1] ;//ȫ�������ر�ʶ,0���ֲ˵����£�1ȫ�˵�����
	int  last_menu_para_step; //�ϴ����ص�ĩβ�˵����²��裻0Ϊ��ʼ״̬��δ�����ز˵�
	int  last_other_para_step;
	int  last_ic_para_step;
	int  last_ic_key_step;
	char name[60+1];
	char last_node_set[200+1]; //�ϴ����صĲ˵��ڵ㼯��
	char stack_detail[64+1]; //�˵����ص�ջ������
	char settle_time[14+1];
};
struct TPOS_MENU_NODE {
	char menu_ver[8+1];//�˵��汾��
	int node_id;//�˵���id
	int pre_node_id;//ǰ���
	int right_node_id;
	int left_node_id;
	char is_leaf[1+1];//Ҷ����־ 0��ʾ��Ҷ��� 1��ʾ��Ҷ���
	char pre_flag[1+1];//ǰ����־ 0Ϊǰ�������� 1Ϊǰ�����ҽ��
};
struct MENU_NODE_INFO {
	int node_id;
	char func_code[3+1];
	char void_flag[1+1];
	int func_disp_indx;
	char title [32+1];
	char op_code[128+1];
	char use[1+1];
	int step;
};
struct TPOS_PARA_INFO {
	char para_type[2+1];
	int id;
	char ver[4];
	int step;
	char detail[300+1];
//	int last_op;//0�޸�1����2ɾ��
  char control_info[20+1];
  char use[1+1];
	int last_step;
};

struct STACK {
	int use;
	int max;
	struct TPOS_MENU_NODE nodes[30];
};
typedef struct
{
	char para_type[3];
	int id;
}para_set_stru;
//int db_init_stack(char *psam,int *stack[]);
//int db_pop(int *stack[30] , int *id,int *use);
//int db_push(int *stack[30],int id,int *use);

//���ݿ�����ṹ
/*typedef struct
{
   char sys_date             [8 + 1];
   char sys_time             [6 + 1];
   char qs_date              [8 + 1];
   char acq_insti_code       [11 + 1];
   char app_insti_code       [11 + 1];
   char acq_msg_type         [4 + 1];
   char acq_trans_type       [4 + 1];
   char app_type             [4 + 1];
   char app_msg_type         [4 + 1];
   char app_trans_type       [4 + 1];
   char resp_cd_app          [6 + 1];
   char pay_acct_no          [30 + 1];
   char card_attr            [2 + 1];
   char iss_insti_code       [8 + 1];
   char amount_pay           [12 + 1];
   char amount_real          [12 + 1];
   char fee                  [12 + 1];
   char acq_tra_no           [6 + 1];
   char app_tra_no           [6 + 1];
   char app_date             [8 + 1];
   char app_time             [6 + 1];
   char psam                 [16+1];
   char tel_no               [20 + 1];
   char tpdu                 [10+1];
   char msg_seq				 [6 + 1];
   char prog_ver             [4+1];
   char menu_ver             [8+1];
   char acq_mac              [16+1];
   char acq_addition         [256 + 1];
   char app_addition         [256 + 1];
   char sys_ref_no           [12 + 1];
   char pos_entry_md_cd      [3 + 1];
   char rcv_acct_no          [30 + 1];
   char trans_curr_cd        [3 + 1];
   char resp_cd_auth_id      [6 + 1];
   char void_flag            [1 + 1];
   char permit_void          [1 + 1];
   char step                 [1 + 1];
   char resp_cd_rcv			 [6 + 1];
   char s_random	         [10 + 1];
   char func_code            [3+1];
}tl_tpos_log_def;
*/
#endif
