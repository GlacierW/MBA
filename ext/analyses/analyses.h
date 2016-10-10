#ifndef __ANALYSES_H_
#define __ANALYSES_H_
#include <stddef.h>

#define MAX_REPORT_LEN 4096

enum MBA_ANALYSES_ERRNO {
    ANALYSES_ERR_FAIL,
    ANALYSES_ERR_NOT_INIT,
    ANALYSES_ERR_BUSY,
    ANALYSES_ERR_CREAT_THREAD,
    ANALYSES_ERR_NO_DIFT,
};
extern int analyses_errno;

enum MBA_ANALYSES_TASK_STATUS {
    ANALYSES_TASK_SUCCESS,
    ANALYSES_TASK_RUNNING,
    ANALYSES_TASK_FAIL,
    ANALYSES_TASK_FAIL_IMPO,
    ANALYSES_TASK_FAIL_EXPO,
    ANALYSES_TASK_FAIL_EXEC,
    ANALYSES_TASK_FAIL_INVO,
};

struct analyses_task_result{
    int status;
    char report[MAX_REPORT_LEN+1];
};

extern char const *analyses_strerr(int err_num);

extern int analyses_get_result(struct analyses_task_result* result);

extern int analyses_start_analyze_tainted_files(char const *host_path);

extern int analyses_task_running(void);

extern int analyses_scan_tainted_files(char *report);

#endif
