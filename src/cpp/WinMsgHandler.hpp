#pragma once

#include "OsMsgHandler.hpp"

class WinMsgHandler : public OsMsgHandler
{
public:
    Result handleOsMsgs() override;
};
