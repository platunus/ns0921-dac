/*
 * ASoC Driver for NS NS0921 DAC.
 *
 * Author:	Naoki Serizawa <platunus70@gmail.com>
 *		Copyright 2018
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>

#include "../codecs/ns0921.h"

/*
static bool slave_bck;
static bool slave_lrck;
*/

static int snd_ns_ns0921_dac_init(struct snd_soc_pcm_runtime *rtd)
{
/*
    if (slave_bck && slave_lrck) {
        struct snd_soc_dai_link *dai = rtd->dai_link;
        dai->name = "NS0921-DAC (Complete Slave)";
        dai->stream_name = "NS0921-DAC (Complete Slave) HiFi";
        dai->dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF
            | SND_SOC_DAIFMT_CBM_CFM;
	} else if (slave_bck) {
        struct snd_soc_dai_link *dai = rtd->dai_link;
        dai->name = "NS0921-DAC (BCK Slave)";
        dai->stream_name = "NS0921-DAC (BCK Slave) HiFi";
        dai->dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF
            | SND_SOC_DAIFMT_CBM_CFS;
	}
*/

	return 0;
}

static inline int get_coeff(int rate, int width)
{
	int i;  

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++) {
		if (coeff_div[i].rate == rate && coeff_div[i].width == width)
			return i;
	}
	return -1;
}

static int snd_ns_ns0921_dac_hw_params(struct snd_pcm_substream *substream,
				       struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

    int i = get_coeff(params_rate(params), params_width(params));
    return snd_soc_dai_set_bclk_ratio(cpu_dai, coeff_div[i].bclk_ratio);
}

/* machine stream operations */
static struct snd_soc_ops snd_ns_ns0921_dac_ops = {
	.hw_params = snd_ns_ns0921_dac_hw_params,
};

static struct snd_soc_dai_link snd_ns_ns0921_dac_dai[] = {
{
	.name		= "NS0921-DAC (Master)",
	.stream_name	= "NS0921-DAC (Master) HiFi",
	.cpu_dai_name	= "bcm2708-i2s.0",
	.codec_dai_name	= "ns0921-hifi",
	.platform_name	= "bcm2708-i2s.0",
	.codec_name	= "ns0921.1-0041",
	.dai_fmt	= SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
//				SND_SOC_DAIFMT_CBS_CFS,
				SND_SOC_DAIFMT_CBM_CFM,
	.ops		= &snd_ns_ns0921_dac_ops,
	.init		= snd_ns_ns0921_dac_init,
},
};

/* audio machine driver */
static struct snd_soc_card snd_ns_ns0921_dac = {
	.name         = "snd_ns_ns0921_dac",
	.owner        = THIS_MODULE,
	.dai_link     = snd_ns_ns0921_dac_dai,
	.num_links    = ARRAY_SIZE(snd_ns_ns0921_dac_dai),
};

static int snd_ns_ns0921_dac_probe(struct platform_device *pdev)
{
	int ret = 0;

	snd_ns_ns0921_dac.dev = &pdev->dev;
	
	if (pdev->dev.of_node) {
		struct device_node *i2s_node;
		struct snd_soc_dai_link *dai = &snd_ns_ns0921_dac_dai[0];
		i2s_node = of_parse_phandle(pdev->dev.of_node, "i2s-controller", 0);

		if (i2s_node) {
			dai->cpu_dai_name = NULL;
			dai->cpu_of_node = i2s_node;
			dai->platform_name = NULL;
			dai->platform_of_node = i2s_node;
		}

/*
        slave_bck = of_property_read_bool(pdev->dev.of_node,
                        "ns0921-dac,slave_bck");
        slave_lrck = of_property_read_bool(pdev->dev.of_node,
                        "ns0921-dac,slave_lrck");
*/

	}
	
	ret = snd_soc_register_card(&snd_ns_ns0921_dac);
	if (ret)
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n", ret);

	return ret;
}

static int snd_ns_ns0921_dac_remove(struct platform_device *pdev)
{
	return snd_soc_unregister_card(&snd_ns_ns0921_dac);
}

static const struct of_device_id snd_ns_ns0921_dac_of_match[] = {
	{ .compatible = "ns,ns0921-dac", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_ns_ns0921_dac_of_match);

static struct platform_driver snd_ns_ns0921_dac_driver = {
        .driver = {
                .name   = "snd-ns0921-dac",
                .owner  = THIS_MODULE,
                .of_match_table = snd_ns_ns0921_dac_of_match,
        },
        .probe          = snd_ns_ns0921_dac_probe,
        .remove         = snd_ns_ns0921_dac_remove,
};

module_platform_driver(snd_ns_ns0921_dac_driver);

MODULE_AUTHOR("Naoki Serizawa <platunu70@gmail.com>");
MODULE_DESCRIPTION("ASoC Driver for NS NS0921 DAC");
MODULE_LICENSE("GPL v2");
