#pragma once
#include <bts/network/channel_id.hpp>
#include <bts/peer/peer_host.hpp>
#include <fc/time.hpp>
#include <fc/network/ip.hpp>
#include <unordered_set>

namespace bts { namespace peer {
  /// these belong as part of the peer proto channel, not part of
  ///  the message class.
  enum message_code
  {
     generic         = 0,
     config          = 1,
     announce        = 2,
     known_hosts     = 3,
     error_report    = 4,
     subscribe       = 5,
     unsubscribe     = 6,
     get_known_hosts = 7,
     get_subscribed  = 8,
     announce_inv    = 9,
     get_announce    = 10,
  };

  struct config_msg
  {
      static const message_code type;
      /** The host_id is the city hash of the IP
       *  a hash is used to 'randomize' IPs and ensure that
       *  a client is connected to a wide range of IP 
       *  addresses and not a bunch of IPs from the same
       *  subnet.
       */
      uint64_t get_host_id()const;

      /** 
       *  A list of features supported by this client.
       */
      std::unordered_set<std::string>  supported_features;
      std::unordered_set<uint32_t>     subscribed_channels;
      fc::ip::endpoint                 public_contact;
      fc::time_point                   timestamp;
  };

  struct announce_inv_msg
  {
      static const message_code type;
      std::vector<uint64_t> announce_msgs;
  };
  struct get_announce_msg
  {
      static const message_code type;
      uint64_t announce_id;
  };


  struct known_hosts_msg
  {
      static const message_code type;
      known_hosts_msg(){}
      known_hosts_msg( std::vector<host> h )
      :hosts( std::move(h) ){}
      std::vector<host> hosts;
  };

  /**
   *  When a new node connects to the network, they can broadcast
   *  their IP and features so that other nodes can connect to them.  Because
   *  broadcasts can be expensive, connect messages 
   */
  struct announce_msg : public config_msg
  {
      static const message_code type;
      announce_msg();

      /** returns a hash for this announcement used when broadcasting the
       * message on the network. 
       */
      uint64_t        announce_id()const; 

      /** checks to make sure the work is sufficient and recent */
      bool            validate_work()const;

      /** this method will not return until validate_work() returns true or
       *  stop_birthday_search() has been called from another thread.
       */
      void            find_birthdays();
      void            stop_birthday_search();

      uint32_t        birthday_a;
      uint32_t        birthday_b;

      private:
      volatile bool _cancel_birthday_search;
  };

  struct subscribe_msg
  {
     static const message_code type;
     subscribe_msg(){}
     subscribe_msg( std::vector<network::channel_id> chans )
     :channels( std::move(chans) ){}

     std::vector<network::channel_id> channels;
  };

  struct unsubscribe_msg
  {
     static const message_code type;
     std::vector<network::channel_id> channels;
  };

  struct get_subscribed_msg 
  {
     static const message_code type;
  };

  struct get_known_hosts_msg
  {
      static const message_code type;
      get_known_hosts_msg(){}
  };

  struct error_report_msg
  {
     static const message_code type;
     uint32_t     code;
     std::string  message;
  };

} } // bts::peer

#include <fc/reflect/reflect.hpp>
FC_REFLECT( bts::peer::config_msg,       
    (supported_features)
    (subscribed_channels)
    (public_contact)
    (timestamp)
    )

FC_REFLECT_ENUM( bts::peer::message_code,
  (generic)
  (config)
  (known_hosts)
  (error_report)
  (subscribe)
  (unsubscribe)
  (get_known_hosts)
  (get_subscribed)
  )

FC_REFLECT( bts::peer::announce_inv_msg, (announce_msgs) )
FC_REFLECT( bts::peer::get_announce_msg, (announce_id) )
FC_REFLECT( bts::peer::known_hosts_msg,  (hosts) )
FC_REFLECT( bts::peer::error_report_msg, (code)(message) )
FC_REFLECT_DERIVED( bts::peer::announce_msg, (bts::peer::config_msg), (birthday_a)(birthday_b) )
FC_REFLECT( bts::peer::subscribe_msg,    (channels ) )
FC_REFLECT( bts::peer::unsubscribe_msg,  (channels ) )
FC_REFLECT( bts::peer::get_known_hosts_msg, BOOST_PP_SEQ_NIL ); 

