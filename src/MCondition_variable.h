//
// Created by FDC on 2021/12/7.
//

#ifndef MTHREAD_MCONDITION_VARIABLE_H
#define MTHREAD_MCONDITION_VARIABLE_H
#include "Types.h"
#include "Mutex.h"

class MCondition_variable
{
public:
    MCondition_variable();
    ~MCondition_variable();

    MCondition_variable(MCondition_variable&) = delete;
    MCondition_variable& operator=(MCondition_variable&) = delete;

    void notify_all() noexcept;
    void notify_one() noexcept;

    void wait()
};
#endif //MTHREAD_MCONDITION_VARIABLE_H
