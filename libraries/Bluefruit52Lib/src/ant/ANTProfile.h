#ifndef ANTPROFILE_H
#define ANTPROFILE_H

#include <stdio.h>

#include "ant_interface.h"
#include "nrf_error.h"
#include "ant_event.h"
#include "ant_channel_config.h"


#define ANTPLUS_NETWORK_NUMBER  0                           /**< Network number. */
#define MAIN_DATA_INTERVAL          4       /**< The number of background data pages sent between main data pages.*/
#define BACKGROUND_DATA_INTERVAL    64      /**< The number of main data pages sent between background data page.
                                                 Background data page is sent every 65th message. */
#define TX_TOGGLE_DIVISOR           4       /**< The number of messages between changing state of toggle bit. */

enum ANTTransmissionMode
{
   RX,
   TX,
   Both
};

class ANTProfile
{
public:

   ANTProfile(ANTTransmissionMode mode);

   bool Setup(uint8_t channel);

   void ProcessMessage(ant_evt_t* evt);

protected:

   virtual void DecodeMessage(uint8_t* buffer) = 0;
   virtual void EncodeMessage() = 0;
   uint32_t SendMessage();

   uint8_t m_channel_number; ///< Channel number assigned to the profile.
   uint8_t m_message_payload[ANT_STANDARD_DATA_PAYLOAD_SIZE];
   ANTTransmissionMode m_op_mode;

   ant_channel_config_t m_channel_sens_config;
   ant_channel_config_t m_disp_config;

private:

};



class ANTProfileEntry
{
public:

   ANTProfileEntry(ANTProfile* e = NULL)
   {
      m_entry = e;
      m_next = NULL;
   }

   ANTProfile* m_entry;
   ANTProfileEntry* m_next;
};


class ANTProfileList
{
public:

   ANTProfileList()
   {
      m_head = NULL;
      m_tail = NULL;
   }

   void AddProfile(ANTProfile* p)
   {
      ANTProfileEntry* entry = new ANTProfileEntry(p);
      if (m_tail == NULL)
      {
         m_tail = entry;
         m_head = entry;
      }
      else
      {
         m_tail->m_next = entry;
      }
   }

   ANTProfileEntry* m_head;
   ANTProfileEntry* m_tail;
};
#endif