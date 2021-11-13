#pragma once
#ifndef _EXECUTION_REQUEST_H
#define _EXECUTION_REQUEST_H

typedef enum ExecutionStatus
{
    WAITING,
    RUNNING,
    ENDED
};

typedef struct ExecutionRequest
{
    unsigned int idExecution = 0;
    unsigned int requestedBy = 0;
    ExecutionStatus status;
    int outId = 0;
    int targetValue = 0;
};

#endif