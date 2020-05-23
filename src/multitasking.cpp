
#include <multitasking.h>

using namespace tinyos;
using namespace tinyos::common;

Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));

    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;
    
    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;
    
    // cpustate->gs = 0;
    // cpustate->fs = 0;
    // cpustate->es = 0;
    // cpustate->ds = 0;
    
    // cpustate->error = 0;
    
    // cpustate->eip = 0;
    cpustate->eip = (uint32_t)entrypoint;
    cpustate->cs = gdt->CodeSegmentSelector();
    // cpustate->ss = 0;
    cpustate->eflags = 0x202;
    
}

Task::~Task()
{

}


TaskManager::TaskManager()
{
    numTask = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{

}

bool TaskManager::AddTask(Task* task)
{
    if(numTask >= 256){
        return false;
    }
    tasks[numTask++] = task;
    return true;
}

CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTask <= 0)
        return cpustate;

    if(currentTask >= 0)
        tasks[currentTask]->cpustate = cpustate;
    //有多个任务，上个任务处理后直接换
    if(++currentTask >= numTask)
        currentTask %= numTask;
    return tasks[currentTask]->cpustate;
}
