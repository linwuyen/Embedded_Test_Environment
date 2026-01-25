#include "protocol_parser.h"

// 初始化
void Parser_Init(ProtocolParser *parser, PacketReceivedCallback callback) {
    if (parser == NULL) return;

    parser->state = STATE_WAIT_HEADER_1;
    parser->rx_index = 0;
    parser->expected_len = 0;
    parser->on_packet_received = callback;
}

// 簡單的校驗和計算 (可替換成 CRC16 或 CRC32)
static uint8_t CalculateChecksum(uint8_t *data, uint16_t len) {
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum; // 簡單範例：回傳累加和 (Sum Check)
}

// 核心：狀態機引擎 (State Machine Engine)
// 每次只餵入一個 Byte，絕對不會阻塞 (O(1) 複雜度)
void Parser_InputByte(ProtocolParser *parser, uint8_t byte) {
    if (parser == NULL) return;

    switch (parser->state) {
        
        // 狀態 1: 等待第一個 Header (0x55)
        case STATE_WAIT_HEADER_1:
            if (byte == FRAME_HEADER_1) {
                parser->state = STATE_WAIT_HEADER_2;
            }
            // 如果不是 Header，就忽略，繼續等 (形同丟棄雜訊)
            break;

        // 狀態 2: 等待第二個 Header (0xAA)
        case STATE_WAIT_HEADER_2:
            if (byte == FRAME_HEADER_2) {
                parser->state = STATE_WAIT_LEN;
            } else {
                // 如果第二個不是 0xAA，可能剛才那個 0x55 只是雜訊，回到原點
                // (更嚴謹的寫法是檢查這個 byte 是否為 header 1)
                parser->state = STATE_WAIT_HEADER_1; 
                if (byte == FRAME_HEADER_1) parser->state = STATE_WAIT_HEADER_2; 
            }
            break;

        // 狀態 3: 等待長度 (Length)
        case STATE_WAIT_LEN:
            // 假設協定規定：長度 byte 代表「後面的 Payload 長度」
            // 這裡要做防呆，避免長度爆掉
            if (byte > MAX_PACKET_SIZE) {
                parser->state = STATE_WAIT_HEADER_1; // 長度不合理，重置
            } else {
                parser->expected_len = byte;
                parser->rx_index = 0; // 準備開始存 Payload
                
                if (parser->expected_len == 0) {
                    // 如果長度是 0，直接跳去驗證 Checksum (看協定設計)
                    parser->state = STATE_WAIT_CHECKSUM;
                } else {
                    parser->state = STATE_WAIT_DATA;
                }
            }
            break;

        // 狀態 4: 接收資料本體 (Payload)
        case STATE_WAIT_DATA:
            parser->rx_buffer[parser->rx_index++] = byte;
            
            // 收滿了預期的長度，往下個狀態走
            if (parser->rx_index >= parser->expected_len) {
                parser->state = STATE_WAIT_CHECKSUM;
            }
            break;

        // 狀態 5: 檢查校驗碼 (Checksum)
        case STATE_WAIT_CHECKSUM:
            {
                // 計算剛剛收到的資料的 Checksum
                uint8_t cal_sum = CalculateChecksum(parser->rx_buffer, parser->expected_len);
                
                // 比對收到的 Checksum (byte) 和 算出來的 (cal_sum)
                if (cal_sum == byte) {
                    // 校驗成功！呼叫 Callback 通知上層
                    if (parser->on_packet_received != NULL) {
                        parser->on_packet_received(parser->rx_buffer, parser->expected_len);
                    }
                } else {
                    // 校驗失敗 (Checksum Error)，丟棄封包
                    // 可以在這裡加一個 Error Callback
                }
                
                // 無論成功失敗，都回到起點等待下一個封包
                parser->state = STATE_WAIT_HEADER_1;
            }
            break;
            
        default:
            parser->state = STATE_WAIT_HEADER_1;
            break;
    }
}