#define reg_gpio_ctrl *(volatile int *)(V2P(GPIO_REG_CTRL))
#define reg_gpio_data *(volatile int *)(V2P(GPIO_REG_DATA))

/* 初始化 */
void LightInit(void)
{
    reg_gpio_ctrl |= (1 << n); /* 写GPIO ctrl 寄存器第n位为1，使能GPIO */
}

/* 拉高 */
void LightHigh(void)
{
    reg_gpio_data |= (1 << m); /* 写GPIO data寄存器第m位为1，GPIO输出高电平 */
}

void LightLow(void)
{
    reg_gpio_data &= ~(1 << m); /* 写GPIO data寄存器第m位为0，GPIO输出低电平* */
}