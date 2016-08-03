#define exit(x) mock_ptr->exit(x)
#define getpagesize() mock_ptr->getpagesize()
#define calloc(x, y) mock_ptr->calloc(x, y)

#define dift_log(x, ...) mock_ptr->dift_log(x)
