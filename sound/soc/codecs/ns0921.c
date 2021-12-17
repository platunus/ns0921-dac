/*
 * ns0921.c  --  NS0921 ALSA SoC Audio driver
 *
 * Copyright 2018 NS Technology Reseaech
 *
 * Author: Naoki Serizawa <platunus70@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "ns0921.h"

/* codec private data */
struct ns0921_priv {
	struct regmap *regmap;
	int rate;
	int width;
	int fmt;
};

/*
 * ns0921 register cache
 */
static const struct reg_default ns0921_reg_defaults[] = {
	{ NS0921_REG,	0x00 },
};

static inline int get_coeff(int rate, int width)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].rate == rate && coeff_div[i].width == width)
			return i;
	}
	return -1;
}


static int ns0921_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct ns0921_priv *ns0921 = snd_soc_codec_get_drvdata(codec);

	int i;
	uint8_t reg;
	int rate = params_rate(params);
	int width = params_width(params);

	i = get_coeff(rate, width);
	if (i == -1) {
		printk(KERN_ERR 
			"Invalid parameters: ns0921_hw_params: rate:%d, width:%d\n",
			rate, width);
		return -EINVAL;
	}

	ns0921->rate = rate;
	ns0921->width = width;

	switch (ns0921->fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		reg = NS0921_LRCK_DISABLE | NS0921_BCK_DISABLE | coeff_div[i].reg;
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		reg = NS0921_LRCK_DISABLE | NS0921_BCK_ENABLE | coeff_div[i].reg;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		reg = NS0921_LRCK_ENABLE | NS0921_BCK_ENABLE | coeff_div[i].reg;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_write(codec, NS0921_REG, reg);
	printk(KERN_INFO "ns0921_hw_params: rate:%d width:%d reg:%02x\n", 
		rate, width, reg);

	return 0;
}

static int ns0921_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct ns0921_priv *ns0921 = snd_soc_codec_get_drvdata(codec);

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	default:
		return -EINVAL;
	}

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
	case SND_SOC_DAIFMT_CBM_CFS:
	case SND_SOC_DAIFMT_CBS_CFS:
		ns0921->fmt = fmt;
		break;
	default:
		return -EINVAL;
	}

	/* set iface */
	return 0;
}

static int ns0921_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct ns0921_priv *ns0921 = snd_soc_codec_get_drvdata(codec);

	switch (level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
	case SND_SOC_BIAS_STANDBY:
		/* ..then sync in the register cache. */
		regcache_sync(ns0921->regmap);
		break;

	case SND_SOC_BIAS_OFF:
		printk(KERN_INFO "ns0921_set_bias_level reg:%2x\n", 
			NS0921_BCK_DISABLE);
		snd_soc_write(codec, NS0921_REG, NS0921_BCK_DISABLE);
		break;
	}
	return 0;
}

#define NS0921_RATES (SNDRV_PCM_RATE_8000_192000)

#define NS0921_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
	SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static const struct snd_soc_dai_ops ns0921_dai_ops = {
	.hw_params	= ns0921_hw_params,
	.set_fmt	= ns0921_set_dai_fmt,
};

static struct snd_soc_dai_driver ns0921_dai = {
	.name = "ns0921-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = NS0921_RATES,
		.formats = NS0921_FORMATS,
	},
	.ops = &ns0921_dai_ops,
};

static struct snd_soc_codec_driver soc_codec_dev_ns0921 = {
	.set_bias_level = ns0921_set_bias_level,
	.idle_bias_off = true,
	.suspend_bias_off = true,
};

static const struct of_device_id ns0921_of_match[] = {
	{ .compatible = "ns,ns0921", },
	{ }
};
MODULE_DEVICE_TABLE(of, ns0921_of_match);

static const struct regmap_config ns0921_regmap = {
	.reg_bits = 2,
	.val_bits = 6,
	.max_register = NS0921_REG,

	.reg_defaults = ns0921_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(ns0921_reg_defaults),
	.cache_type = REGCACHE_RBTREE,
};

static int ns0921_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct ns0921_priv *ns0921;
	int ret;

	ns0921 = devm_kzalloc(&i2c->dev, sizeof(struct ns0921_priv),
			      GFP_KERNEL);
	if (ns0921 == NULL)
		return -ENOMEM;

	ns0921->rate = 0;
	ns0921->regmap = devm_regmap_init_i2c(i2c, &ns0921_regmap);
	if (IS_ERR(ns0921->regmap))
		return PTR_ERR(ns0921->regmap);

	i2c_set_clientdata(i2c, ns0921);

	ret =  snd_soc_register_codec(&i2c->dev,
			&soc_codec_dev_ns0921, &ns0921_dai, 1);

	return ret;
}

static int ns0921_i2c_remove(struct i2c_client *client)
{
	snd_soc_unregister_codec(&client->dev);
	return 0;
}

static const struct i2c_device_id ns0921_i2c_id[] = {
	{ "ns0921", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ns0921_i2c_id);

static struct i2c_driver ns0921_i2c_driver = {
	.driver = {
		.name = "ns0921",
		.of_match_table = ns0921_of_match,
	},
	.probe =    ns0921_i2c_probe,
	.remove =   ns0921_i2c_remove,
	.id_table = ns0921_i2c_id,
};

static int __init ns0921_modinit(void)
{
	int ret = 0;
	ret = i2c_add_driver(&ns0921_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register NS0921 I2C driver: %d\n",
		       ret);
	}
	return ret;
}
module_init(ns0921_modinit);

static void __exit ns0921_exit(void)
{
	i2c_del_driver(&ns0921_i2c_driver);
}
module_exit(ns0921_exit);

MODULE_DESCRIPTION("ASoC SERI5131 driver");
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_LICENSE("GPL");
