#ifdef _DEBUG_
#define PRINT_DEBUG 1
#else 
#define PRINT_DEBUG 0
#endif

#define DEBUG(fmt, ...) do { if(PRINT_DEBUG) \
   fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__,\
           __FUNCTION__,__VA_ARGS__); } while(0)

/*#define DEBUG_NA(...) do { if(PRINT_DEBUG) \
   fprintf(stderr,__VA_ARGS__); } while(0)
*/
#define DEBUG_NA(...) do { if(PRINT_DEBUG) \
   fprintf(stderr, "%s:%d:%s(): %s", __FILE__, __LINE__,\
           __FUNCTION__,__VA_ARGS__); } while(0)
