#ifndef __TASKS_H
#define __TASKS_H

/**
 * @brief  Initialize all application modules
 */
void Tasks_Init(void);

/**
 * @brief  Process all modules in round-robin (cooperative multitasking)
 */
void Tasks_Process(void);

#endif /* __TASKS_H */
