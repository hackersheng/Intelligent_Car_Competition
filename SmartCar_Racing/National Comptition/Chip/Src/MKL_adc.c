/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       MK60_adc.c
 * @brief      ADC函数
 * @author     野火科技
 * @version    v5.0
 * @date       2013-08-28
 */

#include "common.h"
#include "MKL_adc.h"

ADC_MemMapPtr ADCN[1] = {ADC0_BASE_PTR}; //定义一个指针数组保存 ADCN 的地址

static void     adc_start   (ADCn_Ch_e, ADC_nbit);    //开始adc转换

/*!
 *  @brief      ADC初始化
 *  @param      ADCn_Ch_e    ADC通道
 *  @since      v5.0
 *  @note       此初始化仅支持软件触发，不是每个通道都支持ADC 软件触发，
                具体说明见 ADCn_Ch_e 的注释说明
 *  Sample usage:       adc_init (ADC0_SE10 );    //初始化 ADC0_SE10 ，使用 PTA7 管脚
 */
void adc_init(ADCn_Ch_e adcn_ch)
{

    uint8 adcn = adcn_ch >> 5 ;
    //uint8 ch = adcn_ch & 0x1F;
    //ADC_MemMapPtr adc_ptr = ADCN[adcn];

    switch(adcn)
    {
    case ADC0:       /*   ADC0  */
        SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK );        //开启ADC0时钟
        SIM_SOPT7 &= ~(SIM_SOPT7_ADC0ALTTRGEN_MASK  | SIM_SOPT7_ADC0PRETRGSEL_MASK);
        SIM_SOPT7 |= SIM_SOPT7_ADC0TRGSEL(0);
        break;

    default:
        ASSERT(0);
    }

    switch(adcn_ch)
    {

    case ADC0_SE0:
        port_init(PTE20, ALT0);
        break;
    case ADC0_SE1:
        port_init(PTE16, ALT0);
        break;
    case ADC0_SE2:
        port_init(PTE18, ALT0);
        break;
    case ADC0_SE3:
        port_init(PTE22, ALT0);
        break;
    case ADC0_SE4a:
        port_init(PTE21, ALT0);
        break;
    case ADC0_SE5a:
        port_init(PTE17, ALT0);
        break;
    case ADC0_SE6a:
        port_init(PTE19, ALT0);
        break;
    case ADC0_SE7a:
        port_init(PTE23, ALT0);
        break;
    case ADC0_SE8:
        port_init(PTB0, ALT0);
        break;
    case ADC0_SE9:
        port_init(PTB1, ALT0);
        break;
    case ADC0_SE11:
        port_init(PTC2, ALT0);
        break;
    case ADC0_SE12:
        port_init(PTB2, ALT0);
        break;
    case ADC0_SE13:
        port_init(PTB3, ALT0);
        break;
    case ADC0_SE14:
        port_init(PTC0, ALT0);
        break;
    case ADC0_SE15:
        port_init(PTC1, ALT0);
        break;
    case ADC0_SE23:
        port_init(PTE30, ALT0);
        break;

    case Temp0_Sensor:   // Temperature Sensor,内部温度测量，可用ADC函数
    case VREFH0:         // 参考高电压,可用ADC函数 ,结果恒为 2^n-1
    case VREFL0:         // 参考低电压,可用ADC函数 ,结果恒为 0
        break;

    default:
        ASSERT(0);      //断言，传递的管脚不支持 ADC 单端软件触发，请换 其他管脚
        break;
    }

#if 0
    //设置输入模式
    ADC_SC1_REG(adc_ptr,0) =  (0
                               | ADC_SC1_ADCH(ch)           //通道选择
                               //| ADC_SC1_DIFF_MASK        //差分模式选择（注释表示 单通道）

                               );

    ADC_SC3_REG(adc_ptr) =
#endif

}

/*!
 *  @brief      获取ADC采样值(不支持B通道)
 *  @param      ADCn_Ch_e    ADC通道
 *  @param      ADC_nbit     ADC精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
 *  @return     采样值
 *  @since      v5.0
 *  Sample usage:       uint16 var = adc_once(ADC0_SE10, ADC_8bit);
 */
uint16 adc_once(ADCn_Ch_e adcn_ch, ADC_nbit bit) //采集某路模拟量的AD值
{
    ADCn_e adcn = (ADCn_e)(adcn_ch >> 5) ;

    uint16 result = 0;

    adc_start(adcn_ch, bit);      //启动ADC转换

    while (( ADC_SC1_REG(ADCN[adcn], 0 ) & ADC_SC1_COCO_MASK ) != ADC_SC1_COCO_MASK);   //只支持 A通道
    result = ADC_R_REG(ADCN[adcn], 0);
    ADC_SC1_REG(ADCN[adcn], 0) &= ~ADC_SC1_COCO_MASK;
    return result;
}

/*!
 *  @brief      启动ADC软件采样(不支持B通道)
 *  @param      ADCn_Ch_e    ADC通道
 *  @param      ADC_nbit     ADC精度（ ADC_8bit,ADC_12bit, ADC_10bit, ADC_16bit ）
 *  @since      v5.0
 *  @note       此函数内部调用，启动后即可等待数据采集完成
 *  Sample usage:       adc_start(ADC0_SE10, ADC_8bit);
 */
void adc_start(ADCn_Ch_e adcn_ch, ADC_nbit bit)
{
    ADCn_e adcn = (ADCn_e)(adcn_ch >> 5) ;
    uint8 ch = (uint8)(adcn_ch & 0x1F);

    //初始化ADC默认配置
    ADC_CFG1_REG(ADCN[adcn]) = (0
                                //| ADC_CFG1_ADLPC_MASK         //ADC功耗配置，0为正常功耗，1为低功耗
                                | ADC_CFG1_ADIV(2)              //时钟分频选择,分频系数为 2^n,2bit
                                | ADC_CFG1_ADLSMP_MASK          //采样时间配置，0为短采样时间，1 为长采样时间
                                | ADC_CFG1_MODE(bit)
                                | ADC_CFG1_ADICLK(0)            //0为总线时钟,1为总线时钟/2,2为交替时钟（ALTCLK），3为 异步时钟（ADACK）。
                               );


    ADC_CFG2_REG(ADCN[adcn])  = (0
                                 //| ADC_CFG2_MUXSEL_MASK       //ADC复用选择,0为a通道，1为b通道。
                                 //| ADC_CFG2_ADACKEN_MASK      //异步时钟输出使能,0为禁止，1为使能。
                                 | ADC_CFG2_ADHSC_MASK          //高速配置,0为正常转换序列，1为高速转换序列
                                 | ADC_CFG2_ADLSTS(0)           //长采样时间选择，ADCK为4+n个额外循环，额外循环，0为20，1为12，2为6，3为2
                                );

    //写入 SC1A 启动转换
    ADC_SC1_REG(ADCN[adcn], 0 ) = (0
                                   | ADC_SC1_AIEN_MASK          // 转换完成中断,0为禁止，1为使能
                                   //| ADC_SC1_DIFF_MASK        // 差分模式使能,0为单端，1为差分
                                   | ADC_SC1_ADCH( ch )
                                  );

    //ADC_SC1_REG(ADCN[adcn], 1 ) = 0;
}

/*!
 *  @brief      停止ADC软件采样
 *  @param      ADCn_e       ADC模块号（ ADC0、 ADC1）
 *  @since      v5.0
 *  Sample usage:       adc_stop(ADC0);
 */
void adc_stop(ADCn_e adcn)
{
    ADC_SC1_REG(ADCN[adcn], 0) = (0
                                  | ADC_SC1_AIEN_MASK                       // 转换完成中断,0为禁止，1为使能
                                  //| ADC_SC1_DIFF_MASK                     // 差分模式使能,0为单端，1为差分
                                  | ADC_SC1_ADCH(Module0_Dis)               //输入通道选择,此处选择禁止通道
                                 );
}

