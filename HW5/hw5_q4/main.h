/*
 * main.h
 *
 *  Created on: Apr 6, 2018
 *      Author: VIJOY-PC
 */

#ifndef MAIN_H_
#define MAIN_H_

#define SYSTEM_CLOCK            (120000000)
#define TIMER_PERIOD            (pdMS_TO_TICKS(250))    //250 ms
#define SIGNAL_QUEUE_LENGTH     (1)

#define TASK1_RATE               (2)
#define TASK2_RATE               (4)

#define TOGGLE_LED               (0x00000001)
#define LOG_STRING               (0x00000002)

#define STRING_QUEUE_LENGTH      (100)


#endif /* MAIN_H_ */
