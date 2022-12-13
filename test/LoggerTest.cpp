#include "../webssh/Logger.h"
#include "../webssh/Buffer.h"

int main()
{
    Logger* logger = Logger::Instance();
    logger->init(0);
    for(int i=0; i<10; i++){
        LOG_INFO("%d\n", i);
    }
    // while(true);
    return 0;
}