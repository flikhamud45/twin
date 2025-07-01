#include <windows.h>
#define DEFAULT_PORT "12345"

enum class WinapiError
{
    winapiError = 1,
    wsaError,
    otherError
};

const char* lastWinapiFunction = "";
int lastError = 0;

BOOL wsaInitiated = FALSE;