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

#ifndef TEST_I_COMMON_H
#define TEST_I_COMMON_H

#include <algorithm>
#include <deque>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <string>

#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "libxml/tree.h"

#include "common.h"
#include "common_inotify.h"
#include "common_istatistic.h"
#include "common_isubscribe.h"
#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_data_base.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_session_data.h"
#include "stream_statemachine_control.h"

#include "stream_dec_common.h"

//#include "stream_module_document_common.h"

#include "stream_module_htmlparser.h"

#include "stream_module_net_common.h"

#include "net_configuration.h"
#include "net_defines.h"

#include "http_common.h"
#include "http_defines.h"

#include "test_i_connection_common.h"
#include "test_i_connection_manager_common.h"
#include "test_i_defines.h"
//#include "test_i_message.h"
//#include "test_i_session_message.h"

// forward declarations
class Stream_IAllocator;
class Test_I_Stream_Message;
class Test_I_Stream_SessionMessage;
struct Test_I_ConnectionState;

typedef int Stream_HeaderType_t;
typedef int Stream_CommandType_t;

typedef Stream_Statistic Test_I_RuntimeStatistic_t;

typedef Common_IStatistic_T<Test_I_RuntimeStatistic_t> Test_I_StatisticReportingHandler_t;

struct Test_I_AllocatorConfiguration
 : Stream_AllocatorConfiguration
{
  inline Test_I_AllocatorConfiguration ()
   : Stream_AllocatorConfiguration ()
  {
    // *NOTE*: this facilitates (message block) data buffers to be scanned with
    //         'flex's yy_scan_buffer() method
    buffer = NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE;
  };
};

struct Test_I_StockItem
{
  inline Test_I_StockItem ()
   : /*description ()
     ,*/ ISIN ()
   , symbol ()
   , WKN ()
  {};

  inline bool operator== (const Test_I_StockItem& rhs_in)
  {
    return (ISIN == rhs_in.ISIN);
  };

  //std::string description;
  std::string ISIN;
  std::string symbol;
  std::string WKN;
};

struct Test_I_StockRecord
{
  inline Test_I_StockRecord ()
   : change (0.0)
   , item (NULL)
   , timeStamp (ACE_Time_Value::zero)
   , value (0.0)
  {};

  inline bool operator== (const Test_I_StockRecord& rhs_in)
  {
    // sanity check(s)
    ACE_ASSERT (rhs_in.item);
    ACE_ASSERT (item);

    return (*item == *rhs_in.item);
  };

  double            change;
  Test_I_StockItem* item;
  ACE_Time_Value    timeStamp;
  double            value;
};

struct Test_I_MessageData
{
  inline Test_I_MessageData ()
   : HTTPRecord (NULL)
   , HTMLDocument (NULL)
   , stockItem ()
  {};
  inline ~Test_I_MessageData ()
  {
    if (HTTPRecord)
      delete HTTPRecord;
    if (HTMLDocument)
      xmlFreeDoc (HTMLDocument);
  };

  HTTP_Record*     HTTPRecord;
  xmlDocPtr        HTMLDocument;
  Test_I_StockItem stockItem;
};
typedef Stream_DataBase_T<Test_I_MessageData> Test_I_MessageData_t;

struct less_stock_item
{
  bool operator () (const struct Test_I_StockItem& lhs_in,
                    const struct Test_I_StockItem& rhs_in) const
  {
    return (lhs_in.ISIN < rhs_in.ISIN);
  }
};
typedef std::set<Test_I_StockItem, less_stock_item> Test_I_StockItems_t;
typedef Test_I_StockItems_t::iterator Test_I_StockItemsIterator_t;

typedef std::list<Test_I_StockRecord> Test_I_StockRecords_t;
typedef Test_I_StockRecords_t::const_iterator Test_I_StockRecordsIterator_t;

enum Test_I_SAXParserState
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
  inline Test_I_SAXParserContext ()
   : Stream_Module_HTMLParser_SAXParserContextBase ()
   , data (NULL)
   , state (SAXPARSER_STATE_INVALID)
  {};

  Test_I_StockRecord*   data;
  Test_I_SAXParserState state;
};

struct Test_I_Configuration;
struct Test_I_Stream_Configuration;
struct Test_I_UserData
 : Stream_UserData
{
  inline Test_I_UserData ()
   : Stream_UserData ()
   , configuration (NULL)
   , streamConfiguration (NULL)
  {};

  Test_I_Configuration*        configuration;
  Test_I_Stream_Configuration* streamConfiguration;
};

struct Test_I_Stream_SessionData
 : Stream_SessionData
{
  inline Test_I_Stream_SessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   , data ()
   , format (STREAM_COMPRESSION_FORMAT_INVALID)
   //, parserContext (NULL)
   , targetFileName ()
   , userData (NULL)
  {};

  inline Test_I_Stream_SessionData& operator+= (const Test_I_Stream_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionData::operator+= (rhs_in);

    connectionState =
      (connectionState ? connectionState : rhs_in.connectionState);
    data.insert (data.end (), rhs_in.data.begin (), rhs_in.data.end ());
    //parserContext = (parserContext ? parserContext : rhs_in.parserContext);
    targetFileName = (targetFileName.empty () ? rhs_in.targetFileName
                                              : targetFileName);
    //userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

  Test_I_ConnectionState*                   connectionState;
  Test_I_StockRecords_t                     data; // html parser/spreadsheet writer module
  enum Stream_Decoder_CompressionFormatType format; // decompressor module
  //Test_I_SAXParserContext*                  parserContext; // html parser/handler module
  std::string                               targetFileName; // file writer module
  Test_I_UserData*                          userData;
};
typedef Stream_SessionData_T<Test_I_Stream_SessionData> Test_I_Stream_SessionData_t;

struct Test_I_Stream_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline Test_I_Stream_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   ///////////////////////////////////////
   , userData (NULL)
  {};

  Test_I_UserData* userData;
};

// forward declarations
struct Test_I_Configuration;
typedef Stream_Base_T<ACE_SYNCH_MUTEX,
                      ////////////////////
                      ACE_MT_SYNCH,
                      Common_TimePolicy_t,
                      ////////////////////
                      int,
                      int,
                      Stream_StateMachine_ControlState,
                      Test_I_Stream_State,
                      ////////////////////
                      Test_I_Stream_Configuration,
                      ////////////////////
                      Test_I_RuntimeStatistic_t,
                      ////////////////////
                      Stream_ModuleConfiguration,
                      Test_I_Stream_ModuleHandlerConfiguration,
                      ////////////////////
                      Test_I_Stream_SessionData,   // session data
                      Test_I_Stream_SessionData_t, // session data container (reference counted)
                      Test_I_Stream_SessionMessage,
                      Test_I_Stream_Message> Test_I_StreamBase_t;
struct Test_I_Stream_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  inline Test_I_Stream_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , configuration (NULL)
   , connection (NULL)
   , connectionManager (NULL)
   , HTTPForm ()
   , HTTPHeaders ()
   //, hostName ()
   , inbound (true)
   , libreOfficeHost (TEST_I_DEFAULT_PORT,
                      ACE_TEXT_ALWAYS_CHAR (ACE_LOCALHOST),
                      ACE_ADDRESS_FAMILY_INET)
   , libreOfficeRc ()
   , libreOfficeSheetStartColumn (0)
   , libreOfficeSheetStartRow (TEST_I_DEFAULT_LIBREOFFICE_START_ROW - 1)
   , mode (STREAM_MODULE_HTMLPARSER_SAX)
   , passive (false)
   , printProgressDot (false)
   , socketConfiguration (NULL)
   , socketHandlerConfiguration (NULL)
   , stockItems ()
   , stream (NULL)
   , targetFileName ()
   , URL ()
  {
    crunchMessages = HTTP_DEFAULT_CRUNCH_MESSAGES; // HTTP parser module

    traceParsing = NET_PROTOCOL_DEFAULT_YACC_TRACE; // HTTP parser module
    traceScanning = NET_PROTOCOL_DEFAULT_LEX_TRACE; // HTTP parser module
  };

  Test_I_Configuration*                     configuration;
  Test_I_IConnection_t*                     connection; // net source/IO module
  Test_I_Stream_InetConnectionManager_t*    connectionManager; // net source/IO module
  bool                                      crunchMessages; // HTTP parser module
  HTTP_Form_t                               HTTPForm; // HTTP get module
  HTTP_Headers_t                            HTTPHeaders; // HTTP get module
  bool                                      inbound; // IO module
  ACE_INET_Addr                             libreOfficeHost; // spreadsheet writer module
  std::string                               libreOfficeRc; // spreadsheet writer module
  unsigned int                              libreOfficeSheetStartColumn; // spreadsheet writer module
  unsigned int                              libreOfficeSheetStartRow; // spreadsheet writer module
  Stream_Module_HTMLParser_Mode             mode; // html parser module
  bool                                      passive; // net source module
  bool                                      printProgressDot; // file writer module
  Net_SocketConfiguration*                  socketConfiguration;
  Test_I_Stream_SocketHandlerConfiguration* socketHandlerConfiguration;
  Test_I_StockItems_t                       stockItems; // HTTP get module
  Test_I_StreamBase_t*                      stream; // net source module
  std::string                               targetFileName; // file writer module
  std::string                               URL; // HTTP get module
};

struct Stream_SignalHandlerConfiguration
{
  inline Stream_SignalHandlerConfiguration ()
   : //messageAllocator (NULL)
   /*,*/ statisticReportingInterval (ACE_Time_Value::zero)
   , useReactor (true)
  {};

  //Stream_IAllocator* messageAllocator;
  ACE_Time_Value     statisticReportingInterval; // statistic collecting interval (second(s)) [0: off]
  bool               useReactor;
};

struct Test_I_Stream_Configuration
 : Stream_Configuration
{
  inline Test_I_Stream_Configuration ()
   : Stream_Configuration ()
   , moduleHandlerConfiguration (NULL)
  {};

  Test_I_Stream_ModuleHandlerConfiguration* moduleHandlerConfiguration;
};

struct Test_I_Stream_State
 : Stream_State
{
  inline Test_I_Stream_State ()
   : Stream_State ()
   , currentSessionData (NULL)
   , userData (NULL)
  {};

  Test_I_Stream_SessionData* currentSessionData;
  Test_I_UserData*           userData;
};

struct Test_I_Configuration
{
  inline Test_I_Configuration ()
   : signalHandlerConfiguration ()
   , socketConfiguration ()
   , socketHandlerConfiguration ()
   , moduleConfiguration ()
   , moduleHandlerConfiguration ()
   , streamConfiguration ()
   , userData ()
   , useReactor (NET_EVENT_USE_REACTOR)
  {};

  // **************************** signal data **********************************
  Stream_SignalHandlerConfiguration        signalHandlerConfiguration;
  // **************************** socket data **********************************
  Net_SocketConfiguration                  socketConfiguration;
  Test_I_Stream_SocketHandlerConfiguration socketHandlerConfiguration;
  // **************************** stream data **********************************
  Stream_ModuleConfiguration               moduleConfiguration;
  Test_I_Stream_ModuleHandlerConfiguration moduleHandlerConfiguration;
  Test_I_Stream_Configuration              streamConfiguration;
  // *************************** protocol data *********************************
  Test_I_UserData                          userData;
  bool                                     useReactor;
};

typedef Stream_IModuleHandler_T<Test_I_Stream_ModuleHandlerConfiguration> Test_I_IModuleHandler_t;
typedef Stream_MessageAllocatorHeapBase_T<Test_I_AllocatorConfiguration,

                                          Test_I_Stream_Message,
                                          Test_I_Stream_SessionMessage> Test_I_MessageAllocator_t;

#endif