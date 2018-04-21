/*
 * pack_msg.h
 *
 *  Created on: Apr 12, 2018
 *      Author: VIJOY-PC
 */

#ifndef UTILS_PACK_MSG_H_
#define UTILS_PACK_MSG_H_

typedef enum{
    SENSOR_UV,
    SENSOR_PRESSURE
}data_src_t;

//message packet from sensor tasks to comm tasks
typedef struct{
    data_src_t source;
    float payload;
}msg_pack_t;

//message packet from client to server
typedef struct{
    uint32_t clientInfo;
    uint32_t clientStatus;

    float uv_payLoad;
    float pr_payLoad;
}client_pack_t;

#endif /* UTILS_PACK_MSG_H_ */
