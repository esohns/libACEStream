/***************************************************************************
*   Copyright (C) 2009 by Erik Sohns   *
*   erik.sohns@web.de   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef TEST_I_HTTP_GET_COMMON_H
#define TEST_I_HTTP_GET_COMMON_H

#include <list>
#include <set>
#include <string>

#include "libxml/tree.h"

#include "stream_base.h"
#include "stream_control_message.h"

#include "stream_dec_common.h"

#include "stream_document_defines.h"

#include "stream_module_htmlparser.h"

#include "http_common.h"

#include "test_i_common.h"
#include "test_i_configuration.h"

#include "test_i_http_get_connection_common.h"
#include "test_i_http_get_defines.h"

struct Test_I_StockItem
{
  Test_I_StockItem ()
   : /*description ()
   ,*/ ISIN ()
   , symbol ()
   , WKN ()
   , isStock (true)
  {};
  inline bool operator== (const Test_I_StockItem& rhs_in) { return (ISIN == rhs_in.ISIN); }

  //std::string description;
  std::string ISIN;
  std::string symbol;
  std::string WKN;
  bool        isStock;
};

struct Test_I_StockRecord
{
  Test_I_StockRecord ()
   : change (0.0)
   , item (NULL)
   , timeStamp (ACE_Time_Value::zero)
   , value (0.0)
  {};
  inline bool operator== (const Test_I_StockRecord& rhs_in) { ACE_ASSERT (rhs_in.item); ACE_ASSERT (item); return (*item == *rhs_in.item); }

  double                   change;
  struct Test_I_StockItem* item;
  ACE_Time_Value           timeStamp;
  double                   value;
};

struct Test_I_MessageData
{
  Test_I_MessageData ()
   : HTTPRecord (NULL)
   , HTMLDocument (NULL)
   , stockItem ()
  {};
  virtual ~Test_I_MessageData ()
  {
    if (HTTPRecord)
      delete HTTPRecord;
    if (HTMLDocument)
      xmlFreeDoc (HTMLDocument);
  };
  inline void operator+= (Test_I_MessageData rhs_in) { ACE_UNUSED_ARG (rhs_in); ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline operator struct HTTP_Record&() const { ACE_ASSERT (HTTPRecord); return *HTTPRecord; }

  struct HTTP_Record*     HTTPRecord;
  xmlDocPtr               HTMLDocument;
  struct Test_I_StockItem stockItem;
};
//typedef Stream_DataBase_T<struct Test_I_MessageData> Test_I_MessageData_t;

struct less_stock_item
{
  inline bool operator () (const struct Test_I_StockItem& lhs_in, const struct Test_I_StockItem& rhs_in) const { return (lhs_in.ISIN < rhs_in.ISIN); }
};
typedef std::set<struct Test_I_StockItem, less_stock_item> Test_I_StockItems_t;
typedef Test_I_StockItems_t::iterator Test_I_StockItemsIterator_t;

typedef std::list<struct Test_I_StockRecord> Test_I_StockRecords_t;
typedef Test_I_StockRecords_t::const_iterator Test_I_StockRecordsIterator_t;

//struct Test_I_HTTPGet_ConnectionConfiguration;
//struct Test_I_HTTPGet_StreamConfiguration;
struct Test_I_HTTPGet_UserData
 : Stream_UserData
{
  Test_I_HTTPGet_UserData ()
   : Stream_UserData ()
   //, connectionConfiguration (NULL)
   //, streamConfiguration (NULL)
  {};

  //struct Test_I_HTTPGet_ConnectionConfiguration* connectionConfiguration;
  //struct Test_I_HTTPGet_StreamConfiguration*     streamConfiguration;
};

struct Test_I_HTTPGet_SessionData
 : Test_I_SessionData
{
  Test_I_HTTPGet_SessionData ()
   : Test_I_SessionData ()
   , data ()
   , format (STREAM_COMPRESSION_FORMAT_INVALID)
   //, parserContext (NULL)
   , targetFileName ()
   , userData (NULL)
  {};
  struct Test_I_HTTPGet_SessionData& operator+= (const struct Test_I_HTTPGet_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionData::operator+= (rhs_in);

    data.insert (data.end (), rhs_in.data.begin (), rhs_in.data.end ());
    //parserContext = (parserContext ? parserContext : rhs_in.parserContext);
    targetFileName = (targetFileName.empty () ? rhs_in.targetFileName
                                              : targetFileName);
    //userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

  Test_I_StockRecords_t                     data; // html parser/spreadsheet writer module
  enum Stream_Decoder_CompressionFormatType format; // decompressor module
  //Test_I_SAXParserContext*                  parserContext; // html parser/handler module
  std::string                               targetFileName; // file writer module

  struct Test_I_HTTPGet_UserData*           userData;
};
typedef Stream_SessionData_T<struct Test_I_HTTPGet_SessionData> Test_I_HTTPGet_SessionData_t;

enum Test_I_SAXParserState : int
{
  SAXPARSER_STATE_INVALID = -1,
  ////////////////////////////////////////
  SAXPARSER_STATE_IN_HTML = 0,
  ////////////////////////////////////////
  SAXPARSER_STATE_IN_HEAD,
  SAXPARSER_STATE_IN_BODY,
  ////////////////////////////////////////
  //SAXPARSER_STATE_IN_HEAD_TITLE,
  ////////////////////////////////////////
  SAXPARSER_STATE_IN_BODY_DIV_CONTENT,
  SAXPARSER_STATE_IN_SYMBOL_H1_CONTENT,
  ////////////////////////////////////////
  SAXPARSER_STATE_READ_CHANGE,
  SAXPARSER_STATE_READ_DATE,
  SAXPARSER_STATE_READ_ISIN_WKN,
  SAXPARSER_STATE_READ_SYMBOL,
  SAXPARSER_STATE_READ_VALUE
};
struct Test_I_SAXParserContext
 : Stream_Module_HTMLParser_SAXParserContextBase
{
  Test_I_SAXParserContext ()
   : Stream_Module_HTMLParser_SAXParserContextBase ()
   , record (NULL)
   , state (SAXPARSER_STATE_INVALID)
  {};

  struct Test_I_StockRecord* record;
  enum Test_I_SAXParserState state;
};

class Test_I_Stream_Message;
class Test_I_Stream_SessionMessage;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct Common_FlexParserAllocatorConfiguration> Test_I_ControlMessage_t;
//struct Test_I_HTTPGet_StreamConfiguration;
//struct Test_I_HTTPGet_ModuleHandlerConfiguration;
//static constexpr const char stream_name_string_[] =
//    ACE_TEXT_ALWAYS_CHAR ("HTTPGetStream");
//typedef Stream_Configuration_T<stream_name_string_,
//                               struct Common_FlexParserAllocatorConfiguration,
//                               struct Test_I_HTTPGet_StreamConfiguration,
//                               struct Stream_ModuleConfiguration,
//                               struct Test_I_HTTPGet_ModuleHandlerConfiguration> Test_I_HTTPGet_StreamConfiguration_t;
//typedef Stream_Base_T<ACE_MT_SYNCH,
//                      Common_TimePolicy_t,
//                      stream_name_string_,
//                      enum Stream_ControlType,
//                      enum Stream_SessionMessageType,
//                      enum Stream_StateMachine_ControlState,
//                      struct Test_I_HTTPGet_StreamState,
//                      struct Test_I_HTTPGet_StreamConfiguration,
//                      Test_I_Statistic_t,
//                      Test_I_StatisticHandlerReactor_t,
//                      struct Common_FlexParserAllocatorConfiguration,
//                      struct Stream_ModuleConfiguration,
//                      struct Test_I_HTTPGet_ModuleHandlerConfiguration,
//                      struct Test_I_HTTPGet_SessionData,
//                      Test_I_HTTPGet_SessionData_t,
//                      Test_I_ControlMessage_t,
//                      Test_I_Stream_Message,
//                      Test_I_Stream_SessionMessage> Test_I_StreamBase_t;
struct Test_I_HTTPGet_ModuleHandlerConfiguration
 : Test_I_ModuleHandlerConfiguration
{
  Test_I_HTTPGet_ModuleHandlerConfiguration ()
   : Test_I_ModuleHandlerConfiguration ()
   , allocatorConfiguration (NULL)
   , configuration (NULL)
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , fileName ()
   , HTTPForm ()
   , HTTPHeaders ()
   , libreOfficeHost (TEST_I_DEFAULT_PORT,
                      ACE_TEXT_ALWAYS_CHAR (ACE_LOCALHOST),
                      ACE_ADDRESS_FAMILY_INET)
   , libreOfficeRc ()
   , libreOfficeSheetStartColumn (0)
   , libreOfficeSheetStartRow (TEST_I_DEFAULT_LIBREOFFICE_START_ROW - 1)
   , mode (STREAM_MODULE_HTMLPARSER_MODE_SAX)
   , stockItems ()
   , streamConfiguration (NULL)
   , URL ()
  {};

  struct Common_FlexParserAllocatorConfiguration* allocatorConfiguration;
  struct Test_I_HTTPGet_Configuration*            configuration;
  Test_I_IConnection_t*                           connection; // net source/IO module
  Test_I_HTTPGet_ConnectionConfigurations_t*      connectionConfigurations;
  Test_I_HTTPGet_InetConnectionManager_t*         connectionManager; // net source/IO module
  std::string                                     fileName; // spreadsheet writer module
  HTTP_Form_t                                     HTTPForm; // HTTP get module
  HTTP_Headers_t                                  HTTPHeaders; // HTTP get module
  ACE_INET_Addr                                   libreOfficeHost; // spreadsheet writer module
  std::string                                     libreOfficeRc; // spreadsheet writer module
  unsigned int                                    libreOfficeSheetStartColumn; // spreadsheet writer module
  unsigned int                                    libreOfficeSheetStartRow; // spreadsheet writer module
  enum Stream_Module_HTMLParser_Mode              mode; // HTML parser module
  Test_I_StockItems_t                             stockItems; // HTTP get module
  Test_I_HTTPGet_StreamConfiguration_t*           streamConfiguration; // net source module
  std::string                                     URL; // HTTP get module
};
//typedef std::map<std::string,
//                 struct Test_I_HTTPGet_ModuleHandlerConfiguration> Test_I_HTTPGet_ModuleHandlerConfigurations_t;
//typedef Test_I_HTTPGet_ModuleHandlerConfigurations_t::const_iterator Test_I_HTTPGet_ModuleHandlerConfigurationsConstIterator_t;

struct Test_I_HTTPGet_StreamConfiguration
 : Stream_Configuration
{
  Test_I_HTTPGet_StreamConfiguration ()
   : Stream_Configuration ()
   , userData (NULL)
  {};

  struct Test_I_HTTPGet_UserData* userData;
};

struct Test_I_HTTPGet_StreamState
 : Stream_State
{
  Test_I_HTTPGet_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
   , userData (NULL)
  {};

  struct Test_I_HTTPGet_SessionData* sessionData;

  struct Test_I_HTTPGet_UserData*    userData;
};

//typedef std::map<std::string,
//                 Test_I_HTTPGet_ConnectionConfiguration_t> Test_I_HTTPGet_ConnectionConfigurations_t;
//typedef Test_I_HTTPGet_ConnectionConfigurations_t::iterator Test_I_HTTPGet_ConnectionConfigurationIterator_t;

struct Test_I_HTTPGet_Configuration
 : Test_I_Configuration
{
  Test_I_HTTPGet_Configuration ()
   : Test_I_Configuration ()
   //, allocatorConfiguration ()
   , connectionConfigurations ()
   , streamConfiguration ()
   , userData ()
  {};

  // *NOTE*: use the stream configurations' allocator configuration
  //struct Common_FlexParserAllocatorConfiguration allocatorConfiguration;
  Test_I_HTTPGet_ConnectionConfigurations_t      connectionConfigurations;
  Test_I_HTTPGet_StreamConfiguration_t           streamConfiguration;

  struct Test_I_HTTPGet_UserData                 userData;
};

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Common_FlexParserAllocatorConfiguration,
                                          Test_I_ControlMessage_t,
                                          Test_I_Stream_Message,
                                          Test_I_Stream_SessionMessage> Test_I_MessageAllocator_t;

#endif
