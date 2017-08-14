
#ifndef __file_generate_h
#define __file_generate_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define MAX_DOWN_FILE  512
struct generate_list_s {
		char settle_date[9];
		int id;
		char url[128];
};

typedef struct generate_list_s generate_list_t;

typedef struct {
		int list;
		int fetch;
		generate_list_t download_list[MAX_DOWN_FILE];
} Generate_list;

int process();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __db_hander_h */
