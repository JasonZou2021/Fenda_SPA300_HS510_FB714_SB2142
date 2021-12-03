
#include <stdio.h>
#include <string.h>
#define LOG_MODULE_ID ID_APUSR
#include "log_utils.h"
#include "dsp_hint_sound.h"
#include "qpc.h"

#define hint_printf(f, a...) LOGD(f, ##a)
#define hint_printf_T(f, a...) LOGD("[%s][%d]"f,__FUNCTION__, __LINE__, ##a)

#define SPEAKER_HINT_NUM 4

static Comb_Hint_t *hint_data_head;
static Comb_Hint_t *hint_data_read;

Comb_Hint_t * hintSrcListDestroy(Comb_Hint_t *hint_data_list)
{
	if(hint_data_list == NULL)
	{
		hint_printf("hint list is NULL\n");
		return NULL;
	}

	Comb_Hint_t *temp;

	while(hint_data_list)
	{
		temp = hint_data_list->next;
		free(hint_data_list);
		hint_data_list = temp;
	}
	return hint_data_list;
}

void hintEndProcess()
{
	hint_data_head = hintSrcListDestroy(hint_data_head);
	hint_data_read = NULL;
}

int judgeNextSrcData()
{
	if(hint_data_read == NULL || hint_data_read->next == NULL)
	{
		hint_printf("next hint data is NULL!\n");
		return -1;
	}
	else
	{
		hint_printf("next hint data not NULL!\n");
		return 0;
	}
}

int getCurHintInterval()
{
	if(hint_data_read == NULL)
	{
		hint_printf("current hint data is NULL!\n");
		return 0;
	}
	return hint_data_read->interval_time;
}

DSP_HINT_SOUND_P * getNextSrcData()
{
	if(hint_data_head == NULL)
	{
		hint_printf("hint data is NULL!\n");
		return NULL;
	}

	if(hint_data_read == NULL)
	{
		hint_data_read = hint_data_head;
	}
	else
	{
		if(hint_data_read->next == NULL)
		{
			hint_printf("hint data is NULL!\n");
			return NULL;
		}
		hint_data_read = hint_data_read->next;
	}
	return hint_data_read->pCurHint;
}

void HintSrcAdd(Comb_Hint_t *add_data)
{
	Comb_Hint_t *hint_data_pthis;
	add_data->next = NULL;

	if(hint_data_head == NULL)
	{
		hint_data_head = add_data;
	}
	else
	{
		hint_data_pthis = hint_data_head;
		while(hint_data_pthis->next != NULL)
		{
			hint_data_pthis = hint_data_pthis->next;
		}
		hint_data_pthis->next = add_data;
	}
}

int Hint_SingleDataAdd(DSP_HINT_SOUND_P *single_data, int interval_time)
{
	int i = 0;
	Comb_Hint_t *temp;
	do
	{
		if((temp = (Comb_Hint_t *)malloc(sizeof(Comb_Hint_t))) == NULL)
		{
			hint_printf("malloc error!\n");
			return -1;
		}
		if(i < 1)
		{
			temp->pCurHint = single_data;
			temp->interval_time = interval_time;
			HintSrcAdd(temp);//add a hint to hint list
		}

	}while(i++ < 1);

	free(temp);
	return 0;
}

#if (SUPPORT_SPEAKER_TEST)
int Hint_SpeakerDataInit()
{
	int ret = 0;
	int i = 0;

	DSP_HINT_SOUND_P *hint_speaker_data[SPEAKER_HINT_NUM]=
	{
		hint_data_map[eHintData_Left],

		hint_data_map[eHintData_Left],

		hint_data_map[eHintData_Right],

		hint_data_map[eHintData_Right],
	};

	for(i = 0; i < SPEAKER_HINT_NUM; i++)
	{
		ret = Hint_SingleDataAdd(hint_speaker_data[i], DEFAULT_INTEVAL_TIME);
		if(ret < 0)
		{
			hint_printf("hint data init error\n");
			return ret;
		}
	}
	return 0;
}
#endif

int hintDataAdd(eHint_Type Hint_Type)
{
	int ret = 0;
	hint_printf("[%s][%d]Hint_Type:%d\n", __FUNCTION__, __LINE__, Hint_Type);
	switch(Hint_Type)
	{
		case eHint_BtReady:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_BtConnect], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_BtConnect:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_BtConnect], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
				return ret;
			}
			break;
		}
		case eHint_BtDisconnect:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_BtDisconnect], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
#if (SUPPORT_BT_SEARCH)
		case eHint_BtSearching:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_BtSearching], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
#endif
#if (SUPPORT_DETECT_SEARCH)
		case eHint_DetectSearching:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_DetectSearching], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
#endif
#if (SUPPORT_SRC_NAME)
		case eHint_SrcUSB:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcUSB], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcCard:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcCard], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcAux:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcAux], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcLine:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcLine], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcI2S:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcI2S], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcOpt:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcOpt], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcCoax:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcCoax], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcARC:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcARC], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrceARC:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrceARC], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcHDMI0:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcHDMI0], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcHDMI1:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcHDMI1], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcHDMI2:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcHDMI2], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_SrcBT:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_SrcBT], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
#endif
#if (SUPPORT_SPEAKER_TEST)
		case eHint_Speaker:{
			ret = Hint_SpeakerDataInit();
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
				return ret;
			}
			break;
		}
#endif
#if (SUPPORT_FW_VERSION)
		case eHint_FwVersion:{
			ret = FwVersionProc();
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
				return ret;
			}
			break;
		}
#endif
#if (SUPPORT_INFO)
		case eHint_Info:{
			ret = InfoProc();
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
				return ret;
			}
			break;
		}
#endif
#if (SUPPORT_EQ_TYPE)
		case eHint_EQMovie:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_EQMovie], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_EQMusic:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_EQMusic], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
		case eHint_EQGame:{
			ret = Hint_SingleDataAdd(hint_data_map[eHintData_EQGame], DEFAULT_INTEVAL_TIME);
			if(ret < 0)
			{
				hint_printf("hint data init error\n");
			}
			break;
		}
#endif
		default:{
			return -1;
			break;
		}
	}

	return ret;
}
