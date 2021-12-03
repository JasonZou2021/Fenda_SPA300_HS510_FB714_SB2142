#ifndef __REGMAP_SPA100_GPIO_H__
#define __REGMAP_SPA100_GPIO_H__

/* This header is included by entry header which should have defined RF_GRP */

struct gpio_ctr1_regs
{
	unsigned int gpio_ctl_sel[8];		// 6.0~6.7
	unsigned int gpio_oe[8];		// 6.8~6.15
	unsigned int gpio_o[8];			// 6.16~6.23
	unsigned int gpio_in[4];		// 6.24~6.27

};
#define GPIO_CTRL1_REG  ((volatile struct gpio_ctr1_regs *)RF_GRP(6, 0))

struct gpio_ctr2_regs
{
	unsigned int gpio_in_inv[8];		// 7.0~7.7
	unsigned int gpio_out_inv[8];		// 7.8~7.15
	unsigned int gpio_od[8];		// 7.16
};
#define GPIO_CTRL2_REG  ((volatile struct gpio_ctr2_regs *)RF_GRP(7, 0))

struct pad_ctl_regs
{
	unsigned int reserved[20];		// 101.0
	unsigned int spi_soft_pad_ctl0;	// 101.20
	unsigned int spi_soft_pad_ctl1;	// 101.21
	unsigned int spi_soft_pad_ctl2;	// 101.22
	unsigned int reserved_23[2];	// 101.23
	unsigned int gpio_first[3];		// 101.25
	unsigned int reserved_28[4];	// 101.28
};
#define PAD_CTL_REG ((volatile struct pad_ctl_regs *)RF_GRP(101, 0))


#endif /* __REGMAP_SPA100_GPIO_H__ */

