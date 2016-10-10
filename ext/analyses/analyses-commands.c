#include "analyses-commands.h"

#include "qemu-common.h"
#include "monitor/monitor.h"

#include "analyses.h"

void do_get_analysis_result(Monitor *mon, const QDict *qdict) {
    int ret;
    struct analyses_task_result result;

    ret = analyses_get_result(&result);
    if (ret == -1) {
        monitor_printf(mon, "%s\n", analyses_strerr(analyses_errno));
        return;
    }

    monitor_printf(mon, "report:\n------------------\n%s\n------------------\n", result.report);
}

#include "../agent/agent.h"

void do_analyze_tainted_files(Monitor *mon, const QDict *qdict) {
    char const * sample_path = qdict_get_str(qdict, "sample_path");
    int ret;

    ret = analyses_start_analyze_tainted_files(sample_path);
    if (ret == -1) {
        monitor_printf(mon, "analyses: %s\n", analyses_strerr(analyses_errno));
        return;
    }
    monitor_printf(mon, "analysis started\n");
}

void do_scan_tainted_files(Monitor *mon, const QDict *qdict) {
    char report[MAX_REPORT_LEN+1];
    analyses_scan_tainted_files(report);
    monitor_printf(mon, "tainted files:\n%s\n", report);
}
