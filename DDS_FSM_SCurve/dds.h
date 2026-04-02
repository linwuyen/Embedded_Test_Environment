/**
 * @file dds.h
 * @brief DDS 系統統一標頭檔 (All-in-One Header)
 * @details 此檔案整合所有 DDS 模組的標頭檔，讓 main.c 只需要 include 一次
 */
#ifndef DDS_H
#define DDS_H

// 基礎配置與定義
#include "modules/dds_config.h"
#include "modules/dds_defs.h"

// 核心模組
#include "modules/dds_core.h"

// 服務層
#include "modules/dds_service.h"

// IPC 通訊層 (僅雙核心需要)
#include "modules/dds_ipc.h"

#endif // DDS_H
