#include "AddressBookModel.hpp"
#include <QIcon>

#include <fc/exception/exception.hpp>
#include <fc/log/logger.hpp>

namespace Detail 
{
    class AddressBookModelImpl
    {
       public:
          QIcon                                   _default_icon;
          std::vector<Contact>                    _contacts;
          bts::addressbook::addressbook_ptr       _abook;
    };
}



AddressBookModel::AddressBookModel( QObject* parent, bts::addressbook::addressbook_ptr abook )
:QAbstractTableModel(parent),my( new Detail::AddressBookModelImpl() )
{
   my->_abook = abook;
   my->_default_icon.addFile(QStringLiteral(":/images/user.png"), QSize(), QIcon::Normal, QIcon::Off);
   /*
   auto known = abook->get_known_bitnames();
   my->_contacts.reserve(known.size());
   for( auto itr = known.begin(); itr != known.end(); ++itr )
   {
       auto opt_contact = my->_abook->get_contact_by_bitname( *itr );
       if( !opt_contact )
       {
          wlog( "broken addressbook, unable to find ${name} ", ("name",*itr) );
       }
       else
       {
          my->_contacts.push_back( *opt_contact );
       }
   }
   */
}
AddressBookModel::~AddressBookModel()
{
}

int AddressBookModel::rowCount( const QModelIndex& parent )const
{
    return my->_contacts.size();
}

int AddressBookModel::columnCount( const QModelIndex& parent  )const
{
    return NumColumns;
}

bool AddressBookModel::removeRows( int row, int count, const QModelIndex& parent )
{
   return false;
}

QVariant AddressBookModel::headerData( int section, Qt::Orientation orientation, int role )const
{
    if( orientation == Qt::Horizontal )
    {
       switch( role )
       {
          case Qt::DecorationRole:
             switch( (Columns)section )
             {
                case UserIcon:
                    return my->_default_icon;
                default:
                   return QVariant();
             }
          case Qt::DisplayRole:
          {
              switch( (Columns)section )
              {
                 case FirstName:
                     return tr("First Name");
                 case LastName:
                     return tr("Last Name");
                 case Id:
                     return tr("Id");
                 case Age:
                     return tr("Age");
                 case Repute:
                     return tr("Repute");
                 case UserIcon:
                 case NumColumns:
                     break;
              }
          }
          case Qt::SizeHintRole:
              switch( (Columns)section )
              {
                  case UserIcon:
                      return QSize( 32, 16 );
                  default:
                      return QVariant();
              }
       }
    }
    else
    {
    }
    return QVariant();
}

QVariant AddressBookModel::data( const QModelIndex& index, int role )const
{
    if( !index.isValid() ) return QVariant();

    const Contact& current_contact = my->_contacts[index.row()];
    switch( role )
    {
       case Qt::SizeHintRole:
           switch( (Columns)index.column() )
           {
               case UserIcon:
                   return QSize( 48, 48 );
               default:
                   return QVariant();
           }
       case Qt::DecorationRole:
          switch( (Columns)index.column() )
          {
             case UserIcon:
                 if( current_contact.icon.isNull() ) 
                    return my->_default_icon;
                 return current_contact.icon;
             default:
                return QVariant();
          }
       case Qt::DisplayRole:
          switch( (Columns)index.column() )
          {
             case FirstName:
                 return current_contact.first_name;
             case LastName:
                 return current_contact.last_name;
             case Id:
                 return current_contact.bit_id;
             case Age:
                 return 0;
             case Repute:
                 return 0;

             case UserIcon:
             case NumColumns:
                return QVariant();
          }
    }
    return QVariant();
}

int AddressBookModel::storeContact( const Contact& contact_to_store )
{
   if( contact_to_store.wallet_account_index == -1 )
   {
       auto num_contacts = my->_contacts.size();
       beginInsertRows( QModelIndex(), num_contacts, num_contacts );
          my->_contacts.push_back(contact_to_store);
          my->_contacts.back().wallet_account_index =  my->_contacts.size()-1;
       endInsertRows();
       // TODO: store to disk...
       return my->_contacts.back().wallet_account_index;
   }
   else
   {
       FC_ASSERT( contact_to_store.wallet_account_index < int(my->_contacts.size()) );
       auto row = contact_to_store.wallet_account_index;
       my->_contacts[row] = contact_to_store;

       Q_EMIT dataChanged( index( row, 0 ), index( row, NumColumns - 1) );
       return contact_to_store.wallet_account_index;
   }
}

