/*!
 * @copyright	COPYRIGHT NOTIFICATION (c) 2013 Rockwell Automation, Inc. All rights reserved.
 * @file		crc.h
 * @date		Aug 27, 2014 12:51:55 PM
 * @author		Michael House
 * @version		Initial file
 */

#ifndef CRC_H_
#define CRC_H_

typedef unsigned long	ULONG;
ULONG crc32(char *data, int len, ULONG crc);

#endif /* CRC_H_ */
