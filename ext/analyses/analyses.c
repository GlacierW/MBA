#include "analyses.h"

#include <pthread.h>

#include "net/slirp.h"
#include "qmp-commands.h"

#include "../tsk/tsk.h"
#include "../dift/dift.h"
#include "../agent/agent.h"

#define API_SUCCEED() do{return 0;}while(0)
#define API_ERROR(x) do{analyses_errno = x; return -1;}while(0)
#define TASK_FAIL(x, y) do{tc->status = x; strncpy(tc->detail, y, MAX_DETAIL_LEN); agent_printf("analysis failed"); return NULL;}while(0)
#define TASK_SUCCEED() do{tc->status = ANALYSES_TASK_SUCCESS; agent_printf("analysis finished\n"); return NULL;}while(0)
#define MAX_DETAIL_LEN 100
#define MAX_PATH_LEN 256

int analyses_errno;

struct task_context {
    char detail[MAX_DETAIL_LEN+1];
    char report[MAX_REPORT_LEN+1];
    char host_sample_path[MAX_PATH_LEN+1];
    int status;
    pthread_t tid;
};
static struct task_context tc[1];

char const *analyses_strerr(int err_num) {
    static char const *err_strs[] = {
        "general fail",
        "agent not initialized",
        "analysis thread is busy now",
        "create thread failed",
        "dift not enabled",
    };
    return err_strs[err_num];
}

static void strstatus(char *buf, size_t bufsize) {
    static char const *status_strs[] = {
        "analysis succedded",
        "analysis is running",
        "analysis failed",
        "agent import function failed",
        "agent export function failed",
        "agent execute function failed",
        "agent invoke function failed",
    };
    strncpy(buf, status_strs[tc->status], bufsize);
    buf[bufsize-1] = '\0';
    bufsize -= 1+strlen(buf);
    if (bufsize > 0) {
        strncat(buf, tc->detail, bufsize);
    }
}

int analyses_get_result(struct analyses_task_result *result) {
    if (tc->status != ANALYSES_TASK_SUCCESS) {
        strstatus(result->report, MAX_REPORT_LEN);
    }
    else {
        strcpy(result->report, tc->report);
    }
    result->status = tc->status;
    API_SUCCEED();
}

static char const *get_ide0_hd0_image_path(void) {
    BlockInfoList *block_list, *info;

    block_list = qmp_query_block(NULL);

    for (info = block_list; info; info = info->next) {
        if(info->value->has_inserted)
        {
            if(strcmp(info->value->device, "ide0-hd0") == 0)
                return info->value->inserted->file;
        }
    }
    return NULL;
}

static int disk_block_size(void) {
    //XXX: now hardcoded, just for speed up
    return 4096;
}

static void* analyze_tainted_files(void *args) {
#define GUEST_DIR "C:\\users\\dsns\\"
#define GUEST_TSK_DIR "/users/dsns/"
#define GUEST_SAMPLE_PATH GUEST_DIR"sample.exe"
#define GUEST_SAMPLE_TSK_PATH GUEST_TSK_DIR"sample.exe"
#define GUEST_SYNC_PATH GUEST_DIR"sync.exe"
#define GUEST_SYNC_CMDLINE GUEST_SYNC_PATH" -accepteula"
    int sample_execute_time = 5;

    char const *image_path = get_ide0_hd0_image_path();
    UT_array *file_blocks;
    TSK_DADDR_T first, last, *addr_tuple;
    CONTAMINATION_RECORD tag = 0x1;

    uint64_t haddr;
    char *filename;
    UT_array *filenames;
    int report_remain_size = MAX_REPORT_LEN;
    char **p;

    enum MBA_AGENT_RETURN ret;

    if (image_path == NULL)
        TASK_FAIL(ANALYSES_TASK_FAIL, ": ide0-hd0 image path not found");

#define TRY_TILL_NOT_BUSY(x) do {x sleep(1);} while (ret == AGENT_RET_EBUSY)
    // import sample and sync
    TRY_TILL_NOT_BUSY({ret = agent_import(GUEST_SYNC_PATH, "sync.exe");});
    if (ret != AGENT_RET_SUCCESS)
        TASK_FAIL(ANALYSES_TASK_FAIL_IMPO, ": sync");

    TRY_TILL_NOT_BUSY({ret = agent_import(GUEST_SAMPLE_PATH, tc->host_sample_path);});
    if (ret != AGENT_RET_SUCCESS)
        TASK_FAIL(ANALYSES_TASK_FAIL_IMPO, ": sample");

    // flush change to disk
    TRY_TILL_NOT_BUSY({ret = agent_execute(GUEST_SYNC_CMDLINE);});
    if (ret != AGENT_RET_SUCCESS)
        TASK_FAIL(ANALYSES_TASK_FAIL_EXEC, ": sync (after import)");
    while (agent_is_exec()) {
        sleep(1);
    }

    // taint blocks that belongs to sample

    file_blocks = tsk_find_haddr_by_filename(image_path, GUEST_SAMPLE_TSK_PATH);
    if (file_blocks == NULL)
        TASK_FAIL(ANALYSES_TASK_FAIL, ": sample not found on disk");

    for (addr_tuple=(TSK_DADDR_T*)utarray_front(file_blocks);
            addr_tuple != NULL;
            addr_tuple = (TSK_DADDR_T*)utarray_next(file_blocks, addr_tuple))
    {
        first = addr_tuple[0];
        last = addr_tuple[1];

        filenames = tsk_get_filename_by_haddr(image_path, first);
        if (filenames == NULL) {
            continue;
        }

        for (p = (char**)utarray_front(filenames);
                p != NULL;
                p = (char**)utarray_next(filenames, p))
        {
            filename = *p;
            if (strcasecmp(filename, GUEST_SAMPLE_TSK_PATH) == 0) {
                dift_contaminate_disk_or(first, last-first+1, tag);
                break;
            }
        }
        utarray_free(filenames);
    }
    utarray_free(file_blocks);

    // invoke sample
    TRY_TILL_NOT_BUSY({ret = agent_invoke(GUEST_SAMPLE_PATH);});
    if (ret != AGENT_RET_SUCCESS)
        TASK_FAIL(ANALYSES_TASK_FAIL_INVO, "sample");

    // wait for a sample behavior
    sleep(sample_execute_time);

    // sync all changes to disk
    TRY_TILL_NOT_BUSY({ret = agent_execute(GUEST_SYNC_CMDLINE);});
    if (ret != AGENT_RET_SUCCESS)
        TASK_FAIL(ANALYSES_TASK_FAIL_EXEC, ": sync (after run sample)");
    while (agent_is_exec()) {
        sleep(1);
    }
#undef TRY_TILL_NOT_BUSY

    // generate human-readable report
    tc->report[0] = '\0';
    for (haddr = 0; haddr < HD_MAX_SIZE; haddr += disk_block_size()) {
        if (dift_get_disk_dirty(haddr) != tag)
            continue;
        filenames = tsk_get_filename_by_haddr(image_path, haddr);
        if (filenames == NULL) {
            agent_printf("no file at tainted byte [%llu]\n", haddr);
            continue;
        }

        for (p = (char**)utarray_front(filenames);
                p != NULL;
                p = (char**)utarray_next(filenames, p))
        {
            filename = *p;
            strncat(tc->report, filename, report_remain_size);
            report_remain_size -= strlen(filename);
            if (report_remain_size <= 0)
                break;
            
            strncat(tc->report, "\n", 1);
            report_remain_size -= 1;
        }
        utarray_free(filenames);
        if (report_remain_size <= 0)
            break;
    }

    TASK_SUCCEED();
}

int analyses_start_analyze_tainted_files(char const *host_sample_path) {
    pthread_t tid;
    int ret;

    if (!agent_is_ready())
        API_ERROR(ANALYSES_ERR_NOT_INIT);

    if (!dift_is_enabled())
        API_ERROR(ANALYSES_ERR_NO_DIFT);
    
    if (analyses_task_running())
        API_ERROR(ANALYSES_ERR_BUSY);

    // set status before the thread runs to prevent overwritting the result
    tc->status = ANALYSES_TASK_RUNNING;
    strncpy(tc->host_sample_path, host_sample_path, MAX_PATH_LEN);
    tc->host_sample_path[MAX_PATH_LEN] = '\0';
    ret = pthread_create(
            &tid,
            NULL,
            analyze_tainted_files,
            NULL
            );
    if (ret != 0) {
        tc->status = ANALYSES_TASK_FAIL;
        API_ERROR(ANALYSES_ERR_CREAT_THREAD);
    }

    tc->tid = tid;
    API_SUCCEED();
}

int analyses_task_running(void) {
    return tc->status == ANALYSES_TASK_RUNNING;
}

int analyses_scan_tainted_files(char *report) {
    char **p;
    uint64_t haddr;
    UT_array *filenames;
    char *filename;
    CONTAMINATION_RECORD clean_tag = 0x0;
    char const *image_path = get_ide0_hd0_image_path();
    int report_remain_size = MAX_REPORT_LEN;

    report[0] = '\0';
    for (haddr = 0; haddr < HD_MAX_SIZE; haddr += disk_block_size()) {
        if (dift_get_disk_dirty(haddr) == clean_tag)
            continue;
        filenames = tsk_get_filename_by_haddr(image_path, haddr);
        if (filenames == NULL) {
            agent_printf("no file at tainted byte [%llu]\n", haddr);
            continue;
        }

        for (p = (char**)utarray_front(filenames);
                p != NULL;
                p = (char**)utarray_next(filenames, p))
        {
            filename = *p;
            if (strstr(report, filename) != NULL)
                break;
            strncat(report, filename, report_remain_size);
            report_remain_size -= strlen(filename);
            if (report_remain_size <= 0)
                break;
            
            strncat(report, "\n", 1);
            report_remain_size -= 1;
        }
        utarray_free(filenames);
        if (report_remain_size <= 0)
            break;
    }
    API_SUCCEED();
}
