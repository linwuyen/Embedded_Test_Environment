#ifndef PROTOCOL_PARSER_H
#define PROTOCOL_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// --- 設定區 (可根據不同協定修改) ---
#define MAX_PACKET_SIZE  64      // 最大封包長度
#define FRAME_HEADER_1   0x55    // 封包表頭 Byte 1
#define FRAME_HEADER_2   0xAA    // 封包表頭 Byte 2 (若只有一個 Header，可不用)

// --- 狀態機列舉 (State Enum) ---
// 這是讓程式碼「可讀性」變高的關鍵，不用 magic number
typedef enum {
    STATE_WAIT_HEADER_1,    // 等待表頭1 (0x55)
    STATE_WAIT_HEADER_2,    // 等待表頭2 (0xAA)
    STATE_WAIT_LEN,         // 等待長度
    STATE_WAIT_DATA,        // 接收資料內容
    STATE_WAIT_CHECKSUM     // 等待校驗碼
} ParserState_t;

// --- 回呼函式型別定義 (Callback Type) ---
// 解析成功後，呼叫此函數通知上層。
// payload: 資料內容指標, len: 資料長度
typedef void (*PacketReceivedCallback)(uint8_t *payload, uint16_t len);

// --- 解析器物件結構 (Context Struct) ---
// 這是核心結構，實現「高內聚」，所有解析需要的變數都在這
typedef struct {
    // 內部狀態 (Private)
    ParserState_t state;           // 目前狀態機走到哪
    uint8_t       rx_buffer[MAX_PACKET_SIZE]; // 暫存正在組裝的封包
    uint16_t      rx_index;        // 目前收到第幾個 byte
    uint16_t      expected_len;    // 預期這個封包有多長
    
    // 設定與介面 (Public/Config)
    PacketReceivedCallback on_packet_received; // 解析成功後的動作 (Function Pointer)

} ProtocolParser;

// --- API ---
void Parser_Init(ProtocolParser *parser, PacketReceivedCallback callback);
void Parser_InputByte(ProtocolParser *parser, uint8_t byte);

#endif // PROTOCOL_PARSER_H