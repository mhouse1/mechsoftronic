/*****************************************************
 * These are all word addresses. Please shift the
 * offset accordingly when using them in IORD or IOWR.
 * E.g.: Write to a 32-bit user_datain_0 port
 *   IOWR_32DIRECT(BASE, DATA_OUT_0 * 4, 0XFFFFFFFF)
 * Another example: Read a 16-bit edge capture reg
 *   Data0 = IORD_32DIRECT(SLAVE_TEMPLATE_0_BASE, DATA_IN_0 * 4);
 * 
*****************************************************/

#define DATA_IN_0 0x00
#define DATA_IN_1 0x10
#define DATA_IN_2 0x20
#define DATA_IN_3 0x30
#define DATA_IN_4 0x40
#define DATA_IN_5 0x50
#define DATA_IN_6 0x60
#define DATA_IN_7 0x70
#define DATA_IN_8 0x80
#define DATA_IN_9 0x90
#define DATA_IN_10 0xA0
#define DATA_IN_11 0xB0
#define DATA_IN_12 0xC0
#define DATA_IN_13 0xD0
#define DATA_IN_14 0xE0
#define DATA_IN_15 0xF0

#define DATA_OUT_0 0x01
#define DATA_OUT_1 0x11
#define DATA_OUT_2 0x21
#define DATA_OUT_3 0x31
#define DATA_OUT_4 0x41
#define DATA_OUT_5 0x51
#define DATA_OUT_6 0x61
#define DATA_OUT_7 0x71
#define DATA_OUT_8 0x81
#define DATA_OUT_9 0x91
#define DATA_OUT_10 0xA1
#define DATA_OUT_11 0xB1
#define DATA_OUT_12 0xC1
#define DATA_OUT_13 0xD1
#define DATA_OUT_14 0xE1
#define DATA_OUT_15 0xF1

#define IRQ_MASK_0 0x02
#define IRQ_MASK_1 0x12
#define IRQ_MASK_2 0x22
#define IRQ_MASK_3 0x32
#define IRQ_MASK_4 0x42
#define IRQ_MASK_5 0x52
#define IRQ_MASK_6 0x62
#define IRQ_MASK_7 0x72
#define IRQ_MASK_8 0x82
#define IRQ_MASK_9 0x92
#define IRQ_MASK_10 0xA2
#define IRQ_MASK_11 0xB2
#define IRQ_MASK_12 0xC2
#define IRQ_MASK_13 0xD2
#define IRQ_MASK_14 0xE2
#define IRQ_MASK_15 0xF2

#define EDGE_CAPTURE_0 0x03
#define EDGE_CAPTURE_1 0x13
#define EDGE_CAPTURE_2 0x23
#define EDGE_CAPTURE_3 0x33
#define EDGE_CAPTURE_4 0x43
#define EDGE_CAPTURE_5 0x53
#define EDGE_CAPTURE_6 0x63
#define EDGE_CAPTURE_7 0x73
#define EDGE_CAPTURE_8 0x83
#define EDGE_CAPTURE_9 0x93
#define EDGE_CAPTURE_10 0xA3
#define EDGE_CAPTURE_11 0xB3
#define EDGE_CAPTURE_12 0xC3
#define EDGE_CAPTURE_13 0xD3
#define EDGE_CAPTURE_14 0xE3
#define EDGE_CAPTURE_15 0xF3

#define PRIORITIZED_INTERRUPT_SRC 0x100
