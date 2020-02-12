#include "ANTProfile.h"

ANTProfile::ANTProfile(ANTTransmissionMode mode)
{
   m_op_mode = mode;
}

void ANTProfile::ProcessMessage(ant_evt_t* evt)
{
   if (evt->channel == m_channel_number)
   {
      switch (evt->event)
      {
            case EVENT_TX:
               EncodeMessage();
               SendMessage();
               break;

            case EVENT_RX:
               if (evt->message.ANT_MESSAGE_ucMesgID == MESG_BROADCAST_DATA_ID
               || evt->message.ANT_MESSAGE_ucMesgID == MESG_ACKNOWLEDGED_DATA_ID
               || evt->message.ANT_MESSAGE_ucMesgID == MESG_BURST_DATA_ID)
               {
                  DecodeMessage(evt->message.ANT_MESSAGE_aucPayload);
               }
               break;

            default:
               break;
      }
   }
}

bool ANTProfile::Setup(uint8_t channel)
{
   uint32_t err_code;

   m_channel_number = channel;
   m_channel_sens_config.channel_number = channel;
   m_disp_config.channel_number = channel;

   if (m_op_mode == ANTTransmissionMode::TX)
   {
      // Fill tx buffer for the first frame
      EncodeMessage();

      uint32_t err_code;
      // Set Channel Number.
      err_code = sd_ant_channel_assign(m_channel_sens_config.channel_number,
                                       m_channel_sens_config.channel_type,
                                       m_channel_sens_config.network_number,
                                       m_channel_sens_config.ext_assign);
      if (err_code != NRF_SUCCESS)
      {
         return err_code;
      }

      // Set Channel ID.
      err_code = sd_ant_channel_id_set(m_channel_sens_config.channel_number,
                                       m_channel_sens_config.device_number,
                                       m_channel_sens_config.device_type,
                                       m_channel_sens_config.transmission_type);
      if (err_code != NRF_SUCCESS)
      {
         return err_code;
      }

      // Set Channel RF frequency.
      err_code = sd_ant_channel_radio_freq_set(m_channel_sens_config.channel_number, m_channel_sens_config.rf_freq);
      if (err_code != NRF_SUCCESS)
      {
         return err_code;
      }

      // Set Channel period.
      if (!(m_channel_sens_config.ext_assign & EXT_PARAM_ALWAYS_SEARCH) && (m_channel_sens_config.channel_period != 0))
      {
         err_code = sd_ant_channel_period_set(m_channel_sens_config.channel_number, m_channel_sens_config.channel_period);
      }

      if (err_code != NRF_SUCCESS)
      {
         return false;
      }

      SendMessage();
    }
    else if (m_op_mode == ANTTransmissionMode::RX)
    {
        err_code = ant_channel_init(&m_disp_config);
        if (err_code != NRF_SUCCESS)
        {
            return false;
        }
    }

    err_code = sd_ant_channel_open(m_channel_number);
    if (err_code != NRF_SUCCESS)
    {
        return false;
    }

   return true;
}

uint32_t ANTProfile::SendMessage()
{
   uint32_t err_code = sd_ant_broadcast_message_tx(m_channel_number, sizeof(m_message_payload), m_message_payload);

   return err_code;
}
