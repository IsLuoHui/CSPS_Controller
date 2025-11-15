#include "uCI2C.h"
#include <stdio.h>

uint8_t uC_I2C_Addr=0xBE;
extern I2C_HandleTypeDef hi2c2;

/**
 * @brief 计算二进制补码校验和
 * @param bytes 输入字节数组
 * @param len   字节数
 * @return 校验和 (uint8_t)
 * sum + checksum = 0 (mod 256)
 */
static uint8_t uC_I2C_Checksum(const uint8_t *bytes,const uint8_t len) {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += bytes[i];
    }
    return (uint8_t)((~sum + 1) & 0xFF);
}

/**
 * @brief 从uC读取两个8位寄存器
 * @param reg_addr  寄存器地址 (偶数地址有效)
 * @param word     返回的 16 位寄存器值
 * @return HAL_OK / HAL_ERROR
 */
HAL_StatusTypeDef uC_I2C_Read(const uint8_t reg_addr, uint16_t *word) {
    // Step 1: 计算写传输校验和 (包含 I2C 地址 + 寄存器地址)
    const uint8_t sum_bytes[2]={uC_I2C_Addr,reg_addr};
    const uint8_t checksum_w = uC_I2C_Checksum(sum_bytes, 2);

    // Step 2: 打包发送地址和校验字节
    uint8_t tx_buf[2]={reg_addr,checksum_w};
    uint8_t rx_buf[3];
    if (HAL_I2C_Master_Transmit(&hi2c2, 0xBE, tx_buf, 2, HAL_MAX_DELAY) != HAL_OK) {
        printf("I2C Write failed (reg=0x%02X)\r\n", reg_addr);
        return HAL_ERROR;
    }

    // Step 3: 读传输 (返回 LSB + MSB + CheckSum_r)
    if (HAL_I2C_Master_Receive(&hi2c2, 0xBE, rx_buf, 3, HAL_MAX_DELAY) != HAL_OK) {
        printf("I2C Read failed (reg=0x%02X)\r\n", reg_addr);
        return HAL_ERROR;
    }

    // Step 4: 验证读校验和 (只算 LSB + MSB)
    const uint8_t checksum_r = rx_buf[2];
    const uint8_t cs_calc = uC_I2C_Checksum(rx_buf, 2);
    if (cs_calc != checksum_r) {
        printf("Checksum mismatch (reg=0x%02X, calc=0x%02X, recv=0x%02X)\r\n",
                     reg_addr, cs_calc, checksum_r);
        return HAL_ERROR;
    }

    *word = (rx_buf[1] << 8) | rx_buf[0];
    return HAL_OK;
}

/**
 * @brief 向uC写入两个8位寄存器
 * @param reg_addr  寄存器地址 (偶数地址有效)
 * @param word     返回的 16 位寄存器值
 * @return HAL_OK / HAL_ERROR
 */
HAL_StatusTypeDef uC_I2C_Write(const uint8_t reg_addr,const uint16_t word)
{
    // Step 1: 计算校验和：设备地址(写方向) + 寄存器地址 + LSB + MSB
    const uint8_t valLSB = (uint8_t)(word & 0xFF);
    const uint8_t valMSB = (uint8_t)((word >> 8) & 0xFF);
    const uint8_t sum_bytes[4]={uC_I2C_Addr,reg_addr,valLSB,valMSB};
    const uint8_t checksum_w = uC_I2C_Checksum(sum_bytes, 4);

    // Step 2: 组装发送数据包
    uint8_t tx_buf[4]={reg_addr,valLSB,valMSB,checksum_w};
    return HAL_I2C_Master_Transmit(&hi2c2, uC_I2C_Addr, tx_buf, 4, HAL_MAX_DELAY);
}