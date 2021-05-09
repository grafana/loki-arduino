#ifndef Util_h
#define Util_h


#define LOKI_DEBUG
#define LOKI_SERIAL Serial

#ifdef LOKI_DEBUG
#define LOKI_DEBUG_PRINT(...)           \
    {                                   \
        LOKI_SERIAL.print(__VA_ARGS__); \
    }
#define LOKI_DEBUG_PRINTLN(...)            \
    {                                     \
        LOKI_SERIAL.println(__VA_ARGS__); \
    }
#define LOKI_DEBUG_PRINTF(...)           \
    {                                    \
        LOKI_SERIAL.printf(__VA_ARGS__); \
    }
#else
#define LOKI_DEBUG_PRINT(...) \
    {                         \
    }
#define LOKI_DEBUG_PRINTLN(...) \
    {                           \
    }
#define LOKI_DEBUG_PRINTF(...) \
    {                          \
    }
#endif


#endif