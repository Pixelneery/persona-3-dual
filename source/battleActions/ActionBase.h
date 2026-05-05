#pragma once 
#include <nds.h>
#include <stdio.h>
#include <string>


struct ActionBase
{
    bool inProgress = false;
    u32 targetIndex = 0; 
    std::string name = "";

    virtual void execute() = 0;
    virtual bool update() = 0;
    virtual ~ActionBase() = default;
};
