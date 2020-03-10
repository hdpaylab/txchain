#ifndef ECCAUTOINITRELEASEHANDLER_H
#define ECCAUTOINITRELEASEHANDLER_H

#include <memory>

class ECCVerifyHandle;
class EccAutoInitReleaseHandler
{
public:
	static void initEcc();

private:
    EccAutoInitReleaseHandler();

    ~EccAutoInitReleaseHandler();

private:
    std::unique_ptr<ECCVerifyHandle> verifyHandle;
};


#endif // ECCAUTOINITRELEASEHANDLER_H
