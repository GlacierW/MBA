// Library functions
#define exit(x) mock_ptr->(x)
#define pthread_create(x, y, z, a) mock_ptr->pthread_create(x, y, z, a)
#define fopen(x, y) mock_ptr->fopen(x, y)
#define write(x, y, z) mock_ptr->write(x, y, z)
#define read(x, y, z) mock_ptr->read(x, y, z)

// Static functions
#define agent_cleanup() mock_ptr->agent_cleanup()
#define as_write(x, y, z) mock_ptr->as_write(x, y, z)
#define as_read(x, y, z) mock_ptr->as_read(x, y, z)
#define import_host_file() mock_ptr->import_host_file()
#define export_guest_file() mock_ptr->export_guest_file()
#define execute_guest_cmd_return() mock_ptr->execute_guest_cmd_return()
#define execute_guest_cmd_noreturn() mock_ptr->execute_guest_cmd_noreturn()
#define export_agent_log() mock_ptr->export_agent_log()
#define connect_agent_server() mock_ptr->connect_agent_server()

// Public API
#define agent_is_ready() mock_ptr->agent_is_ready()
#define agent_is_exec() mock_ptr->agent_is_exec()
#define agent_import(x, y) mock_ptr->agent_import(x, y)
#define agent_export(x, y) mock_ptr->agent_export(x, y)
#define agent_execute(x) mock_ptr->agent_execute(x)
#define agent_invoke(x) mock_ptr->agent_invoke(x)
#define agent_logfile(x) mock_ptr->agent_logfile(x)
#define agent_init(x, y) mock_ptr->agent_init(x, y)
